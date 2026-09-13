#ifndef __FREERTOS_DEMO__
#define __FREERTOS_DEMO__

#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "queue.h"
#include "semphr.h"
#include "string.h"
#include "event_groups.h"
void FreeRTOS_Start(void);
uint8_t Key_GetNum_RTOS(void);

#endif
