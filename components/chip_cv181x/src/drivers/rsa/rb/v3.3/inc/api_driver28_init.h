/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* api_driver28_init.h
 *
 * Security-IP-28 Driver Initialization Interface
 */



#ifndef DRIVER28_INIT_H_
#define DRIVER28_INIT_H_


/*----------------------------------------------------------------------------
 * Driver28_Init
 *
 * Initialize the driver. This function must be called before any other
 * driver API function can be called.
 *
 * Returns 0 for success and -1 for failure.
 */
int
Driver28_Init(void);

/*----------------------------------------------------------------------------
 * Driver28_Exit
 *
 * Initialize the driver. After this function is called no other driver API
 * function can be called except Driver28_Init().
 */
void
Driver28_Exit(void);


#endif /* DRIVER28_INIT_H_ */

/* end of file api_driver28_init.h */
