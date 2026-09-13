/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "FreeRTOS_demo.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "mpu6050.h"
#include <stdio.h>
#include "usart.h"
#include "MAX30102.h"
#include "menu.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY_TASK_PRIORITY       5
#define KEY_STK_SIZE            128  

#define StopWatch_TASK_PRIORITY   3
#define LED_TASK_PRIORITY         3
#define ShowTime_TASK_PRIORITY    3
#define Menu_TASK_PRIORITY        3
#define Game_TASK_PRIORITY        3
#define Emoji_TASK_PRIORITY       3
#define MPU6050_TASK_PRIORITY     3
#define Gradienter_TASK_PRIORITY  3
#define SetTime_TASK_PRIORITY     3

#define StopWatch_STK_SIZE      128  
#define LED_STK_SIZE            128
#define ShowTime_STK_SIZE       256
#define Menu_STK_SIZE           256  
#define Game_STK_SIZE           256  
#define Emoji_STK_SIZE          256  
#define MPU6050_STK_SIZE        350  
#define Gradienter_STK_SIZE     350  
#define SetTime_STK_SIZE        350  

#define START_TASK_PRIORITY     1
#define START_STK_SIZE          128  
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

QueueHandle_t Key_Queue;

TaskHandle_t Start_Task_Handle;
TaskHandle_t Key_Task_Handle;
TaskHandle_t ShowTime_Task_Handle;
TaskHandle_t Menu_Task_Handle;
TaskHandle_t StopWatch_Task_Handle;
TaskHandle_t LED_Task_Handle;
TaskHandle_t MPU6050_Task_Handle;
TaskHandle_t Game_Task_Handle;
TaskHandle_t Emoji_Task_Handle;
TaskHandle_t Gradienter_Task_Handle;
TaskHandle_t SetTime_Task_Handle;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

osThreadId_t OLEDTaskHandle;
const osThreadAttr_t OLEDTask_attributes = {
  .name = "OLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t MPU6050TaskHandle;
const osThreadAttr_t MPU6050Task_attributes = {
  .name = "MPU6050Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t MAX30102TaskHandle;
const osThreadAttr_t MAX30102Task_attributes = {
  .name = "MAX30102Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


void StartOLEDTask(void *argument);
void StartMPU6050Task(void *argument);
void StartMAX30102Task(void *argument);

/*=================================*/
void Start_Task(void *pvParameters);
void Key_Task(void *pvParameters);
void ShowTime_Task(void *pvParameters);
void Menu_Task(void *pvParameters);
void StopWatch_Task(void *pvParameters);
void LED_Task(void *pvParameters);
void MPU6050_Task(void *pvParameters);
void Game_Task(void *pvParameters);
void Emoji_Task(void *pvParameters);
void Gradienter_Task(void *pvParameters);
void SetTime_Task(void *pvParameters);
/*==================================*/

/* USER CODE END FunctionPrototypes */



// MAX30102任务函数
void StartMAX30102Task(void *argument)
{
    MAX30102_Init();
    for(;;)
    {
      MAX30102_Print();

      // MAX30102_PrintData();
      osDelay(5000); // Delay for 1 second
    }
}

void FreeRTOS_Start(void)
{
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    Key_Queue = xQueueCreate(5, sizeof(uint8_t)); 

    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½
    xTaskCreate((TaskFunction_t)Start_Task,
                (const char*   )"Start_Task",
                (uint16_t      )START_STK_SIZE,
                (void*         )NULL,
                (UBaseType_t   )START_TASK_PRIORITY,
                (TaskHandle_t* )&Start_Task_Handle);

    vTaskStartScheduler();
}

//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
void Start_Task(void *pvParameters)
{
    taskENTER_CRITICAL(); 

    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    xTaskCreate(Key_Task, "Key_Task", KEY_STK_SIZE, NULL, KEY_TASK_PRIORITY, &Key_Task_Handle);

    xTaskCreate(ShowTime_Task, "Time_Task", ShowTime_STK_SIZE, NULL, ShowTime_TASK_PRIORITY, &ShowTime_Task_Handle);
    xTaskCreate(Menu_Task,     "Menu_Task", Menu_STK_SIZE, NULL, Menu_TASK_PRIORITY, &Menu_Task_Handle);
    xTaskCreate(Game_Task,     "Game",      Game_STK_SIZE, NULL, Game_TASK_PRIORITY, &Game_Task_Handle);
    xTaskCreate(Emoji_Task,    "Emoji",     Emoji_STK_SIZE, NULL, Emoji_TASK_PRIORITY, &Emoji_Task_Handle);
    xTaskCreate(StopWatch_Task, "StopWatch", StopWatch_STK_SIZE, NULL, StopWatch_TASK_PRIORITY, &StopWatch_Task_Handle);
    xTaskCreate(LED_Task,       "LED",       LED_STK_SIZE, NULL, LED_TASK_PRIORITY, &LED_Task_Handle);
    xTaskCreate(MPU6050_Task,   "MPU6050",   MPU6050_STK_SIZE, NULL, MPU6050_TASK_PRIORITY, &MPU6050_Task_Handle);
    xTaskCreate(Gradienter_Task,"Gradienter",Gradienter_STK_SIZE, NULL, Gradienter_TASK_PRIORITY, &Gradienter_Task_Handle);
    xTaskCreate(SetTime_Task,   "SetTime",   SetTime_STK_SIZE, NULL, SetTime_TASK_PRIORITY, &SetTime_Task_Handle);

    vTaskSuspend(Menu_Task_Handle);
    vTaskSuspend(StopWatch_Task_Handle);
    vTaskSuspend(LED_Task_Handle);
    vTaskSuspend(MPU6050_Task_Handle);
    vTaskSuspend(Game_Task_Handle);
    vTaskSuspend(Emoji_Task_Handle);
    vTaskSuspend(Gradienter_Task_Handle);
    vTaskSuspend(SetTime_Task_Handle);

    vTaskDelete(Start_Task_Handle); 
    taskEXIT_CRITICAL();            
}

// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
extern uint8_t Key_Num; 
void Key_Task(void *pvParameters)
{
    uint8_t key = 0;
    while(1)
    {
        if(Key_Num != 0)
        {
            key = Key_Num;
            Key_Num = 0; 
            xQueueSend(Key_Queue, &key, 0);
        }
        vTaskDelay(20); // 20msï¿½ï¿½ï¿½Ò»ï¿½ï¿½,ï¿½ï¿½ï¿½Ñ¼ï¿½Öµï¿½ï¿½ï¿½Íµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    }
}


//ï¿½Ó¶ï¿½ï¿½ï¿½ï¿½Ð¶ï¿½È¡ï¿½ï¿½Öµ
uint8_t Key_GetNum_RTOS(void)
{
    uint8_t key_val = 0;
    BaseType_t xStatus;
    
    // ï¿½ï¿½ï¿½ï¿½ 0 ï¿½ï¿½Ê¾ï¿½ï¿½ï¿½È´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ì·ï¿½ï¿½ï¿½
    xStatus = xQueueReceive(Key_Queue, &key_val, 0); 
    
    if(xStatus == pdPASS)
    {
        return key_val;
    }
    else
    {
        return 0; // Ã»ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½0
    }
}

//UIï¿½ï¿½ï¿½ï¿½
void ShowTime_Task(void *pvParameters)
{
    int flag;
    while(1)
    {
        flag = First_Page_Clock(); 
        
        if(flag == 1) // ï¿½ï¿½ï¿½ï¿½Ëµï¿½
        {
            vTaskResume(Menu_Task_Handle);
            vTaskSuspend(NULL);
        }
        else if(flag == 2) // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        {
             vTaskResume(SetTime_Task_Handle);
             vTaskSuspend(NULL);
        }
        vTaskDelay(5); //ï¿½Í·ï¿½CPU
    }
}

void Menu_Task(void *pvParameters)
{
    int selection;
    while(1)
    {
        selection = Menu(); // Menuï¿½ï¿½ï¿½Ú·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        
        switch(selection)
        {
            case 0: // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò³
                vTaskResume(ShowTime_Task_Handle);
                vTaskSuspend(NULL);
                break;
            case 2: // ï¿½ï¿½ï¿½
                vTaskResume(StopWatch_Task_Handle);
                vTaskSuspend(NULL);
                break;
            case 3: // ï¿½Öµï¿½Í²
                vTaskResume(LED_Task_Handle);
                vTaskSuspend(NULL);
                break;
            case 4: // MPU6050
                vTaskResume(MPU6050_Task_Handle);
                vTaskSuspend(NULL);
                break;
            case 5: // ï¿½ï¿½Ï·
                vTaskResume(Game_Task_Handle);
                vTaskSuspend(NULL);
                break;
            case 6: // ï¿½ï¿½ï¿½ï¿½
                vTaskResume(Emoji_Task_Handle);
                vTaskSuspend(NULL);
                break;
            case 7: // Ë®Æ½ï¿½ï¿½
                vTaskResume(Gradienter_Task_Handle);
                vTaskSuspend(NULL);
                break;
            default: 
                vTaskResume(ShowTime_Task_Handle);
                vTaskSuspend(NULL);
                break;
        }
        vTaskDelay(5);
    }
}

void StopWatch_Task(void *pvParameters)
{
    while(1)
    {
        MenuToFunction();
		    StopWatch(); //ï¿½ï¿½ï¿½ï¿½ï¿½Ë³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ñ£¬·ï¿½ï¿½ï¿½Ò»Ö±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½â£¬ï¿½ï¿½ï¿½ï¿½MenuToFunction();ï¿½ï¿½ï¿½á±»ï¿½ï¿½ï¿½ï¿½Ö´ï¿½ï¿½
        vTaskResume(Menu_Task_Handle);//Ò»ï¿½ï¿½ï¿½Ë³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Òªï¿½ï¿½ï¿½Â³ï¿½Ê¼ï¿½ï¿½ï¿½Ëµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        vTaskSuspend(NULL);//ï¿½ï¿½FreeRTOSï¿½ï¿½Ö±ï¿½Ó»Øµï¿½ï¿½Ëµï¿½ï¿½ï¿½ï¿½ï¿½Íºï¿½ï¿½ï¿½
        vTaskDelay(5);
    }
}

void LED_Task(void *pvParameters)
{
    while(1)
    {
        MenuToFunction();
		LED();
        vTaskResume(Menu_Task_Handle);
        vTaskSuspend(NULL);
        vTaskDelay(5);
    }
}

void MPU6050_Task(void *pvParameters)
{
    while(1)
    {
		MenuToFunction();
        MPU6050();
        vTaskResume(Menu_Task_Handle);
        vTaskSuspend(NULL);
        vTaskDelay(5);
    }
}

void Game_Task(void *pvParameters)
{
    while(1)
    {
        MenuToFunction();
		Game(); 
        vTaskResume(Menu_Task_Handle);
        vTaskSuspend(NULL);
        vTaskDelay(5);
    }
}

void Emoji_Task(void *pvParameters)
{
    while(1)
    {
        MenuToFunction();
		Emoji();
        vTaskResume(Menu_Task_Handle);
        vTaskSuspend(NULL);
        vTaskDelay(5);
    }
}

void Gradienter_Task(void *pvParameters)
{
    while(1)
    {
        MenuToFunction();
		Gradienter();
        vTaskResume(Menu_Task_Handle);
        vTaskSuspend(NULL);
        vTaskDelay(5);
    }
}

void SetTime_Task(void *pvParameters)
{
    while(1)
    {
        SettingPage(); 
        vTaskResume(ShowTime_Task_Handle); 
        vTaskSuspend(NULL);
        vTaskDelay(5);
    }
}

/* USER CODE END Application */

