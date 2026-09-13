#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

extern volatile uint8_t OLED_Buffer[8][128];

void OLED_Init(void);
void OLED_Update(void);
void OLED_WriteData(uint8_t data);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_ReverseArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char ch, uint8_t FontSize);
uint8_t OLED_ShowCharReverse(uint8_t Line, uint8_t Column, char ch, uint8_t FontSize,uint8_t mode);
void OLED_ShowString(uint8_t Line, uint8_t Column, const char *str,uint8_t FontSize);
uint8_t OLED_ShowStringReverse(uint8_t Line, uint8_t Column, const char *str,uint8_t FontSize, uint8_t mode);
uint32_t OLED_Power(uint32_t base, uint32_t exp);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize); 
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t num, uint8_t len, uint8_t FontSize);
void OLED_ShowHex(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize);
void OLED_ShowBin(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize);
uint8_t OLED_ShowNum_Reverse(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize,uint8_t mode);
uint8_t OLED_ShowSignedNum_Reverse(uint8_t Line, uint8_t Column, int32_t num, uint8_t len, uint8_t FontSize,uint8_t mode);
uint8_t OLED_ShowHex_Reverse(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize,uint8_t mode);
uint8_t OLED_ShowBin_Reverse(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize,uint8_t mode);
void OLED_DisplayInit(void);
uint8_t OLED_ShowCN(uint8_t Line, uint8_t Column, const uint8_t Num,uint8_t mode);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t mode);
uint8_t OLED_ShowBMP(uint8_t col,uint8_t page,uint8_t col_size,uint8_t page_size,const uint8_t* bmp,uint8_t mode);
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
uint8_t OLED_ShowImage_Reverse(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image, uint8_t mode);
void OLED_Printf(uint8_t Line, uint8_t Column, uint8_t FontSize, const char* format, ...);
uint8_t OLED_GetPoint(int16_t X, int16_t Y);
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void OLED_DrawRect(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t IsFilled);
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled);
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled);
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled);
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled);
void OLED_ShowChar_Pixel(int16_t X, int16_t Y, char ch);
void OLED_ShowNumPixel(int16_t X, int16_t Y, uint32_t Num, uint8_t len);

#endif
