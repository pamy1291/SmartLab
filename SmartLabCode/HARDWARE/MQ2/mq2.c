#include "mq2.h"
#include <math.h>
#include "delay.h"
//#include "delay.h"
//mq2 mq7 mq135的采集
//=============================================================================
//变量名称：
//DMA_buf[0]  MQ2    烟雾
//DMA_buf[1]  MQ7    一氧化碳
//DMA_buf[2]  MQ135  空气质量
//=============================================================================
//转化ppm相关数据定义
#define CAL_PPM 20  // 校准环境中PPM值
#define RL			5		// RL阻值，实际环境矫正为R0
static float R0 = 14; // 元件在洁净空气中的阻值
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
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AIN;                       //模拟输入
	GPIO_Struct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	ADC_InitTypeDef ADC_Struct={0};	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                            //设置ADC时钟 72/6<14
	ADC_Struct.ADC_Mode = ADC_Mode_Independent;                  //独立工作模式
	ADC_Struct.ADC_ContinuousConvMode = ENABLE;                  //连续模式
	ADC_Struct.ADC_ScanConvMode = DISABLE;                        //多通道模式
	ADC_Struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件触发启动
	ADC_Struct.ADC_DataAlign = ADC_DataAlign_Right;              //右对齐
	ADC_Struct.ADC_NbrOfChannel = 1;                             //规定了顺序进行规则转换的 ADC 通道的数目
	ADC_Init(ADC1,&ADC_Struct);
	//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);//MQ2	
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2,ADC_SampleTime_239Cycles5);//MQ7	
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,3,ADC_SampleTime_239Cycles5);//MQ135

	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE); 
	
	//校准:减小误差
	ADC_ResetCalibration(ADC1);                            //重置寄存器
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);        //等待重置完成
	ADC_StartCalibration(ADC1);                            //启动校准，用校准寄存器 校准 ADC1
	while(ADC_GetCalibrationStatus(ADC1)==SET);             //等待校准完成		
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);                 //启动转换 使能或者失能指定的 ADC 的软件转换启动功能 			
}

void DMA_Config(void)
{
	DMA_InitTypeDef DMA_Struct={0};
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);               //开启DMA时钟

	DMA_Struct.DMA_PeripheralBaseAddr =(u32) &ADC1->DR;             //定义DMA外设基地址
	DMA_Struct.DMA_DIR = DMA_DIR_PeripheralSRC;                     //外设作为数据传输的来源
	DMA_Struct.DMA_BufferSize = 1;                                  //地址递增3次（单位为字宽）
	DMA_Struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址寄存器不变
	DMA_Struct.DMA_MemoryInc = DMA_MemoryInc_Enable;                //内存地址寄存器递增
	DMA_Struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;    //数据宽度为16位
	DMA_Struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
	DMA_Struct.DMA_Mode = DMA_Mode_Circular;                        //工作在循环缓存模式
	DMA_Struct.DMA_Priority = DMA_Priority_High;                    //优先级
	DMA_Struct.DMA_M2M = DMA_M2M_Disable;                           //DMA通道没有设置为内存到内存传输
	DMA_Struct.DMA_MemoryBaseAddr = (u32)&DMA_buf[0];               //内存基地址
	
	DMA_Init(DMA1_Channel1,&DMA_Struct);
	DMA_Cmd(DMA1_Channel1,ENABLE);
}

//不采用DMA的多通道采集方法
//u16 ADC_Result(u8 ADC_Channel_x)
//{
//	u16 ADC_val = ADC_GetConversionValue(ADC1); //返回最近一次 ADCx 规则组的转换结果
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_x,1,ADC_SampleTime_239Cycles5);
//	ADC_SoftwareStartConvCmd(ADC1,ENABLE); //启动转换	 使能或者失能指定的 ADC 的软件转换启动功能 	
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
 
 // MQ2传感器数据处理，实际电压
float MQ2_GetVol(void)
{
	//u16 adcx = Get_Adc_Average(ADC_Channel_1, 10);//不开启dma
	float Vrl = 3.3f * DMA_buf[0] / 4095.0;
	return  Vrl;
}
// 传感器校准函数，经测试为14
void MQ2_PPM_Calibration(float RS)
{
	R0 = RS / pow(CAL_PPM/613.9f, -1/2.407f);
}
//获取实际ppm
float MQ2_GetPPM(void)
{
	float Vrl = MQ2_GetVol();
	//Vrl = 3.3;
	float RS = (3.3f - Vrl) / Vrl * RL; 
	if(boot_time_ms < 3000) // 获取系统执行时间，3s前进行校准
	{
		  MQ2_PPM_Calibration(RS);
	}
	//MQ2_PPM_Calibration(RS);
	//return 2*R0;
	float ppm = 613.9f * pow(RS/R0, -2.407f);
	return  ppm;
}
