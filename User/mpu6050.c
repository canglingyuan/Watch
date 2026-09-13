#include "mpu6050.h"
#include "mpu6050_reg.h"
#include "i2c.h"
#include "main.h"
#include <stdint.h>
#include <stdio.h>

#define MPU6050_ADDR 0x68
// #define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址

static void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {reg, data};
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR<<1, buffer, 2, 100);
}

static void MPU6050_ReadRegs(uint8_t reg, uint8_t* data, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR<<1, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, (MPU6050_ADDR<<1|0x01), data, len, 100);
}

static uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data;
    MPU6050_ReadRegs(reg, &data, 1);
    return data;
}

void MPU6050_Init(void)
{
    MPU6050_WriteReg(PWR_MGMT_1, 0x01);      //电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
    MPU6050_WriteReg(PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(SMPLRT_DIV, 0x04);		//采样率分频寄存器，配置采样率,现在希望5ms采样一次，即200Hz,需要从1kHz分频，分频系数=1000/200-1=4
	MPU6050_WriteReg(CONFIG, 0x06);			//配置寄存器，配置DLPF，这个可以把采样频率降到1kHz
	MPU6050_WriteReg(GYRO_CONFIG, 0x18);	    //陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(WHO_AM_I);		//返回WHO_AM_I寄存器的值
}

// Read accelerometer data(读取加速度计数据)
void MPU6050_ReadAccel(int16_t* ax, int16_t* ay, int16_t* az)
{
    uint8_t buf[6];
    MPU6050_ReadRegs(ACCEL_XOUT_H, buf, 6);
    
    *ax = (int16_t)(buf[0] << 8 | buf[1]);
    *ay = (int16_t)(buf[2] << 8 | buf[3]);
    *az = (int16_t)(buf[4] << 8 | buf[5]);
}

// Read gyroscope data(读取陀螺仪数据)
void MPU6050_ReadGyro(int16_t* gx, int16_t* gy, int16_t* gz)
{
    uint8_t buf[6];
    MPU6050_ReadRegs(GYRO_XOUT_H, buf, 6);
    
    *gx = (int16_t)(buf[0] << 8 | buf[1]);
    *gy = (int16_t)(buf[2] << 8 | buf[3]);
    *gz = (int16_t)(buf[4] << 8 | buf[5]);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    MPU6050_ReadAccel(AccX,AccY,AccZ);
    MPU6050_ReadGyro(GyroX, GyroY, GyroZ);
}

/*
// Print accelerometer and gyroscope data to the console(打印加速度计和陀螺仪数据到控制台)
void MPU6050_PrintData(void)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    MPU6050_ReadAccel(&ax, &ay, &az);
    MPU6050_ReadGyro(&gx, &gy, &gz);

    printf("Accel: X=%d, Y=%d, Z=%d | Gyro: X=%d, Y=%d, Z=%d\r\n", ax, ay, az, gx, gy, gz);
}
*/
