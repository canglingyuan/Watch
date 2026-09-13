#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>

void Show_Chinese(void);
void Show_Clock_UI(void);
int First_Page_Clock(void);
void Show_SettingPage_UI(void);
int Setting_Page(void);
int Menu(void);
void Menu_Animation(void);
void MenuToFunction(void);
void Set_Selection(uint8_t move_flag,uint8_t Pre_Selection,uint8_t Target_Selection);
void Show_StopWatch_UI(void);
void Show_Time_Only(void);
int StopWatch(void);
void StopWatch_Tick(void);
int LED(void);
void Show_LED_UI(void);
void Show_MPU6050_UI(void);
void MPU6050_Calculation(void);
int MPU6050(void);
void Show_Game_UI(void);
int Game(void);
int Emoji(void);
int Gradienter(void);
void Show_Gradienter_UI(void);

#endif