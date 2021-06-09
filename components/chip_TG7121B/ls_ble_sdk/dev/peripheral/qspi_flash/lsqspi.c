#include <string.h>
#include "lsqspi.h"
#include "field_manipulate.h"
#include "ls_dbg.h"
#include "lsqspi_msp.h"
#include "compile_flag.h"
#include "cpu.h"

XIP_BANNED void lsqspi_init(struct lsqspi_instance *inst)
{
    lsqspi_msp_init(inst);
    inst->reg->CSTIM = FIELD_BUILD(LSQSPI_AUTO_CS_HOLD,0);
    inst->reg->DLY = lsqspi_dly_get(inst);
    inst->reg->RDCAP = FIELD_BUILD(LSQSPI_DLY_RD_CAP, lsqspi_rd_cap_dly_get(inst));
    inst->reg->CFG = FIELD_BUILD(LSQSPI_BAUDRATE,lsqspi_baudrate_get(inst))|FIELD_BUILD(LSQSPI_DAC_ENABLE,1)
        |FIELD_BUILD(LSQSPI_CPHA,0)|FIELD_BUILD(LSQSPI_CPOL,0)|FIELD_BUILD(LSQSPI_ENABLE,1);
}

XIP_BANNED void lsqspi_direct_read_config(struct lsqspi_instance *inst,struct lsqspi_direct_read_config_param *param)
{
    inst->reg->MODE_BITS = param->mode_bits;
    inst->reg->RDINS = FIELD_BUILD(LSQSPI_RD_OPCODE, param->opcode) | FIELD_BUILD(LSQSPI_RD_ADDR_TRANS_TYPE,param->quad_addr)
        | FIELD_BUILD(LSQSPI_RD_DATA_TRANS_TYPE,param->quad_data) | FIELD_BUILD(LSQSPI_RD_MODE_BIT_EN,param->mode_bits_en)
        | FIELD_BUILD(LSQSPI_RD_NUM_DUMMY, param->dummy_bytes);
}

#if (defined(BOOT_ROM))
XIP_BANNED static void lsqspi_operation_wait(reg_lsqspi_t *reg)
{
    reg->CFG;
    reg->CFG;
    reg->CFG;
    reg->CFG;
    reg->CFG;
    reg->CFG;
    reg->CFG;
    reg->CFG;
    while(REG_FIELD_RD(reg->CFG, LSQSPI_IDLE)==0);
}
#else
XIP_BANNED static void lsqspi_operation_wait(reg_lsqspi_t *reg)
{
    while(REG_FIELD_RD(reg->CFG, LSQSPI_IDLE)==0);
}
#endif

XIP_BANNED static void stig_read_start(reg_lsqspi_t *reg,struct stig_start_param *param,uint8_t start_length,bool hold_cs,bool quad_data)
{
    reg->STIG_CMD = FIELD_BUILD(LSQSPI_CMD_OPCODE,param->opcode) | FIELD_BUILD(LSQSPI_OPCODE_EN, param->opcode_en) | FIELD_BUILD(LSQSPI_DATA_XFER_TYPE, quad_data)
        | FIELD_BUILD(LSQSPI_ADDR_EN, param->addr_en) | FIELD_BUILD(LSQSPI_ADDR_XFER_TYPE, param->quad_addr) 
        | FIELD_BUILD(LSQSPI_NUM_DUMMY_BYTES, param->dummy_bytes) | FIELD_BUILD(LSQSPI_DUMMY_EN, param->dummy_bytes_en)
        | FIELD_BUILD(LSQSPI_MODE_EN, param->mode_bits_en) | FIELD_BUILD(LSQSPI_NUM_RDATA_BYTES, start_length - 1)
        | FIELD_BUILD(LSQSPI_RDATA_EN, start_length ? 1 : 0);
    reg->STIG_ADDR = param->addr;
    reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_HOLD_CS, hold_cs) | FIELD_BUILD(LSQSPI_STIG_GO,1);
    lsqspi_operation_wait(reg);
    uint32_t data = reg->STIG_RD[0];
    memcpy(param->data,&data,start_length);
}

XIP_BANNED static void stig_read_continue(reg_lsqspi_t *reg,uint32_t *data,uint16_t size,bool quad_data)
{
    reg->STIG_CMD = FIELD_BUILD(LSQSPI_DATA_XFER_TYPE, quad_data) | FIELD_BUILD(LSQSPI_NUM_RDATA_BYTES, 8 - 1) | FIELD_BUILD( LSQSPI_RDATA_EN , 1);
    while(size > 8)
    {
         reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_HOLD_CS, 1) | FIELD_BUILD(LSQSPI_STIG_GO,1);
         size -= 8;
         lsqspi_operation_wait(reg);
         data[0] = reg->STIG_RD[0];
         data[1] = reg->STIG_RD[1];
         data += 2;
    }
    if(size)
    {
        reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_HOLD_CS, 0) | FIELD_BUILD(LSQSPI_STIG_GO,1);
        lsqspi_operation_wait(reg);
        uint32_t buf[2];
        buf[0] = reg->STIG_RD[0];
        buf[1] = reg->STIG_RD[1];
        memcpy(data,buf,size);
    }
}

XIP_BANNED static void stig_rd_wr(struct lsqspi_instance *inst,struct lsqspi_stig_rd_wr_param *param,
    void (*stig_start)(reg_lsqspi_t *reg,struct stig_start_param *param,uint8_t start_length,bool hold_cs,bool quad_data),
    void (*stig_continue)(reg_lsqspi_t *reg,uint32_t *data,uint16_t size,bool quad_data))
{
    uint8_t start_length;
    bool start_hold = true;
    if(param->size <= 4)
    {
        start_length = param->size;
        start_hold = false;
    }else
    {
        start_length = (uint32_t)param->start.data % 4 ? 4 - (uint32_t)param->start.data % 4 : 0;
    }
    enter_critical();
    inst->reg->MODE_BITS = param->mode_bits;
    stig_start(inst->reg,&param->start,start_length,start_hold,param->quad_data);
    stig_continue(inst->reg, (void *)(param->start.data + start_length), param->size - start_length , param->quad_data);
    exit_critical();
}


XIP_BANNED void lsqspi_stig_read_data(struct lsqspi_instance *inst,struct lsqspi_stig_rd_wr_param *param)
{
    stig_rd_wr(inst,param,stig_read_start,stig_read_continue);
}

XIP_BANNED static void stig_write_start(reg_lsqspi_t *reg,struct stig_start_param *param,uint8_t start_length,bool hold_cs,bool quad_data)
{
    reg->STIG_CMD = FIELD_BUILD(LSQSPI_CMD_OPCODE,param->opcode) | FIELD_BUILD(LSQSPI_OPCODE_EN, param->opcode_en) | FIELD_BUILD(LSQSPI_DATA_XFER_TYPE, quad_data)
        | FIELD_BUILD(LSQSPI_ADDR_EN, param->addr_en) | FIELD_BUILD(LSQSPI_ADDR_XFER_TYPE, param->quad_addr) 
        | FIELD_BUILD(LSQSPI_NUM_DUMMY_BYTES, param->dummy_bytes) | FIELD_BUILD(LSQSPI_DUMMY_EN, param->dummy_bytes_en)
        | FIELD_BUILD(LSQSPI_MODE_EN, param->mode_bits_en) | FIELD_BUILD(LSQSPI_NUM_WDATA_BYTES, start_length - 1)
        | FIELD_BUILD(LSQSPI_WDATA_EN, start_length ? 1 : 0);
    reg->STIG_ADDR = param->addr;
    uint32_t data;
    memcpy(&data,param->data,start_length);
    reg->STIG_WR[0] = data;
    reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_HOLD_CS, hold_cs) | FIELD_BUILD(LSQSPI_STIG_GO,1);
    lsqspi_operation_wait(reg);
}


XIP_BANNED static void stig_write_continue(reg_lsqspi_t *reg,uint32_t *data,uint16_t size,bool quad_data)
{
    reg->STIG_CMD = FIELD_BUILD(LSQSPI_DATA_XFER_TYPE, quad_data) | FIELD_BUILD(LSQSPI_NUM_WDATA_BYTES, 4 - 1) | FIELD_BUILD( LSQSPI_WDATA_EN , 1);
    while(size > 4)
    {
        reg->STIG_WR[0] = data[0];
        reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_HOLD_CS, 1) | FIELD_BUILD(LSQSPI_STIG_GO,1);
        data += 1;
        size -= 4;
        lsqspi_operation_wait(reg);
    }
    if(size)
    {
        reg->STIG_CMD = FIELD_BUILD(LSQSPI_DATA_XFER_TYPE, quad_data) | FIELD_BUILD(LSQSPI_NUM_WDATA_BYTES, size - 1) | FIELD_BUILD( LSQSPI_WDATA_EN , 1);
        reg->STIG_WR[0] = *data;
        reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_HOLD_CS, 0) | FIELD_BUILD(LSQSPI_STIG_GO,1);
        lsqspi_operation_wait(reg);
    }
}

XIP_BANNED void lsqspi_stig_write_data(struct lsqspi_instance *inst,struct lsqspi_stig_rd_wr_param *param)
{
    stig_rd_wr(inst,param,stig_write_start,stig_write_continue);
}

XIP_BANNED void lsqspi_stig_write_register(struct lsqspi_instance *inst,uint8_t opcode,uint8_t *data,uint8_t length)
{
    LS_RAM_ASSERT(length && length <= 4);
    struct lsqspi_stig_rd_wr_param param;
    param.start.data = data;
    param.start.opcode = opcode;
    param.start.dummy_bytes_en = 0;
    param.start.addr_en = 0;
    param.start.mode_bits_en = 0;
    param.start.opcode_en = 1;
    param.size = length;
    param.quad_data = false;
    lsqspi_stig_write_data(inst,&param);
}

XIP_BANNED void lsqspi_stig_send_command(struct lsqspi_instance *inst,uint8_t opcode)
{
    inst->reg->STIG_CMD = FIELD_BUILD(LSQSPI_CMD_OPCODE, opcode) | FIELD_BUILD(LSQSPI_OPCODE_EN, 1);
    inst->reg->STIG_GO = FIELD_BUILD(LSQSPI_STIG_GO, 1);
    lsqspi_operation_wait(inst->reg);
}

XIP_BANNED void lsqspi_stig_read_register(struct lsqspi_instance *inst,uint8_t opcode,uint8_t *data,uint8_t length)
{
    LS_RAM_ASSERT(length && length <= 8);
    struct lsqspi_stig_rd_wr_param param;
    param.start.data = data;
    param.start.opcode = opcode;
    param.start.dummy_bytes_en = 0;
    param.start.addr_en = 0;
    param.start.mode_bits_en = 0;
    param.start.opcode_en = 1;
    param.size = length;
    param.quad_data = false;
    lsqspi_stig_read_data(inst,&param);
}

