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

#ifndef __PERF_EVENTS_H__
#define __PERF_EVENTS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_CPU_XUANTIE_C906 || CONFIG_CPU_XUANTIE_C906FD || CONFIG_CPU_XUANTIE_C906FDV \
    || CONFIG_CPU_XUANTIE_C907 || CONFIG_CPU_XUANTIE_C907FD || CONFIG_CPU_XUANTIE_C907FDV || CONFIG_CPU_XUANTIE_C907FDVM \
    || CONFIG_CPU_XUANTIE_C907_RV32 || CONFIG_CPU_XUANTIE_C907FD_RV32 || CONFIG_CPU_XUANTIE_C907FDV_RV32 || CONFIG_CPU_XUANTIE_C907FDVM_RV32 \
    || CONFIG_CPU_XUANTIE_C908 || CONFIG_CPU_XUANTIE_C908V || CONFIG_CPU_XUANTIE_C908I \
    || CONFIG_CPU_XUANTIE_C908X || CONFIG_CPU_XUANTIE_C908X_CP || CONFIG_CPU_XUANTIE_C908X_CP_XT \
    || CONFIG_CPU_XUANTIE_C920V2 || CONFIG_CPU_XUANTIE_C920V3 || CONFIG_CPU_XUANTIE_C920V3_CP || CONFIG_CPU_XUANTIE_C920V3_CP_XT \
    || CONFIG_CPU_XUANTIE_R908 || CONFIG_CPU_XUANTIE_R908FD || CONFIG_CPU_XUANTIE_R908FDV \
    || CONFIG_CPU_XUANTIE_R908_CP || CONFIG_CPU_XUANTIE_R908FD_CP || CONFIG_CPU_XUANTIE_R908FDV_CP \
    || CONFIG_CPU_XUANTIE_R908_CP_XT || CONFIG_CPU_XUANTIE_R908FD_CP_XT || CONFIG_CPU_XUANTIE_R908FDV_CP_XT
#define CONFIG_PMU_EVENTS_CONFIGURABLE
#elif CONFIG_CPU_XUANTIE_C910V2 || CONFIG_CPU_XUANTIE_C910V3 || CONFIG_CPU_XUANTIE_C910V3_CP || CONFIG_CPU_XUANTIE_C910V3_CP_XT \
    || CONFIG_CPU_XUANTIE_R910 \
    || CONFIG_CPU_XUANTIE_E906 || CONFIG_CPU_XUANTIE_E906F || CONFIG_CPU_XUANTIE_E906P || CONFIG_CPU_XUANTIE_E906FP || CONFIG_CPU_XUANTIE_E906FD || CONFIG_CPU_XUANTIE_E906FDP \
    || CONFIG_CPU_XUANTIE_E907 || CONFIG_CPU_XUANTIE_E907F || CONFIG_CPU_XUANTIE_E907P || CONFIG_CPU_XUANTIE_E907FP || CONFIG_CPU_XUANTIE_E907FD || CONFIG_CPU_XUANTIE_E907FDP
#define CONFIG_PMU_EVENTS_FIXED
#else /* E901, E902 */
#define CONFIG_PMU_EVENTS_NOT_SUPPORT
#endif // CONFIG_CPU_XUANTIE_XXX

#if defined(CONFIG_PMU_EVENTS_FIXED)
#define CONFIG_NR_HPM_EVENT_FIXED 32
#define CONFIG_PMU_COUNTERS_NR 32
extern uint8_t counters_idx_fixed[CONFIG_NR_HPM_EVENT_FIXED];
extern uint16_t events_fixed[CONFIG_NR_HPM_EVENT_FIXED];
#elif defined(CONFIG_PMU_EVENTS_CONFIGURABLE)
#define CONFIG_PMU_COUNTERS_NR 19
#else
#endif // CONFIG_PMU_EVENTS_FIXED or CONFIG_PMU_EVENTS_CONFIGURABLE

/* PMU HPM counter events macro definitions */
#define PMU_HPM_L1_icache_Access                  0x01
#define PMU_HPM_L1_icache_Miss                    0x02
#define PMU_HPM_iTLB_Miss                         0x03
#define PMU_HPM_D_UTLB_MISS                       0x04
#define PMU_HPM_jTLB_Miss                         0x05
#define PMU_HPM_Condition_Branch_Mispred          0x06
#define PMU_HPM_Condition_Branch                  0x07
#define PMU_HPM_Indirect_Branch_Miss              0x08
#define PMU_HPM_Indirect_Branch                   0x09
#define PMU_HPM_LSU_Spec_Fail                     0x0A
#define PMU_HPM_Store_Instruction                 0x0B
#define PMU_HPM_L1_dcache_load_access             0x0C
#define PMU_HPM_L1_dcache_load_miss               0x0D
#define PMU_HPM_L1_dcache_store_access            0x0E
#define PMU_HPM_L1_dcache_store_miss              0x0F
#define PMU_HPM_L2_Load_Access                    0x10
#define PMU_HPM_L2_Load_Miss                      0x11
#define PMU_HPM_L2_Store_Access                   0x12
#define PMU_HPM_L2_Store_Miss                     0x13
#define PMU_HPM_RF_Launch_Fail                    0x14
#define PMU_HPM_RF_Reg_Launch_Fail                0x15
#define PMU_HPM_Issue_Instruction                 0x16
#define PMU_HPM_LSU_Cross_4K_Stall                0x17
#define PMU_HPM_LSU_Other_Stall                   0x18
#define PMU_HPM_SQ_Discard                        0x19
#define PMU_HPM_SQ_Data_Discard                   0x1A
#define PMU_HPM_IFU_Branch_Target_Mispred         0x1B
#define PMU_HPM_IFU_Branch_Target_Instruction     0x1C
#define PMU_HPM_ALU_Instruction                   0x1D
#define PMU_HPM_Vector_SIMD_Instruction           0x1F
#define PMU_HPM_CSR_Instruction                   0x20
#define PMU_HPM_ATOMIC_Instruction                0x21
#define PMU_HPM_Interupt_Numer                    0x23
#define PMU_HPM_Environment_Call                  0x25
#define PMU_HPM_Long_Jump                         0x26
#define PMU_HPM_Stalled_Cycles_Frontend           0x27
#define PMU_HPM_Stalled_Cycles_Backend            0x28
#define PMU_HPM_SYNC_Stall                        0x29
#define PMU_HPM_Float_Point_Instruction           0x2A
#define PMU_HPM_M_Mode_Cycles                     0x2B
#define PMU_HPM_S_Mode_Cycles                     0x2C
#define PMU_HPM_U_Mode_Cycles                     0x2D
#define PMU_HPM_Exception_Number                  0x2E
#define PMU_HPM_Flush_Number                      0x2F
#define PMU_HPM_LOAD_Instruction                  0x30
#define PMU_HPM_Fused_Instruction                 0x31
#define PMU_HPM_MULT_Instruction                  0x32
#define PMU_HPM_DIV_Instruction                   0x33
#define PMU_HPM_Mult_Inner_Forward                0x34
#define PMU_HPM_DIV_Buffer_Hit                    0x35
#define PMU_HPM_Branch_Instruction                0x36
#define PMU_HPM_Uncondition_Branch                0x37
#define PMU_HPM_Branch_Mispred                    0x38
#define PMU_HPM_Uncondition_Branch_Mispred        0x39
#define PMU_HPM_Taken_Branch_Mispred              0x3A
#define PMU_HPM_Taken_Condition_Branch            0x3B
#define PMU_HPM_Taken_Condition_Branch_Mispred    0x3C
#define PMU_HPM_Unalign_LOAD_Instruction          0x3D
#define PMU_HPM_Unalign_STORE_Instruction         0x3E
#define PMU_HPM_LR_Instruction                    0x3F
#define PMU_HPM_SC_Instruction                    0x40
#define PMU_HPM_AMO_Instruction                   0x41
#define PMU_HPM_Barrier_Instruction               0x42
#define PMU_HPM_Failed_SC_Instruction             0x43
#define PMU_HPM_Bus_Barrier                       0x44
#define PMU_HPM_FP_DIV_Instruction                0x45
#define PMU_HPM_FP_LOAD_Instruction               0x46
#define PMU_HPM_FP_STORE_Instruction              0x47
#define PMU_HPM_Vector_DIV_Instruction            0x48
#define PMU_HPM_Vector_LOAD_Instruction           0x49
#define PMU_HPM_Vector_STORE_Instruction          0x4A
#define PMU_HPM_Vector_Micro_Op                   0x4B
#define PMU_HPM_ECC_Interrupt                     0x4C
#define PMU_HPM_Async_Abort_Interrupt             0x4D
#define PMU_HPM_IF_Stall                          0x4E
#define PMU_HPM_IP_Stall                          0x4F
#define PMU_HPM_IB_Stall                          0x50
#define PMU_HPM_IF_Refill_Stall                   0x51
#define PMU_HPM_IF_Mmu_Stall                      0x52
#define PMU_HPM_IB_Mispred_Stall                  0x53
#define PMU_HPM_IB_Fifo_Stall                     0x54
#define PMU_HPM_IB_Ind_Btb_Rd_Stall               0x55
#define PMU_HPM_IB_Vsetvl_Stall                   0x56
#define PMU_HPM_ID_Stall                          0x57
#define PMU_HPM_RF_Stall                          0x58
#define PMU_HPM_EU_Stall                          0x59
#define PMU_HPM_ID_Inst_Pipedown                  0x5A
#define PMU_HPM_RF_Inst_Pipedown                  0x5B
#define PMU_HPM_ID_One_Inst_Pipedown              0x5C
#define PMU_HPM_ID_CSR_Before_Fence_Stall         0x5D
#define PMU_HPM_ID_VSETVL_Fof_Stall               0x5E
#define PMU_HPM_ID_Flush_Stall                    0x5F
#define PMU_HPM_ID_Misprediction_Stall            0x60
#define PMU_HPM_ID_IID_Not_Vld_Stall              0x61
#define PMU_HPM_RF_One_Inst_Pipedown              0x62
#define PMU_HPM_RF_RAW_Stall                      0x63
#define PMU_HPM_RF_WAW_Stall                      0x64
#define PMU_HPM_RF_Structure_Stall                0x65
#define PMU_HPM_RF_CSR_After_Fence_Stall          0x66
#define PMU_HPM_EU_IU_Full                        0x67
#define PMU_HPM_EU_IU_Control_Full                0x68
#define PMU_HPM_EU_CP0_Full                       0x69
#define PMU_HPM_EU_LSU_LOAD_Full                  0x6A
#define PMU_HPM_EU_LSU_STORE_Full                 0x6B
#define PMU_HPM_EU_VFPU_Full                      0x6C
#define PMU_HPM_EU_BJU_Full                       0x6D
#define PMU_HPM_IU_Dp_Stall_Pipe0                 0x6E
#define PMU_HPM_IU_MULT_Stall_Pipe0               0x6F
#define PMU_HPM_IU_DIV_EX1_Stall_Pipe0            0x70
#define PMU_HPM_IU_Dp_Stall_Pipe1                 0x71
#define PMU_HPM_IU_MULT_Stall_Pipe1               0x72
#define PMU_HPM_IU_DIV_EX1_Stall_Pipe1            0x73
#define PMU_HPM_IU_DP_Wb_Conflict_Pipe0           0x74
#define PMU_HPM_IU_DP_WAW_Stall_Pipe0             0x75
#define PMU_HPM_IU_DP_Uncommit_Pipe0              0x76
#define PMU_HPM_IU_DP_Wb_Conflict_Pipe1           0x77
#define PMU_HPM_IU_DP_WAW_Stall_Pipe1             0x78
#define PMU_HPM_IU_DP_Uncommit_Pipe1              0x79
#define PMU_HPM_IU_MULT_Uncommit                  0x7A
#define PMU_HPM_IU_MULT_Wb_Stall                  0x7B
#define PMU_HPM_IU_DIV_Uncommit                   0x7C
#define PMU_HPM_IU_DIV_Wb_Stall                   0x7D
#define PMU_HPM_LSU_LOAD_WAW_Stall                0x7E
#define PMU_HPM_LSU_LOAD_Commit_Stall             0x7F
#define PMU_HPM_LSU_LOAD_RAW_Stall                0x80
#define PMU_HPM_LSU_STORE_Commit_Stall            0x81
#define PMU_HPM_Vidu_Rf_No_Pipedown               0x82
#define PMU_HPM_VPU_Stall_Pipe0                   0x83
#define PMU_HPM_VPU_Stall_Pipe1                   0x84
#define PMU_HPM_VPU_Struct_Hazard_Stall_Pipe0     0x85
#define PMU_HPM_VPU_Uncommit_Stall_Pipe0          0x86
#define PMU_HPM_VPU_VLSU_Stall_Pipe0              0x87
#define PMU_HPM_VPU_Struct_Hazard_Stall_Pipe1     0x88
#define PMU_HPM_VPU_Uncommit_Stall_Pipe1          0x89
#define PMU_HPM_VPU_VLSU_Stall_Pipe1              0x8A
#define PMU_HPM_VFPU_FDIV                         0x8B
#define PMU_HPM_VDIV_Busy                         0x8B
#define PMU_HPM_BJU_CP0_Stall                     0x8C
#define PMU_HPM_BJU_IBUF_Stall                    0x8D
#define PMU_HPM_BJU_Wb_Stall                      0x8E
#define PMU_HPM_BJU_Pipedown_Stall                0x8F
#define PMU_HPM_RTU_Flush                         0x90
#define PMU_HPM_RTU_IU_Not_No_OP                  0x91
#define PMU_HPM_RTU_BJU_Not_No_OP                 0x92
#define PMU_HPM_RTU_LSU_Not_No_OP                 0x93
#define PMU_HPM_RTU_CP0_Not_No_OP                 0x94
#define PMU_HPM_RTU_VFPU_Not_No_OP                0x95
#define PMU_HPM_RTU_Only_IU_Not_No_OP             0x96
#define PMU_HPM_RTU_Only_BJU_Not_No_OP            0x97
#define PMU_HPM_RTU_Only_LSU_Not_No_OP            0x98
#define PMU_HPM_RTU_Only_CP0_Not_No_OP            0x99
#define PMU_HPM_RTU_Only_VFPU_Not_No_OP           0x9A
#define PMU_HPM_L1_Dcache_Access                  0x9B
#define PMU_HPM_L1_Dcache_Miss                    0x9C
#define PMU_HPM_L1_Dcache_Exclusive_Eviction      0x9D
#define PMU_HPM_Icache_Prefetch                   0x9E
#define PMU_HPM_Dcache_Amr_Active                 0x9F
#define PMU_HPM_Icache_Prefetch_Miss              0xA0
#define PMU_HPM_Dcache_Refill_Casued_by_Prefetch  0xA1
#define PMU_HPM_Dcache_Hit_Caused_by_Prefetch     0xA2
#define PMU_HPM_Store_Dtlb_Miss                   0xA3
#define PMU_HPM_Load_Dtlb_Miss                    0xA4
#define PMU_HPM_L2_Access                         0xA5
#define PMU_HPM_L2_Miss                           0xA6
#define PMU_HPM_Snb_Read_Create_Vld               0xA7
#define PMU_HPM_Snb_Read_Create_Stall             0xA8
#define PMU_HPM_Snb_Write_Create_Vld              0xA9
#define PMU_HPM_Snb_Write_Create_Stall            0xAA
#define PMU_HPM_Iq_Full                           0xAB
#define PMU_HPM_Vidu_Vec0_Stall                   0xAC
#define PMU_HPM_Vidu_Vec1_Stall                   0xAD
#define PMU_HPM_Vidu_Vec0_Depend_Stall            0xAE
#define PMU_HPM_Vidu_Vec0_Struct_Hazard_Stall     0xAF
#define PMU_HPM_Vidu_Vec1_Depend_Stall            0xB0
#define PMU_HPM_Vidu_Vec1_Struct_Hazard_Stall     0xB1
#define PMU_HPM_Vidu_Total_Cycle                  0xB2
#define PMU_HPM_Vidu_Vec0_Cycle                   0xB3
#define PMU_HPM_Vidu_Vec1_Cycle                   0xB4

#define PMU_MHPM_EVENTS_MAX                       0xB4

extern char* perf_event_name[PMU_MHPM_EVENTS_MAX + 1];

#ifdef __cplusplus
}
#endif

#endif