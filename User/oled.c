#include "oled.h"
#include "OLED_Font.h"
#include "i2c.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#define OLED_ADDR 0x78
#define OLED_CMD 0x00
#define OLED_DATA 0x40

uint8_t volatile OLED_Buffer[8][128]; // OLED显示缓冲区，8页，每页128列
static uint8_t CurrentPage = 0; // 当前页码
static uint8_t CurrentCol = 0; // 当前列号

extern I2C_HandleTypeDef hi2c1;

// OLED_Write函数用于向OLED显示屏写入数据或命令。
static void OLED_Write(uint8_t data,uint8_t type)
{
    uint8_t buffer[2]={type,data};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buffer, 2, HAL_MAX_DELAY);
}

// OLED_Read函数用于从OLED显示屏读取数据。
static uint8_t OLED_Read(uint8_t type, uint8_t *data, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, &type, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, OLED_ADDR|0x01, data, len, HAL_MAX_DELAY);
    return *data;
}

// OLED_Init函数用于初始化OLED显示屏。
void OLED_Init(void)
{
    HAL_Delay(100);
    OLED_Write(0xAE, OLED_CMD); // 设置显示关闭
    OLED_Write(0xD5, OLED_CMD); // 设置显示时钟分频
    OLED_Write(0x80, OLED_CMD); // 设置分频因子，震荡频率
    OLED_Write(0xA8, OLED_CMD); // 设置多路复用比率
    OLED_Write(0x3F, OLED_CMD); // 1/64 duty
    OLED_Write(0xD3, OLED_CMD); // 设置显示偏移
    OLED_Write(0x00, OLED_CMD); // 无偏移
    OLED_Write(0x40, OLED_CMD); // 设置起始行地址
    OLED_Write(0x8D, OLED_CMD); // 设置电荷泵
    OLED_Write(0x14, OLED_CMD); // 启用电荷泵
    OLED_Write(0x20, OLED_CMD); // 存储器寻址模式
    OLED_Write(0x00, OLED_CMD); // 水平寻址模式
    OLED_Write(0xA1, OLED_CMD); // 设置段重映射
    OLED_Write(0xC8, OLED_CMD); // 设置COM输出扫描方向
    OLED_Write(0xDA, OLED_CMD); // 设置COM引脚硬件配置
    OLED_Write(0x12, OLED_CMD);
    OLED_Write(0x81, OLED_CMD); // 设置对比度控制
    OLED_Write(0xCF, OLED_CMD);
    OLED_Write(0xD9, OLED_CMD); // 设置预充电压
    OLED_Write(0xF1, OLED_CMD);
    OLED_Write(0xDB, OLED_CMD); // 设置VCOMH电压倍率
    OLED_Write(0x40, OLED_CMD);
    OLED_Write(0xA4, OLED_CMD); // 设置全局显示开启
    OLED_Write(0xA6, OLED_CMD); // 设置正常/倒转显示
    HAL_Delay(100);
    OLED_Write(0xAF, OLED_CMD); // 设置显示开启
}

// OLED_Update函数用于将缓冲区的内容更新到OLED显示屏上。
void OLED_Update(void)
{
    for(uint8_t page=0; page<8; page++)
    {
        OLED_Write(0xB0 + page, OLED_CMD); // 设置页地址
        OLED_Write(0x00, OLED_CMD); // 设置列地址低位
        OLED_Write(0x10, OLED_CMD); // 设置列地址高位
        for(uint8_t col=0; col<128; col++)
        {
            OLED_Write(OLED_Buffer[page][col], OLED_DATA);
        }
    }
}

// OLED_WriteData函数用于向OLED显示屏写入数据到缓冲区。
void OLED_WriteData(uint8_t data)
{
    if(CurrentPage < 8 && CurrentCol < 128)
    {
        OLED_Buffer[CurrentPage][CurrentCol] = data;
        CurrentCol++;
        if(CurrentCol >= 128)
        {
            CurrentCol = 0;
            CurrentPage++;
            if(CurrentPage >= 8)
            {
                CurrentPage = 0;
            }
        }
    }
}

// OLED_SetCursor函数用于设置OLED显示屏的光标位置。
void OLED_SetCursor(uint8_t Page, uint8_t Column)
{
    if(Page<8) CurrentPage = Page;
    if(Column<128) CurrentCol = Column;
}

// OLED_Clear函数用于清空OLED显示屏的内容。
void OLED_Clear(void)
{
    memset(OLED_Buffer, 0, sizeof(OLED_Buffer));
    CurrentPage = 0;
    CurrentCol = 0;
}

// OLED_ClearArea函数用于清空OLED显示屏的指定区域。
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height) //x:0~127, y:0~63
{
    for(uint8_t page = y/8; page < (y + height + 7)/8; page++)
    {
        for(uint16_t col = x; col < x + width; col++)
        {
            if(page < 8 && col < 128)
            {
                OLED_Buffer[page][col] &= ~(0x01 << (page*8 % 8)); // 清除指定区域的像素
            }
        }
    }
}

// OLED_ReverseArea函数用于反转OLED显示屏的指定区域的像素。
void OLED_ReverseArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    for(uint8_t page = y/8; page < (y + height + 7)/8; page++)
    {
        for(uint16_t col = x; col < x + width; col++)
        {
            if(page < 8 && col < 128)
            {
                OLED_Buffer[page][col] ^= (0x01 << (page*8 % 8)); // 反转指定区域的像素
            }
        }
    }
}

// OLED_ShowChar函数用于在OLED显示屏上显示一个字符。
void OLED_ShowChar(uint8_t Line, uint8_t Column, char ch,uint8_t FontSize) //Line:1~8
{
    uint8_t i;
    if(FontSize == 6) // 6x8字体
    {
        OLED_SetCursor(Line-1, (Column-1)*6);
        for(i=0; i<6; i++)
        {
            // OLED_Buffer[Line][Column+i] = OLED_F6x8[ch-' '][i];
            // OLED_Update();
            OLED_WriteData(OLED_F6x8[ch-' '][i]);
        }
    }
    else if(FontSize == 8) // 8x16字体
    {
        OLED_SetCursor((Line-1)*2, (Column-1)*8);
        for(i=0; i<8; i++)
        {
            OLED_WriteData(OLED_F8x16[ch-' '][i]);
        }
        OLED_SetCursor((Line-1)*2+1, (Column-1)*8);
        for(i=0; i<8; i++)
        {
            OLED_WriteData(OLED_F8x16[ch-' '][i+8]);
        }
    }
    else if(FontSize == 12) // 12x24字体
    {
        OLED_SetCursor((Line-1)*3, (Column-1)*12);
        for(i=0; i<12; i++)
        {
            OLED_WriteData(OLED_F12x24[ch-' '][i]);
        }
        OLED_SetCursor((Line-1)*3+1, (Column-1)*12);
        for(i=0; i<12; i++)
        {
            OLED_WriteData(OLED_F12x24[ch-' '][i+12]);
        }
        OLED_SetCursor((Line-1)*3+2, (Column-1)*12);
        for(i=0; i<12; i++)
        {
            OLED_WriteData(OLED_F12x24[ch-' '][i+24]);
        }
    }
}

// 显示一个字符带有反色效果
uint8_t OLED_ShowCharReverse(uint8_t Line, uint8_t Column, char ch,uint8_t FontSize, uint8_t mode)
{
    uint8_t i,temp;
    if(FontSize == 6) // 6x8字体
    {
        OLED_SetCursor(Line-1, (Column-1)*6);
        for(i=0; i<6; i++)
        {
            temp = OLED_F6x8[ch-' '][i];
            OLED_WriteData(mode ? temp : ~temp);
        }
    }
    else if(FontSize == 8) // 8x16字体
    {
        OLED_SetCursor((Line-1)*2, (Column-1)*8);
        for(i=0; i<8; i++)
        {
            temp = OLED_F8x16[ch-' '][i];
            OLED_WriteData(mode ? temp : ~temp);
        }
        OLED_SetCursor((Line-1)*2+1, (Column-1)*8);
        for(i=0; i<8; i++)
        {
            temp = OLED_F8x16[ch-' '][i+8];
            OLED_WriteData(mode ? temp : ~temp);
        }
    }
    else if(FontSize == 12) // 12x24字体
    {
        OLED_SetCursor((Line-1)*3, (Column-1)*12);
        for(i=0; i<12; i++)
        {
            temp = OLED_F12x24[ch-' '][i];
            OLED_WriteData(mode ? temp : ~temp);
        }
        OLED_SetCursor((Line-1)*3+1, (Column-1)*12);
        for(i=0; i<12; i++)
        {
            temp = OLED_F12x24[ch-' '][i+12];
            OLED_WriteData(mode ? temp : ~temp);
        }
        OLED_SetCursor((Line-1)*3+2, (Column-1)*12);
        for(i=0; i<12; i++)
        {
            temp = OLED_F12x24[ch-' '][i+24];
            OLED_WriteData(mode ? temp : ~temp);
        }
    }
    return mode;
}

// 显示一个字符串
void OLED_ShowString(uint8_t Line, uint8_t Column, const char *str,uint8_t FontSize)
{
    for(; *str != '\0'; str++)
    {
        OLED_ShowChar(Line, Column++, *str, FontSize);
    }
}

// 显示一个字符串带有反色效果
uint8_t OLED_ShowStringReverse(uint8_t Line, uint8_t Column, const char *str,uint8_t FontSize, uint8_t mode)
{
    for(; *str != '\0'; str++)
    {
        mode = OLED_ShowCharReverse(Line, Column++, *str, FontSize, mode);
    }
    return mode;
}

// OLED次方函数
uint32_t OLED_Power(uint32_t base, uint32_t exp)
{
    uint32_t result = 1;
    for(uint32_t i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

// OLED显示数字函数
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize) //Line:1~8
{
    uint8_t i;
    for(i=0; i<len; i++)
    {
        OLED_ShowChar(Line, Column + i, (num / OLED_Power(10, len - i - 1)) % 10 + '0', FontSize);
    }
}

// OLED显示带符号数字
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t num, uint8_t len, uint8_t FontSize) //Line:1~8
{
    if(num < 0)
    {
        OLED_ShowChar(Line, Column, '-', FontSize);
        num = -num;
    }
    else
    {
        OLED_ShowChar(Line, Column, '+', FontSize);
    }
    OLED_ShowNum(Line, Column + 1, num, len - 1, FontSize);
}

// OLED显示十六进制
void OLED_ShowHex(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize) //Line:1~8
{
    uint8_t i, digit;
    for(i=0; i<len; i++)
    {
        digit = num / OLED_Power(16, len - i - 1) % 16;
        if(digit < 10)
        {
            OLED_ShowChar(Line, Column + i, digit + '0', FontSize);
        }
        else
        {
            OLED_ShowChar(Line, Column + i, digit - 10 + 'A', FontSize);
        }
    }
}

// OLED显示二进制
void OLED_ShowBin(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize) //Line:1~8
{
    uint8_t i;
    for(i=0; i<len; i++)
    {
        OLED_ShowChar(Line, Column + i, (num / OLED_Power(2, len - i - 1)) % 2 + '0', FontSize);
    }
}


/*============为前面各显示数字函数反相==============*/
uint8_t OLED_ShowNum_Reverse(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize,uint8_t mode)
{
    uint8_t i;
    for(i=0; i<len; i++)
    {
        OLED_ShowCharReverse(Line, Column + i, (num / OLED_Power(10, len - i - 1)) % 10 + '0', FontSize,mode);
    }
    return mode;
}

uint8_t OLED_ShowSignedNum_Reverse(uint8_t Line, uint8_t Column, int32_t num, uint8_t len, uint8_t FontSize,uint8_t mode) //Line:1~8
{
    if(num < 0)
    {
        OLED_ShowCharReverse(Line, Column, '-', FontSize,mode);
        num = -num;
    }
    else
    {
        OLED_ShowCharReverse(Line, Column, '+', FontSize,mode);
    }
    
    OLED_ShowNum_Reverse(Line, Column + 1, num, len - 1, FontSize,mode);
    return mode;
}

// OLED显示十六进制
uint8_t OLED_ShowHex_Reverse(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize,uint8_t mode) //Line:1~8
{
    uint8_t i, digit;
    for(i=0; i<len; i++)
    {
        digit = num / OLED_Power(16, len - i - 1) % 16;
        if(digit < 10)
        {
            OLED_ShowCharReverse(Line, Column + i, digit + '0', FontSize,mode);
        }
        else
        {
            OLED_ShowCharReverse(Line, Column + i, digit - 10 + 'A', FontSize,mode);
        }
    }
    return mode;
}

// OLED显示二进制
uint8_t OLED_ShowBin_Reverse(uint8_t Line, uint8_t Column, uint32_t num, uint8_t len, uint8_t FontSize,uint8_t mode) //Line:1~8
{
    uint8_t i;
    for(i=0; i<len; i++)
    {
        OLED_ShowCharReverse(Line, Column + i, (num / OLED_Power(2, len - i - 1)) % 2 + '0', FontSize,mode);
    }
    return mode;
}

// OLED运行初始化
void OLED_DisplayInit(void)
{
    OLED_Init();
    OLED_Clear();
    OLED_Update();
}

// OLED显示汉字
uint8_t OLED_ShowCN(uint8_t Line, uint8_t Column, const uint8_t Num,uint8_t mode) //Line:1~8
{
    uint8_t i;
    uint8_t Page_start = (Line-1)*2;
    uint8_t Col_start = (Column-1)*16;

    OLED_SetCursor(Page_start, Col_start);
    for(i=0; i<16; i++)
    {
        uint8_t data = OLED_F10x16[Num][i];
        OLED_WriteData(mode ? data : ~data);
    }

    OLED_SetCursor(Page_start+1, Col_start);
    for(i=16; i<32; i++)
    {
        uint8_t data = OLED_F10x16[Num][i];
        OLED_WriteData(mode ? data : ~data);
    }
    return mode;
}

// OLED画点函数
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)
{
    if(x>=128 || y>=64) return;

    uint8_t page=y/8;
    uint8_t bit_pos=y%8;

    if(mode)
        OLED_Buffer[page][x] |= (1<<bit_pos);
    else
        OLED_Buffer[page][x] &= ~(1<<bit_pos);
}

// OLED显示位图
uint8_t OLED_ShowBMP(uint8_t col,uint8_t page,uint8_t col_size,uint8_t page_size,const uint8_t* bmp,uint8_t mode)
{
    uint16_t index = 0;
    
    for (; page < page+page_size; page++) 
    {
        OLED_SetCursor(page, col);
        for (; col < col+col_size; col++) 
        {
            uint8_t data = bmp[index++];
            OLED_WriteData(mode ? data : ~data); 
        }
    }
    return mode;
}

// OLED显示图像
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
    uint8_t i = 0, j = 0;
    int16_t Page, Shift;
    
    /*遍历指定图像涉及的相关页*/
    for (j = 0; j < ((Height - 1) / 8 + 1); j++)
    {
        /*遍历指定图像涉及的相关列*/
        for (i = 0; i < Width; i++)
        {
            int16_t screen_x = X + i;
            if (screen_x >= 0 && screen_x <= 127)    
            {
                Page = Y / 8;
                Shift = Y % 8;
                
                uint16_t image_index = (uint16_t)j * Width + i;
                
                if (Page + j >= 0 && Page + j <= 7)    
                {
                    if (image_index < ((Height * Width) / 8))
                        OLED_Buffer[Page + j][screen_x] |= Image[image_index] << Shift;
                }
                
                if (Page + j + 1 >= 0 && Page + j + 1 <= 7)    
                {                    
                    if (image_index < ((Height * Width) / 8))
                        OLED_Buffer[Page + j + 1][screen_x] |= Image[image_index] >> (8 - Shift);
                }
            }
        }
    }
}

uint8_t OLED_ShowImage_Reverse(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image, uint8_t mode)
{
    uint8_t i = 0, j = 0;
    int16_t Page, Shift;
    
    for (j = 0; j < ((Height - 1) / 8 + 1); j++)
    {
        for (i = 0; i < Width; i++)
        {
            int16_t screen_x = X + i;
            if (screen_x >= 0 && screen_x <= 127)
            {
                Page = Y / 8;
                Shift = Y % 8;
                uint16_t image_index = j * Width + i;
                
                uint8_t current_byte = 0;
                if (image_index < ((Height * Width) / 8)) {
                    current_byte = Image[image_index];
                }

                if (mode != 1) {
                    current_byte = ~current_byte;
                }

                if (Page + j >= 0 && Page + j <= 7)
                {
                    OLED_Buffer[Page + j][screen_x] &= ~(0xFF << Shift);
                    OLED_Buffer[Page + j][screen_x] |= (current_byte << Shift); //低位在最上面
                }
                
                if (Page + j + 1 >= 0 && Page + j + 1 <= 7)
                {                    
                    OLED_Buffer[Page + j + 1][screen_x] &= ~(0xFF >> (8 - Shift));
                    OLED_Buffer[Page + j + 1][screen_x] |= (current_byte >> (8 - Shift));
                }
            }
        }
    }
    return mode;
}

// OLED使用printf打印格式化字符串
void OLED_Printf(uint8_t Line, uint8_t Column, uint8_t FontSize, const char* format, ...)
{
    char String[256];
    va_list arg;
    uint16_t i=0;

    // 1、坐标转换核心逻辑
    uint16_t X_Pixel,Y_Pixel;

    if(FontSize==6) //6*8
    {
        X_Pixel=(Column-1)*6;
        Y_Pixel=(Line-1)*8;
    }
    else //8*16
    {
        X_Pixel=(Column-1)*8;
        Y_Pixel=(Line-1)*16;
    }

    // 2、格式化字符
    va_start(arg,format);
    vsprintf(String, format, arg);
    va_end(arg);

    // 3、绘制字符
    while(String[i]!='\0')
    {
        if(String[i]=='\n')
        {
            Y_Pixel+= (FontSize==6)?8:16;
            X_Pixel= (Column-1)*((FontSize==6)?6:8);
            i++;
            continue;
        }

        if((uint8_t)String[i] >127)
        {
            i+=3;
            continue;
        }

        const uint8_t *fontData;
        uint8_t w, h;
        uint8_t cx, cy;
        
        if (FontSize == 8) 
        {
            fontData = OLED_F8x16[String[i] - ' '];
            w = 8; h = 16;
        } 
        else 
        {
            fontData = OLED_F6x8[String[i] - ' '];
            w = 6; h = 8;
        }

        if (X_Pixel + w > 128) break; // 超出屏幕宽度停止

        for (cx = 0; cx < w; cx++)
        {
            uint16_t colData = 0;
            if (h == 16) colData = fontData[cx] | (fontData[cx + 8] << 8);
            else colData = fontData[cx];

            for (cy = 0; cy < h; cy++)
            {
                if (Y_Pixel + cy >= 64) break; 
                
                if ((colData >> cy) & 0x01) OLED_DrawPoint(X_Pixel + cx, Y_Pixel + cy, 1);
                else OLED_DrawPoint(X_Pixel + cx, Y_Pixel + cy, 0);
            }
        }
        
        X_Pixel += w;
        i++;
    }
}

// 获取某点的状态
uint8_t OLED_GetPoint(int16_t X, int16_t Y)
{
    if(X<0 || X>127 || Y<0 || Y>63) return 0;
    return (OLED_Buffer[Y/8][X] >> (Y%8)) & 0x01;
}

// 画线
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1)
{
    int16_t x=X0,y=Y0;
    int16_t dx=abs(X1-X0),dy=abs(Y1-Y0);
    int8_t sx=(X0<X1):1?-1;
    int8_t sy=(Y0<Y1):1?-1;
    int16_t err=dx-dy;  //err>0 : k<1
    int16_t e2;

    while(1)
    {
        OLED_DrawPoint(x, y, 1);
        if(x==X1 && y==Y1)  break;
        e2 = 2 * err;
        if(e2 > -dy)    {err -= dy; x += sx;}
        if(e2 < dx)    {err += dx; y += sy;}
    }
}

// 画矩形
void OLED_DrawRect(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t IsFilled)
{
    if(IsFilled)
    {
        uint8_t i;
        if(IsFilled)
        {
            for(i=0;i<Height;i++)
                OLED_DrawLine(X, Y+i, X+Width-1, Y+i);
        }
    }
    else
    {
        OLED_DrawLine(X, Y, X+Width-1, Y);
        OLED_DrawLine(X, Y+Height-1, X+Width-1, Y+Height-1);
        OLED_DrawLine(X, Y, X, Y+Height-1);
        OLED_DrawLine( X+Width-1, Y,  X+Width-1, Y1);
    }
}

// 画圆
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled)
{
    int16_t x = 0;
    int16_t y = Radius;
    int16_t d = 3 - 2 * Radius;

    while(x<=y)
    {
        if(IsFilled)
        {
            OLED_DrawLine(X - x, Y + y, X + x, Y + y);
            OLED_DrawLine(X - y, Y + x, X + y, Y + x);
            OLED_DrawLine(X - x, Y - y, X + x, Y - y);
            OLED_DrawLine(X - y, Y - x, X + y, Y - x);
        }
        else 
        {
            OLED_DrawPoint(X + x, Y + y, 1);
            OLED_DrawPoint(X - x, Y + y, 1);
            OLED_DrawPoint(X + x, Y - y, 1);
            OLED_DrawPoint(X - x, Y - y, 1);
            OLED_DrawPoint(X + y, Y + x, 1);
            OLED_DrawPoint(X - y, Y + x, 1);
            OLED_DrawPoint(X + y, Y - x, 1);
            OLED_DrawPoint(X - y, Y - x, 1);
        }
        if (d < 0) d = d + 4 * x + 6;
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// 画三角形
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled)
{
    if (IsFilled)
    {
        // 寻找包围盒
        int16_t minX = X0 < X1 ? (X0 < X2 ? X0 : X2) : (X1 < X2 ? X1 : X2);
        int16_t maxX = X0 > X1 ? (X0 > X2 ? X0 : X2) : (X1 > X2 ? X1 : X2);
        int16_t minY = Y0 < Y1 ? (Y0 < Y2 ? Y0 : Y2) : (Y1 < Y2 ? Y1 : Y2);
        int16_t maxY = Y0 > Y1 ? (Y0 > Y2 ? Y0 : Y2) : (Y1 > Y2 ? Y1 : Y2);

        // 限制在屏幕范围内
        if (minX < 0) minX = 0; if (maxX > 127) maxX = 127;
        if (minY < 0) minY = 0; if (maxY > 63) maxY = 63;

        for (int16_t y = minY; y <= maxY; y++)
        {
            for (int16_t x = minX; x <= maxX; x++)
            {
                // 使用叉乘判断点是否在三角形内
                int16_t edge0 = (X1 - X0) * (y - Y0) - (Y1 - Y0) * (x - X0);
                int16_t edge1 = (X2 - X1) * (y - Y1) - (Y2 - Y1) * (x - X1);
                int16_t edge2 = (X0 - X2) * (y - Y2) - (Y0 - Y2) * (x - X2);

                // 如果三个叉乘结果符号相同（或为0），则点在三角形内
                if ((edge0 >= 0 && edge1 >= 0 && edge2 >= 0) || 
                    (edge0 <= 0 && edge1 <= 0 && edge2 <= 0))
                {
                    OLED_DrawPoint(x, y, 1);
                }
            }
        }
    }
    else
    {
        OLED_DrawLine(X0, Y0, X1, Y1);
        OLED_DrawLine(X1, Y1, X2, Y2);
        OLED_DrawLine(X2, Y2, X0, Y0);
    }
}

// 画椭圆
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled)
{
    int16_t x = 0, y = B;
    int32_t a2 = A * A, b2 = B * B;
    int32_t d = b2 - a2 * B + a2 / 4;

    // 区域1
    while (b2 * x <= a2 * y)
    {
        if (IsFilled)
        {
            OLED_DrawLine(X - x, Y + y, X + x, Y + y);
            OLED_DrawLine(X - x, Y - y, X + x, Y - y);
        }
        else
        {
            OLED_DrawPoint(X + x, Y + y, 1);
            OLED_DrawPoint(X - x, Y + y, 1);
            OLED_DrawPoint(X + x, Y - y, 1);
            OLED_DrawPoint(X - x, Y - y, 1);
        }

        if (d < 0) d += b2 * (2 * x + 3);
        else { d += b2 * (2 * x + 3) + a2 * (-2 * y + 2); y--; }
        x++;
    }

    // 区域2
    d = b2 * (x * x + x) + a2 * (y * y - y) - a2 * b2 / 4 + (b2 + a2) / 2; // 近似初始值
    // 更精确的初始值: d = b2*(x+0.5)^2 + a2*(y-1)^2 - a2*b2
    
    while (y >= 0)
    {
        if (IsFilled)
        {
            OLED_DrawLine(X - x, Y + y, X + x, Y + y);
            OLED_DrawLine(X - x, Y - y, X + x, Y - y);
        }
        else
        {
            OLED_DrawPoint(X + x, Y + y, 1);
            OLED_DrawPoint(X - x, Y + y, 1);
            OLED_DrawPoint(X + x, Y - y, 1);
            OLED_DrawPoint(X - x, Y - y, 1);
        }
        
        if (d < 0) { d += b2 * (2 * x + 2) + a2 * (-2 * y + 3); x++; }
        else d += a2 * (-2 * y + 3);
        y--;
    }
}

// 画圆弧
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled)
{
    float angle;
    int16_t x_curr, y_curr;
    int16_t x_pre, y_pre;
    uint8_t first_point = 1;
    
    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif

    // 步长为1度，保证精度
    for (int16_t i = StartAngle; i <= EndAngle; i++)
    {
        angle = (float)i * M_PI / 180.0f;
        x_curr = X + (int16_t)(Radius * cos(angle));
        y_curr = Y + (int16_t)(Radius * sin(angle));

        if (IsFilled)
        {
            // 填充模式：画扇形，从圆心连接到圆弧点
            OLED_DrawLine(X, Y, x_curr, y_curr);
        }
        else
        {
            // 轮廓模式：为了防止点之间有空隙，使用线段连接上一个点和当前点
            if (!first_point)
            {
                OLED_DrawLine(x_pre, y_pre, x_curr, y_curr);
            }
            else
            {
                OLED_DrawPoint(x_curr, y_curr, 1);
                first_point = 0;
            }
            x_pre = x_curr;
            y_pre = y_curr;
        }
    }
}

// 指定像素位置显示一个字符（6x8）
void OLED_ShowChar_Pixel(int16_t X, int16_t Y, char ch)
{
    uint8_t i, shift;
    uint8_t c = ch - ' '; // 得到偏移值
    
    for (i = 0; i < 6; i++) // 遍历6列
    {
        uint8_t temp = OLED_F6x8[c][i];
        for (shift = 0; shift < 8; shift++) // 遍历8行
        {
            // 如果超出屏幕范围则不画
            if ((X + i) > 127 || (Y + shift) > 63) continue;
            
            if (temp & (1 << shift)) 
            {
                OLED_DrawPoint(X + i, Y + shift, 1); // 点亮
            }
            else 
            {
                OLED_DrawPoint(X + i, Y + shift, 0); // 熄灭（擦除背景，防止重叠）
            }
        }
    }
}

// 在指定像素位置显示数字
void OLED_ShowNumPixel(int16_t X, int16_t Y, uint32_t Num, uint8_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        uint8_t single_num = Num / OLED_Pow(10, len - i - 1) % 10;
        OLED_ShowCharPixel(X + (i * 6), Y, single_num + '0');
    }
}
