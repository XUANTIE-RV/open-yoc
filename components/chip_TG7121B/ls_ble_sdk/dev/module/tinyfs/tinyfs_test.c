#include "tinyfs.h"
#define RECORD_KEY1  1
#define RECORD1 "abcd"
#define RECORD_KEY2  2
#define RECORD2 "efgh"
#define RECORD_KEY3  3
#define RECORD3 "1234567890"


#define RECORD_KEY4  4
#define RECORD4 "abcd4"
#define RECORD_KEY5  5
#define RECORD5 "efgh5"
#define RECORD_KEY6  6
#define RECORD6 "12345678906"


uint8_t tmp[20];
void tinyfs_test()
{
#if 1
    tinyfs_dir_t dir0,dir1,dir2, dir3;
    uint8_t rslt = tinyfs_mkdir(&dir0,ROOT_DIR,5);
    rslt = tinyfs_mkdir(&dir1,dir0,5);
    rslt = tinyfs_mkdir(&dir2,dir1,5);
    rslt = tinyfs_mkdir(&dir3,dir2,5);
    
    rslt = tinyfs_write(dir2,RECORD_KEY1,RECORD1,sizeof(RECORD1));
    rslt = tinyfs_write(dir2,RECORD_KEY2,RECORD2,sizeof(RECORD2));
    rslt = tinyfs_write(dir1,RECORD_KEY3,RECORD3,sizeof(RECORD3));

    rslt = tinyfs_write(dir0,RECORD_KEY4,RECORD4,sizeof(RECORD4));
    rslt = tinyfs_write(dir1,RECORD_KEY6,RECORD6,sizeof(RECORD6));
    rslt = tinyfs_write(dir3,RECORD_KEY5,RECORD5,sizeof(RECORD5));
    
    //
#endif
    tinyfs_print_dir_tree();


    
    uint16_t length = 20;
    rslt = tinyfs_read(dir2,RECORD_KEY1,tmp,&length);
    length = 20;
    rslt = tinyfs_read(dir1,RECORD_KEY3,tmp,&length);
    rslt = tinyfs_read(dir2,RECORD_KEY2,tmp,&length);
    uint16_t list_length;
    rslt = tinyfs_record_list_get(dir2,&list_length,(uint16_t*)tmp);
    rslt = tinyfs_del_record(dir1,RECORD_KEY3);
    rslt = tinyfs_del_dir(dir0,false);
    rslt = tinyfs_del_dir(dir0,true);
}

