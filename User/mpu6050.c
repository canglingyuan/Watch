#include "mpu6050.h"
#include "i2c.h"
#include "main.h"
#include <stdio.h>

#define MPU6050_ADDR 0x68
#define PWR_MGMT_1 0x6B // Power management register 1(电源管理寄存器1)
#define ACCEL_XOUT_H 0x3B // Accelerometer X-axis high byte(加速度计X轴高字节)
#define GYRO_XOUT_H 0x43 // Gyroscope X-axis high byte(陀螺仪X轴高字节)

extern I2C_HandleTypeDef hi2c1;

static void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {reg, data};
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR<<1, buffer, 2, 100);
}

static void MPU6050_ReadRegs(uint8_t reg, uint8_t* data, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR<<1, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR<<1, data, len, 100);
}

void MPU6050_Init(void)
{
    MPU6050_WriteReg(PWR_MGMT_1, 0x00); // Wake up the MPU6050(唤醒MPU6050)
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

// Print accelerometer and gyroscope data to the console(打印加速度计和陀螺仪数据到控制台)
void MPU6050_PrintData(void)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    MPU6050_ReadAccel(&ax, &ay, &az);
    MPU6050_ReadGyro(&gx, &gy, &gz);

    printf("Accel: X=%d, Y=%d, Z=%d | Gyro: X=%d, Y=%d, Z=%d\r\n", ax, ay, az, gx, gy, gz);
}
