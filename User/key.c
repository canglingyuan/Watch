#include "key.h"
#include "main.h"
#include "stm32f1xx_hal_gpio.h"
#include "cmsis_os.h"
#include <stdint.h>

static int press_time=0;
uint8_t IsKey;

uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if(IsKey)
	{
		Temp=IsKey;
		IsKey=0;
		return Temp;
	}
	else
	{
		return 0;
	}
}

void KeyEnter_Tick(void)
{
    if(HAL_GPIO_ReadPin(KEY_Enter_GPIO_Port, KEY_Enter_Pin)==KEY_DOWN)
    {
        press_time++;
    }
    if(HAL_GPIO_ReadPin(KEY_Enter_GPIO_Port, KEY_Enter_Pin)==KEY_UP)
    {
        press_time=0;
    }
}

uint8_t KeyAll_GetState(void)
{
    if( HAL_GPIO_ReadPin(KEY_Enter_GPIO_Port, KEY_Enter_Pin) == GPIO_PIN_RESET)
    {
        if(press_time>1000)
            return 4;
        else
            return 3;
    }
    else if(HAL_GPIO_ReadPin(KEY_Last_GPIO_Port, KEY_Last_Pin) == GPIO_PIN_RESET)
    {
        return 1;
    }
    else if(HAL_GPIO_ReadPin(KEY_Next_GPIO_Port, KEY_Next_Pin) == PIO_PIN_RESET)
    {
        return 2;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==KEY_Enter)
    {
        osDelay(30);
        while(Key_GetState(KEY_Enter)==KEY_DOWN);
        osDelay(50);
    }
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrentState,PreState;
	Count++;
	if(Count>=20)
	{
		Count=0;
		PreState=CurrentState;
		CurrentState=Key_GetState();
		if(PreState!=0 && CurrentState==0)
		{
			IsKey=PreState;
		}
	}
}