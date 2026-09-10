#ifndef __KEY_H__
#define __KEY_H__

#include <stdint.h>

enum KEY_NUM{KEY_Enter,KEY_Next,KEY_Back};
enum KEY_STATE{KEY_DOWN,KEY_UP,KEY_UNKNOWN};

uint8_t Key_GetState(uint8_t key_num);

#endif