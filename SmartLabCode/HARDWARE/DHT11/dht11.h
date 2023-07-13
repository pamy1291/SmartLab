#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"
#include "delay.h"


/* ����GPIO�ţ�Ĭ��ΪPB12 */
#define DHT11_IO 		            GPIOC
#define DHT11_PIN		            GPIO_Pin_5
#define DHT11_APB2PeriphRCC     RCC_APB2Periph_GPIOC
/* ��ʼ�����������DHT11������Ӧ�򷵻�1������0 */
u8 DHT11_Init(void);
/* ��DHT11��ȡ���ݣ�û��С������ */
u8 DHT11_Read_Data(u8 *temp,u8 *humi);

#endif


