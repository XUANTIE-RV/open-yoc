/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "crc_test.h"

char crc_data[CRC_DATA_LEN];
uint8_t src_data[CRC_DATA_LEN];
uint8_t out_data1[CRC_DATA_LEN];
uint16_t out_data2[CRC_DATA_LEN];
uint32_t out_data3[CRC_DATA_LEN];

int test_crc_func(char *args)
{
    int ret;
    test_crc_args_t crc_td;
    
    sscanf(args,"%d,%[^,],%d,%d",&crc_td.crc,&crc_td.crc_mode,crc_data,&crc_td.size);
    TEST_CASE_TIPS("CRC init value is:%d",crc_td.crc);
    TEST_CASE_TIPS("CRC testing mode is:%d",crc_td.crc_mode);
    hex_to_array(crc_data,crc_td.size,src_data);
    TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
    TEST_CASE_TIPS("CRC testing data size is:%d",crc_td.size);

    switch (crc_td.crc_mode)
    {
    case 0:
        out_data = csi_crc7_be(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 1:
        out_data = csi_crc8(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 2:
        out_data = csi_crc8_maxim(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 3:
        out_data2 = csi_crc16(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 4:
        out_data2 = csi_crc16_ccitt(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 5:
        out_data2 = csi_crc16_dnp(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 6:
        out_data2 = csi_crc16_itu(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;  
    case 7:
        out_data3 = csi_crc32_le(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    case 8:
        out_data3 = csi_crc32_be(crc_td.crc, src_data, crc_td.size);
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break; 
    default:
        TEST_CASE_ASSERT(1 == 0,"CRC mode choose error");
        TEST_PRINT_HEX("The result:", out_data, crc_td.size);
        break;
    }
}



int test_crc_performance(char *args)
{
    int ret;
    int time_ms;
    int num_of_loop;
    float performance;
    test_crc_args_t crc_td;
    uint8_t src_data[CRC_DATA_LEN];
    uint32_t get_data[5];
    uint8_t out_data1[CRC_DATA_LEN];
    uint16_t out_data2[CRC_DATA_LEN];
    uint32_t out_data3[CRC_DATA_LEN];
    
    ret = args_parsing(args,get_data,5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    crc_td.crc = (uint8_t)get_data[0];
    crc_td.crc_mode = (uint8_t)get_data[1];
    crc_td.size = (uint32_t)get_data[2];
    crc_td.rand_data_mode = (uint8_t)get_data[3];
    num_of_loop = (uint32_t)get_data[4];

    TEST_CASE_TIPS("CRC init value is:%d",crc_td.crc);
    TEST_CASE_TIPS("CRC testing mode is:%d",crc_td.crc_mode);
    TEST_CASE_TIPS("CRC testing data size is:%d",crc_td.size);
    TEST_CASE_TIPS("CRC data mode is:%d",crc_td.rand_data_mode);
    TEST_CASE_TIPS("CRC testing num_of_loop is:%d",num_of_loop);

    switch (crc_td.crc_mode)
    {
    case 0:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data = csi_crc7_be(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc7_be performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data = csi_crc7_be(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc7_be performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 1:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data = csi_crc8(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc8 performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data = csi_crc8(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc8 performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 2:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data = csi_crc8_maxim(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc8_maxim performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data = csi_crc8_maxim(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc8_maxim performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 3:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16 performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16 performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 4:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16_ccitt(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16_ccitt performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16_ccitt(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16_ccitt performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 5:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16_dnp(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16_dnp performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16_dnp(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16_dnp performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 6:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16_itu(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16_itu performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16_itu(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16_itu performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 7:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data3 = csi_crc32_le(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc32_le performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data3 = csi_crc32_le(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc32_le performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 8:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data3 = csi_crc32_be(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc32_be performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                // TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data3 = csi_crc32_be(crc_td.crc, src_data, crc_td.size);
                // TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc32_be performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    default:
        TEST_CASE_ASSERT(1 == 0,"CRC mode choose error");
        break;
    }
}


int test_crc_stability(char *args)
{
    int ret;
    int time_ms;
    int num_of_loop;
    float performance;
    test_crc_args_t crc_td;
    uint8_t src_data[CRC_DATA_LEN];
    uint32_t get_data[5];
    uint8_t out_data1[CRC_DATA_LEN];
    uint16_t out_data2[CRC_DATA_LEN];
    uint32_t out_data3[CRC_DATA_LEN];
    
    ret = args_parsing(args,get_data,5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    crc_td.crc = (uint8_t)get_data[0];
    crc_td.crc_mode = (uint8_t)get_data[1];
    crc_td.size = (uint32_t)get_data[2];
    crc_td.rand_data_mode = (uint8_t)get_data[3];
    num_of_loop = (uint32_t)get_data[4];

    TEST_CASE_TIPS("CRC init value is:%d",crc_td.crc);
    TEST_CASE_TIPS("CRC testing mode is:%d",crc_td.crc_mode);
    TEST_CASE_TIPS("CRC testing data size is:%d",crc_td.size);
    TEST_CASE_TIPS("CRC data mode is:%d",crc_td.rand_data_mode);
    TEST_CASE_TIPS("CRC testing num_of_loop is:%d",num_of_loop);

    switch (crc_td.crc_mode)
    {
    case 0:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data1 = csi_crc7_be(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data1, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc7_be performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data1 = csi_crc7_be(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data1, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc7_be performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 1:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data1 = csi_crc8(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data1, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc8 performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data1 = csi_crc8(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data1, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc8 performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 2:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data1 = csi_crc8_maxim(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data1, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc8_maxim performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data1 = csi_crc8_maxim(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data1, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc8_maxim performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 3:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16 performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16 performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 4:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16_ccitt(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16_ccitt performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16_ccitt(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16_ccitt performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 5:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16_dnp(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16_dnp performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16_dnp(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16_dnp performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 6:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data2 = csi_crc16_itu(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc16_itu performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data2 = csi_crc16_itu(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data2, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc16_itu performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 7:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data3 = csi_crc32_le(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc32_le performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data3 = csi_crc32_le(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc32_le performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    case 8:
        switch (crc_td.rand_data_mode)
        {
        case 0:
            tst_timer_restart();
            generate_rand_array(src_data,255,crc_td.size);
            TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
            for(int i = 0;i<num_of_loop;i++)
            {
                out_data3 = csi_crc32_be(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Only change data once:test crc32_be performance value is %f times/ms",performance);
            break;
        case 1:
            tst_timer_restart();
            for(int i = 0;i<num_of_loop;i++)
            {
                generate_rand_array(src_data,255,crc_td.size);
                TEST_PRINT_HEX("SRC DATA:", src_data, crc_td.size);
                out_data3 = csi_crc32_be(crc_td.crc, src_data, crc_td.size);
                TEST_PRINT_HEX("The result:", out_data3, crc_td.size);
            }
            time_ms = tst_timer_get_interval();
            performance = (1.0*num_of_loop)/time_ms;
            TEST_CASE_TIPS("Change data every loop:test crc32_be performance value is %f times/ms",performance);
            break;
        default:
            TEST_CASE_ASSERT(1 == 0,"CRC rand data mode choose error");
            break;
        }
        break;
    default:
        TEST_CASE_ASSERT(1 == 0,"CRC mode choose error");
        break;
    }
}
