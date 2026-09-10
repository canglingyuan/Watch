#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <stdint.h>

void MPU6050_Init(void);
void MPU6050_ReadAccel(int16_t* ax, int16_t* ay, int16_t* az);
void MPU6050_ReadGyro(int16_t* gx, int16_t* gy, int16_t* gz);
void MPU6050_PrintData(void);
#endif
