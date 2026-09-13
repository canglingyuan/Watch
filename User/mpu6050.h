#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <stdint.h>

void MPU6050_Init(void);
void MPU6050_ReadAccel(int16_t* ax, int16_t* ay, int16_t* az);
void MPU6050_ReadGyro(int16_t* gx, int16_t* gy, int16_t* gz);
// void MPU6050_PrintData(void);
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
#endif
