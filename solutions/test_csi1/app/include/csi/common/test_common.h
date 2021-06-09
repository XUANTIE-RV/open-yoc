#ifndef __TEST_COMMON__
#define __TEST_COMMON__

typedef int (*test_func)(void *args);

typedef struct {
    char *test_func_name;
    test_func test_func_p;
}test_func_map;


typedef struct {
    char *name;
    test_func function;
    uint8_t args_num;
}test_func_info_t;


extern volatile int8_t _dev_idx;

extern int args_parsing(void *args, uint32_t *value, uint8_t num);
extern void cmd_parsing(void *cml, void *mc, void *args);

extern void transfer_data(char *addr, uint32_t size);
extern void dataset(char *addr, uint32_t size, uint8_t pattern);


extern void mdelay(uint32_t ms);

#endif
