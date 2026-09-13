#include "rtc.h"
#include "oled.h"
#include "key.h"
#include "stm32f1xx_hal_rtc.h"
#include <stdint.h>

const uint8_t Return[]={
	0x00,0x00,0x80,0xC0,0xE0,0xF0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0xF8,0x00,
	0x00,0x01,0x03,0x07,0x0F,0x1F,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,
}; //返回图标

RTC_DateTypeDef RTC_Date;
RTC_TimeTypeDef RTC_Time;

// 设置日期UI
void DateSet_UI()
{
    OLED_Clear();

    OLED_ShowImage_Reverse(0, 0, 16, 16, Return, 1);
    OLED_ShowCN(2,1,8,1); //年
    OLED_ShowCN(3,1,9,1); // 月
    OLED_ShowCN(4,1,4,1); // 日
    OLED_ShowString(2,3,":",8);
    OLED_ShowString(3,3,":",8);
    OLED_ShowString(4,3,":",8);
    OLED_ShowNum(2,4,20,2,8); // 显示20xx

    HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &RTC_Time, RTC_FORMAT_BIN);

    OLED_ShowNum_Reverse(2, 6, RTC_Date.Year, 2, 8, 1);
    OLED_ShowNum_Reverse(3, 4, RTC_Date.Month, 2, 8, 1);
    OLED_ShowNum_Reverse(4, 4, RTC_Date.Date, 2, 8, 1);
}

// 设置时间UI
void TimeSet_UI()
{
    OLED_Clear();

    OLED_ShowImage_Reverse(0, 0, 16, 16, Return, 1);
    OLED_ShowCN(1,1,6,1); // 时
    OLED_ShowCN(2,1,10,1); // 分
    OLED_ShowCN(3,1,11,1); // 秒
    OLED_ShowString(1,3,":",8);
    OLED_ShowString(2,3,":",8);
    OLED_ShowString(3,3,":",8);

    HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &RTC_Time, RTC_FORMAT_BIN);

    OLED_ShowNum_Reverse(1, 4, RTC_Time.Hours, 2, 8, 1);
    OLED_ShowNum_Reverse(2, 4, RTC_Time.Minutes, 2, 8, 1);
    OLED_ShowNum_Reverse(3, 4, RTC_Time.Seconds, 2, 8, 1);
}

uint8_t KeyNum1,KeyNum2,KeyNum3,KeyNum4,KeyNum5,KeyNum6;

// 1、设置年份
int SetYear(void)
{
    while(1)
    {
        KeyNum1=Key_GetNum();
        if(KeyNum1==1)  Change_RTC_Time(0,1);
        else if(KeyNum1 == 2) Change_RTC_Time(0, 0); 
        else if(KeyNum1 == 3) return 1;

        DateSet_UI();
        OLED_ShowNum_Reverse(2, 6, MyRTC_Time[0], 2, 8, 0);
        OLED_Update();
    }
}

// 2、设置月份
int SetMonth(void)
{
    while(1)
    {
        KeyNum2 = Key_GetNum();
        if(KeyNum2 == 1)      Change_RTC_Time(1, 1); 
        else if(KeyNum2 == 2) Change_RTC_Time(1, 0); 
        else if(KeyNum2 == 3) return 1;

        DateSet_UI();
        OLED_ShowNum_Reverse(3, 4, MyRTC_Time[1], 2, 8, 0);
        OLED_Update();
    }
}

// 3、设置日期
int SetDay(void)
{
    while(1)
    {
        KeyNum3 = Key_GetNum();
        if(KeyNum3 == 1)      Change_RTC_Time(2, 1); 
        else if(KeyNum3 == 2) Change_RTC_Time(2, 0); 
        else if(KeyNum3 == 3) return 1;

        DateSet_UI();
        OLED_ShowNum_Reverse(4, 4, MyRTC_Time[2], 2, 8, 0);
        OLED_Update();
    }
}

// 4、设置时针
int SetHour(void)
{
    while(1)
    {
        KeyNum4 = Key_GetNum();
        if(KeyNum4 == 1)      Change_RTC_Time(3, 1); 
        else if(KeyNum4 == 2) Change_RTC_Time(3, 0); 
        else if(KeyNum4 == 3) return 1;

        TimeSet_UI();
        OLED_ShowNum_Reverse(1, 4, MyRTC_Time[3], 2, 8, 0);
        OLED_Update();
    }
}

// 5、设置分针
int SetMin(void)
{
    while(1)
    {
        KeyNum5 = Key_GetNum();
        if(KeyNum5 == 1)      Change_RTC_Time(4, 1); 
        else if(KeyNum5 == 2) Change_RTC_Time(4, 0); 
        else if(KeyNum5 == 3) return 1;

        TimeSet_UI();
        OLED_ShowNum_Reverse(2, 4, MyRTC_Time[4], 2, 8, 0);
        OLED_Update();
    }
}

// 6、设置秒针
int SetSec(void)
{
    while(1)
    {
        KeyNum6 = Key_GetNum();
        if(KeyNum6 == 1)      Change_RTC_Time(5, 1); 
        else if(KeyNum6 == 2) Change_RTC_Time(5, 0); 
        else if(KeyNum6 == 3) return 1;

        TimeSet_UI();
        OLED_ShowNum_Reverse(3, 4, MyRTC_Time[5], 2, 8, 0);
        OLED_Update();
    }
}

uint8_t KeyNum;
int settime_flag=1;
int settime_flag1=0;

int SetTime()
{
    while(1)
    {
        KeyNum=Key_GetNum();
        uint8_t settime_flag_temp=0;
        if(KeyNum==1)
        {
            settime_flag--;
            if(settime_flag<=0) settime_flag=7;
        }
        else if(KeyNum==2)
        {
            settime_flag++;
            if(settime_flag>=8) settime_flag=1;
        }
        else if(KeyNum==3)
		{
			OLED_Clear();
			settime_flag_temp=settime_flag;
		}

        if(settime_flag_temp==1)
        {
            settime_flag=1;
            settime_flag1=0;
            return 1;
        }
		else if(settime_flag_temp==2)
        {
            SetYear();
            OLED_ShowNum_Reverse(2, 6, MyRTC_Time[0], 2, 8, 1);
        }
		else if(settime_flag_temp==3)
        {
            SetMonth();
            OLED_ShowNum_Reverse(3, 4, MyRTC_Time[1], 2, 8, 1);
        }
		else if(settime_flag_temp==4)
        {
            SetDay();
            OLED_ShowNum_Reverse(4, 4, MyRTC_Time[2], 2, 8, 1);
        }
		else if(settime_flag_temp==5)
        {
            SetHour();
            OLED_ShowNum_Reverse(1, 4, MyRTC_Time[3], 2, 8, 1);
        }
		else if(settime_flag_temp==6)
        {
            SetMin();
            OLED_ShowNum_Reverse(2, 4, MyRTC_Time[4], 2, 8, 1);
        }
		else if(settime_flag_temp==7)
        {
            SetSec();
            OLED_ShowNum_Reverse(3, 4, MyRTC_Time[5], 2, 8, 1);
        }

        if(settime_flag==1)
        {
            if(settime_flag!=settime_flag1&&settime_flag1!=2)
            {
                DateSet_UI();
            }
            OLED_ShowImage_Reverse(0,0,16,16,Return1,0);
            OLED_ShowCN(2,1,8,1);
            OLED_ShowCN(3,1,9,1);
            OLED_ShowCN(4,1,4,1);
            settime_flag1=settime_flag;
        }
        else if(settime_flag==2)
        {
            OLED_ShowImage_Reverse(0,0,16,16,Return1,1);
            OLED_ShowCN(2,1,8,0);
            OLED_ShowCN(3,1,9,1);
            OLED_ShowCN(4,1,4,1);
            settime_flag1=settime_flag;
        }
        else if(settime_flag==3)
        {
            OLED_ShowImage_Reverse(0,0,16,16,Return1,1);
            OLED_ShowCN(2,1,8,1);
            OLED_ShowCN(3,1,9,0);
            OLED_ShowCN(4,1,4,1);
            settime_flag1=settime_flag;
        }
        else if(settime_flag==4)
        {
            if(set_time_flag1==5) DateSet_UI();
            OLED_ShowImage_Reverse(0,0,16,16,Return1,1);
            OLED_ShowCN(2,1,8,1);
            OLED_ShowCN(3,1,9,1);
            OLED_ShowCN(4,1,4,0);
            settime_flag1=settime_flag;
        }
        else if(settime_flag==5)
        {
            if(settime_flag!=settime_flag1&&settime_flag1==4)
            {
                TimeSet_UI();
            }
            OLED_ShowCN(1,1,6,0);
            OLED_ShowCN(2,1,10,1);
            OLED_ShowCN(3,1,11,1);
            settime_flag1=settime_flag;
        }
        else if(settime_flag==6)
        {
            OLED_ShowCN(1,1,6,1);
            OLED_ShowCN(2,1,10,0);
            OLED_ShowCN(3,1,11,1);
            settime_flag1=settime_flag;
        }
        else if(settime_flag==7)
        {
            if(settime_flag1==1) TimeSet_UI();
            OLED_ShowCN(1,1,6,1);
            OLED_ShowCN(2,1,10,1);
            OLED_ShowCN(3,1,11,0);
            settime_flag1=settime_flag;
        }
        OLED_Update();
    }
}