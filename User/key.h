#ifndef __KEY_H__
#define __KEY_H__

#include <stdint.h>

enum KEY_NUM{KEY_Last,KEY_Next,KEY_Enter};
// enum KEY_STATE{KEY_DOWN,KEY_UP,KEY_UNKNOWN};

uint8_t Key_GetNum(void);
uint8_t KeyAll_GetState(void);

#endif