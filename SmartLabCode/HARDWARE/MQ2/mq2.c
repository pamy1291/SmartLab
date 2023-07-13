#include "mq2.h"
#include <math.h>
#include "delay.h"
//#include "delay.h"
//mq2 mq7 mq135�Ĳɼ�
//=============================================================================
//�������ƣ�
//DMA_buf[0]  MQ2    ����
//DMA_buf[1]  MQ7    һ����̼
//DMA_buf[2]  MQ135  ��������
//=============================================================================
//ת��ppm������ݶ���
#define CAL_PPM 20  // У׼������PPMֵ
#define RL			5		// RL��ֵ��ʵ�ʻ�������ΪR0
static float R0 = 14; // Ԫ���ڽྻ�����е���ֵ
/***RS/RL*************ppm******************
 * 1.651428	          200               *
 * 1.437143	          300               *
 * 1.257143	          400               *
 * 1.137143	          500               *
 * 1				          600               *
 * 0.928704	          700               *
 * 0.871296	          800               *
 * 0.816667	          900               *
 * 0.785714	          1000              *
 * 0.574393	          2000              *
 * 0.466047	          3000              *
 * 0.415581	          4000              *
 * 0.370478	          5000              *
 * 0.337031	          6000              *
 * 0.305119	          7000              *
 * 0.288169	          8000              *
 * 0.272727	          9000              *
 * 0.254795	          10000             *
 *                                      *
 * ppm = 613.9f * pow(RS/RL, -2.074f)   *
 ***************************************/
 
u16 arr_count[2]={0};
u16 DMA_buf[3]={0};

void ADC1_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_Struct={0};
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AIN;                       //ģ������
	GPIO_Struct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	ADC_InitTypeDef ADC_Struct={0};	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                            //����ADCʱ�� 72/6<14
	ADC_Struct.ADC_Mode = ADC_Mode_Independent;                  //��������ģʽ
	ADC_Struct.ADC_ContinuousConvMode = ENABLE;                  //����ģʽ
	ADC_Struct.ADC_ScanConvMode = DISABLE;                        //��ͨ��ģʽ
	ADC_Struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת���������������
	ADC_Struct.ADC_DataAlign = ADC_DataAlign_Right;              //�Ҷ���
	ADC_Struct.ADC_NbrOfChannel = 1;                             //�涨��˳����й���ת���� ADC ͨ������Ŀ
	ADC_Init(ADC1,&ADC_Struct);
	//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);//MQ2	
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2,ADC_SampleTime_239Cycles5);//MQ7	
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,3,ADC_SampleTime_239Cycles5);//MQ135

	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE); 
	
	//У׼:��С���
	ADC_ResetCalibration(ADC1);                            //���üĴ���
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);        //�ȴ��������
	ADC_StartCalibration(ADC1);                            //����У׼����У׼�Ĵ��� У׼ ADC1
	while(ADC_GetCalibrationStatus(ADC1)==SET);             //�ȴ�У׼���		
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);                 //����ת�� ʹ�ܻ���ʧ��ָ���� ADC �����ת���������� 			
}

void DMA_Config(void)
{
	DMA_InitTypeDef DMA_Struct={0};
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);               //����DMAʱ��

	DMA_Struct.DMA_PeripheralBaseAddr =(u32) &ADC1->DR;             //����DMA�������ַ
	DMA_Struct.DMA_DIR = DMA_DIR_PeripheralSRC;                     //������Ϊ���ݴ������Դ
	DMA_Struct.DMA_BufferSize = 1;                                  //��ַ����3�Σ���λΪ�ֿ�
	DMA_Struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //�����ַ�Ĵ�������
	DMA_Struct.DMA_MemoryInc = DMA_MemoryInc_Enable;                //�ڴ��ַ�Ĵ�������
	DMA_Struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;    //���ݿ��Ϊ16λ
	DMA_Struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_Struct.DMA_Mode = DMA_Mode_Circular;                        //������ѭ������ģʽ
	DMA_Struct.DMA_Priority = DMA_Priority_High;                    //���ȼ�
	DMA_Struct.DMA_M2M = DMA_M2M_Disable;                           //DMAͨ��û������Ϊ�ڴ浽�ڴ洫��
	DMA_Struct.DMA_MemoryBaseAddr = (u32)&DMA_buf[0];               //�ڴ����ַ
	
	DMA_Init(DMA1_Channel1,&DMA_Struct);
	DMA_Cmd(DMA1_Channel1,ENABLE);
}

//������DMA�Ķ�ͨ���ɼ�����
//u16 ADC_Result(u8 ADC_Channel_x)
//{
//	u16 ADC_val = ADC_GetConversionValue(ADC1); //�������һ�� ADCx �������ת�����
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_x,1,ADC_SampleTime_239Cycles5);
//	ADC_SoftwareStartConvCmd(ADC1,ENABLE); //����ת��	 ʹ�ܻ���ʧ��ָ���� ADC �����ת���������� 	
//	while((ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC))==RESET);
//	
//	//float ADC_Cha = (ADC_val*3.3/4095);
//	//printf("ADC_val == %d\r\n",ADC_val);
//	return ADC_val;
//}
//u16 Get_Adc_Average(u8 ch,u8 times)
//{
//	u16 temp_val=0;
//	u8 t;
//	for(t=0;t<times;t++)
//	{
//		temp_val+=ADC_Result(ch);
//		Delay_ms(5);
//	}
//	return temp_val/times;
//} 	

void MQ2_Init(void)
{
	ADC1_Config();
	DMA_Config();
}
 
 // MQ2���������ݴ���ʵ�ʵ�ѹ
float MQ2_GetVol(void)
{
	//u16 adcx = Get_Adc_Average(ADC_Channel_1, 10);//������dma
	float Vrl = 3.3f * DMA_buf[0] / 4095.0;
	return  Vrl;
}
// ������У׼������������Ϊ14
void MQ2_PPM_Calibration(float RS)
{
	R0 = RS / pow(CAL_PPM/613.9f, -1/2.407f);
}
//��ȡʵ��ppm
float MQ2_GetPPM(void)
{
	float Vrl = MQ2_GetVol();
	//Vrl = 3.3;
	float RS = (3.3f - Vrl) / Vrl * RL; 
	if(boot_time_ms < 3000) // ��ȡϵͳִ��ʱ�䣬3sǰ����У׼
	{
		  MQ2_PPM_Calibration(RS);
	}
	//MQ2_PPM_Calibration(RS);
	//return 2*R0;
	float ppm = 613.9f * pow(RS/R0, -2.407f);
	return  ppm;
}
