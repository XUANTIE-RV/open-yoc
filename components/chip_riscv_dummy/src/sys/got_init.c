 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if CONFIG_PIE_ENABLE

/* ========================================================= */

#include <stdint.h>

typedef struct {
    uint64_t offset;
    uint64_t type;
    uint64_t symbol_addend;
} elf64_rela;

typedef enum {
    R_RISCV_NONE = 0,
    R_RISCV_32 = 1,
    R_RISCV_64 = 2,
    R_RISCV_RELATIVE = 3,
    R_RISCV_COPY = 4,
    R_RISCV_JUMP_SLOT = 5,
} RISCV_relocation_type;

/* We need to use .rela.dyn table in the elf to initialize GOT.
** The .rela.dyn table looks like this (use `llvm-readelf -r out.elf` to view):
**
**   Relocation section '.rela.dyn' at offset 0x3718 contains 21 entries:
**       Offset             Info             Type               Symbol's Value  Symbol's Name + Addend
**   0000000050002fe8  0000000000000003 R_RISCV_RELATIVE                  50002710
**   0000000050002ff0  0000000000000003 R_RISCV_RELATIVE                  50002710
**
** Every entry consists of three 8-bytes numbers (corresponding to "elf64_rela" above),
**   1. the first number ("Offset") is the destination address (most in GOT section)
**   2. the middle one ("Info" and interpretted as "Type") is the relocation type (we
**      only handle R_RISCV_RELATIVE here)
**   3. the last one ("Symbol Addend") is the content we need to fill.
**
** So what the following code does is like this for each entry:
**   1. fill 0x50002710 (virtual address) at the 8-bytes slot at 0x50002fe8 (virtual address)
**   2. fill 0x50002710 at 0x50002ff0, and so on...
*/

void GOTInitialize(int64_t rela_dyn_start, int64_t rela_dyn_end, int64_t dram_load_start, uint64_t dram_virtual_info_ptr) {
    /* The numbers in the .rela.dyn table are all about virtual address, they need to be fixed
    ** by adding the offset between virtual address (stored statically in data section) and
    ** the actual load address (by symbol)
    */
    uint64_t dram_virtual_start = *((uint64_t*)dram_virtual_info_ptr);
    int64_t load_offset = dram_load_start - (int64_t)dram_virtual_start;
    for (elf64_rela *item = (elf64_rela*)(rela_dyn_start); item != (elf64_rela*)(rela_dyn_end); ++item) {
        uint64_t relocation_type = item->type;
        if (relocation_type == (uint64_t)R_RISCV_RELATIVE) {
            /* Fill the symbol_addend to offset (destination) */
            uint64_t *offset = (uint64_t*)(item->offset + load_offset);
            *offset = item->symbol_addend + load_offset;
        }
        /* TODO: may need to handle other type of relocation */
    }
}
/* ========================================================= */
/* ========================================================= */

#endif
