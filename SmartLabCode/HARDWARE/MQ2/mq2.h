#ifndef __MQ2_H
#define __MQ2_H

#include "sys.h"
//#include "GPIOLIKE51.h"


extern u16 DMA_buf[3];
extern u16 arr_count[2];

void ADC1_Config(void);
u16 ADC_Result(u8 ADC_Channel_x);
u16 Get_Adc_Average(u8 ch,u8 times);
void DMA_Config(void);

void MQ2_Init(void);
float MQ2_GetVol(void);
float MQ2_GetPPM(void);//MQ2电压数据转换为ppm


#endif
