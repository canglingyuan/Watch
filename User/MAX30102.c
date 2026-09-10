#include "MAX30102.h"
#include "main.h"
#include "i2c.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define MAX30102_ADDR 0xAE
#define REG_INTR_STATUS_1 0x00 // Interrupt Status 1(中断状态寄存器1)
#define REG_INTR_STATUS_2 0x01 // Interrupt Status 2(中断状态寄存器2)
#define REG_INTR_ENABLE_1 0x02 // Interrupt Enable 1(中断使能寄存器1)
#define REG_INTR_ENABLE_2 0x03 // Interrupt Enable 2(中断使能寄存器2)
#define REG_FIFO_WR_PTR 0x04 // FIFO Write Pointer(写指针寄存器)
#define REG_FIFO_RD_PTR 0x06 // FIFO Read Pointer(读指针寄存器)
#define REG_FIFO_DATA 0x07 // FIFO Data(数据寄存器)
#define REG_FIFO_CONFIG 0x08 // FIFO Configuration(配置寄存器)
#define REG_MODE_CONFIG 0x09 // Mode Configuration(模式配置寄存器)
#define REG_SPO2_CONFIG 0x0A // SpO2 Configuration(血氧配置寄存器)
#define REG_LED1_PA 0x0C // LED1 Pulse Amplitude(LED1脉冲幅度寄存器)
#define REG_LED2_PA 0x0D // LED2 Pulse Amplitude(LED2脉冲幅度寄存器)

#define SAMPLE_RATE 100 // Sample rate(采样率)

extern I2C_HandleTypeDef hi2c1;

static void MAX30102_WriteReg(uint8_t reg, uint8_t value) 
{
    uint8_t data[2] = {reg, value};
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR<<1, data, 2, HAL_MAX_DELAY);
}

static void MAX30102_ReadRegs(uint8_t reg, uint8_t *value,uint8_t len) 
{
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR<<1, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR<<1, value, len, HAL_MAX_DELAY);
}

void MAX30102_Init(void) 
{
    // Reset the device(复位设备)
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x40);
    HAL_Delay(100);

    // Configure the device(配置设备)
    MAX30102_WriteReg(REG_FIFO_CONFIG, 0x4F); //FIFO配置:样本平均数为4，FIFO滚动模式，FIFO满时触发中断
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x03); //模式配置：SpO2模式
    MAX30102_WriteReg(REG_SPO2_CONFIG, 0x27); //血氧配置：ADC分辨率为18位，采样率为100Hz
    MAX30102_WriteReg(REG_LED1_PA, 0x24); //LED1  脉冲幅度：红光LED电流为6.4mA
    MAX30102_WriteReg(REG_LED2_PA, 0x24); //LED2  脉冲幅度：红外LED电流为6.4mA
}

void MAX30102_ReadFIFO(uint32_t *red, uint32_t *ir) 
{
    uint8_t data[6];
    MAX30102_ReadRegs(REG_FIFO_DATA, data, 6);

    *red = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2]; // Red data(红光数据)
    *ir = ((uint32_t)data[3] << 16) | ((uint32_t)data[4] << 8) | data[5];  // IR data(红外数据)
    *red &= 0x03FFFF; // Mask to 18 bits(掩码为18位)
    *ir &= 0x03FFFF;  // Mask to 18 bits
}

void MAX30102_CalcHR_SPo2(uint32_t *red, uint32_t *ir, int len, float *hr, float *spo2) 
{
    float red_dc = 0, ir_dc = 0; // DC components(直流分量)
    float red_ac = 0, ir_ac = 0; // AC components(交流分量)
    
    for(int i = 0; i < len; i++) 
    {
        red_dc += red[i];
        ir_dc += ir[i];
    }
    red_dc /= len;
    ir_dc /= len;

    for(int i = 0; i < len; i++) 
    {
        red_ac += fabsf(red[i] - red_dc); // fabsf()函数用于计算浮点数的绝对值
        ir_ac += fabsf(ir[i] - ir_dc);
    }
    red_ac /= len;
    ir_ac /= len;

    float ratio = (red_ac / red_dc) / (ir_ac / ir_dc);
    *spo2 = -45.060f * ratio * ratio + 30.354f * ratio + 94.845f; // Empirical formula(经验公式)

    // 心率：红外信号的峰值检测
    int peaks = 0;
    for(int i = 1; i < len - 1; i++) 
    {
        if(ir[i] > ir[i - 1] && ir[i] > ir[i + 1] && ir[i] > ir_dc) 
        {
            peaks++;
        }
    }
    *hr = (peaks*60.0f)/(len/(float)SAMPLE_RATE); // Calculate heart rate(计算心率)
}

void MAX30102_MeasureHR_Spo2(float *hr, float *spo2) 
{
    const int len = 100; // Number of samples(样本数量)
    uint32_t red[len], ir[len];

    for(int i = 0; i < len; i++) 
    {
        MAX30102_ReadFIFO(&red[i], &ir[i]);
        HAL_Delay(10); // Delay between samples(样本间延迟)
    }

    MAX30102_CalcHR_SPo2(red, ir, len, hr, spo2);
}

void MAX30102_Print(void) 
{
    uint32_t red, ir;
    MAX30102_ReadFIFO(&red, &ir);
    printf("RED:%lu IR:%lu\r\n", red, ir);
}

void MAX30102_PrintData(void) 
{
    float hr, spo2;
    MAX30102_MeasureHR_Spo2(&hr, &spo2);
    printf("Heart Rate: %.2f bpm, SpO2: %.2f%%\r\n", hr, spo2);
}