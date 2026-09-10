#ifndef MAX30102_H
#define MAX30102_H

#include <stdint.h>

void MAX30102_Init(void);
void MAX30102_ReadFIFO(uint32_t *red, uint32_t *ir);
void MAX30102_CalcHR_SPo2(uint32_t *red, uint32_t *ir, int len, float *hr, float *spo2);
void MAX30102_MeasureHR_Spo2(float *hr, float *spo2);
void MAX30102_Print(void);
void MAX30102_PrintData(void);

#endif
