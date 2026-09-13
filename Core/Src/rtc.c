/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* Private variables ---------------------------------------------------------*/
// 0-5:年-秒
uint8_t MyRTC_Time[6];

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
  RTC_TamperTypeDef sTamper = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the RTC Tamper
  */
  sTamper.Tamper = RTC_TAMPER_1;
  sTamper.Trigger = RTC_TAMPERTRIGGER_LOWLEVEL;
  if (HAL_RTCEx_SetTamper(&hrtc, &sTamper) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static uint8_t RTC_Get_MAXDay(uint8_t year,uint8_t month)
{
  uint8_t max_day = 31;
    switch (month)
    {
        case 4: case 6: case 9: case 11:
            max_day = 30;
            break;
        case 2:
          
            if ((year % 4) == 0) max_day = 29;
            else max_day = 28;
            break;
        default:
            max_day = 31;
            break;
    }
    return max_day;
}

void MyRTC_ReadTime(void)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  MyRTC_Time[0] = sDate.Year;
  MyRTC_Time[1] = sDate.Month;
  MyRTC_Time[2] = sDate.Date;
  MyRTC_Time[3] = sTime.Hours;
  MyRTC_Time[4] = sTime.Minutes;
  MyRTC_Time[5] = sTime.Seconds;
}

void MyRTC_SetTime(void)
{
  RTC_DateTypeDef sDate = {0};
  RTC_TimeTypeDef sTime = {0};

  sTime.Hours   = MyRTC_Time[3];
  sTime.Minutes = MyRTC_Time[4];
  sTime.Seconds = MyRTC_Time[5];
  

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
      
  }

  sDate.Year  = MyRTC_Time[0];
  sDate.Month = MyRTC_Time[1];
  sDate.Date  = MyRTC_Time[2];
  sDate.WeekDay = RTC_WEEKDAY_MONDAY; 

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
  }
}

void Change_RTC_Time(uint8_t index, uint8_t flag)
{
  MyRTC_ReadTime(); 

  int32_t val = MyRTC_Time[index];
  int32_t max_val = 0, min_val = 0;

  switch(index)
  {
      case 0: max_val = 99; min_val = 0; break; // Year
      case 1: max_val = 12; min_val = 1; break; // Month
      case 2: max_val = RTC_Get_MAXDay(MyRTC_Time[0], MyRTC_Time[1]); min_val = 1; break; // Date
      case 3: max_val = 23; min_val = 0; break; // Hour
      case 4: max_val = 59; min_val = 0; break; // Minute
      case 5: max_val = 59; min_val = 0; break; // Second
  }

  if(flag == 0) 
  {
      val++;
      if(val > max_val) val = min_val;
  }
  else 
  {
      val--;
      if(val < min_val) val = max_val;
  }

  MyRTC_Time[index] = (uint8_t)val;

  if(index == 0 || index == 1)
  {
      uint8_t current_max_day = RTC_Get_MAXDay(MyRTC_Time[0], MyRTC_Time[1]);
      if(MyRTC_Time[2] > current_max_day)
      {
          MyRTC_Time[2] = current_max_day;
      }
  }


  MyRTC_SetTime();
}

/* USER CODE END 1 */

