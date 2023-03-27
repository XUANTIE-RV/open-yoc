#ifndef __GUI_DISPLAY_H__
#define __GUI_DISPLAY_H__
#include "cvi_type.h"
#define GUILINENUMBER 10


CVI_S32 GUI_Display_Start();
CVI_S32 GUI_Display_Stop();
void Gui_Label_SendEvent(CVI_U8 EventType,void *Data);





#endif