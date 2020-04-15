/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

/*
This file provides Hardware Abstraction Layer of CPU power management support.
*/

#include "k_api.h"
#include "cpu_pwr_config.h"

//#if RHINO_CONFIG_CPU_PWR_MGMT

#include "stdlib.h"
#include "k_atomic.h"
#include "dl_list.h"
#ifdef RHINO_CONFIG_SMP
#include "k_critical.h"
#endif
#include "cpu_pwr_lib.h"
#include "cpu_pwr_hal_lib.h"

#ifdef RHINO_CONFIG_SMP
#include "cpu_ipi.h"
#endif
#include "cpu_pwr_api.h"
#include "pwr_debug.h"

/* debug switch of this file */

#undef CPU_PWR_HAL_DBG

/* forward declarations */

/*
board_cpu_pwr_topo_create() must be provided by board or platform,
different board/platform may has different cpu topology.
*/

extern kstat_t board_cpu_pwr_topo_create (void);

/*
To put all the CPU nodes into CPU topology tree, we need a root mount
point, a node named "cpu_pwr_root0" is created by cpu_pwr_hal_lib_init()
before any CPU node is created.
*/

static cpu_pwr_t    cpu_pwr_root_node;
cpu_pwr_t *         p_cpu_pwr_root_node = &cpu_pwr_root_node;

/* the index of this p_cpu_leaf_node_array implies the cpu logic id */

cpu_pwr_t *         p_cpu_leaf_node_array[CPUS_NUM_MAX];
static kmutex_t     cpu_pwr_hal_mux;

#ifdef RHINO_CONFIG_SMP
static kspinlock_t cpu_pwr_hal_spin;

static void cpu_pwr_live_bit_set_init (void);
static void cpu_pwr_live_cpubit_set (cpu_pwr_t * pChild);
static void cpu_pwr_live_cpubit_clear (cpu_pwr_t * pChild);
#endif /* RHINO_CONFIG_SMP */
static int  num_of_bit_one_get (uint32_t n);

/* callback function array */

#define CPU_TREE_MUX_LOCK()    (void) krhino_mutex_lock (&cpu_pwr_hal_mux, RHINO_WAIT_FOREVER)
#define CPU_TREE_MUX_UNLOCK()  (void) krhino_mutex_unlock (&cpu_pwr_hal_mux)

#ifdef RHINO_CONFIG_SMP
#define CPU_TREE_SPIN_TAKE() krhino_spin_lock_irq_save (&cpu_pwr_hal_spin)
#define CPU_TREE_SPIN_GIVE() krhino_spin_unlock_irq_restore (&cpu_pwr_hal_spin)
#else
#define CPU_TREE_SPIN_TAKE() do{}while(0)
#define CPU_TREE_SPIN_GIVE() do{}while(0)
#endif

static cpu_pwr_t * cpu_pwr_node_find_by_name_
    (
    cpu_pwr_t * p_cpu_node,
    char      * pName,
    uint32_t    index
    )
    {
    DL_NODE   * p_node;
    cpu_pwr_t * pChild;
    cpu_pwr_t * pTarget;

    if (pName == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: name == NULL\n");       
        return NULL;
        }

    if ((!strncmp(p_cpu_node->name, pName, CPU_PWR_NODE_NAME_LEN))
        && (p_cpu_node->unit == index))
        {
        /* find it, and return */

        return p_cpu_node;
        }

    /* if this node is not match, search his child list */
    
    p_node = DLL_FIRST(&p_cpu_node->childList);
    while (p_node != NULL)
        {
        pChild = container_of(p_node, cpu_pwr_t, node);
        pTarget = cpu_pwr_node_find_by_name_ (pChild, pName, index);
        if (pTarget != NULL)
            {
            return pTarget;
            }

        p_node = DLL_NEXT(p_node);
        }

    return NULL;    
    }

cpu_pwr_t * cpu_pwr_node_find_by_name
    (
    char *  pName,
    uint32_t  index
    )
    {
    return cpu_pwr_node_find_by_name_(
                p_cpu_pwr_root_node, pName, index);
    }

kstat_t cpu_pwr_node_init_
    (
    uint32_t    level,
    char      * name,
    uint32_t    unit,
    cpu_pwr_t * p_cpu_node
    )
    {
    if (name == NULL || p_cpu_node == NULL)
        {
        return RHINO_PWR_MGMT_ERR;
        }

    memset (p_cpu_node, 0, sizeof (cpu_pwr_t));

    p_cpu_node->level = level;
    p_cpu_node->name  = name;
    p_cpu_node->unit  = unit;

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
    p_cpu_node->currentPState = CPU_PSTATE_NONE;
    p_cpu_node->supportBitsetP = 0;
#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */

    p_cpu_node->currentCState = CPU_CSTATE_NONE;
    p_cpu_node->supportBitsetC = 0;

#ifdef RHINO_CONFIG_SMP
    p_cpu_node->childLiveCpuSet = 0;
#endif /* RHINO_CONFIG_SMP */

    PWR_DBG(DBG_INFO, "init %s%d done\n",name,unit);
    
    return RHINO_SUCCESS;
    }

/*
* cpu_pwr_node_init_static will create and initialize a cpu power management node
* according the given level number, node name and node unit.
*
* This routine is normally used to create cpu power management topology tree by
* boards/platform.
*
* For example, cpu_pwr_node_init_static(2, "core", 0, p_cpu_node) will init a
* cpu power management node named "core0" in level2 of the cpu power management
* topology tree, and return the struction point address, but it do not belong
* to any other node and do not has any child at this time,
* cpu_pwr_child_add(pParent, pChild) could be used to create relationship
* with other node in the topology tree.
*
* the space of p_cpu_node is provided by caller.
*/

kstat_t cpu_pwr_node_init_static
    (
    uint32_t    level,
    char      * name,
    uint32_t    unit,
    cpu_pwr_t * p_cpu_node /* IN */
    )
    {
    if (name == NULL || p_cpu_node == NULL)
        {
        return RHINO_PWR_MGMT_ERR;
        }

    return cpu_pwr_node_init_(level, name, unit, p_cpu_node);
    }

/*
* cpu_pwr_node_init_dyn() is same as cpu_pwr_node_init_static(),
* the difference is the space of pp_cpu_node is mallocced in
* running time.
*/

kstat_t cpu_pwr_node_init_dyn
    (
    uint32_t    level,
    char      * name,
    uint32_t    unit,
    cpu_pwr_t **pp_cpu_node /* IN and OUT */
    )
    {
    cpu_pwr_t * p_cpu_node;

    if (name == NULL || pp_cpu_node == NULL)
        {
        return RHINO_PWR_MGMT_ERR;
        }
    
    p_cpu_node = (cpu_pwr_t *) malloc (sizeof (cpu_pwr_t));
    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "malloc failed\n");
        return RHINO_PWR_MGMT_ERR;
        }

    *pp_cpu_node = p_cpu_node;

    return cpu_pwr_node_init_(level, name, unit, *pp_cpu_node);
    }

/*******************************************************************************
*
* cpu_pwr_leaf_node_record - record the cpu power management node as the leaf
*                          node in the cpu power management topology tree.
*
* There are many cpu power management node on topology tree, but not all of them
* are leaf node, the leaf node means the node which do not has any children on
* topology tree.
*
* For example, there is a hardware hierarchy as below:
*
*   +-----------------------------------------------------------------+
*   |                                                                 |
*   |                             package0                            |
*   |                                                                 |
*   | +---------------------------+     +---------------------------+ |
*   | |                           |     |                           | |
*   | |           core0           |     |           core1           | |
*   | |                           |     |                           | |
*   | | +---------+   +---------+ |     | +---------+   +---------+ | |
*   | | |  SMT0   |   |   SMT1  | |     | |  SMT2   |   |   SMT3  | | |
*   | | +---------+   +---------+ |     | +---------+   +---------+ | |
*   | +---------------------------+     +---------------------------+ |
*   |                                                                 |
*   | +---------------------------+     +---------------------------+ |
*   | |                           |     |                           | |
*   | |           core2           |     |           core3           | |
*   | |                           |     |                           | |
*   | | +---------+   +---------+ |     | +---------+   +---------+ | | 
*   | | |  SMT4   |   |   SMT5  | |     | |  SMT6   |   |   SMT7  | | |
*   | | +---------+   +---------+ |     | +---------+   +---------+ | |
*   | +---------------------------+     +---------------------------+ |
*   |                                                                 |
*   +-----------------------------------------------------------------+
*
* the topology will be something as below:
*
*   package0                        -> not leaf node
*       |
*       +----core0                  -> not leaf node
*       |       |
*       |       +----SMT0           -> leaf node
*       |       |
*       |       +----SMT1           -> leaf node
*       |
*       +----core1                  -> not leaf node
*       |       |
*       |       +----SMT2           -> leaf node
*       |       |
*       |       +----SMT3           -> leaf node
*       |
*       +----core2                  -> not leaf node
*       |       |
*       |       +----SMT4           -> leaf node
*       |       |
*       |       +----SMT5           -> leaf node
*       |
*       +----core3                  -> not leaf node
*               |
*               +----SMT6           -> leaf node
*               |
*               +----SMT7           -> leaf node
*   
* Only the last level node, the SMT is the leaf node, all other nodes on
* the tree are not leaf node.
*
* The leaf node information is used to speed up the searching time,
* the other power management module in system is normally only track the
* cpu index, such as using cpu_pwr_p_state_set(cpuIndex,stateP) to put current
* cpu into stateP. If there is no leaf node information in this library,
* cpu_pwr_p_state_set(cpuIndex,stateP) will search the cpu node of current CPU
* first from root node and walk though down level by level, in the worst
* case, it will walk through all the nodes in the topology to match the target
* node.
*
* The leaf node information is setup by this way:
* a) a array named p_cpu_leaf_node_array[32] with 32 point(type is cpu_pwr_t *) is
*    created in cpu_pwr_hal_lib_init(); 
* b) all the 32 point is set to NULL in cpu_pwr_hal_lib_init();
* c) when BSP/platform build the topology, it knows which node is the leaf
*    node and the cpu index information of that cpu, when a leaf node is find,
*    BSP/platform should call cpu_pwr_leaf_node_record(p_cpu_node, cpuIndex) to save
*    the cpu_pwr_t * information of cpu indicated by cpuIndex into 
*    p_cpu_leaf_node_array[cpuIndex];
* d) though there are many cpu power management node on topology tree, only
*    the leaf node represent the real hardware execute unit, in other words
*    the number of leaf nodes on topology tree and the real CPU(execute
*    unit) number is same.
* e) each execute unit will occupy a position in p_cpu_leaf_node_array[32]
* 
* When there is requirement to find the cpu_pwr_t * according the cpuIndex,
* just return the p_cpu_leaf_node_array[cpuIndex] is fine and save much time.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_leaf_node_record
    (
    cpu_pwr_t * p_cpu_node,
    uint32_t        cpuIndex
    )
    {

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_node == NULL\n");

        return RHINO_PWR_MGMT_ERR;
        }

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                   cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_leaf_node_array[cpuIndex] != NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_leaf_node_array[%d] != NULL\n",cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    /* set the flag to a default value */

    (void)rhino_atomic_set (&p_cpu_node->stateUpdatingC, FALSE);
#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT    
    (void)rhino_atomic_set (&p_cpu_node->stateUpdatingP, FALSE);
#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */
    p_cpu_leaf_node_array[cpuIndex] = p_cpu_node;

    PWR_DBG(DBG_INFO, "%s%d connect to logic id %d done\n",
        p_cpu_node->name, p_cpu_node->unit, cpuIndex);

    return RHINO_SUCCESS;
    }

/*
* cpu_pwr_child_add will add the p_cpu_child as a child in it's childList,
* and save the p_cpu_parent in child node for quick reference.
*/

void cpu_pwr_child_add
    (
    cpu_pwr_t * p_cpu_parent,  /* parent cpu handle    */    
    cpu_pwr_t * p_cpu_child    /* child cpu handle     */
    )
    {
    DL_NODE * p_node;

    CPU_TREE_MUX_LOCK();

    p_node = DLL_FIRST(&p_cpu_parent->childList);
    while (p_node != NULL)
        {
        /* pChild is already a child of pParent*/

        if (p_node == &(p_cpu_child->node))
            {
            PWR_DBG (DBG_INFO, "warning: cpu[%s] is already a child of cpu[%s]\n",
                     p_cpu_child->name, p_cpu_parent->name);

            CPU_TREE_MUX_UNLOCK();
            return;
            }
        p_node = DLL_NEXT(p_node);
        }

    /* add child into parent's child list */

    DLL_ADD(&(p_cpu_parent->childList), &(p_cpu_child->node));

    /* set the parent info for quick reference */

    p_cpu_child->pParent = p_cpu_parent;

    PWR_DBG(DBG_INFO, "parent %s%d added child %s%d\n",
        p_cpu_parent->name,p_cpu_parent->unit, 
        p_cpu_child->name,p_cpu_child->unit);


    PWR_DBG(DBG_INFO, "%s%d in level%d has %d children\n",
        p_cpu_parent->name,p_cpu_parent->unit, p_cpu_parent->level,
        dllCount(&p_cpu_parent->childList));

    CPU_TREE_MUX_UNLOCK();
    }


/*
* cpu_pwr_child_c_state_update() will walk through all the children of given node
* and update it's C state flag to given C value.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

static kstat_t cpu_pwr_child_c_state_update
    (
    cpu_pwr_t *     p_cpu_node,
    cpu_cstate_t    cpu_c_state
    )
    {
    DL_NODE * p_node;
#ifdef CPU_PWR_HAL_DBG
    uint32_t    preStatus;    
#endif /* CPU_PWR_HAL_DBG */

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* search all the child of current cpu node */

    p_node = DLL_FIRST(&p_cpu_node->childList);    

    while (p_node != NULL)
        {
        p_cpu_node = container_of (p_node, cpu_pwr_t, node);

#ifdef CPU_PWR_HAL_DBG        
        preStatus = p_cpu_node->currentCState;
#endif /* CPU_PWR_HAL_DBG */

        /* update P state */

        p_cpu_node->currentCState = cpu_c_state;

#ifdef CPU_PWR_HAL_DBG

        PWR_DBG(DBG_INFO, "level[%d] %s%d update C state from C%d -> C%d\n",
            p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
            preStatus, p_cpu_node->currentCState);
#endif

        /* update all the child of current node if any */

        (void)cpu_pwr_child_c_state_update(p_cpu_node, cpu_c_state);

        /* move to next node and continue */

        p_node = DLL_NEXT(p_node);
        }

    return RHINO_SUCCESS;    
    }

static kstat_t cpu_pwr_c_method_set_by_level_
    (
    uint32_t               level,
    cpu_pwr_t *        p_cpu_node,
    FUNC_CPU_CSTATE_SET  cpuCStateSetFunc
    )
    {
    kstat_t    ret = RHINO_SUCCESS;
    DL_NODE * p_node;
    cpu_pwr_t *   pCpuNodeChild;

    PWR_DBG(DBG_INFO, "[%d],%s%d in level%d\n",level, 
        p_cpu_node->name,p_cpu_node->unit, p_cpu_node->level);

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->level == level)
        {
        /* bind P state set/get method into this node */

        p_cpu_node->cpuCStateSetFunc = cpuCStateSetFunc;

        /* set C state to C0 by default, is here the right place ?*/

        p_cpu_node->currentCState = CPU_CSTATE_C0;

        PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) C-state-set to 0x%08x\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->level, cpuCStateSetFunc);
        PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) currentCState to C%d\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->level, p_cpu_node->currentCState);      

        /* update C state of it's all children to default C0 */
        
        return cpu_pwr_child_c_state_update(p_cpu_node, CPU_CSTATE_C0);

        }

    CPU_TREE_MUX_LOCK();

    /* search all the child of current cpu node */

    p_node = DLL_FIRST(&p_cpu_node->childList);

    while (p_node != NULL)
        {
        pCpuNodeChild = container_of (p_node, cpu_pwr_t, node);

        ret = cpu_pwr_c_method_set_by_level_(level, pCpuNodeChild, cpuCStateSetFunc);
        if (ret == RHINO_SUCCESS)
            {
            /* move to next node and continue */

            p_node = DLL_NEXT(p_node);

            PWR_DBG(DBG_INFO, "[%d],%s%d in level%d set C RHINO_SUCCESS, next node = 0x%08x\n",level, 
                pCpuNodeChild->name,pCpuNodeChild->unit, pCpuNodeChild->level, p_node);            
            }
        else
            {
            PWR_DBG(DBG_ERR, "cpu_pwr_p_method_set_by_level_(%d,x,x,x) error\n",level);

            CPU_TREE_MUX_UNLOCK();
            return RHINO_PWR_MGMT_ERR;
            }
        }
    
    CPU_TREE_MUX_UNLOCK();

    return RHINO_SUCCESS;
    }

/*
* cpu_pwr_c_method_set_by_level() update all the node according the given level, 
* to know the method of controlling C state, normally it will be called by platform
* in board_cpu_pwr_topo_create().
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_method_set_by_level
    (
    uint32_t              level,
    FUNC_CPU_CSTATE_SET cpuCStateSetFunc
    )
    {
    return cpu_pwr_c_method_set_by_level_(
                level,
                p_cpu_pwr_root_node,
                cpuCStateSetFunc);
    }

static kstat_t cpu_pwr_c_state_set_
    (
    cpu_pwr_t *   p_cpu_node,
    cpu_cstate_t  cpu_c_state,
    int           master,
    int           all_cores_need_sync
    )
    {
    kstat_t     ret = RHINO_SUCCESS;
    uint32_t    stateCMatch = 0;
    cpu_pwr_t * pParent;
    cpu_pwr_t * pCpuNodeTmp;
    DL_NODE   * p_node;
    int         allCoresWantSameCx = TRUE;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    pParent = p_cpu_node->pParent;

    stateCMatch = (CPU_STATE_BIT(cpu_c_state) & p_cpu_node->supportBitsetC);

#ifdef CPU_PWR_HAL_DBG
    if ((p_cpu_node->cpuCStateSetFunc != NULL) && (stateCMatch == 0))
        {
        PWR_DBG(DBG_INFO, "%s%d(with supportCBitset[0x%08x]) "
            "do not support C state P%d, will ask help from parent %s%d\n",
            p_cpu_node->name, p_cpu_node->unit,
            p_cpu_node->supportBitsetC, cpu_c_state,
            pParent->name, pParent->unit);
        }
#endif /* CPU_PWR_HAL_DBG */

    /*
     * If current p_cpu_node support cpuCStateSetFunc() and match the C state
     * which it support, use it directly and update C state of all the children
     * at same time.
     */

    if ((p_cpu_node->cpuCStateSetFunc != NULL) && (stateCMatch != 0))
        {

        /*
         * cpu_c_state == CPU_CSTATE_C1, C1 state must a independent
         * state that each core support, each core could enter/exit
         * C1 state independently, so each core could update it's C1
         * state independently;
         *
         * C2/C3 or higher is dependent on platfrom/arch, some platform
         * support C2/C3 per-core, but some platform do not support.
         * all_cores_need_sync is assert here to check this information,
         * 1) all_cores_need_sync == TRUE, means that this Cx state is not
         * per-core independent, it need all cores enter/exit this Cx
         * state at same time.
         * 2) all_cores_need_sync == FALSE, means that this Cx state is
         * pre-core independent, it could enter/exit this Cx state
         * independently.
         *
         * When all_cores_need_sync is TRUE, we leave the responsibility
         * to master(means the last core enter idle mode) to update
         * all cores C state of Cx(C2/C3 or higher).
         */

        if ((cpu_c_state == CPU_CSTATE_C1) ||
            ((all_cores_need_sync == TRUE) && (master == TRUE))
            )
            {
            /* 
             * The new C state should be recorded before calling the real C
             * control method, the reason is when C0->C1~Cn, the CPU will stop
             * to execute the instruction.
             */
            
            /* update the current C state */
            
            p_cpu_node->currentCState = cpu_c_state;

            /* hold input C state into desireCState */
            
            p_cpu_node->desireCState = cpu_c_state; 
            
            /* broadcast the current C state to all the children */
            
            ret = cpu_pwr_child_c_state_update(p_cpu_node, cpu_c_state);
            
            if (ret != RHINO_SUCCESS)
                {
                PWR_DBG(DBG_ERR, "%s%d -> boardcase state C%d to children failed\n",
                p_cpu_node->name, p_cpu_node->unit, cpu_c_state);
                return RHINO_PWR_MGMT_ERR;
                }
        
            PWR_DBG(DBG_INFO, "%s%d is going to C%d\n",
                p_cpu_node->name, p_cpu_node->unit, cpu_c_state);
            }

        /* call the real C idle method */

        ret = p_cpu_node->cpuCStateSetFunc(cpu_c_state, master);

        if (ret != RHINO_SUCCESS)
            {
            PWR_DBG(DBG_ERR, "%s%d -> cpuCStateSetFunc(P%d) failed\n",
            p_cpu_node->name, p_cpu_node->unit, cpu_c_state);
            return RHINO_PWR_MGMT_ERR;
            }

        return RHINO_SUCCESS;
        }

    /* 
     * if we
     * 1) - do not support cpuCStateSetFunc()
     *    or 
     * 2) - the request C state is not support, for example, the current
     *      level only support C0~C1, C2~C3 is controlled by higher level.
     *
     * ask parent for help if any.
     */

    if ((pParent != NULL) && 
        (pParent != &cpu_pwr_root_node))
        {
        /* 
         * if come here, which means the Cx state of current node level
         * is not independent, it need coordination of all cores of
         * current level, when all cores reach an agreement that they
         * want to enter same Cx state, then the master(last core enter
         * Cx idle state) verify this request and take final action
         * to close door to enter the real Cx state.
         */

        all_cores_need_sync = TRUE;
        
        p_cpu_node->desireCState  = cpu_c_state;

        if (master == TRUE)
            {
            /*
             * it is responsibility of master(means the last core
             * enter idle mode) check/verify to see if all the
             * cores in this level want to enter same Cx state.
             */

            p_node = DLL_FIRST(&pParent->childList);

            while (p_node != NULL)
                {
                pCpuNodeTmp = container_of (p_node, cpu_pwr_t, node);
                if (cpu_c_state != pCpuNodeTmp->desireCState)
                    {
                    allCoresWantSameCx = FALSE;
                    break;
                    }

                /* move to next node and continue */

                p_node = DLL_NEXT(p_node);            
                }

            if (allCoresWantSameCx == TRUE)
                {
                /* 
                 * if all the cores under this pParent want to
                 * enter same Cx state, lets do it, note master
                 * has value of TRUE here.
                 */ 

                return cpu_pwr_c_state_set_(pParent, cpu_c_state,
                                                 master, all_cores_need_sync);
                }
            }
        else
            {
            /*
             * if all the cores under this parent has not reached
             * an agreement about Cx state(C2/C3 or higher) or current
             * core is not master, just call cpu_pwr_c_state_set_()
             * with master(FALSE).
             *
             * on ARM, the none-master core normally will execute a simple
             * WFI through cpu_pwr_c_state_set_(), but we leave Cx
             * as C0 still here, all the cores will tagged as C2/C3 or
             * higher when master core enter C2/C3.
             */

            return cpu_pwr_c_state_set_(pParent, cpu_c_state,
                                             master, all_cores_need_sync);
            }
        }

    PWR_DBG(DBG_INFO, "p_cpu_node(%s%d) has no parent\n",
                          p_cpu_node->name, p_cpu_node->unit);

    return RHINO_SUCCESS;    
    }

/*******************************************************************************
*
* cpu_pwr_c_state_get - get C state of CPU indicated by cpuIndex
*
* This routine will get C state of CPU indicated by cpuIndex, the C state will
* return in pCpuCStatus.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_state_get
    (
    uint32_t   cpuIndex,
    cpu_cstate_t * pCpuCState
    )
    {
    cpu_pwr_t *  p_cpu_node;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    * pCpuCState = p_cpu_node->currentCState;

    return RHINO_SUCCESS;
    }

/*******************************************************************************
*
* cpu_pwr_c_state_set - put CPU into given C state
*
* This routine will put CPU into given C state, under SMP environment,
* cpuIndex is fetched locally from cpu_index_get which implies that 
* this function is always applied to the current core.
* This function has to be called on the CPU for which the C state is being set.
* the caller has to ensure that the code will be executed on the same core
* (such as called by task with CPU affinity or from ISR context).
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_state_set
    (
    cpu_cstate_t       targetCpuCState
    )
    {
    uint32_t        cpuIdx;
    cpu_pwr_t *   p_cpu_node;
    kstat_t         ret = RHINO_SUCCESS;
    cpu_pwr_t *   pParent = NULL;
#ifdef RHINO_CONFIG_SMP    
    uint32_t        stateCMatch = 0;
#endif /* RHINO_CONFIG_SMP */
    int            master;
#ifdef RHINO_CONFIG_SMP
    uint32_t        cpuNums = CPUS_NUM_MAX();
    uint32_t        cpuIdxTmp;
    cpu_pwr_t *   pCpuNodeTmp;
    cpuset_t        targetCpuSet = 0;
    cpuIdx          = cpu_index_get ();
#else /* RHINO_CONFIG_SMP */
    cpuIdx          = 0;
#endif

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIdx];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, 
            "did not find p_cpu_node in p_cpu_leaf_node_array with cpuIndex[%d]\n",
            cpuIdx);

        return RHINO_PWR_MGMT_ERR;
        }
    
    if (p_cpu_node->currentCState == targetCpuCState)
        {
        return RHINO_SUCCESS;
        }

    pParent = p_cpu_node->pParent;

    /*
     * at any time for one CPU, only one C state updating process is allowed,
     * when a C state updating is going on, it set stateUpdatingC to 1,
     * if the other thread(a task or interrupt) try to start other C state
     * updating, it will encount stateUpdatingC == 1, and should
     * return immediately.
     *
     * a exception should be consider, reschedule()->windCpuPwrDown()->
     * cpu_pwr_c_state_set(CPU_CSTATE_C1(C2,C3,...)) will make
     * CPU stopping execute instruction and never return, which will
     * leave stateUpdatingC as TRUE and never be cleaned to FALSE.
     *
     * when cpu_pwr_c_state_set(CPU_CSTATE_C0) is called again
     * to make C state changed from C1(C2,C3,...) to C0, we check
     * the value of stateUpdatingC, if it is TURE, which implies that
     * stateUpdatingC is not cleaned in previous state change(from C0
     * to C1,C2,C3,...), stateUpdatingC will be forced to FALSE.
     */

    if (targetCpuCState == CPU_CSTATE_C0
        && p_cpu_node->currentCState != CPU_CSTATE_C0
        && rhino_atomic_get (&p_cpu_node->stateUpdatingC) == TRUE)
        {
        (void)rhino_atomic_set (&p_cpu_node->stateUpdatingC, FALSE);
        }

    if (rhino_atomic_cas (&p_cpu_node->stateUpdatingC, FALSE, TRUE))
        {

        if (targetCpuCState == CPU_CSTATE_C0)
            {

            /* 
             * cpu_c_state == CPU_CSTATE_C0, which means current call
             * stack is:
             * intEnt() -> windCpuPwrUp() -> cpu_pwr_c_state_set(C0)
             * what we need to do here is set currentCState/desireCState
             * to C0.
             * For the first core which wakeup there maybe need some
             * prepare work need to do, so the master(the first one
             * open door) should be distinguished here.
             */

            CPU_TREE_SPIN_TAKE();

#ifdef RHINO_CONFIG_SMP

            /* 
             * If the childLiveCpuSet of parent is zero here,
             * it must be the first core wake up. If the childLiveCpuSet
             * of parent is not zero, it must be the subsequent core which
             * waked up after first core.
             */

            if (CPUSET_IS_ZERO(pParent->childLiveCpuSet))
                {
                master = TRUE;
                }
            else
                {
                master = FALSE;
                }

            /* update the live bit set of parent */

            cpu_pwr_live_cpubit_set(p_cpu_node);

#else /* RHINO_CONFIG_SMP */

            /* 
             * for UP, we are always the master, because there is 
             * only one core in system.
             */

            master = TRUE;

#endif /* !RHINO_CONFIG_SMP */

            CPU_TREE_SPIN_GIVE();

            /*
             * master == TRUE, means we are the first one wakeup,
             * a liite more work need to do than other cores here
             * is update all the parent to C0 state. And send IPI
             * to other cores to update it's self Cx state when
             * wake up from C2 or high Cx state(C3/C4...).
             */

            if (master == TRUE)
                {
                /* call the real C control method */

                if (p_cpu_node->cpuCStateSetFunc != NULL)
                    {

                    /* 
                     * Note, the master value is TRUE here, 
                     * cpuCStateSetFunc() could take proper action
                     * based on this value, because we are the first
                     * core wake up.
                     */

                    ret = p_cpu_node->cpuCStateSetFunc(targetCpuCState, master);
                    if (ret != RHINO_SUCCESS)
                        {
                        PWR_DBG(DBG_ERR, "%s%d -> cpuCStateSetFunc(P%d) "
                            "failed\n",p_cpu_node->name,
                            p_cpu_node->unit, targetCpuCState);
                        ret = RHINO_PWR_MGMT_ERR;
                        }
                    }

#ifdef RHINO_CONFIG_SMP

                /*
                 * if CPU is wake up from C2 or high Cx state(C3/C4...),
                 * the other cores state will still leave in old C2 or
                 * high Cx state(C3/C4...), when you call cpu_pwr_topo_show()
                 * maybe we will get following result:
                 *
                 * level[01] - cpu-package0 - C0
                 *     level[02] - cpu-cluster0 - C0 - P2
                 *         level[03] - cpu-core0 - C0
                 *         level[03] - cpu-core1 - C0
                 *         level[03] - cpu-core2 - C2
                 *         level[03] - cpu-core3 - C2
                 *
                 * core0/core1 has the right Cx state C0, but core2/core3 still
                 * leave in C2 through all cores will enter/exit C2(C3,C4...)
                 * at same time, this is not correct, to make sure the state of
                 * all cores is correct, we send a IPI to all other cores under
                 * same cluster to force other cores will go through this path:
                 * intEnt() -> windCpuPwrUp() -> cpu_pwr_c_state_set(C0)
                 * to update its Cx state and p_cpu_node->stateUpdatingC into
                 * right value.
                 *
                 * The reason for not updating directly the C state of other cores
                 * is: cpu_pwr_c_state_set() is designed to called on calling core
                 * itself, for example cpu_pwr_c_state_set() called on core0
                 * should not impact/modify the Cx state of core1, and vice versa.
                 * With this principle, we should avoid to update the Cx state
                 * of other cores, instead we send IPI to other cores to make
                 * sure other core will execute cpu_pwr_c_state_set() to determine
                 * and update the Cx state and p_cpu_node->stateUpdatingC itself.
                 *
                 * Some platform/arch support per-core independent C2/C3 or
                 * higher C state, in this case, there is no need to touch
                 * (means send IPI to)other cores.
                 */

                stateCMatch = (CPU_STATE_BIT(p_cpu_node->currentCState) & 
                               p_cpu_node->supportBitsetC);

                if ((p_cpu_node->currentCState >= CPU_CSTATE_C2) &&
                    (stateCMatch == 0))
                    {

                    /*
                     * force update all other cores under same cluster
                     * (has same parent) to C0.
                     */

                    for (cpuIdxTmp = 0; cpuIdxTmp < cpuNums; cpuIdxTmp++)
                        {
                        pCpuNodeTmp = p_cpu_leaf_node_array[cpuIdxTmp];
                        if ((cpuIdxTmp != cpuIdx) && 
                            (pCpuNodeTmp->pParent == p_cpu_node->pParent))
                            {
                            CPUSET_SET(targetCpuSet, cpuIdxTmp);
                            }
                        }

                    send_ipi_to(ipi_intr_type, targetCpuSet);
                    }

#endif /* RHINO_CONFIG_SMP */

                /* update C state to C0 */

                p_cpu_node->currentCState = CPU_CSTATE_C0;
                p_cpu_node->desireCState  = CPU_CSTATE_C0;                

                }
            else /* master is FALSE */
                {
                /*
                 * If come here, we must be subsequent core wake up
                 * after first core, what we need to do is updating
                 * self state to C0 and call function if need,
                 * normmally cpuCStateSetFunc() will do nothing here.
                 */

                if (p_cpu_node->cpuCStateSetFunc != NULL)
                    {
                    ret = p_cpu_node->cpuCStateSetFunc(targetCpuCState, master);
                    if (ret != RHINO_SUCCESS)
                        {
                        PWR_DBG(DBG_ERR, "%s%d -> cpuCStateSetFunc(P%d) "
                            "failed\n", p_cpu_node->name,
                            p_cpu_node->unit, targetCpuCState);

                        ret = RHINO_PWR_MGMT_ERR;
                        }
                    }
 
                p_cpu_node->currentCState = CPU_CSTATE_C0;
                p_cpu_node->desireCState  = CPU_CSTATE_C0;

                }

            /* also update all the parent to C0 when any child enter C0 */

            pParent = p_cpu_node->pParent;
            while (pParent != NULL && pParent != &cpu_pwr_root_node)
                {
                /* update C state to C0 */

                CPU_TREE_SPIN_TAKE();

                pParent->currentCState = CPU_CSTATE_C0;
                pParent->desireCState  = CPU_CSTATE_C0;

                CPU_TREE_SPIN_GIVE();

                pParent = pParent->pParent;
                }            
            }
        /* cpu_c_state != CPU_CSTATE_C0 */
        else
            {

            /* 
             * if come here, cpu_c_state must is not CPU_CSTATE_C0,
             * it must be C1 or high C2/C3..., it must be going
             * to idle Cx mode.
             */

            CPU_TREE_SPIN_TAKE();

#ifdef RHINO_CONFIG_SMP

            /* clear the live bit set of parent first */
            
            cpu_pwr_live_cpubit_clear (p_cpu_node);

            /*
             * If the childLiveCpuSet of parent is zero here,
             * it must be master core(the last one)going to idle.
             * If the childLiveCpuSet of parent is not zero, it
             * must not be the last one going to idle.
             */

            if (CPUSET_IS_ZERO(pParent->childLiveCpuSet))
                {
                master = TRUE;
                }
            else
                {
                master = FALSE;
                }

#else /* RHINO_CONFIG_SMP */

            /* 
             * for UP, we are always the master, because there is 
             * only one core in system.
             */

            master = TRUE;

#endif /* !RHINO_CONFIG_SMP */

            CPU_TREE_SPIN_GIVE();

            ret = cpu_pwr_c_state_set_(p_cpu_node, targetCpuCState,
                                            master, FALSE);
            }

        /* 
         * after C state change completed, it should set stateUpdatingC
         * to 0 to allow C state updating again.
         *
         * a exception should be consider, reschedule()->windCpuPwrDown()->
         * cpu_pwr_c_state_set(CPU_CSTATE_C1(C2,C3,...)) will make
         * CPU stopping execute instruction and never return, which will
         * lead this line never be executed and leave stateUpdatingC as TRUE.
         *
         * when cpu_pwr_c_state_set(CPU_CSTATE_C0) is called again
         * to make C state changed from C1(C2,C3,...) to C0, we check
         * the value of stateUpdatingC, if it is TURE, which implies that
         * stateUpdatingC is not cleaned in previous state change(from C0
         * to C1,C2,C3,...), stateUpdatingC will be forced to FALSE.
         */

        (void)rhino_atomic_set (&p_cpu_node->stateUpdatingC, FALSE);

        }

    return ret;
    }

static kstat_t cpu_pwr_c_state_capability_set_
    (
    cpu_pwr_t *     p_cpu_node,
    int             cpuIndex,
    uint32_t        supportBitsetC
    )
    {
    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* 
     * if this level support setting the P state, it implies that it
     * maintain the supportBitsetP value.
     */

    if (p_cpu_node->cpuCStateSetFunc != NULL) 
        {
        p_cpu_node->supportBitsetC = supportBitsetC;

        PWR_DBG(DBG_INFO, "level %d, p_cpu_node(%s%d) supportBitsetC = 0x%016llx\n",
            p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
            p_cpu_node->supportBitsetC);

        return RHINO_SUCCESS;
        }

    /* 
     * if this level do not support setting the P state, it implies that it
     * does not maintain the supportBitsetP value, it should ask parent for
     * this value, also the parent will continue to ask it's parent for help
     * if need.
     */
    
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_c_state_capability_set_(p_cpu_node->pParent,
                                       cpuIndex, supportBitsetC);
        }

    /* 
     * In the way from leaf cpu node to root node, one node must save the
     * info of supportBitsetC, or it is a bug.
     */

    PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: level %d, p_cpu_node(%s%d) "
        "supportBitsetC = 0x%016llx\n",
        p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
        p_cpu_node->supportBitsetC);

    return RHINO_PWR_MGMT_ERR;    
    }

static kstat_t cpu_pwr_c_state_capability_get_
    (
    cpu_pwr_t *   p_cpu_node,
    int             cpuIndex,
    uint32_t* pSupportBitsetC
    )
    {
    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* 
     * if this level support setting the C state, it implies that it
     * maintain the supportBitsetC value.
     */

    if (p_cpu_node->cpuCStateSetFunc != NULL) 
        {
        *pSupportBitsetC |= p_cpu_node->supportBitsetC;

        PWR_DBG(DBG_INFO, "level %d, p_cpu_node(%s%d) supportBitsetC = 0x%016llx\n",
            p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
            p_cpu_node->supportBitsetC);

        /* do not return, continue check if any support C state from parent */
        }

    /* 
     * if this level do not support setting the C state, it implies that it
     * does not maintain the supportBitsetC value, it should ask parent for
     * this value, also the parent will continue to ask it's parent for help
     * if need.
     */

    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_c_state_capability_get_(p_cpu_node->pParent,
                                       cpuIndex, pSupportBitsetC);
        }

    return RHINO_SUCCESS;    
    }

/*
* cpu_pwr_c_state_capability_set() will set the C state supported of given CPU.
* There is two way to set the C state of CPU which is supplied by this library,
* one is according cpu index, other is according cpu level.
*
* If all the cpu node in same level support same C state, user can use
* cpu_pwr_c_state_capability_set_by_level(level, supportBitsetC) to set all
* nodes in same level at one time.
* If different CPU support different C state, user can use this routine to set
* support C state for each cpu individually.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_state_capability_set
    (
    uint32_t        cpuIndex,
    uint32_t  supportBitsetC
    )
    {
    cpu_pwr_t *   p_cpu_node;
    kstat_t         ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_c_state_capability_set_(p_cpu_node, cpuIndex, supportBitsetC);
    
    CPU_TREE_MUX_UNLOCK();

    return ret;    
    }

/*
* cpu_pwr_c_state_capability_get() will return the all C state supported of given CPU.
* 
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_state_capability_get
    (
    uint32_t    cpuIndex,
    uint32_t  * pSupportBitsetC
    )
    {
    cpu_pwr_t * p_cpu_node;
    kstat_t     ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    /* bzero return value first */

    * pSupportBitsetC = 0;

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_c_state_capability_get_(p_cpu_node, cpuIndex, pSupportBitsetC);
    
    CPU_TREE_MUX_UNLOCK();

    PWR_DBG(DBG_INFO, "%s%d C support bit set 0x%08x\n",
        p_cpu_node->name, p_cpu_node->unit, * pSupportBitsetC);

    return ret;    
    }

static kstat_t cpu_pwr_c_state_capability_set_by_level_
    (
    uint32_t              level,
    cpu_pwr_t *       p_cpu_node,
    uint32_t      supportBitsetC
    )
    {
    kstat_t    ret = RHINO_SUCCESS;
    DL_NODE * p_node;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->level == level)
        {
        /* 
         * supportBitsetC value is only maintained by node which
         * support setting the C state. The struction element
         * supportBitsetC in node which do not support cpuCStateSetFunc()
         * has no meaning.
         */

        if (p_cpu_node->cpuCStateSetFunc != NULL)
            {
            p_cpu_node->supportBitsetC = supportBitsetC;

            PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) supportBitsetC "
                              "to 0x%08x\n", p_cpu_node->name, p_cpu_node->unit,
                              p_cpu_node->level, p_cpu_node->supportBitsetC);

            return RHINO_SUCCESS;
            }
        else
            {
            PWR_DBG(DBG_ERR, "try to set supportBitsetC[0x%08x] for %s%d in level %d "
                             "which do not support cpuCStateSetFunc()\n",
                             supportBitsetC, p_cpu_node->name, 
                             p_cpu_node->unit, p_cpu_node->level);
            return RHINO_PWR_MGMT_ERR;
            }
        }    

    /* search all the child of current cpu node */

    p_node = DLL_FIRST(&p_cpu_node->childList);

    while (p_node != NULL)
        {
        p_cpu_node = (cpu_pwr_t *)container_of (p_node, cpu_pwr_t, node);

        ret = cpu_pwr_c_state_capability_set_by_level_(level, p_cpu_node,
                                                supportBitsetC);
        if (ret == RHINO_SUCCESS)
            {
            /* move to next node and continue */

            p_node = DLL_NEXT(p_node);
            }
        else
            {
            PWR_DBG(DBG_ERR, "execute error, level[%d], %s%d, "
                             "supportBitsetC[0x%08x]\n",level,
                             p_cpu_node->name, p_cpu_node->unit, supportBitsetC);

            return RHINO_PWR_MGMT_ERR;
            }
        }

    return RHINO_SUCCESS;
    }

/*
* cpu_pwr_c_state_capability_set_by_level() will set the C state supported
* of all CPU node in given level number.
*
* There is two way to set the C state of CPU which is supplied by this library,
* one is according cpu index, other is according cpu level.
*
* If all the cpu node in same level support same C state, user can use this
* routine to set all nodes in same level at one time.
* If different CPU support different C state, user can use 
* cpu_pwr_c_state_capability_set(cpuIndex, supportBitsetP) to set support C state
* for each cpu individually.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_state_capability_set_by_level
    (
    uint32_t level,
    uint32_t supportBitsetC
    )
    {
    return cpu_pwr_c_state_capability_set_by_level_(level,
              p_cpu_pwr_root_node,supportBitsetC);
    }

static kstat_t cpu_pwr_c_state_latency_save_
    (
    cpu_pwr_t *     p_cpu_node,
    cpu_cstate_t    cpuState, /* C state */
    uint32_t        latency
    )
    {
    int       i;
    size_t    spaceSize = 0;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /*
     * Note that comparing enums like this is technically
     * not valid. There is some ambiguity because while enums are
     * intended to be represented by integers, it's up to the
     * compiler to decide whether they should be signed or unsigned.
     * In this case, we happen to know that the cpu_cstate_t is defined
     * to only use unsigned integer values, so we can temporarily
     * get away with using a cast here to silence the compiler
     * warning, but this test should probably be removed.
     */

    for (i = 0; i <= CPU_CSTATE_MAX; i++)
        {
        if (cpuState == CPU_CSTATE_C0 + i)
            break;
        }

    if (i > CPU_CSTATE_MAX)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: cpuState(%d) > CPU_CSTATE_MAX(%d)\n",
            cpuState, CPU_PSTATE_MAX);
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->cpuCStateSetFunc != NULL)
        {
        /* how many C state is supportted */

        if (p_cpu_node->stateNumC == 0)
            {
            p_cpu_node->stateNumC = num_of_bit_one_get(p_cpu_node->supportBitsetC);

            if (p_cpu_node->stateNumC == 0)
                {
                PWR_DBG(DBG_ERR, "%s%d->supportBitsetC(%d), stateNumC() = 0\n",
                                     p_cpu_node->name, p_cpu_node->unit,
                                     p_cpu_node->supportBitsetC);

                return RHINO_PWR_MGMT_ERR;
                }
            }
            
        /* malloc the space to save frequency info */

        if (p_cpu_node->pPairLatency == NULL)
            {
            spaceSize = sizeof(state_val_pair_t) * p_cpu_node->stateNumC;
            p_cpu_node->pPairLatency = (state_val_pair_t *)malloc(spaceSize);   

            if (p_cpu_node->pPairLatency == NULL)
                {
                PWR_DBG(DBG_ERR, "zalloc failed\n");

                return RHINO_PWR_MGMT_ERR;
                }

            /* clean space */

            memset(p_cpu_node->pPairLatency, 0, spaceSize);

            /* init value to init value CPU_LATENCY_UNKNOW */
            for (i = 0; i < p_cpu_node->stateNumC; i++)
                {
                p_cpu_node->pPairLatency[i].value = (uint32_t)CPU_LATENCY_UNKNOW;
                }       
            
            }

        /* if we support this C state, save the corresponding latency info */

        if (CPU_STATE_BIT(cpuState) & p_cpu_node->supportBitsetC)
            {

            /* check if info has been saved before */

            for (i = 0; i < p_cpu_node->stateNumC; i++)
                {
                if ((p_cpu_node->pPairLatency[i].state == cpuState)&&
                    (p_cpu_node->pPairLatency[i].value == latency))
                    {
                    return RHINO_SUCCESS;
                    }
                }

            for (i = 0; i < p_cpu_node->stateNumC; i++)
                {
                /* search a free space on pPairLatency to save info */

                if (p_cpu_node->pPairLatency[i].value == (uint32_t)CPU_LATENCY_UNKNOW)
                    {
                    p_cpu_node->pPairLatency[i].state = cpuState;
                    p_cpu_node->pPairLatency[i].value = latency;

                    return RHINO_SUCCESS;
                    }
                }

            /* if search free space failed */
            
            if (i == p_cpu_node->stateNumC)
                {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d) search free space failed\n",
                    p_cpu_node->name, p_cpu_node->unit);                
                return RHINO_PWR_MGMT_ERR;
                }
            }
        }
    
    /* 
     * if come here, there is two possible case:
     * 1) this node do not support cpuCStateSetFunc
     *    or
     * 2) this node support cpuCStateSetFunc, but the given C state cpuState
     *    is not supported
     * what need to do here is ask parent for help if any.
     */
        
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_c_state_latency_save_(p_cpu_node->pParent, cpuState, latency);
        }
    
    PWR_DBG(DBG_INFO, "p_cpu_node(%s%d) has no parent\n",
        p_cpu_node->name, p_cpu_node->unit);
    
    return RHINO_SUCCESS;  
    }

/*
* cpu_pwr_c_state_latency_save() save the cpu latency information of C state,
* it will let cpu topology system to know how long when the CPU wake up from
* C state, normally it will be called by platform in board_cpu_pwr_topo_create().
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_c_state_latency_save
    (
    uint32_t    cpuIndex,   
    cpu_cstate_t cpuState,
    uint32_t    latency
    )
    {
    cpu_pwr_t *  p_cpu_node;
    kstat_t         ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);
        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_c_state_latency_save_(p_cpu_node, cpuState, latency);

    CPU_TREE_MUX_UNLOCK();

    return ret;
    }

static uint32_t cpu_pwr_c_state_latency_get_
    (
    cpu_pwr_t *   p_cpu_node,
    cpu_cstate_t  CState
    )
    {
    int i;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_node is NULL\n", p_cpu_node);

        return (uint32_t)CPU_LATENCY_UNKNOW;
        }

    /* 
     * if we support cpuCStateSetFunc, then we should save the CPU latency
     * in initialization stage already.
     */

    if (p_cpu_node->cpuCStateSetFunc != NULL)
        {

        /* 
         * if the query C state is supported, then we must has the CPU
         * latency info, or it is a bug which is something such as 
         * pPairLatency is is not initialized in the boot stage.
         */

        if (CPU_STATE_BIT(CState) & p_cpu_node->supportBitsetC)
            {
            if (p_cpu_node->pPairLatency != NULL)
                {
                for (i = 0; i < p_cpu_node->stateNumC; i++)
                    {
                    if (CState == p_cpu_node->pPairLatency[i].state)
                        {
                        return p_cpu_node->pPairLatency[i].value;
                        }
                    }
                }
            else
                {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d)->pPStateAttrArray is not "
                                    "initialized\n",
                                    p_cpu_node->name, p_cpu_node->unit);

                return (uint32_t)CPU_LATENCY_UNKNOW;
                }
            }
        }

    /* if come here, ask parent for help */

    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_c_state_latency_get_(p_cpu_node->pParent, CState);
        }
  
    return (uint32_t)CPU_LATENCY_UNKNOW;
    }

/*
* This routine gets the Cstate latency of a logical CPU for the specified 
* Cstate. Cstate latency is the time when CPU wake up from idle/sleep state,
* normally this time is a fixed value which is decided by hardware design and
* electrical characteristics. The deeper idle/sleep state CPU enter, the longer
* latency time is need when CPU wakeup.
*
* Tickless module will use latency info to evaluate which C state should enter,
* for example, if latency C1 is 5 ticks, latency C2 is 10 ticks, than if there
* is nothing to do for the system in the feature 10 ticks, system would enter C2
* state and wake up after 10 ticks.
*
* RETURNS: Cstate latency(microsecond) of a logical CPU for specified Cstate
* or CPU_LATENCY_UNKNOW when failed.
*/

uint32_t cpu_pwr_c_state_latency_get
    (
    uint32_t  cpuIndex,
    cpu_cstate_t CState
    )
    {
    cpu_pwr_t *  p_cpu_node;
    
    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return (uint32_t)CPU_LATENCY_UNKNOW;
        }

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    return cpu_pwr_c_state_latency_get_(p_cpu_node, CState);
    }

#ifdef RHINO_CONFIG_SMP

static void cpu_pwr_child_index_mask_set_
    (
    cpu_pwr_t *   p_cpu_node,
    uint32_t          level,
    uint32_t          childIndexMask
    )
    {
    DL_NODE     * p_node;
    cpu_pwr_t * pChild;

    if (p_cpu_node == NULL)
        {     
        return;
        }
    
    p_node = DLL_FIRST(&p_cpu_node->childList);
    while (p_node != NULL)
        {
        pChild = container_of(p_node, cpu_pwr_t, node);
        if (pChild->level == level)
            {
            pChild->childIndexMask = childIndexMask;
            }
        cpu_pwr_child_index_mask_set_(pChild,level,childIndexMask);

        p_node = DLL_NEXT(p_node);
        }

    return;    
    }

/*******************************************************************************
*
* cpu_pwr_child_index_mask_set - set the mask information
*
* For example, 
*
* package0                       -> level1
*    |
*    +----cluster0               -> level2
*    |       |
*    |       +----core0          -> level3
*    |       |
*    |       +----core1          -> level3
*    |       |
*    |       +----core2          -> level3
*    |       |
*    |       +----core3          -> level3
*    |
*    +----cluster1               -> level2
*            |
*            +----core4          -> level3
*            |
*            +----core5          -> level3
*            |
*            +----core6          -> level3
*            |
*            +----core7          -> level3
*
* cluster0 belongs to level2 and it has four child(core0,core1,core2,core3)
* cluster1 belongs to level2 and it has four child(core4,core5,core6,core7)
* So both cluster0 and cluster1 should has childIndexMask of 0x3.
* When cpu_pwr_live_cpubit_clear(core4) is called, we could know the first
* bit of cluster1 should be cleared, through this way:
*
* core4->unit(which is 4[0b100]) & childIndexMask of cluster1(which
* is 0x3[0b11]) = 0x0.
*
* and so on,
*
* core6->unit(which is 6[0b110]) & childIndexMask of cluster1(which
* is 0x3[0b11]) = 0x2.
*
* core3->unit(which is 3[0b011]) & childIndexMask of cluster0(which
* is 0x3[0b11]) = 0x3.
*/

void cpu_pwr_child_index_mask_set
    (
    uint32_t  level,
    uint32_t  childIndexMask
    )
    {
    cpu_pwr_child_index_mask_set_(
            p_cpu_pwr_root_node,
            level,
            childIndexMask);
    }

/*******************************************************************************
*
* cpu_pwr_live_bit_set_init_ - help function of cpu_pwr_live_bit_set_init.
* it will set the relative bit of p_cpu_parent->childLiveCpuSet according the
* p_cpu_child->unit, it also respect the p_cpu_parent->childIndexMask.
*/

static void cpu_pwr_live_bit_set_init_
    (
    cpu_pwr_t * p_cpu_parent,  /* parent cpu handle    */    
    cpu_pwr_t * p_cpu_child    /* child cpu handle     */
    )    
    {
    uint32_t        childIdx;

    if ((p_cpu_parent == NULL) || (p_cpu_child == NULL))
        {
        return;
        }

    childIdx = p_cpu_parent->childIndexMask & p_cpu_child->unit;

    /* record live bit set of this child in parent's childLiveCpuSet */

    CPUSET_ATOMIC_SET (p_cpu_parent->childLiveCpuSet, childIdx);
    }

/*
* cpu_pwr_live_bit_set_init - initialize the live bit of all node in the
* cpu topology tree to a definitive state, means all cores/cluster/package
* are in live state.
*
* It is normally called in initialization stage of cpu topology.
*/

static void cpu_pwr_live_bit_set_init (void)
    {
    cpu_pwr_t * pChild;
    cpu_pwr_t * pParent;
    uint32_t        cpuIdx;    
    uint32_t        cpuNums;
#ifdef RHINO_CONFIG_SMP    
    cpuNums       = CPUS_NUM_MAX();
#else
    cpuNums       = 1;
#endif

    for (cpuIdx = 0; cpuIdx < cpuNums; cpuIdx++)
        {
        pChild = p_cpu_leaf_node_array[cpuIdx];
        if (pChild == NULL)
            {
            PWR_DBG(DBG_ERR, "pChild[%d] == NULL\n",cpuIdx);

            return;
            }

        pParent = pChild->pParent;

        if ((pParent != NULL) && (pParent != p_cpu_pwr_root_node))
            {
            cpu_pwr_live_bit_set_init_(pParent, pChild);
            }
        }
    }

/*
* cpu_pwr_live_cpubit_set - set the live bit of parent's childLiveCpuSet
* This function is used to set the live bit of parent's childLiveCpuSet,
* it will continue set the relative bit of grandfather's childLiveCpuSet,
* and so on. Because any core wake up, the parent must be in wake up/live
* state, it is normally called after core wake up.
*
* childLiveCpuSet, which is used to indicate the live state of all
* children of this node. If we are the leaf node on the topo tree, then
* childLiveCpuSet has no meanning. 
*
* childLiveCpuSet will be initialized to indicated that all the children
* of this node is in live state, when any child is going to idle state,
* the relative bit of liveCpuSet is cleared.
*
* childLiveCpuSet could be in hierarchy, the child of current node could
* be the parent of others, only when all the children clear the bit in
* childLiveCpuSet of parent, then the parent could clear the bit in
* childLiveCpuSet of grandfather.
*
* if the highest node in topo tree has childLiveCpuSet with value zero(0),
* it means all the cores in system are in idle state.
*/

static void cpu_pwr_live_cpubit_set
    (
    cpu_pwr_t *   pChild
    )
    {
    uint32_t          childIdx;
    cpu_pwr_t *   pParent = pChild->pParent;

    while ((pParent != NULL) && 
           (pParent != p_cpu_pwr_root_node))
        {
        childIdx = pParent->childIndexMask & pChild->unit;

        CPUSET_ATOMIC_SET (pParent->childLiveCpuSet, childIdx);

        /* broadcast relative live bit to upper level */

        pParent = pParent->pParent;
        }
    }

/*
* cpu_pwr_live_cpubit_clear() is used to clear the live bit of parent's
* childLiveCpuSet, also when parent's childLiveCpuSet is zero, it will
* continue clear the relative bit of grandfather's childLiveCpuSet, and so on.
* It is normally called before core is going to idle.
*/

static void cpu_pwr_live_cpubit_clear
    (
    cpu_pwr_t *   pChild
    )
    {
    uint32_t          childIdx;
    cpu_pwr_t *   pParent = pChild->pParent;

    while ((pParent != NULL) && 
           (pParent != p_cpu_pwr_root_node))
        {
        childIdx = pParent->childIndexMask & pChild->unit;

        CPUSET_ATOMIC_CLR (pParent->childLiveCpuSet, childIdx);

        /*
         * if all the child is idle, clear the relative live
         * bit to upper level.
         */

        if (CPUSET_IS_ZERO(pParent->childLiveCpuSet))
            {
            pParent = pParent->pParent;
            }
        else
            {
            break;
            }
        }

    }

#endif /* RHINO_CONFIG_SMP */

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT

/*
* cpu_pwr_child_p_state_update() will walk through all the children of given node
* and update it's P state flag to given P value.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

static kstat_t cpu_pwr_child_p_state_update
    (
    cpu_pwr_t *     p_cpu_node,
    cpu_pstate_t    cpuPState
    )
    {
    DL_NODE * p_node;
#ifdef CPU_PWR_HAL_DBG
    uint32_t    preStatus;    
#endif /* PWR_DBG */

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* search all the child of current cpu node */

    p_node = DLL_FIRST(&p_cpu_node->childList);    

    while (p_node != NULL)
        {
        p_cpu_node = container_of (p_node, cpu_pwr_t, node);

#ifdef CPU_PWR_HAL_DBG        
        preStatus = p_cpu_node->currentPState;
#endif /* CPU_PWR_HAL_DBG */

        /* update P state */

        p_cpu_node->currentPState = cpuPState;

        /* also update desirePState at same time */

        p_cpu_node->desirePState = cpuPState;

#ifdef CPU_PWR_HAL_DBG

        PWR_DBG(DBG_INFO, "level[%d] %s%d update P state from P%d -> P%d\n",
            p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
            preStatus, p_cpu_node->currentPState);
#endif /* CPU_PWR_HAL_DBG */

        /* update all the child of current node if any */

        (void) cpu_pwr_child_p_state_update(p_cpu_node, cpuPState);

        /* move to next node and continue */

        p_node = DLL_NEXT(p_node);
        }

    return RHINO_SUCCESS;    
    }

static kstat_t cpu_pwr_p_method_set_by_level_
    (
    uint32_t               level,
    cpu_pwr_t *        p_cpu_node,
    FUNC_CPU_PSTATE_SET  cpuPStateSetFunc
    )
    {
    kstat_t    ret = RHINO_SUCCESS;
    DL_NODE * p_node;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->level == level)
        {
        /* bind P state set/get method into this node */

        p_cpu_node->cpuPStateSetFunc = cpuPStateSetFunc;

        /* set P state to P0 by default, is here the right place ?*/

        p_cpu_node->currentPState = CPU_PSTATE_P0;

        PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) P-state-set to 0x%08x\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->level, p_cpu_node->cpuPStateSetFunc);
        PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) currentPState to P%d\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->level, p_cpu_node->currentPState);

        /* update P state of it's all children to default P0 */

        return cpu_pwr_child_p_state_update(p_cpu_node, CPU_PSTATE_P0);

        }    

    CPU_TREE_MUX_LOCK();

    /* search all the child of current cpu node */

    p_node = DLL_FIRST(&p_cpu_node->childList);

    while (p_node != NULL)
        {
        p_cpu_node = (cpu_pwr_t *)container_of (p_node, cpu_pwr_t, node);

        ret = cpu_pwr_p_method_set_by_level_(level, p_cpu_node, cpuPStateSetFunc);
        if (ret == RHINO_SUCCESS)
            {
            /* move to next node and continue */

            p_node = DLL_NEXT(p_node);
            }
        else
            {
            PWR_DBG(DBG_ERR, "cpu_pwr_p_method_set_by_level_(%d,x,x,x) error\n",level);

            CPU_TREE_MUX_UNLOCK();
            return RHINO_PWR_MGMT_ERR;
            }
        }
    
    CPU_TREE_MUX_UNLOCK();

    return RHINO_SUCCESS;
    }

/*
* cpu_pwr_p_method_set_by_level() update all the node according the given level,
* to know the method of controlling P state, normally it will be called by
* platform in board_cpu_pwr_topo_create().
*/

kstat_t cpu_pwr_p_method_set_by_level
    (
    uint32_t              level,   
    FUNC_CPU_PSTATE_SET cpuPStateSetFunc
    )
    {
    return cpu_pwr_p_method_set_by_level_(
                level,
                p_cpu_pwr_root_node,
                cpuPStateSetFunc);
    }

static kstat_t cpu_pwr_p_state_set_
    (
    cpu_pwr_t *   p_cpu_node,
    cpu_pstate_t       cpuPState
    )
    {
    kstat_t      ret = RHINO_SUCCESS;
    uint32_t     statePMatch = 0;
    cpu_pstate_t targetPState;
    cpu_pwr_t  * pParent;
    cpu_pwr_t  * pCpuNodeTmp;
    DL_NODE    * p_node;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    pParent = p_cpu_node->pParent;

    /* verify if current node(p_cpu_node) support the request P state */

    statePMatch = (CPU_STATE_BIT(cpuPState) & p_cpu_node->supportBitsetP);

#ifdef CPU_PWR_HAL_DBG
    if ((p_cpu_node->cpuPStateSetFunc != NULL) && (statePMatch == 0))
        {
        PWR_DBG(DBG_INFO, "%s%d(with supportPBitset[0x%08x]) "
            "do not support P state P%d, will ask help from parent %s%d\n",
            p_cpu_node->name, p_cpu_node->unit,
            p_cpu_node->supportBitsetP, cpuPState,
            pParent->name, pParent->unit);
        }
#endif /* CPU_PWR_HAL_DBG */

    /*
     * If current p_cpu_node support cpuPStateSetFunc() and match the P state
     * which it support, use it directly and update P state of all the children
     * at same time.
     */

    if ((p_cpu_node->cpuPStateSetFunc != NULL) && (statePMatch != 0))
        {
        ret = p_cpu_node->cpuPStateSetFunc(p_cpu_node, cpuPState);

        if (ret != RHINO_SUCCESS)
            {
            PWR_DBG(DBG_ERR, "%s%d -> cpuPStateSetFunc(P%d) failed\n",
            p_cpu_node->name, p_cpu_node->unit, cpuPState);
            return RHINO_PWR_MGMT_ERR;
            }

        /* update the current P state */
        
        p_cpu_node->currentPState = cpuPState;

        /*
         * desirePState should be updated at same time,
         * if not, it will lead a chance to reverse the P state
         * by following logic section when system running with SMP.
         *
         * if (pCpuNodeTmp->desirePState < targetPState)
         *     {
         *     targetPState = pCpuNodeTmp->desirePState;
         *     }
         */

        p_cpu_node->desirePState = cpuPState;

        /* broadcast the current P state to all the children */

        ret = cpu_pwr_child_p_state_update(p_cpu_node, cpuPState);

        if (ret != RHINO_SUCCESS)
            {
            PWR_DBG(DBG_ERR, "%s%d -> boardcase state P%d to children failed\n",
            p_cpu_node->name, p_cpu_node->unit, cpuPState);
            return RHINO_PWR_MGMT_ERR;
            }
        
        return RHINO_SUCCESS;
        }

    /* 
     * if come here, it means:
     * 1) - do not support cpuPStateSetFunc()
     *    or 
     * 2) - the request P state is not support
     * ask parent for help if any.
     */
    
    if ((pParent != NULL) && (pParent != &cpu_pwr_root_node))
        {
        if (p_cpu_node->cpuPStateSetFunc == NULL)
            {

            /*
             * If cpuPStateSetFunc is NULL, it implies that P state is not
             * controlled in this level, it must be controlled by it's parent
             * or higher level, which means that P state will cross many
             * execute unit at same time. But the cpu utilization ratio is
             * calculated on execute unit within CPU monitor system, in SMP
             * environment we don't want to update the overall P state every
             * time the CPU monitor detects a CPU utilization change. The
             * result would be confusing. Instead, we initiate a P state
             * change only when certain criteria have been met.
             * For now, we set the global P state to the best available
             * P state common to all cores. If all cores go to P3, then
             * we can put the system in P3. If one core transitions above
             * P3 (e.g. cpu 0 returns to P0), then we transition the system
             * to P0.
             */ 

            /* hold the P state into desirePState */

            p_cpu_node->desirePState = cpuPState;

            /* calculate the best available P state in this level */

            targetPState = CPU_PSTATE_MAX;

            p_node = DLL_FIRST(&pParent->childList);    

            while (p_node != NULL)
                {
                pCpuNodeTmp = container_of (p_node, cpu_pwr_t, node);

                /* 
                 * skip the execute unit which is in none-executing state,
                 * some execute unit have nothing to do after boot and will
                 * enter C1 state, after these execute units enter C1, it's
                 * P state(currentPState and desirePState) still leave as P0,
                 * though the P state has no meaning to execute unit at
                 * this time, if we do not skip this type node, then the
                 * logic below to get targetPState will always return P0 and
                 * will block CPU going to lower power performance mode.
                 */

                if ((pCpuNodeTmp->currentCState != CPU_CSTATE_C0) &&
                    (pCpuNodeTmp != p_cpu_node))
                    {
                    /* move to next node and continue */

                    p_node = DLL_NEXT(p_node);

                    continue;
                    }

                /* 
                 * use the best desired performance state as the available
                 * P state of this level.
                 */

                if (pCpuNodeTmp->desirePState < targetPState)
                    {
                    targetPState = pCpuNodeTmp->desirePState;
                    }

                /* move to next node and continue */

                p_node = DLL_NEXT(p_node);               
                }

            /* 
             * if the P state of parent is not same with targetPState and 
             * targetPState is valid, set it now.
             */

            if ((pParent->currentPState != targetPState) && 
                (targetPState != CPU_PSTATE_MAX))
                {
                return cpu_pwr_p_state_set_(pParent, targetPState);
                }
            else
                {
                return RHINO_SUCCESS;
                }
            }
        else
            {
            return cpu_pwr_p_state_set_(pParent, cpuPState);     
            }
        }

    PWR_DBG(DBG_INFO, "p_cpu_node(%s%d) has no parent\n",
        p_cpu_node->name, p_cpu_node->unit);

    return RHINO_SUCCESS;    
    }

/*
* cpu_pwr_p_state_get() will get P state of CPU indicated by cpuIndex,
* the P state will return in pCpuPState.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_p_state_get
    (
    uint32_t      cpuIndex,
    cpu_pstate_t * pCpuPState
    )
    {
    cpu_pwr_t *  p_cpu_node;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    * pCpuPState = p_cpu_node->currentPState;

    return RHINO_SUCCESS;
    }


/*
* cpu_pwr_p_state_set() will put CPU(indicated by cpuIndex) into given P state.
*
* Under SMP environment, it is possible that this function is called
* on a core that is not same as the target core(indicated by cpuIndex),
* some architecture require that one core could only operate its own
* register to change P state, such as Intel, pentiumMsrSet() only take
* effect to current core.
*
* Normally call stack is: 
* cpu_pwr_p_state_set() -> cpu_pwr_p_state_set_() -> cpuPStateSetFunc()
* cpuPStateSetFunc is supplied by platform, it is responsibility
* of cpuPStateSetFunc itself to make sure cpuPStateSetFunc change the
* P state of target core which is indicated by cpuIndex.
*
* cpu_pwr_c_state_set() is somehow different, cpu_pwr_c_state_set is always
* by core itself to enter C state, so there is no argument cupIndex of
* cpu_pwr_c_state_set().
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_p_state_set
    (
    uint32_t          cpuIndex,
    cpu_pstate_t       cpuPState
    )
    {
    cpu_pwr_t *   p_cpu_node;
    kstat_t          ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->currentPState == cpuPState)
        return RHINO_SUCCESS;

    /*
     * at any time for one CPU, only one P state updating is allowed,
     * when a P state updating is going on, it set stateUpdatingP to 1,
     * if the other thread(a task or interrupt) try to start other C state
     * updating, it will encount stateUpdatingP == 1, and should
     * return immediately.
     */

    if (rhino_atomic_cas (&p_cpu_node->stateUpdatingP, FALSE, TRUE))
        {
        ret = cpu_pwr_p_state_set_(p_cpu_node, cpuPState);

        /* 
         * after P state change completed, it should set stateUpdatingP
         * to 0 to allow other P state updating again.
         */

        (void)rhino_atomic_set (&p_cpu_node->stateUpdatingP, FALSE);
        }

    return ret;
    }

static kstat_t cpu_pwr_cur_p_state_update_directly_
    (
    cpu_pwr_t *   p_cpu_node,
    int             cpuIndex,
    cpu_pstate_t       cpuPState
    )
    {
    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* 
     * if we has cpuPStateSetFunc, it implies that we should 
     * maintain the currentPState.
     */

    if (p_cpu_node->cpuPStateSetFunc) 
        {
        p_cpu_node->currentPState = cpuPState;
        return RHINO_SUCCESS;
        }

    /* 
     * if we do not has cpuPStateSetFunc, ask parent for help.
     */
    
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_cur_p_state_update_directly_(p_cpu_node->pParent,
                                              cpuIndex, cpuPState);
        }

    PWR_DBG(DBG_INFO, "p_cpu_node(%s%d) has no parent\n",
        p_cpu_node->name, p_cpu_node->unit);

    return RHINO_SUCCESS;    
    }

/*
* cpu_pwr_cur_p_state_update_directly() will update P state of given CPU diretcly.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_cur_p_state_update_directly
    (
    uint32_t          cpuIndex,
    cpu_pstate_t       cpuPState
    )
    {
    cpu_pwr_t *   p_cpu_node;
    kstat_t          ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->currentPState == cpuPState)
        return RHINO_SUCCESS;

    /*
     * at any time for one CPU, only one P state updating is allowed,
     * when a P state updating is going on, it set stateUpdatingC to 1,
     * if the other thread(a task or interrupt) try to start other P state
     * updating, it will encount stateUpdatingC == 1, and should
     * return immediately.
     */

    if (rhino_atomic_cas (&p_cpu_node->stateUpdatingP, FALSE, TRUE))
        {
        ret = cpu_pwr_cur_p_state_update_directly_(p_cpu_node, cpuIndex, cpuPState);

        /* 
         * after C state change completed, it should set stateUpdatingC
         * to 0 to allow C state updating again.
         */

        (void)rhino_atomic_set (&p_cpu_node->stateUpdatingP, FALSE);
        }
    else
        {
        /*PWR_DBG(DBG_INFO, "[exit]cpu%d -> C%d\n",cpuIndex,cpuPState);*/
        }

    return ret;
    }

static kstat_t cpu_pwr_p_state_capability_set_
    (
    cpu_pwr_t *   p_cpu_node,
    int             cpuIndex,
    uint32_t  supportBitsetP
    )
    {
    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* 
     * if this level support setting the P state, it implies that it
     * maintain the supportBitsetP value.
     */

    if (p_cpu_node->cpuPStateSetFunc != NULL) 
        {
        p_cpu_node->supportBitsetP = supportBitsetP;

        PWR_DBG(DBG_INFO, "level %d, p_cpu_node(%s%d) supportBitsetP = 0x%08x\n",
            p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
            p_cpu_node->supportBitsetP);

        return RHINO_SUCCESS;
        }

    /* 
     * if this level do not support setting the P state, it implies that it
     * does not maintain the supportBitsetP value, it should ask parent to
     * save this value if parent support setting P state, if the parent
     * do not support setting the P state, it will continue to ask upper 
     * parent for help if need.
     */
    
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_p_state_capability_set_(p_cpu_node->pParent,
                                       cpuIndex, supportBitsetP);
        }

    /* 
     * In the way from leaf cpu node to root node, one node must save the
     * info of supportBitsetP, or it is a bug.
     */

    PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: level %d, p_cpu_node(%s%d) "
                        "supportBitsetP = 0x%016llx\n",
                         p_cpu_node->level, p_cpu_node->name,
                         p_cpu_node->unit, p_cpu_node->supportBitsetP);

    return RHINO_PWR_MGMT_ERR;    
    }

static kstat_t cpu_pwr_p_state_capability_get_
    (
    cpu_pwr_t * p_cpu_node,
    int         cpuIndex,
    uint32_t  * pSupportBitsetP
    )
    {
    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    /* 
     * if this level support setting the P state, it implies that it
     * maintain the supportBitsetP value.
     */

    if (p_cpu_node->cpuPStateSetFunc != NULL) 
        {
        *pSupportBitsetP |= p_cpu_node->supportBitsetP;

        /* do not return, continue check if any support P state from parent */
        }
    
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_p_state_capability_get_(p_cpu_node->pParent,
                                          cpuIndex, pSupportBitsetP);
        }

    return RHINO_SUCCESS;    
    }


/*
* This routine will set the P state supported of given CPU.
* There is two way to set the P state of CPU which is supplied by this library,
* one is according cpu index, other is according cpu level.
*
* If all the cpu node in same level support same P state, user can use
* cpu_pwr_p_state_capability_set_by_level(level, supportBitsetP) to set all nodes in
* same level at one time.
* If different CPU support different P state, user can use this routine to set
* support P state for each cpu individually.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_p_state_capability_set
    (
    uint32_t  cpuIndex,
    uint32_t  supportBitsetP
    )
    {
    cpu_pwr_t * p_cpu_node;
    kstat_t     ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
            cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, 
            "did not find p_cpu_node in p_cpu_leaf_node_array with cpuIndex[%d]\n",
            cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_p_state_capability_set_(p_cpu_node, cpuIndex, supportBitsetP);
    
    CPU_TREE_MUX_UNLOCK();

    return ret;    
    }

/*
* cpu_pwr_p_state_capability_get() will return the all P state supported
* of given CPU.
* 
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_p_state_capability_get
    (
    uint32_t           cpuIndex,
    uint32_t * pSupportBitsetP
    )
    {
    cpu_pwr_t *  p_cpu_node;
    kstat_t         ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                         "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    /* bzero return value first */

    * pSupportBitsetP = 0;

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_p_state_capability_get_(p_cpu_node, cpuIndex, pSupportBitsetP);
    
    CPU_TREE_MUX_UNLOCK();

    return ret;    
    }

static kstat_t cpu_pwr_p_state_capability_set_by_level_
    (
    uint32_t              level,
    cpu_pwr_t *       p_cpu_node,
    uint32_t      supportBitsetP
    )
    {
    kstat_t    ret = RHINO_SUCCESS;
    DL_NODE * p_node;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->level == level)
        {
        /* 
         * supportBitsetP value is only maintained by node which
         * support setting the P state. The struction element
         * supportBitsetP in node which do not support cpuPStateSetFunc()
         * has no meaning.
         */

        if (p_cpu_node->cpuPStateSetFunc != NULL)
            {
            p_cpu_node->supportBitsetP = supportBitsetP;

            PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) supportBitsetP "
                                 "to 0x%08x\n", p_cpu_node->name, p_cpu_node->unit,
                                 p_cpu_node->level, p_cpu_node->supportBitsetP);

            return RHINO_SUCCESS;
            }
        else
            {
            PWR_DBG(DBG_ERR, "try to set supportBitsetP[0x%08x] for %s%d "
                      "in level %d which do not support cpuPStateSetFunc()\n",
                       supportBitsetP, p_cpu_node->name, 
                       p_cpu_node->unit, p_cpu_node->level);
            return RHINO_PWR_MGMT_ERR;
            }        
        }    

    /* search all the child of current cpu node */

    p_node = DLL_FIRST(&p_cpu_node->childList);

    while (p_node != NULL)
        {
        p_cpu_node = (cpu_pwr_t *)container_of (p_node, cpu_pwr_t, node);

        ret = cpu_pwr_p_state_capability_set_by_level_(level, p_cpu_node,
                                                supportBitsetP);
        if (ret == RHINO_SUCCESS)
            {
            /* move to next node and continue */

            p_node = DLL_NEXT(p_node);
            }
        else
            {
            PWR_DBG(DBG_ERR, "execute error, level[%d], %s%d, "
                             "supportBitsetP[0x%08x]\n",level,
                             p_cpu_node->name, p_cpu_node->unit, supportBitsetP);

            return RHINO_PWR_MGMT_ERR;
            }
        }

    return RHINO_SUCCESS;
    }


/*
* cpu_pwr_p_state_capability_set_by_level() will set the P state supported
* of all CPU node in given level number. There is two way to set the P state
* of CPU which is supplied by this library, one is according cpu index, the
* other is according cpu level.
*
* If all the cpu node in same level support same P state, user can use this
* routine to set all nodes in same level at one time. If different CPU support
* different P state, user can use:
*
*   cpu_pwr_p_state_capability_set(cpuIndex, supportBitsetP)
*
* to set support P state for each cpu individually.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_p_state_capability_set_by_level
    (
    uint32_t         level,
    uint32_t supportBitsetP
    )
    {
    return cpu_pwr_p_state_capability_set_by_level_(level,
                p_cpu_pwr_root_node, supportBitsetP);
    }

static kstat_t cpu_pwr_freq_save_
    (
    cpu_pwr_t * p_cpu_node,
    uint32_t    cpuState, /* P state */
    uint32_t    cpuFreq
    )
    {
    int       i;
    size_t    spaceSize = 0;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    if (cpuState > CPU_PSTATE_MAX)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: cpuState(%d) > CPU_PSTATE_MAX(%d)\n",
            cpuState, CPU_PSTATE_MAX);
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->cpuPStateSetFunc != NULL)
        {
        /* how many P state is supportted */

        if (p_cpu_node->stateNumP == 0)
            {
            p_cpu_node->stateNumP = num_of_bit_one_get(p_cpu_node->supportBitsetP);

            if (p_cpu_node->stateNumP == 0)
                {
                PWR_DBG(DBG_ERR, "%s%d->supportBitsetP(%d), statePNum() = 0\n",
                                     p_cpu_node->name, p_cpu_node->unit,
                                     p_cpu_node->supportBitsetP);
                return RHINO_PWR_MGMT_ERR;
                }
            }
            
        /* malloc the space to save frequency info */

        if (p_cpu_node->pPairFreq == NULL)
            {
            spaceSize = sizeof(state_val_pair_t) * p_cpu_node->stateNumP;
            p_cpu_node->pPairFreq = (state_val_pair_t *)malloc(spaceSize);   

            if (p_cpu_node->pPairFreq == NULL)
                {
                PWR_DBG(DBG_ERR, "zalloc failed\n");

                return RHINO_PWR_MGMT_ERR;
                }

            /* clean space */

            memset(p_cpu_node->pPairFreq, 0, spaceSize);
            }

        /* if we support this P state, save the corresponding frequency info */

        if (CPU_STATE_BIT(cpuState) & p_cpu_node->supportBitsetP)
            {

            /* check if info has been saved before */

            for (i = 0; i < p_cpu_node->stateNumP; i++)
                {
                if ((p_cpu_node->pPairFreq[i].state == cpuState)&&
                    (p_cpu_node->pPairFreq[i].value == cpuFreq))
                    {
                    return RHINO_SUCCESS;
                    }
                }

            for (i = 0; i < p_cpu_node->stateNumP; i++)
                {
                /* search a free space on pPairFreq to save info */

                if (p_cpu_node->pPairFreq[i].value == 0)
                    {
                    p_cpu_node->pPairFreq[i].state = cpuState;
                    p_cpu_node->pPairFreq[i].value = cpuFreq;

                    return RHINO_SUCCESS;
                    }
                }

            /* if search free space failed */
            
            if (i == p_cpu_node->stateNumP)
                {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d) search free space failed\n",
                    p_cpu_node->name, p_cpu_node->unit);                
                return RHINO_PWR_MGMT_ERR;
                }
            }
        }
    
    /* 
     * if come here, there is two possible case:
     * 1) this node do not support cpuPStateSetFunc
     *    or
     * 2) this node support cpuPStateSetFunc, but the given P state cpuState
     *    is not supported
     * what need to do here is ask parent for help if any.
     */
        
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_freq_save_(p_cpu_node->pParent, cpuState, cpuFreq);
        }
    
    PWR_DBG(DBG_INFO, "p_cpu_node(%s%d) has no parent\n",
        p_cpu_node->name, p_cpu_node->unit);
    
    return RHINO_SUCCESS;  
    }

/*
* cpu_pwr_freq_save() saves cpu frequency information of P state, it will let
* cpu topology system to know cpu frequency information of P state.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_freq_save
    (
    uint32_t    cpuIndex,   
    cpu_pstate_t cpuState,
    uint32_t    cpuFreq   /* kHz */
    )
    {
    cpu_pwr_t *  p_cpu_node;
    kstat_t         ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);
        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_freq_save_(p_cpu_node, cpuState, cpuFreq);

    CPU_TREE_MUX_UNLOCK();

    return ret;
    }

/*
* cpu_pwr_freq_get_by_p_state() return CPU frequency of given P state.
* RETURNS: CPU frequency or CPU_FREQ_UNKNOW when failed.
*/

uint32_t cpu_pwr_freq_get_by_p_state
    (
    cpu_pwr_t *   p_cpu_node,
    cpu_pstate_t       PState
    )
    {
    int i;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_node is NULL\n", p_cpu_node);

        return (uint32_t)CPU_FREQ_UNKNOW;
        }

    /* 
     * if we support cpuPStateSetFunc, then we should save the CPU frequency
     * in initialization stage already.
     */

    if (p_cpu_node->cpuPStateSetFunc)
        {

        /* 
         * if the query P state is supported, then we must has the CPU
         * frequency info, or it is a bug which is something such as 
         * pPStateAttrArray is is not initialized in the boot stage.
         */

        if (CPU_STATE_BIT(PState) & p_cpu_node->supportBitsetP)
            {
            if (p_cpu_node->pPairFreq != NULL)
                {
                for (i = 0; i < p_cpu_node->stateNumP; i++)
                    {
                    if (PState == p_cpu_node->pPairFreq[i].state)
                        {
                        return p_cpu_node->pPairFreq[i].value;
                        }
                    }
                }
            else
                {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d)->pPStateAttrArray is not "
                                    "initialized\n",
                                    p_cpu_node->name, p_cpu_node->unit);

                return (uint32_t)CPU_FREQ_UNKNOW;
                }
            }
        }

    /* if come here, ask parent for help */

    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_freq_get_by_p_state(p_cpu_node->pParent, PState);
        }
  
    return (uint32_t)CPU_FREQ_UNKNOW;
    }

static kstat_t cpu_pwr_volt_save_
    (
    cpu_pwr_t *   p_cpu_node,
    cpu_pstate_t       cpuState, /* P state */
    uint32_t          cpuVolt
    )
    {
    int       i;
    size_t    spaceSize = 0;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: p_cpu_node == NULL\n");
        return RHINO_PWR_MGMT_ERR;
        }

    if (cpuState > CPU_PSTATE_MAX)
        {
        PWR_DBG(DBG_ERR, "RHINO_PWR_MGMT_ERR: cpuState(%d) > CPU_PSTATE_MAX(%d)\n",
            cpuState, CPU_PSTATE_MAX);
        return RHINO_PWR_MGMT_ERR;
        }

    if (p_cpu_node->cpuPStateSetFunc != NULL)
        {
        /* how many P state is supportted */
        
        if (p_cpu_node->stateNumP == 0)
            {
            p_cpu_node->stateNumP = num_of_bit_one_get(p_cpu_node->supportBitsetP);

            if (p_cpu_node->stateNumP == 0)
                {
                PWR_DBG(DBG_ERR, "%s%d->supportBitsetP(%d), statePNum() = 0\n",
                                     p_cpu_node->name, p_cpu_node->unit,
                                     p_cpu_node->supportBitsetP);
                return RHINO_PWR_MGMT_ERR;
                }
            }

        /* malloc the space to save frequency info */

        if (p_cpu_node->pPairVolt == NULL)
            {
            spaceSize = sizeof(state_val_pair_t) * p_cpu_node->stateNumP;
            p_cpu_node->pPairVolt = (state_val_pair_t *)malloc(spaceSize);   

            if (p_cpu_node->pPairVolt == NULL)
                {
                PWR_DBG(DBG_ERR, "malloc failed\n");
                return RHINO_PWR_MGMT_ERR;
                }

            /* clean the space */

            memset(p_cpu_node->pPairVolt, 0, spaceSize);
            }

        /* if we support this P state, save the corresponding voltage info */

        if (CPU_STATE_BIT(cpuState) & p_cpu_node->supportBitsetP)
            {
            /* check if info has been saved before */

            for (i = 0; i < p_cpu_node->stateNumP; i++)
                {
                if ((p_cpu_node->pPairVolt[i].state == cpuState)&&
                    (p_cpu_node->pPairVolt[i].value == cpuVolt))
                    {
                    return RHINO_SUCCESS;
                    }
                }

            for (i = 0; i < p_cpu_node->stateNumP; i++)
                {
                /* search a free space on pPairVolt to save info */

                if (p_cpu_node->pPairVolt[i].value == 0)
                    {
                    p_cpu_node->pPairVolt[i].state = cpuState;
                    p_cpu_node->pPairVolt[i].value = cpuVolt;
                    
                    return RHINO_SUCCESS;
                    }
                }

            /* if search free space failed */
            
            if (i == p_cpu_node->stateNumP)
                {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d) search free space failed\n",
                    p_cpu_node->name, p_cpu_node->unit);                
                return RHINO_PWR_MGMT_ERR;
                }            
            }
        }
    
    /* 
     * if come here, there is two possible case:
     * 1) this node do not support cpuPStateSetFunc
     *    or
     * 2) this node support cpuPStateSetFunc, but the given P state cpuState
     *    is not supported
     * what need to do here is ask parent for help if any.
     */
        
    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_volt_save_(p_cpu_node->pParent, cpuState, cpuVolt);
        }
    
    PWR_DBG(DBG_INFO, "p_cpu_node(%s%d) has no parent\n",
        p_cpu_node->name, p_cpu_node->unit);
    
    return RHINO_SUCCESS;  
    }

/*
* cpu_pwr_volt_save() cpu voltage information of P state, it will let
* cpu topology system to know cpu voltage information of P state.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

kstat_t cpu_pwr_volt_save
    (
    uint32_t     cpuIndex,   
    cpu_pstate_t  cpuState,
    uint32_t     cpuVolt   /* uV */
    )
    {
    cpu_pwr_t *  p_cpu_node;
    kstat_t         ret = RHINO_SUCCESS;

    if (cpuIndex >= CPUS_NUM_MAX)
        {
        PWR_DBG(DBG_ERR, "cpuIndex(%d) error, it should be 0 ~ %d\n",
                             cpuIndex, CPUS_NUM_MAX - 1);

        return RHINO_PWR_MGMT_ERR;
        }

    /* fetch the p_cpu_node directly from p_cpu_leaf_node_array according cpuIndex */

    p_cpu_node = p_cpu_leaf_node_array[cpuIndex];

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "did not find p_cpu_node in p_cpu_leaf_node_array "
                            "with cpuIndex[%d]\n", cpuIndex);

        return RHINO_PWR_MGMT_ERR;
        }

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_volt_save_(p_cpu_node, cpuState, cpuVolt);

    CPU_TREE_MUX_UNLOCK();

    return ret;
    }

/*
* cpu_pwr_volt_get_by_p_state() return the CPU voltage of given P state.
* 
* RETURNS: CPU voltage or CPU_VOLT_UNKNOW when failed.
*/

uint32_t cpu_pwr_volt_get_by_p_state
    (
    cpu_pwr_t *   p_cpu_node,
    cpu_pstate_t       PState
    )
    {
    int i;

    if (p_cpu_node == NULL)
        {
        PWR_DBG(DBG_ERR, "p_cpu_node is NULL\n", p_cpu_node);

        return (uint32_t)CPU_VOLT_UNKNOW;
        }

    /* 
     * if we support cpuPStateSetFunc, then we should save the CPU frequency
     * in initialization stage already.
     */

    if (p_cpu_node->cpuPStateSetFunc != NULL)
        {

        /* 
         * if the query P state is supported, then we must has the CPU
         * frequency info, or it is a bug which is something such as 
         * pPStateAttrArray is is not initialized in the boot stage.
         */

        if (CPU_STATE_BIT(PState) & p_cpu_node->supportBitsetP)
            {
            if (p_cpu_node->pPairVolt != NULL)
                {
                for (i = 0; i < p_cpu_node->stateNumP; i++)
                    {
                    if (PState == p_cpu_node->pPairVolt[i].state)
                        return p_cpu_node->pPairVolt[i].value;
                    }
                }
            else
                {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d)->pPStateAttrArray is not "
                                    "initialized\n",
                                    p_cpu_node->name, p_cpu_node->unit);

                return (uint32_t)CPU_VOLT_UNKNOW;
                }
            }
        }

    /* if come here, ask parent for help */

    if (p_cpu_node->pParent != NULL)
        {
        return cpu_pwr_volt_get_by_p_state(p_cpu_node->pParent, PState);
        }
    
    return (uint32_t)CPU_VOLT_UNKNOW;
    }

#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */

#ifdef RHINO_CONFIG_SMP

/* cpu_pwr_send_ipi_to_all() is used to triger all cores in need. */

void cpu_pwr_send_ipi_to_all(void)
    {
    /* TO DO */
    }

#endif /* RHINO_CONFIG_SMP */

/*
* cpu_pwr_hal_lib_init() is called to initialize the cpu hardware
* abstraction layer infrastructure, such as constructing the
* root node which is used as mount point of any other CPU node
* in topology tree, constructing the semphore.
*
* In the end of this routine, it will execute a function named
* board_cpu_pwr_topo_create() which is provided by board/platform
* to constructe the CPU topology tree according the CPU hierarchy info.
*
* Different board has different the CPU hierarchy info. For example,
* a ARM board looks like:
*
* +-----------------------------------------------------------------+
* |                                                                 |
* |                             package0                            |
* |                                                                 |
* | +---------------------------+     +---------------------------+ |
* | |                           |     |                           | |
* | |          cluster0         |     |          cluster1         | |
* | |                           |     |                           | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | |
* | | |  core0  |   |  core1  | |     | |  core0  |   |  core1  | | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | |
* | |                           |     |                           | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | | 
* | | |  core2  |   |  core3  | |     | |  core2  |   |  core3  | | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | |
* | +---------------------------+     +---------------------------+ |
* |                                                                 |
* +-----------------------------------------------------------------+
*
* a x86 board looks like:
*
* +-----------------------------------------------------------------+
* |                                                                 |
* |                             package0                            |
* |                                                                 |
* | +---------------------------+     +---------------------------+ |
* | |                           |     |                           | |
* | |           core0           |     |           core1           | |
* | |                           |     |                           | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | |
* | | |  SMT0   |   |   SMT1  | |     | |  SMT2   |   |   SMT3  | | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | |
* | +---------------------------+     +---------------------------+ |
* |                                                                 |
* | +---------------------------+     +---------------------------+ |
* | |                           |     |                           | |
* | |           core2           |     |           core3           | |
* | |                           |     |                           | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | | 
* | | |  SMT4   |   |   SMT5  | |     | |  SMT6   |   |   SMT7  | | |
* | | +---------+   +---------+ |     | +---------+   +---------+ | |
* | +---------------------------+     +---------------------------+ |
* |                                                                 |
* +-----------------------------------------------------------------+
*
* Though the level name is not same between ARM and x86, but they represent
* the same concept, the CPU is organized in level.
*
* For ARM, the CPU topology can be expressed/abstracted as below:
* 
* package0                        -> level1
*     |
*     +----cluster0               -> level2
*     |       |
*     |       +----core0          -> level3
*     |       |
*     |       +----core1          -> level3
*     |       |
*     |       +----core2          -> level3
*     |       |
*     |       +----core3          -> level3
*     |
*     +----cluster1               -> level2
*             |
*             +----core4          -> level3
*             |
*             +----core5          -> level3
*             |
*             +----core6          -> level3
*             |
*             +----core7          -> level3
* 
* For x86, the CPU topology can be expressed/abstracted as below according 
* the example above:
* 
* package0                        -> level1
*     |
*     +----core0                  -> level2
*     |       |
*     |       +----SMT0           -> level3
*     |       |
*     |       +----SMT1           -> level3
*     |
*     +----core1                  -> level2
*     |       |
*     |       +----SMT2           -> level3
*     |       |
*     |       +----SMT3           -> level3
*     |
*     +----core2                  -> level2
*     |       |
*     |       +----SMT4           -> level3
*     |       |
*     |       +----SMT5           -> level3
*     |
*     +----core3                  -> level2
*             |
*             +----SMT6           -> level3
*             |
*             +----SMT7           -> level3
*
*/

void cpu_pwr_hal_lib_init (void)
    {
    /* clean the space */
    
    memset(p_cpu_pwr_root_node,    0, sizeof(cpu_pwr_t));
    memset(p_cpu_leaf_node_array,  0, sizeof(cpu_pwr_t *) * CPUS_NUM_MAX);

    /* cpu_pwr_root_node is highest level on system, it's level number is 0 */

    p_cpu_pwr_root_node->level = CPU_PWR_TOPO_LEVEL_ROOT;
    p_cpu_pwr_root_node->name  = CPU_PWR_ROOT_NODE_NAME;

    /* only one root cpu in system, it's unit will always be 0 */

    p_cpu_pwr_root_node->unit = 0;
    
#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
    p_cpu_pwr_root_node->currentPState = CPU_PSTATE_NONE;
#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */
    p_cpu_pwr_root_node->currentCState = CPU_CSTATE_NONE;

    DLL_INIT(&p_cpu_pwr_root_node->childList);

    if (krhino_mutex_create (&cpu_pwr_hal_mux, "cpu_pwr_hal_mux")
        != RHINO_SUCCESS)
        {
        PWR_DBG(DBG_ERR, "krhino_mutex_create(cpu_pwr_hal_mux) failed!\n");
        return;
        }

#ifdef RHINO_CONFIG_SMP

    /* initialize the spinlock */

    krhino_spin_init (&cpu_pwr_hal_spin);
#endif

    /* call board function to create CPU power management topology tree */

    board_cpu_pwr_topo_create();

#ifdef RHINO_CONFIG_SMP

    cpu_pwr_live_bit_set_init ();

#endif

    /* 
     * set CPU default idle mode to sleep mode,
     * if tickless configuration is enabled,
     * the idle mode will be updated to tickless mode by tickless module.
     */

    (void)cpu_pwr_idle_mode_set (CPU_IDLE_MODE_SLEEP);

    PWR_DBG(DBG_INFO, "cpu_pwr_hal_lib_init() done!\n");
    }

/* num_of_bit_one_get() return the number of 1 for the given binary value. */

static int num_of_bit_one_get 
    (
    uint32_t n
    )
    {     
    int count = 0;  
      
    while(n != 0)
        {  
        n = n & (n-1);  
          
        count++;  
        }
    
    return count; 
    }

//#endif /* RHINO_CONFIG_CPU_PWR_MGMT */
