#include <aes_test.h>

test_func_info_t aes_test_funcs_map[] = {
		{"AES_INTERFACE",test_aes_interface,1},
        {"AES_FUNC",test_aes_func,5}
};


int test_aes_main(char *args)
{
	uint8_t i;
	void *args_value = NULL;
	int ret;

	for(i=0;i<sizeof(aes_test_funcs_map)/sizeof(test_func_info_t);i++){
		if(!strcmp((void *)_mc_name, aes_test_funcs_map[i].name)){
			args_value = malloc(sizeof(uint32_t)*aes_test_funcs_map[i].args_num);
			if (args_value == NULL){
				TEST_CASE_WARN_QUIT("malloc space failed, unparsed parameter");
			}
			ret = args_parsing(args, (uint32_t *)args_value, aes_test_funcs_map[i].args_num);
			if (ret != 0){
				free(args_value);
				TEST_CASE_WARN_QUIT("parameter resolution error");
			}
			(*(aes_test_funcs_map[i].function))(args_value);
			free(args_value);
			return 0;
		}
	}

	TEST_CASE_TIPS("aes module don't support this command.");
	return -1;
}