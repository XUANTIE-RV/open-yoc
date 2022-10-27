/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#  define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#  define _weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));


static void weak_func(void)
{
}
weak_alias(weak_func, csi_spiflash_initialize)
weak_alias(weak_func, csi_spiflash_uninitialize)
weak_alias(weak_func, csi_spiflash_read)
weak_alias(weak_func, csi_spiflash_program)
weak_alias(weak_func, csi_spiflash_erase_sector)
weak_alias(weak_func, csi_spiflash_erase_chip)
weak_alias(weak_func, csi_spiflash_get_info)
weak_alias(weak_func, csi_eflash_initialize)
weak_alias(weak_func, csi_eflash_uninitialize)
weak_alias(weak_func, csi_eflash_read)
weak_alias(weak_func, csi_eflash_program)
weak_alias(weak_func, csi_eflash_erase_sector)
weak_alias(weak_func, csi_eflash_erase_chip)
weak_alias(weak_func, csi_eflash_get_info)

