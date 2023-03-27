/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#ifndef ___GREENUI_KEYINPUT_H___
#define ___GREENUI_KEYINPUT_H___

#include <stdint.h>
#include <string>
#include "input/KeyCodes.h"

namespace JQuick
{
enum KeyAction
{
    KEY_ACTION_DOWN = 1,
    KEY_ACTION_UP = 2,
};
/**
 * 注入按键事件
 * @param action：1:down; 2:up
 * @param keyCode：键值
 * @param repeatCount：长按时(action为down)的重复事件次数，刚按下时为0
 * @return 返回true表示该事件已被消费；input入口中可考虑依据该返回值，决定是否将事件继续分发给全局监听者。
 */
bool injectKeyEvent(KeyAction action, KeyCode keyCode, int repeatCount);
}

#endif  // ___GREENUI_KEYINPUT_H___
