#include "key.h"
#include "main.h"
#include "stm32f1xx_hal_gpio.h"

uint8_t Key_GetState(uint8_t key_num)
{
    switch (key_num)
    {
        case KEY_Enter:
            return HAL_GPIO_ReadPin(KEY_Enter_GPIO_Port, KEY_Enter_Pin);
        case KEY_Next:
            return HAL_GPIO_ReadPin(KEY_Next_GPIO_Port, KEY_Next_Pin);
        case KEY_Back:
            return HAL_GPIO_ReadPin(KEY_Back_GPIO_Port, KEY_Back_Pin);

        default:
            return KEY_UNKNOWN;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==)
}