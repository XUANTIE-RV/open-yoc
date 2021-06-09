#include <spiflash_test.h>


test_func_info_t spiflash_test_funcs_map[] = {
	{"SPIFLASH_INTERFACE",test_spiflash_interface,1},
	{"SPIFLASH_GET_INFO",spiflash_get_info,8},
	{"SPIFLASH_ERASE_READ", spiflash_erase_read, 3},
	{"SPIFLASH_ERASE_WRITE_ERASE_READ", spiflash_rease_write_erase_read, 3},
	{"SPIFLASH_ERASE_OVERFLOW", spiflash_erase_overflow, 2},
	{"SPIFLASH_ERASE_CHIP", spiflash_erase_chip, 1},
	{"SPIFLASH_ERASE_WRITE_READ", spiflash_erase_write_read, 3},
	{"SPIFLASH_WRITE_OVERFLOW", spiflash_write_overflow, 3},
	{"SPIFLASH_READ_OVERFLOW", spiflash_read_overflow, 3},
	{"SPIFLASH_GET_STATUS", spiflash_get_status, 1}

};


int test_spiflash_main(char *args)
{
	uint8_t i;
	void *args_value = NULL;
	int ret;


	for(i=0;i<sizeof(spiflash_test_funcs_map)/sizeof(test_func_info_t);i++){
		if(!strcmp((void *)_mc_name, spiflash_test_funcs_map[i].name)){
			args_value = malloc(sizeof(uint32_t)*spiflash_test_funcs_map[i].args_num);
			if (args_value == NULL){
				TEST_CASE_WARN_QUIT("malloc space failed, unparsed parameter");
			}
			ret = args_parsing(args, (uint32_t *)args_value, spiflash_test_funcs_map[i].args_num);
			if (ret != 0){
				free(args_value);
				TEST_CASE_WARN_QUIT("parameter resolution error");
			}
			(*(spiflash_test_funcs_map[i].function))(args_value);
			free(args_value);
			return 0;
		}
	}

	TEST_CASE_TIPS("SPIFLASH module don't support this command.");
	return -1;
}
