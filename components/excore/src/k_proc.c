/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "k_elf.h"
#include "utask.h"
#include "task_group.h"

#include <fcntl.h>
#include "fs/vfs_types.h"
#include "fs/vfs_api.h"
#include "bengine_dload.h"

#ifdef AOS_CONFIG_LOAD_ZIP
#include <zlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#endif

kproc_info_t g_proc_info;

int32_t os_proc_mmap_free(unsigned long vaddr_start,
                          unsigned long vaddr_end,
                          uint32_t  pid)
{
    unsigned long vaddr_once;
    unsigned long vaddr_map_start;
    unsigned long vaddr_map_end;
    unsigned long paddr_start, paddr_end;
    size_t    len_once;

    vaddr_map_start = OS_ALIGN_DOWN(vaddr_start, K_MMU_PAGE_ALIGN);
    vaddr_map_end   = OS_ALIGN_UP(vaddr_end, K_MMU_PAGE_ALIGN);

    for (vaddr_once = vaddr_map_start; vaddr_once < vaddr_map_end; vaddr_once += len_once) {
        len_once   = 0;

        paddr_start = k_mmu_va2pa(vaddr_once);

#if (MK_CONFIG_MMU_LVL2 == 1)
        /* try supersection */
        if (len_once == 0
            && vaddr_once % MMU_SUPERSECTION_SIZE == 0
            && vaddr_once + MMU_SUPERSECTION_SIZE <= vaddr_map_end) {
            paddr_end = k_mmu_va2pa(vaddr_once + MMU_SUPERSECTION_SIZE - MMU_SMALLPAGE_SIZE);
            if (paddr_end - paddr_start == (MMU_SUPERSECTION_SIZE - MMU_SMALLPAGE_SIZE)) {
                len_once = MMU_SUPERSECTION_SIZE;
            }
        }

        /* try section */
        if (len_once == 0
            && vaddr_once % MMU_SECTION_SIZE == 0
            && vaddr_once + MMU_SECTION_SIZE <= vaddr_map_end) {
            paddr_end = k_mmu_va2pa(vaddr_once + MMU_SECTION_SIZE - MMU_SMALLPAGE_SIZE);
            if (paddr_end - paddr_start == (MMU_SECTION_SIZE - MMU_SMALLPAGE_SIZE)) {
                len_once = MMU_SECTION_SIZE;
            }
        }

        /* try large page */
        if (len_once == 0
            && vaddr_once % MMU_LARGEPAGE_SIZE == 0
            && vaddr_once + MMU_LARGEPAGE_SIZE <= vaddr_map_end) {
            paddr_end = k_mmu_va2pa(vaddr_once + MMU_LARGEPAGE_SIZE - MMU_SMALLPAGE_SIZE);
            if (paddr_end - paddr_start == (MMU_LARGEPAGE_SIZE - MMU_SMALLPAGE_SIZE)) {
                len_once = MMU_LARGEPAGE_SIZE;
            }
        }

        /* try small page */
        if (len_once == 0
            && vaddr_once % MMU_SMALLPAGE_SIZE == 0
            && vaddr_once + MMU_SMALLPAGE_SIZE <= vaddr_map_end) {
            len_once = MMU_SMALLPAGE_SIZE;
        }

#else
        len_once = MMU_SECTION_SIZE;
#endif
        if (len_once) {
            paddr_start = k_mmu_va2pa(vaddr_once);
            k_phymm_free(pid, paddr_start, len_once, VMM_TYPE_APP_LOAD);
            OS_PRINT_DBG("%s: pid %d unmap v 0x%x-0x%x -> p 0x%x-0x%x, size 0x%x\r\n",
                         __func__, pid, vaddr_once, vaddr_once + len_once,
                         paddr_start, paddr_start + len_once, len_once);
            k_pgt_remove(pid, vaddr_once, len_once);
        } else {
            break;
        }
    }

    return 0;
}

/*
attach a physical memory for proc:
1. alloc physical memory, mapping to vaddr_start ~ vaddr_end
2. clear to 0
3. copy buff, from copy_from to proc's vaddr
*/
int32_t os_proc_mmap_copy(void       *vaddr_start,
                          void       *vaddr_end,
                          void       *copy_from,
                          size_t      copy_len,
                          kvmm_prot_t prot,
                          uint32_t    pid)
{
    void *vaddr_map;
    unsigned long vaddr_map_start;
    unsigned long vaddr_map_end;

    /* mapped address must be aligned */
    vaddr_map_start = OS_ALIGN_DOWN((unsigned long)vaddr_start, K_MMU_PAGE_ALIGN);
    vaddr_map_end   = OS_ALIGN_UP((unsigned long)vaddr_end, K_MMU_PAGE_ALIGN);

    vaddr_map = k_vmm_alloc(pid, NULL, (void *)vaddr_map_start, vaddr_map_end - vaddr_map_start, prot, VMM_TYPE_APP_LOAD);
    if ((unsigned long)vaddr_map != vaddr_map_start) {
        return -__LINE__;
    }

    memset(vaddr_map, 0, vaddr_map_end - vaddr_map_start);

    /* copy text and data */
    memcpy(vaddr_start, copy_from, copy_len);

    return 0;
}

void *os_proc_file2ram(const char *path, int32_t *fsize)
{
    int   file_len;
    int   read_count;
    int   fd;
    void *pbuff;

    fd = vfs_open(path, O_RDONLY);
    if (fd < 0) {
        OS_PRINT_ERR("vfs_open failed\r\n");
        return NULL;
    }

    file_len = vfs_lseek(fd, 0, SEEK_END);
    vfs_lseek(fd, 0, SEEK_SET);

    if (file_len == 0) {
        vfs_close(fd);
        return NULL;
    }

    pbuff = krhino_mm_alloc(file_len);
    if (pbuff == NULL) {
        vfs_close(fd);
        return NULL;
    }

    read_count = vfs_read(fd, pbuff, file_len);

    vfs_close(fd);

    if (read_count != file_len) {
        return NULL;
    }

    *fsize = file_len;
    fd = -1;

    return pbuff;
}

#ifdef AOS_CONFIG_LOAD_ZIP
#define ZIP_INFO_FILE "unzip.info"
static int check_unzip_info(const char *path, unsigned long *unzip_size)
{
    int fd;
    int32_t ret;
    char dir[240] = {0}, *p, info_file[256] = {0}, buf[32];
    vfs_stat_t st;

    ret = vfs_stat(path, &st);
    if (ret < 0) return -__LINE__;
    if (!S_ISREG(st.st_mode)) return -__LINE__;

    if (strstr(path, "/")) {
        p = (char *)(&path[strlen(path) - 1]);
        while (*p != '/') p--;
        memcpy(dir, path, p - path);
    } else {
        getcwd(dir, sizeof(dir) - 1);
    }

    snprintf(info_file, sizeof(info_file) - 1, "%s/%s", dir, ZIP_INFO_FILE);

    memset(&st, 0, sizeof(st));
    ret = vfs_stat(info_file, &st);
    if (ret < 0) return -__LINE__;
    if (!S_ISREG(st.st_mode)) return -__LINE__;

    fd = vfs_open(info_file, O_RDONLY);
    if (fd < 0) return -__LINE__;

    ret = vfs_read(fd, buf, sizeof(buf) - 1);
    vfs_close(fd);
    if (ret < 0) {
        return -__LINE__;
    }

    *unzip_size = (unsigned long)atoi(buf);

    return 0;
}

void *os_proc_zipfile2ram(const char *path)
{
    void *zipfile = NULL, *dstbuf = NULL;
    int32_t fsiz;
    int ret;
    unsigned long bufsiz;

    zipfile = os_proc_file2ram(path, &fsiz);
    if (!zipfile) return NULL;

    if (check_unzip_info(path, &bufsiz) != 0) {
        /* 4x file size should be enough for uncompression */
        bufsiz = fsiz << 2;
    }

    dstbuf = krhino_mm_alloc(bufsiz);
    if (!dstbuf) goto err;

    memset(dstbuf, 0, sizeof(bufsiz));
    ret = uncompress(dstbuf, &bufsiz, zipfile, fsiz);
    if (ret != Z_OK) {
        goto err;
    }

    krhino_mm_free(zipfile);
    return dstbuf;

err:
    if (zipfile) krhino_mm_free(zipfile);
    if (dstbuf) krhino_mm_free(dstbuf);
    return NULL;
}
#endif

int32_t os_proc_check(Elf32_Ehdr *ehdr)
{
    if ((ehdr->e_ident[EI_MAG0] != ELFMAG0) ||
        (ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
        (ehdr->e_ident[EI_MAG2] != ELFMAG2) ||
        (ehdr->e_ident[EI_MAG3] != ELFMAG3)) {
        return -__LINE__;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
        return -__LINE__;
    }

    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
        return -__LINE__;
    }

    if (ehdr->e_machine != EM_ARM) {
        return -__LINE__;
    }

    return  0;
}

int32_t os_proc_check_overlap(Elf32_Ehdr *ehdr)
{
    int32_t i;
    Elf32_Phdr *phdr;
    kproc_app_t *proc;
    preamble_t *preamble = NULL;
    unsigned long text_start;
    unsigned long text_end;
    unsigned long data_start;
    unsigned long data_end;
    unsigned long bss_start;
    unsigned long zero_end;
    unsigned long temp_min, temp_max;
    unsigned long cur_min, cur_max;

    if (ehdr->e_machine != EM_ARM) {
        return -__LINE__;
    }

    phdr = (Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff);

    /* find preamble_t */
    for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
        if (phdr->p_type == PT_LOAD && phdr->p_filesz != 0) {
            if (preamble == NULL) {
                /* only support 1 load segment for execution object file */
                preamble = (preamble_t *)((char *)ehdr + phdr->p_offset);
                break;
            }
        }
    }

    if (preamble == NULL || preamble->magic != PREAMBLE_MAGIC) {
        /* can not find preamble_t */
        return -__LINE__;
    }

    if (!((unsigned long)preamble->text_start <= (unsigned long)preamble->text_end
          && (unsigned long)preamble->text_end <= (unsigned long) preamble->data_start
          && (unsigned long)preamble->data_start <= (unsigned long)preamble->data_end
          && (unsigned long)preamble->data_end <= (unsigned long)preamble->bss_start
          && (unsigned long)preamble->bss_start <= (unsigned long)preamble->zero_end)) {
        /* preamble_t error */
        return -__LINE__;
    }

    cur_min = (preamble->text_start < preamble->data_start) ? (unsigned long)preamble->text_start :
              (unsigned long)preamble->data_start;
    cur_min = (cur_min < (unsigned long)preamble->bss_start) ? cur_min : (unsigned long)preamble->bss_start;
    cur_max = (preamble->text_end > preamble->data_end) ? (unsigned long)preamble->text_end : (unsigned long)preamble->data_end;
    cur_max = (cur_max > (unsigned long)preamble->zero_end) ? cur_max : (unsigned long)preamble->zero_end;

    for (i = 1; i < CONFIG_EXCORE_PROCESS_MAX; i++) {
        proc = &g_proc_info.procs[i];
        if (proc->state != K_PROC_RDY) {
            continue;
        }

        text_start = (unsigned long)proc->preamble.text_start;
        text_end   = (unsigned long)proc->preamble.text_end;
        data_start = (unsigned long)proc->preamble.data_start;
        data_end   = (unsigned long)proc->preamble.data_end;
        bss_start = (unsigned long)proc->preamble.bss_start;
        zero_end   = (unsigned long)proc->preamble.zero_end;

        temp_min = (text_start < data_start) ? text_start : data_start;
        temp_min = (temp_min < bss_start) ? temp_min : bss_start;
        temp_max = (text_end > data_end) ? text_end : data_end;
        temp_max = (temp_max > zero_end) ? temp_max : zero_end;

        if (!((cur_min >= temp_max) || (cur_max <= temp_min))) {
            return -__LINE__;
        }
    }

    return 0;
}

int k_mmu_mmap(unsigned long vaddr, unsigned long paddr, size_t len,
               mmu_mem_attr_t mem_attr, int asid, int32_t privileged);

int32_t os_proc_load(kproc_app_t *proc, Elf32_Ehdr *ehdr)
{
    int32_t i, ret;
    Elf32_Phdr *phdr;
    preamble_t *preamble = NULL;
    unsigned long   copy_from;

    if (ehdr->e_machine != EM_ARM) {
        return -__LINE__;
    }

    phdr = (Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff);

    /* find preamble_t */
    for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
        if (phdr->p_type == PT_LOAD && phdr->p_filesz != 0) {
            if (preamble == NULL) {
                /* only support 1 load segment for execution object file */
                preamble = (preamble_t *)((char *)ehdr + phdr->p_offset);
                break;
            }
        }
    }
    if (preamble == NULL || preamble->magic != PREAMBLE_MAGIC) {
        /* can not find preamble_t */
        return -__LINE__;
    }

    memcpy(&proc->preamble, preamble, sizeof(proc->preamble));

    if (!((unsigned long)preamble->text_start <= (unsigned long)preamble->text_end
          && (unsigned long)preamble->text_end <= (unsigned long) preamble->data_start
          && (unsigned long)preamble->data_start <= (unsigned long)preamble->data_end
          && (unsigned long)preamble->data_end <= (unsigned long)preamble->bss_start
          && (unsigned long)preamble->bss_start <= (unsigned long)preamble->zero_end)) {
        /* preamble_t error */
        return -__LINE__;
    }

    // TODO: There is a bug if the image is compiled by armcc
#if 0
    if ((unsigned long)preamble->text_start < phdr->p_vaddr
        || (unsigned long)preamble->data_end   > phdr->p_vaddr + phdr->p_filesz) {
        /* loadable segment not match the phdr */
        return -__LINE__;
    }
#endif

    copy_from = (unsigned long)ehdr + phdr->p_offset;

    /* are text and data stick togather? */
    if (((unsigned long)preamble->text_end) / K_MMU_PAGE_ALIGN ==
        ((unsigned long)preamble->data_start) / K_MMU_PAGE_ALIGN) {
        /* text and data not separated, load togather */
        ret = os_proc_mmap_copy(preamble->text_start,
                                preamble->zero_end,
                                (void *)(copy_from + (unsigned long)preamble->text_start - phdr->p_vaddr),
                                (size_t)((char *)preamble->data_end - (char *)preamble->text_start),
                                VMM_PROT_ALL, proc->pid);
        if (ret != 0) {
            return ret;
        }
    } else {
        /* load text */
        ret = os_proc_mmap_copy(preamble->text_start,
                                preamble->text_end,
                                (void *)(copy_from + (unsigned long)preamble->text_start - phdr->p_vaddr),
                                (size_t)((char *)preamble->text_end - (char *)preamble->text_start),
                                VMM_PROT_EXE, proc->pid);

        if (ret != 0) {
            return ret;
        }

        /* load data & bss */
        ret = os_proc_mmap_copy(preamble->data_start,
                                preamble->zero_end,
                                (void *)(copy_from + (unsigned long)preamble->data_start - phdr->p_vaddr),
                                (size_t)((char *)preamble->data_end - (char *)preamble->data_start),
                                VMM_PROT_WR, proc->pid);
        if (ret != 0) {
            os_proc_mmap_free((unsigned long)preamble->text_start,
                              (unsigned long)preamble->text_end,
                              proc->pid);
            return ret;
        }
    }

    k_mmu_mmap(0xF0100000, 0x08000000, 0x64, MMU_ATTR_SO, proc->pid, 0);
    return 0;
}

int32_t os_proc_unload(kproc_app_t *proc)
{
    preamble_t *preamble;
    unsigned long vaddr_start, vaddr_end;

    if (proc == NULL) {
        return -__LINE__;
    }

    if (proc->state == K_PROC_RDY) {
        preamble = &proc->preamble;

        if (((unsigned long)preamble->text_end) / K_MMU_PAGE_ALIGN ==
            ((unsigned long)preamble->data_start) / K_MMU_PAGE_ALIGN) {
            vaddr_start = (unsigned long)preamble->text_start;
            vaddr_end   = (unsigned long)preamble->zero_end;
            os_proc_mmap_free(vaddr_start, vaddr_end, proc->pid);
        } else {
            // 1. text segment
            vaddr_start = (unsigned long)preamble->text_start;
            vaddr_end   = (unsigned long)preamble->text_end;
            os_proc_mmap_free(vaddr_start, vaddr_end, proc->pid);
            // 2. data segment
            vaddr_start = (unsigned long)preamble->data_start;
            vaddr_end   = (unsigned long)preamble->zero_end;
            os_proc_mmap_free(vaddr_start, vaddr_end, proc->pid);
        }

        k_asid_dealloc(proc->pid);
        proc->name  = NULL;
        proc->state = K_PROC_NONE;
    }

    return 0;
}

/**
 * Initialize multi-process,
 * The physical memory pool is shared between all process
 */
int32_t os_proc_init(unsigned long paddr_pool, size_t len)
{
    int32_t i;

    /* 1. init apps info */
    memset(&g_proc_info, 0, sizeof(g_proc_info));
    for (i = 0 ; i < CONFIG_EXCORE_PROCESS_MAX ; i++) {
        g_proc_info.procs[i].pid   = i;
        g_proc_info.procs[i].name  = NULL;
        g_proc_info.procs[i].state = K_PROC_NONE;
    }

    /* proc[0] as kernel */
    k_asid_reserve(OS_ASID_KERNEL);
    g_proc_info.procs[OS_ASID_KERNEL].name  = "kernel";
    g_proc_info.procs[OS_ASID_KERNEL].state = K_PROC_RDY;

    bengine_dload_init();
    return 0;
}

int k_proc_init(void *phyaddr, size_t size)
{
    return os_proc_init((unsigned long)phyaddr, size);
}

extern int k_pagetable_init(int asid);
extern void k_pagetable_switch(int asid);

/**
 * Load a proc image,
 * The format must be excutable elf object
 */
static int32_t proc_load(Elf32_Ehdr *ehdr, const char *name)
{
    int pid, cur_pid;
    int32_t ret;
    kstat_t state;
    kproc_app_t *proc = NULL;

    if (os_proc_check(ehdr) != 0) {
        OS_PRINT_ERR("%s: ehdr check failed\r\n", __func__);
        ret = -__LINE__;
        goto _exit1;
    }

    if (os_proc_check_overlap(ehdr) != 0) {
        OS_PRINT_ERR("%s: proc overlap\r\n", __func__);
        ret = -__LINE__;
        goto _exit1;
    }

    /* 3. alloc a proc info */
    pid = k_asid_alloc();
    if (pid >= CONFIG_EXCORE_PROCESS_MAX) {
        k_asid_dealloc(pid);
        ret = -__LINE__;
        goto _exit1;
    }
    proc = &g_proc_info.procs[pid];
    proc->entry = (void *)ehdr->e_entry;
    proc->name  = (name != NULL) ? name : "unnamed";
    proc->state = K_PROC_RDY;

    /* 4. create page table and switch*/
    // int os_copy_kernel_pgtble(int pid)
    // success: return 0;
    // fail none zero
    ret =  k_pgt_init(pid);
    if (ret != 0) {
        ret = -__LINE__;
        goto _exit1;
    }

    krhino_sched_disable();

    cur_pid = krhino_cur_task_get()->pid;
    k_pgt_switch(pid);

    /* 5. iamge load */
    switch (ehdr->e_type) {
    case ET_EXEC :
        ret = os_proc_load(proc, ehdr);
        if (ret != 0) {
            OS_PRINT_ERR("os_proc_load failed, ret = %d\r\n", ret);
            proc->name  = NULL;
            proc->state = K_PROC_NONE;
            k_asid_dealloc(proc->pid);
            goto _exit2;
        }
        break;
    case ET_REL :
    case ET_DYN :
    default:
        /* error type */
        ret = -__LINE__;
        goto _exit2;
    }

    /* 6. user tasks create */
    if (proc->entry != NULL) {
        k_proc_create_pre_hook(proc);
        OS_PRINT_SHOW("create process %s\r\n", (char *)proc->preamble.app_name);
        OS_PRINT_SHOW("pri %d, ustack %p, ustack_size 0x%x, kstack_size 0x%x, entry %p\r\n",
                      proc->preamble.priority, proc->preamble.ustack, proc->preamble.ustack_size,
                      proc->preamble.kstack_size, proc->preamble.main_entry);

        state = krhino_uprocess_create((const name_t *)proc->preamble.app_name,
                                       (const name_t *)proc->preamble.task_name, 0,
                                       proc->preamble.priority, (tick_t)0,
                                       proc->preamble.ustack,
                                       proc->preamble.ustack_size, // ustasck size
                                       proc->preamble.kstack_size, // kstack size
                                       (task_entry_t)proc->preamble.main_entry,
                                       proc->preamble.ret_entry,
                                       proc->pid, UTASK_AUTORUN);

        if (state != RHINO_SUCCESS) {
            ret = __LINE__;
            goto _exit2;
        }

        k_proc_create_post_hook(proc);

    } else {
        OS_PRINT_ERR("%s: Invalid ELF format\r\n", __func__);
    }

    /* 7. user tasks virtual space init */
    k_space_init(&(task_group_get_by_pid(pid)->virt_addr_dyn_space), (unsigned long)proc->preamble.virt_addr_dyn_start,
                 (uint32_t)proc->preamble.virt_addr_dyn_end - (uint32_t)proc->preamble.virt_addr_dyn_start);

    k_pgt_switch(cur_pid);
    krhino_sched_enable();

    return pid;

_exit2:
    k_pgt_switch(cur_pid);
    krhino_sched_enable();

_exit1:
    if (proc != NULL) {
        os_proc_unload(proc);
    }
    return ret;
}

/* check for loadable segment */
static int os_proc_loadable_segment_check(preamble_t *preamble)
{
    int32_t i;
    kproc_app_t *proc;

    if ((unsigned long)preamble->text_start > (unsigned long)preamble->text_end ||
        (unsigned long)preamble->text_end > (unsigned long)preamble->data_start ||
        (unsigned long)preamble->data_start > (unsigned long)preamble->data_end ||
        (unsigned long)preamble->data_end > (unsigned long)preamble->bss_start ||
        (unsigned long)preamble->bss_start > (unsigned long)preamble->zero_end) {
        /* preamble_t error */
        return -__LINE__;
    }

    for (i = 1; i < CONFIG_EXCORE_PROCESS_MAX; i ++) {
        /* index 0 for kernel */
        proc = &g_proc_info.procs[i];
        if (proc->state != K_PROC_RDY) {
            continue;
        }

        if ((unsigned long)preamble->text_start >= (unsigned long)proc->preamble.text_start &&
            (unsigned long)preamble->text_start <= (unsigned long)proc->preamble.zero_end) {
            return -__LINE__;
        }

        if ((unsigned long)preamble->zero_end >= (unsigned long)proc->preamble.text_start &&
            (unsigned long)preamble->zero_end <= (unsigned long)proc->preamble.zero_end) {
            return -__LINE__;
        }
    }
    return 0;
}

int k_proc_load_dync(const char *file, const char *name)
{
    kstat_t state;

    Elf32_Ehdr *ehdr, elf_header;
    preamble_t *preamble = NULL;
    kproc_app_t *proc;
    Elf32_Phdr *phdr;  /* program header */
    int pht_len;

    int ftlen;  /* file total length */
    int fd;
    int i;

    int pid, cur_pid;
    size_t entry_offset;

    elf_layout_t layout;

#if !BENGINE_USE_VFS
    /* BENGINE_USE_VFS:0, indicate load_dync uses native flash API.
     * BENGINE_USE_VFS:1, indicate load_dync uses vfs API.
     *
     * 1. If you use native flash API to load elf,  elf must be saved in romfs
     * 2. If you Use vfs API to load elf, elf can be saved in any type of fs
     */
    if (memcmp(file, "/usr/", strlen("/usr/")) != 0) {
        return -__LINE__;
    }
#endif

    fd = vfs_open(file, O_RDONLY);
    if (fd < 0) {
        return -__LINE__;
    }

    ftlen = vfs_lseek(fd, 0, SEEK_END);
    if (ftlen < sizeof(*ehdr)) {
        vfs_close(fd);
        return -__LINE__;
    }

    /* Step 1: read ELF header and check value of ELF header */
    ehdr = &elf_header;
    vfs_lseek(fd, 0, SEEK_SET);
    vfs_read(fd, ehdr, sizeof(*ehdr));

    /* check ELF header */
    if (os_proc_check(ehdr) || (void *)ehdr->e_entry == NULL || ehdr->e_type != ET_EXEC ||
        ehdr->e_phnum == 0 || ftlen < ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf32_Phdr)) {
        vfs_close(fd);
        return -__LINE__;
    }

    /* Step-2: read program header table and find the first loadable segment */
    pht_len = ehdr->e_phnum * sizeof(Elf32_Phdr);
    phdr = (Elf32_Phdr *)krhino_mm_alloc(pht_len);

    if (phdr == NULL) {
        vfs_close(fd);
        return -__LINE__;
    }

    /* offset of ELF32 program header table in ELF file */
    vfs_lseek(fd, ehdr->e_phoff, SEEK_SET);
    /* read program header table */
    vfs_read(fd, phdr, pht_len);

    /* find the first loadable segment */
    for (i = 0; i < ehdr->e_phnum; i ++) {
        if ((phdr + i)->p_type == PT_LOAD && (phdr + i)->p_filesz != 0) {
            if (ftlen < (phdr + i)->p_offset + sizeof(*preamble)) {
                krhino_mm_free(phdr);
                vfs_close(fd);
                return -__LINE__;
            }

            /* only support one loadable segment for ELF file */
            preamble = krhino_mm_alloc(sizeof(*preamble));
            if (preamble == NULL) {
                krhino_mm_free(phdr);
                vfs_close(fd);
                return -__LINE__;
            }

            /* read loadable segment from ELF file */
            vfs_lseek(fd, (phdr + i)->p_offset, SEEK_SET);

            vfs_read(fd, preamble, sizeof(*preamble));

            if (preamble->magic != PREAMBLE_MAGIC) {
                krhino_mm_free(preamble);
                krhino_mm_free(phdr);
                vfs_close(fd);
                return -__LINE__;
            }
            break;
        }
    }

    /* check loadable segment */
    if (preamble == NULL || os_proc_loadable_segment_check(preamble)) {
        if (preamble) krhino_mm_free(preamble);
        krhino_mm_free(phdr);
        vfs_close(fd);
        return -__LINE__;
    }

    /* Step-3: alloc a proc info */
    pid = k_asid_alloc();
    if (pid >= CONFIG_EXCORE_PROCESS_MAX) {
        krhino_mm_free(preamble);
        krhino_mm_free(phdr);
        vfs_close(fd);
        return -__LINE__;
    }

    proc = &g_proc_info.procs[pid];
    proc->entry = (void *)ehdr->e_entry;
    proc->name  = (name != NULL) ? name : "unnamed";
    proc->state = K_PROC_RDY;
    memcpy(&proc->preamble, preamble, sizeof(*preamble));

    /* Step-4: create page table and switch */
    k_pgt_init(pid);

    //krhino_sched_disable();

    cur_pid = krhino_cur_task_get()->pid;
    k_pgt_switch(pid);

    /* Step-5: load the first section of image */
    entry_offset = (phdr + i)->p_offset + ((unsigned long)preamble->text_start - (phdr + i)->p_vaddr);

    /* register page fault */
    layout.text_start = preamble->text_start;
    layout.text_size = preamble->text_end - preamble->text_start;
    layout.data_start = preamble->data_start;
    layout.data_size = preamble->data_end - preamble->data_start;
    layout.bss_start = preamble->bss_start;
    layout.bss_size = preamble->zero_end - preamble->bss_start;
    if (bengine_dload_reg(pid, &layout, fd, entry_offset) != RHINO_SUCCESS) {
        krhino_mm_free(preamble);
        krhino_mm_free(phdr);
        vfs_close(fd);
        k_asid_dealloc(pid);
        proc->state = K_PROC_NONE;
        k_pgt_switch(cur_pid);
        krhino_sched_enable();
        return -__LINE__;
    }

    bengine_pre_dload(proc->preamble.text_start, 0x8000);

    bengine_pre_dload(proc->preamble.data_start,
                      (size_t)proc->preamble.data_end - (size_t)proc->preamble.data_start);

    k_mmu_mmap(0xF0100000, 0x08000000, 0x64, MMU_ATTR_SO, proc->pid, 0);

    /* 6. user tasks create */
    k_proc_create_pre_hook(proc);

    /*  NOTE:
     *  the ustack is possible to occur pagefault before task of elf runs,
     *  if ustack occurs pagefault, the load_dync would be blocked,
     *  and there is no chance to do create task for elf.
     *
     *  so load task (pftask) used by pagefault must run before create task for elf
     *  with the API of bengine_dload_scheme(1);
     */
    bengine_dload_scheme(1);
    state = krhino_uprocess_create((const name_t *)proc->preamble.app_name,
                                   (const name_t *)proc->preamble.task_name, 0, proc->preamble.priority, (tick_t)0,
                                   proc->preamble.ustack, proc->preamble.ustack_size, // ustasck size
                                   proc->preamble.kstack_size, // kstack size
                                   (task_entry_t)proc->preamble.main_entry,
                                   proc->preamble.ret_entry, proc->pid, UTASK_AUTORUN);

    if (state != RHINO_SUCCESS) {
        bengine_dload_unreg((void *)preamble->text_start);
        krhino_mm_free(preamble);
        krhino_mm_free(phdr);
        vfs_close(fd);
        k_asid_dealloc(proc->pid);
        proc->state = K_PROC_NONE;
        k_pgt_switch(cur_pid);
        krhino_sched_enable();
        return -__LINE__;
    }
    krhino_mm_free(preamble);
    krhino_mm_free(phdr);

    k_proc_create_post_hook(proc);

    /* 7. user tasks virtual space init */
    k_space_init(&(task_group_get_by_pid(pid)->virt_addr_dyn_space), (unsigned long)proc->preamble.virt_addr_dyn_start,
                 (uint32_t)proc->preamble.virt_addr_dyn_end - (uint32_t)proc->preamble.virt_addr_dyn_start);

    k_pgt_switch(cur_pid);

    //krhino_sched_enable();

    return pid;
}

int k_proc_load(const char *file, const char *name)
{
    Elf32_Ehdr *ehdr = NULL;
    int32_t     ret, fsize;

    ehdr = (Elf32_Ehdr *)os_proc_file2ram(file, &fsize);
    if (ehdr == NULL) {
        OS_PRINT_ERR("%s: load %s to ram failed", __func__, file);
        return -__LINE__;
    }

    ret = proc_load(ehdr, name);
    if (ret < 0) {
        OS_PRINT_ERR("%s: load %s failed, ret %d\r\n", __func__, file, ret);
    }

    krhino_mm_free(ehdr);

    return ret;
}

#ifdef AOS_CONFIG_LOAD_ZIP
int k_proc_load_zip(const char *file, const char *name)
{
    Elf32_Ehdr *ehdr = NULL;
    int32_t     ret;

    ehdr = (Elf32_Ehdr *)os_proc_zipfile2ram(file);
    if (ehdr == NULL) {
        OS_PRINT_ERR("%s: load %s to ram failed", __func__, file);
        return -__LINE__;
    }

    ret = proc_load(ehdr, name);
    if (ret < 0) {
        OS_PRINT_ERR("%s: load %s failed, ret %d\r\n", __func__, file, ret);
    }

    krhino_mm_free(ehdr);

    return ret;
}
#endif

int k_proc_ramload(const void *ramaddr, const char *name)
{
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)ramaddr;

    return proc_load(ehdr, name);
}

int k_proc_unload(int pid)
{
    if (pid >= CONFIG_EXCORE_PROCESS_MAX) {
        return -__LINE__;
    }

    if (g_proc_info.procs[pid].pid != pid) {
        return -__LINE__;
    }

    return os_proc_unload(&g_proc_info.procs[pid]);
}

void k_proc_switch(void *p_new, void *p_old)
{
    ktask_t *new_task = (ktask_t *)p_new;
    ktask_t *old_task = (ktask_t *)p_old;

    if (new_task->pid == old_task->pid) {
        return;
    }

    if (new_task->pid != OS_ASID_KERNEL) {
        k_pgt_switch(new_task->pid);
    }
}

void k_proc_show(void)
{
    kproc_app_t   *papp;
    int32_t pid;

    OS_PRINT_SHOW("[k_proc_show] Process Info:\n");
    for (pid = 0 ; pid < CONFIG_EXCORE_PROCESS_MAX ; pid++) {
        papp = &g_proc_info.procs[pid];
        if (papp->state == K_PROC_RDY) {
            OS_PRINT_SHOW("  PID (%u %s):\n", papp->pid, papp->name);
            OS_PRINT_SHOW("  Entry = %p\n", papp->entry);
        }
    }
}

kproc_app_t *k_proc_app_get(int pid)
{
    if (pid >= CONFIG_EXCORE_PROCESS_MAX) {
        return NULL;
    }

    return &g_proc_info.procs[pid];
}

int k_proc_addr_check(int pid, void *addr, size_t size)
{
    kproc_app_t *app;
    preamble_t *preamble;

    if (pid >= CONFIG_EXCORE_PROCESS_MAX) {
        return 0;
    }

    if ((unsigned long)addr == 0 || size == 0) {
        return 0;
    }

    app = &g_proc_info.procs[pid];

    if (app->pid != pid) {
        return 0;
    }

    preamble = &app->preamble;

    if ((unsigned long)addr > (unsigned long)preamble->data_start
        && ((unsigned long)addr + (unsigned long)size) < (unsigned long)preamble->zero_end) {
        return 1;
    }

    return 0;
}

RHINO_WEAK void k_proc_create_pre_hook(kproc_app_t *proc)
{
    return;
}

RHINO_WEAK void k_proc_create_post_hook(kproc_app_t *proc)
{
    return;
}

