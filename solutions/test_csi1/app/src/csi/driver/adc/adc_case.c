#include <adc_test.h>
#include <stdlib.h>

#define MAX_CHS 16
#define SCAN_DATA_NUM  4
#define TEST_SINGLE_CONV_TIMES        1
#define N 128
#define TEST_ADC_SINGLE_CHANNEL0        0
#define TEST_ADC_SINGLE_CHANNEL1        1
#define TEST_ADC_SINGLE_CHANNEL2        2
#define TEST_ADC_SINGLE_CHANNEL3        3
#define TEST_ADC_SINGLE_CHANNEL4        4
#define TEST_ADC_SINGLE_CHANNEL5        5
#define TEST_ADC_SINGLE_CHANNEL6        6
#define TEST_ADC_SINGLE_CHANNEL7        7

static volatile uint32_t scan_complete_flag;


/*
void adc_callback(int32_t idx, adc_event_e event)
{
    if (event == ADC_EVENT_CONVERSION_COMPLETE){
        scan_complete_flag = 1;
    }
}
*/


int test_adc_func(char *args)
{
    int32_t ret,i,ch_num;
    adc_handle_t hd;
    test_adc_args_t td;
    volatile adc_status_t adc_status;
    uint32_t data[4 + 16];
    uint32_t receive_data[N]={0};
    uint32_t receive_data2[N]={0};
//    uint32_t str_length = strlen(receive_data);

    uint8_t ch_ids[MAX_CHS]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    uint32_t get_data[7];

    ret = args_parsing(args, get_data, 7);//解析AT指令
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");
 
    td.idx = get_data[0];//设备号==0
    td.mode = get_data[1];//1: single, 2:continue, 3: scan
    td.cmp_mode = get_data[2];
    td.max = get_data[3];//预期最大值
    td.min = get_data[4];//预期最小值
    td.s_time = get_data[5];//采样时间
    td.count = get_data[6];//转换次数
//    td.transfer_mode =get_data[7];//1:intrpt mode,0:polling mode
    adc_conf_t adc_config;
    adc_cmp_conf_t adc_cmp_config;

    uint32_t mode_array[3] = {ADC_SINGLE,ADC_CONTINUOUS,ADC_SCAN};
    uint32_t ch_array[1] = {TEST_ADC_SINGLE_CHANNEL5};
    uint32_t ch_arrays[4]={TEST_ADC_SINGLE_CHANNEL5,TEST_ADC_SINGLE_CHANNEL6,TEST_ADC_SINGLE_CHANNEL7,TEST_ADC_SINGLE_CHANNEL4};
    uint32_t ch_arrayss[4]={TEST_ADC_SINGLE_CHANNEL4,TEST_ADC_SINGLE_CHANNEL4,TEST_ADC_SINGLE_CHANNEL4,TEST_ADC_SINGLE_CHANNEL4};
//    uint32_t ch_arrayssss[2]={TEST_ADC_SINGLE_CHANNEL4,TEST_ADC_SINGLE_CHANNEL5};
    adc_config.clk_prescaler=0;
    adc_config.mode=mode_array[td.mode];
    adc_config.trigger=0;

    adc_config.conv_cnt=td.count;
    adc_config.sampling_time=td.s_time;
    adc_config.offset=0;

    adc_cmp_config.cmp_data=4000;
    adc_cmp_config.cmp_condition = 1;
    adc_cmp_config.cmp_match_cnt = 16;

    TEST_CASE_TIPS("mode is %d",mode_array[td.mode]);
    TEST_CASE_TIPS("max_value is %d",td.max);
    TEST_CASE_TIPS("min_value is %d",td.min);

    for(i=0;i<1;i++){
        #if 0
        if(adc_config.mode == ADC_SINGLE && td.transfer_mode == 1){
            hd = drv_adc_initialize(td.idx,adc_callback);
            TEST_CASE_ASSERT(hd != NULL,"adc init failed,idx:%d",td.idx);
            adc_config.intrp_mode=1;
            adc_config.channel_array=ch_array;
            adc_config.channel_nbr=1;

            ret = drv_adc_config(hd,&adc_config);
            TEST_CASE_ASSERT(ret == 0,"ADC config failed,act_ret=%d",ret);

            if(td.cmp_mode == 1){
                adc_cmp_config.cmp_channel = ch_array[0];
                ret = drv_adc_comparator_config(hd,&adc_cmp_config);
                TEST_CASE_ASSERT(ret == 0,"ADC COMPARATOR config failed,act_ret=%d",ret);
                TEST_CASE_TIPS("Cmp mode config done.\n");
            }

            TEST_CASE_READY();
            TEST_CASE_TIPS("begin to test ADC_SINGLE && intrpt mode...");
            TEST_CASE_TIPS("cmp_mode = %d",td.cmp_mode);

            ret = drv_adc_start(hd);
            TEST_CASE_ASSERT(ret == 0,"ADC start failed,act_ret=%d",ret);

            while(adc_status.busy != 0){
                adc_status = drv_adc_get_status(hd);
            }
            
            ret = drv_adc_read(hd,receive_data,1);
            TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
            TEST_CASE_TIPS("Channel ID:%d, read_value:%d",ch_ids[i],ret);
            TEST_CASE_TIPS("receive_data is %d",receive_data[0]);

            while(!scan_complete_flag);
            printf("interrupt done...");

            TEST_CASE_ASSERT_QUIT((receive_data[0] >= td.min && receive_data[0] <= td.max),"Test ADC_SINGLE mode done...");

            adc_status = drv_adc_get_status(hd);
            TEST_CASE_ASSERT((adc_status.busy == 0),"ret_status.busy == 0");
        #endif
        if(adc_config.mode == ADC_SINGLE)
        {
            hd = drv_adc_initialize(td.idx,NULL);
            TEST_CASE_ASSERT(hd != NULL,"adc init failed,idx:%d",td.idx);
            adc_config.intrp_mode=0;
            adc_config.channel_array=ch_array;
            adc_config.channel_nbr=1;

            ret = drv_adc_config(hd,&adc_config);
            TEST_CASE_ASSERT(ret == 0,"ADC config failed,act_ret=%d",ret);

            if(td.cmp_mode == 1){
                adc_cmp_config.cmp_channel = ch_array[0];
                ret = drv_adc_comparator_config(hd,&adc_cmp_config);
                TEST_CASE_ASSERT(ret == 0,"ADC COMPARATOR config failed,act_ret=%d",ret);
                TEST_CASE_TIPS("Cmp mode config done.\n");
            }

            TEST_CASE_READY();
            TEST_CASE_TIPS("begin to test ADC_SINGLE mode ");
            TEST_CASE_TIPS("cmp_mode = %d",td.cmp_mode);

            ret = drv_adc_start(hd);
            TEST_CASE_ASSERT(ret == 0,"ADC start failed,act_ret=%d",ret);

            while(adc_status.busy != 0){
            adc_status = drv_adc_get_status(hd);
            }
            
            ret = drv_adc_read(hd,receive_data,td.count);
            TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
            TEST_CASE_TIPS("Channel ID:%d, read_value:%d",ch_ids[i],ret);
            
            TEST_CASE_TIPS("receive_data is %d",receive_data[0]);

            TEST_CASE_ASSERT_QUIT((receive_data[0] >= td.min && receive_data[0] <= td.max),"Test ADC_SINGLE mode done...");

            adc_status = drv_adc_get_status(hd);
            TEST_CASE_ASSERT((adc_status.busy == 0),"ret_status.busy == 0");

        }
    }

    if(adc_config.mode == ADC_CONTINUOUS)
    {
        hd = drv_adc_initialize(td.idx,NULL);
        TEST_CASE_ASSERT(hd != NULL,"adc init failed,idx:%d",td.idx);
        adc_config.intrp_mode=0;
        adc_config.channel_array=ch_array;
        adc_config.channel_nbr=1;
        ret = drv_adc_config(hd,&adc_config);
        TEST_CASE_ASSERT(ret == 0,"ADC config failed,act_ret=%d",ret);

        if((bool)td.cmp_mode == true){
            adc_cmp_config.cmp_channel = ch_arrayss[4];
            ret = drv_adc_comparator_config(hd,&adc_cmp_config);
            TEST_CASE_ASSERT_QUIT(ret == 0,"ADC COMPARATOR config failed,act_ret=%d",ret);
            TEST_CASE_TIPS("CMP config done.\n");
        }

        TEST_CASE_READY();
        TEST_CASE_TIPS("begin to test ADC_CONTINUOUS mode...");

        ret = drv_adc_start(hd);
        TEST_CASE_ASSERT(ret == 0,"ADC start failed,act_ret=%d",ret);

        while(adc_status.busy != 0){
        	adc_status = drv_adc_get_status(hd);
        }

        ret = drv_adc_read(hd,&receive_data2[0],td.count);
        TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
        while(adc_status.busy != 0){
       		 adc_status = drv_adc_get_status(hd);
        }
        TEST_CASE_TIPS("receive_data is %d",receive_data2[0]);

        TEST_CASE_ASSERT((receive_data[0] >= td.min && receive_data[0] <= td.max),"Test ADC_CONTINUOUS mode done...");

        adc_status = drv_adc_get_status(hd);
        TEST_CASE_ASSERT((adc_status.busy == 0),"ret_status.busy != 0");
    }
#if 0
    if(adc_config.mode == ADC_CONTINUOUS  && td.transfer_mode == 1)
    {
        hd = drv_adc_initialize(td.idx,adc_callback);
        TEST_CASE_ASSERT(hd != NULL,"adc init failed,idx:%d",td.idx);
        adc_config.intrp_mode=1;
        adc_config.channel_array=ch_arrayss;
        adc_config.channel_nbr=1;
        ret = drv_adc_config(hd,&adc_config);
        TEST_CASE_ASSERT(ret == 0,"ADC config failed,act_ret=%d",ret);

        if((bool)td.cmp_mode == true){
            adc_cmp_config.cmp_channel = ch_arrayss[4];
            ret = drv_adc_comparator_config(hd,&adc_cmp_config);
            TEST_CASE_ASSERT_QUIT(ret == 0,"ADC COMPARATOR config failed,act_ret=%d",ret);
            TEST_CASE_TIPS("CMP config done.\n");
        }

        TEST_CASE_READY();
        TEST_CASE_TIPS("begin to test ADC_CONTINUOUS mode...");

        ret = drv_adc_start(hd);
        TEST_CASE_ASSERT(ret == 0,"ADC start failed,act_ret=%d",ret);

        while(adc_status.busy != 0){
        adc_status = drv_adc_get_status(hd);
        }

        ret = drv_adc_read(hd,&receive_data2[0],1);
        TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
        TEST_CASE_TIPS("receive_data is %d",receive_data2[0]);

        while(!scan_complete_flag);
        printf("interrupt done...");
        
        TEST_CASE_ASSERT((receive_data[0] >= td.min && receive_data[0] <= td.max),"Test ADC_CONTINUOUS mode done...");

        adc_status = drv_adc_get_status(hd);
        TEST_CASE_ASSERT((adc_status.busy == 0),"ret_status.busy != 0");
    }
#endif
/*
    if(adc_config.mode == ADC_CONTINUOUS){
        hd = drv_adc_initialize(td.idx,NULL);
        TEST_CASE_ASSERT(hd != NULL,"adc init failed,idx:%d",td.idx);

        
            ch_num = 2;
            adc_config.channel_array=&ch_arrayssss[i];
            adc_config.channel_nbr=1;
            ret = drv_adc_config(hd,&adc_config);
            TEST_CASE_ASSERT(ret == 0,"ADC config failed,act_ret=%d",ret);

            if((bool)td.cmp_mode == true){
                adc_cmp_config.cmp_channel = ch_arrayss[4];
                ret = drv_adc_comparator_config(hd,&adc_cmp_config);
                TEST_CASE_ASSERT_QUIT(ret == 0,"ADC COMPARATOR config failed,act_ret=%d",ret);
                TEST_CASE_TIPS("CMP config done.\n");
            }
        
            i=0;
            TEST_CASE_READY();
            TEST_CASE_TIPS("begin to test ADC_CONTINUOUS mode...");

            ret = drv_adc_start(hd);
            TEST_CASE_ASSERT(ret == 0,"ADC start failed,act_ret=%d",ret);

            while(adc_status.busy != 0){
            adc_status = drv_adc_get_status(hd);
            }

            ret = drv_adc_read(hd,&receive_data2[0],1);
            TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
            TEST_CASE_TIPS("receive_data is %d,ch_num is %d\n",receive_data2[0],0);

            ret = drv_adc_read(hd,&receive_data2[1],1);
            TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
            TEST_CASE_TIPS("receive_data is %d,ch_num is %d\n",receive_data2[1],1);

        //    TEST_CASE_ASSERT((receive_data[0] >= td.min && receive_data[0] <= td.max),"Test ADC_CONTINUOUS mode done...");

            adc_status = drv_adc_get_status(hd);
            TEST_CASE_ASSERT((adc_status.busy == 0),"ret_status.busy != 0");

            ret = drv_adc_stop(hd);
            TEST_CASE_ASSERT_QUIT(ret == 0,"ADC stop failed,act_ret=%d",ret);

            adc_status = drv_adc_get_status(hd);
            TEST_CASE_ASSERT_QUIT((adc_status.busy == 0),"ret_status.busy != 0");
            
    }
*/

    if(adc_config.mode == ADC_SCAN)
    {
        adc_config.intrp_mode=0;
        adc_config.trigger=0;
        adc_config.channel_array=ch_arrays;
        adc_config.channel_nbr=4;

        hd = drv_adc_initialize(td.idx,NULL);
        TEST_CASE_ASSERT(hd != NULL,"adc init failed,idx:%d",td.idx);

        ret = drv_adc_config(hd,&adc_config);
        TEST_CASE_ASSERT(ret == 0,"ADC config failed,act_ret=%d",ret);

        TEST_CASE_TIPS("begin to test ADC_SCAN mode...\n");

        if((bool)td.cmp_mode == true){
            adc_cmp_config.cmp_channel = ch_arrays[4];
            ret = drv_adc_comparator_config(hd,&adc_cmp_config);
            TEST_CASE_ASSERT(ret == 0,"ADC COMPARATOR config failed,act_ret=%d",ret);
            TEST_CASE_TIPS("CMP config done.\n");
        }

        TEST_CASE_READY();
        TEST_CASE_TIPS("begin to test ADC_SCAN mode ");

        ret = drv_adc_start(hd);
        TEST_CASE_ASSERT(ret == 0,"ADC start failed,act_ret = %d",ret);
        while(adc_status.busy != 0)
        {
            adc_status = drv_adc_get_status(hd);
        }
        ret = drv_adc_read(hd,&data[0],SCAN_DATA_NUM);

        TEST_CASE_ASSERT(ret == 0,"ADC read failed,act_ret=%d",ret);
        TEST_CASE_TIPS("Channel ID:%d, read_value:%d",ch_ids[0],ret);
        
        while(adc_status.busy != 0){
            adc_status = drv_adc_get_status(hd);
        }

        for(i=0;i<sizeof(ch_arrays);i++){
            TEST_CASE_TIPS("receive_data is %d",data[i]);
        }

        TEST_CASE_ASSERT_QUIT((receive_data[0] >= td.min && receive_data[0] <= td.max),"Test ADC_SCAN mode done...");

        adc_status = drv_adc_get_status(hd);
        TEST_CASE_ASSERT((adc_status.busy == 0),"ret_status.busy == 0");
    }

    ret = drv_adc_stop(hd);
    TEST_CASE_ASSERT_QUIT(ret == 0,"ADC stop failed,act_ret=%d",ret);

    adc_status = drv_adc_get_status(hd);
    TEST_CASE_ASSERT_QUIT((adc_status.busy == 0),"ret_status.busy != 0");

    drv_adc_uninitialize(hd);

    return 0;
}


