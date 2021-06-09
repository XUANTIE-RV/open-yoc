/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

/*
* This file provides cpu_pwr_topology_show()/cpu_pwr_state_show() to show
* information for CPU power management.
*/

#include "k_api.h"
#include "cpu_pwr_config.h"

//#if RHINO_CONFIG_CPU_PWR_MGMT_SHOW

#include <stdio.h>
#include <string.h>
#include <cpu_pwr_lib.h>
#include <cpu_pwr_hal_lib.h>
#include "cpu_pwr_api.h"
#include "k_ffs.h"

/* extern */

extern cpu_pwr_t *     p_cpu_pwr_root_node;
extern cpu_pwr_t *     p_cpu_leaf_node_array[];

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
static void cpu_pwr_p_support_print (cpu_pwr_t * pCpuNode, char * prefix);
#endif
static void cpu_pwr_c_support_print (cpu_pwr_t * pCpuNode, char * prefix);
static void cpu_pwr_node_show (cpu_pwr_t * pCpuNode, char * prefix, 
                             int isLast);

static int cpu_pwr_id_get
    (
    cpu_pwr_t * pCpuNode
    )
    {
    int             cpuId;
    cpu_pwr_t *     pLeafNode;
    uint32_t        cpuNums = CPUS_NUM_MAX;

    for (cpuId = 0; cpuId < cpuNums; cpuId++)
        {
        pLeafNode = p_cpu_leaf_node_array[cpuId];

        if (pLeafNode == pCpuNode)
            {
            return (cpuId);
            }
        }

    return (-1);
    }

static void cpu_pwr_node_show
    (
    cpu_pwr_t *     pCpuNode,
    char *          prefix,
    int             isLast
    )
    {
    DL_NODE *       pNode;
    int             level;
    int             column;
    int             cpuId;
    char            ch;
    int             childIsLast;

    pNode = DLL_FIRST(&pCpuNode->childList);    

    /* 
     * ignore the root node, cpu_pwr_root_node is a mount point
     * for the other nodes in the system
     */

    level = pCpuNode->level;

    if (level != CPU_PWR_TOPO_LEVEL_ROOT)
        {
        if (level > 1)
            {
            column = (level - 1) * 4;

            ch = prefix[column];
            prefix[column] = '\0';
            (void)printf ("%s", prefix);
            prefix[column] = ch;

            (void)printf ("+--");
            }

        if (pNode == NULL)
            {
            /* leaf node should be a logical CPU */

            cpuId = cpu_pwr_id_get (pCpuNode);
            (void)printf ("%s%u (CPU%d)\n", pCpuNode->name, pCpuNode->unit, 
                          cpuId);
            }
        else
            {
            (void)printf ("%s%u\n", pCpuNode->name, pCpuNode->unit);
            }

        /* if no children or this is last node, don't draw line for the branch */

        if (level > 1)
            {
            if (pNode == NULL)
                {
                prefix[column + 4] = ' ';
                }

            if (isLast)
                {
                prefix[column] = ' ';
                }
            }

        (void)printf ("%s\n", prefix);

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
        if (pCpuNode->supportBitsetP != 0)
            cpu_pwr_p_support_print (pCpuNode, prefix);
#endif

        if (pCpuNode->supportBitsetC != 0)
            cpu_pwr_c_support_print (pCpuNode, prefix);
        }

    /* search all the child of current cpu node */

    while (pNode != NULL)
        {
        pCpuNode = container_of (pNode, cpu_pwr_t, node);

        /* start drawing branch for children */

        column = (level + 1) * 4;
        prefix[column] = '|';

        pNode = DLL_NEXT (pNode);

        if (pNode == NULL)
            childIsLast = TRUE;
        else
            childIsLast = FALSE;

        cpu_pwr_node_show (pCpuNode, prefix, childIsLast);
        }
    }

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT

static void cpu_pwr_p_support_print
    (
    cpu_pwr_t * pCpuNode,
    char *        prefix
    )
    {
    uint32_t stateSet;
    uint32_t freq;
    uint32_t volt;
    cpu_pstate_t    pstate;

    (void)printf ("%s P-States:\n", prefix);

    stateSet = pCpuNode->supportBitsetP;
    while (stateSet != 0)
        {
        pstate = (cpu_pstate_t)(FFS_MSB (stateSet) - 1);
        freq = cpu_pwr_freq_get_by_p_state (pCpuNode, pstate) / 1000;
        volt = cpu_pwr_volt_get_by_p_state (pCpuNode, pstate) / 1000;

        (void)printf ("%s   P%d: %4d MHz   %4d mv\n",
                      prefix, pstate, freq, volt);

        stateSet &= ~(1 << pstate);
        }
    }
#endif

static void cpu_pwr_c_support_print
    (
    cpu_pwr_t * pCpuNode,
    char *      prefix
    )
    {
    uint32_t stateSet;
    uint32_t latency = 0;
    int    state;
    int    ix;

    (void)printf ("%s C-States:\n", prefix);

    stateSet = pCpuNode->supportBitsetC;

    while (stateSet != 0)
        {
        state = FFS_LSB (stateSet) - 1;

        for (ix = 0; ix < pCpuNode->stateNumC; ix++)
            {

            if (state == pCpuNode->pPairLatency[ix].state)
                {
                latency = pCpuNode->pPairLatency[ix].value;
                break;
                }
            }

        (void)printf ("%s   C%d: %4d us latency\n", 
                      prefix, state, latency);

        stateSet &= ~(1 << state);
        }
    }

/*
* cpu_pwr_topology_show() will show information for all nodes of the cpu power
* management topology tree. For each node the C-state and P-state capability
* is provided, as applicable, depending on hardware capabilities and configuration.
* 
* For P-state capability the CPU core frequency and voltage setting is 
* displayed. For C-state capabilities the latency information (wake-up time) 
* is displayed.
*/

kstat_t cpu_pwr_topology_show (void)
    {
    char prefix[32];

    (void)printf ("\n         CPU NODE                      CAPABILITIES\n"
             "------------------------------  --------------------------\n");

    /* fill prefix with spaces */

    (void)sprintf (prefix, "%*s", 31, "");

    /* call recursive function to print the tree */

    cpu_pwr_node_show (p_cpu_pwr_root_node, prefix, TRUE);

    return (RHINO_SUCCESS);
    }


/*
* cpu_pwr_state_show() display the CPU power/performance states.
*/

kstat_t cpu_pwr_state_show (void)
    {
    uint32_t    cpuId;
#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT    
    cpu_pstate_t pState;
#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */
    cpu_cstate_t cState;

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
    (void)printf ("\n CPU   P-STATE  C-STATE\n"
                    "-----  -------  -------\n");
#else
    (void)printf ("\n CPU   C-STATE\n"
                    "-----  -------\n");
#endif

    for (cpuId = 0; cpuId < CPUS_NUM_MAX; cpuId++)
        {
#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT        
        (void)cpu_pwr_p_state_get (cpuId, &pState);
#endif
        (void)cpu_pwr_c_state_get (cpuId, &cState);

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
        (void)printf ("CPU%-5u   P%-2u      C%-2u\n",
                      cpuId, pState, cState);
#else
        (void)printf ("CPU%-5u   C%-2u\n",
                      cpuId, cState);
#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */
        }

    return (RHINO_SUCCESS);
    }
//#endif
