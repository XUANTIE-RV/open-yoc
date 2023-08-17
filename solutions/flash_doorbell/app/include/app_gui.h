/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef APP_GUI_H
#define APP_GUI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>

void GUI_Init(void);
int GUI_IR_Display_Start();
int GUI_IR_Display_Stop();
void GUI_IR_Display_Enable(int flag);
void GUI_Show_FaceRecog_Result(bool success);
void GUI_Show_Register_Result(bool success);
void GUI_Show_Text(const char *text);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* APP_GUI_H */