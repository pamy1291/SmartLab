/*-------------------------------------------------*/
/*                     �ʵ��Ŷӿ���                */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�ָ����жϷ�������Դ�ļ�           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //������Ҫ��ͷ�ļ�
#include "stm32f10x_it.h"  //������Ҫ��ͷ�ļ�
#include "main.h"          //������Ҫ��ͷ�ļ�
#include "delay.h"         //������Ҫ��ͷ�ļ�
#include "usart1.h"        //������Ҫ��ͷ�ļ�
#include "usart2.h"        //������Ҫ��ͷ�ļ�
#include "timer4.h"        //������Ҫ��ͷ�ļ�
#include "wifi.h"	       //������Ҫ��ͷ�ļ�
#include "timer3.h"        //������Ҫ��ͷ�ļ�
#include "mqtt.h"          //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*������������2�����жϺ���                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   
{                      
	if((USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)&&(Connect_flag==0)){           //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ��ҵ�ǰ��û�о������Ϸ����� ����if��֧
		if(USART2->DR){                                 								  //����ָ������״̬ʱ������ֵ�ű��浽������	
			Usart2_RxBuff[Usart2_RxCounter]=USART2->DR;                                   //���浽������	
			Usart2_RxCounter ++;                                                          //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
		}		
	}	
	if((USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)&&(Connect_flag==1)){           //��USART_IT_IDLE��־��λ����ʾ�����ж�	�ҵ�ǰ�Ѿ������Ϸ����� ����if��֧
		Usart2_RxCounter = USART2->SR;                                                    //���USART_IT_IDLE��־  ����1   
		Usart2_RxCounter = USART2->DR;                                                    //���USART_IT_IDLE��־  ����2
		DMA_Cmd(DMA1_Channel6,DISABLE);                                                   //�ر�DMA
		Usart2_RxCounter = USART2_RXBUFF_SIZE -  DMA_GetCurrDataCounter(DMA1_Channel6);   //��ȡ���ڽ��յ�������
		memcpy(&MQTT_RxDataInPtr[2],Usart2_RxBuff,Usart2_RxCounter);                      //�������ݵ����ջ�����
		MQTT_RxDataInPtr[0] = Usart2_RxCounter/256;                                       //��¼���ݳ��ȸ��ֽ�
		MQTT_RxDataInPtr[1] = Usart2_RxCounter%256;                                       //��¼���ݳ��ȵ��ֽ�
		MQTT_RxDataInPtr+=RBUFF_UNIT;                                                     //ָ������
		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                                           //���ָ�뵽������β����
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                                         //ָ���λ����������ͷ
		Usart2_RxCounter = 0;                                                             //����2������������������				
		DMA1_Channel6->CNDTR=USART2_RXBUFF_SIZE;                                          //�������ý������ݸ���           
		DMA_Cmd(DMA1_Channel6,ENABLE);                                                    //����DMA
	}
} 
/*-------------------------------------------------*/
/*��������DMA1ͨ��7�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7) != RESET){         //���DMA1_IT_TC7��λ����ʾDMA1ͨ��7��������жϣ�����if	
		DMA_ClearITPendingBit(DMA1_IT_TC7);            //���DMA1ͨ��7 ��������жϱ�־
		DMA_Cmd(DMA1_Channel7,DISABLE);                //�ر�DMA1ͨ��7
		DMA_flag = 0;                                  //��־λ��� ����״̬
	}
}
/*-------------------------------------------------*/
/*����������ʱ��3�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){   //���TIM_IT_Update��λ����ʾTIM3����жϣ�����if	
		switch(Ping_flag){                               //�ж�Ping_flag��״̬
			case 0:										 //���Ping_flag����0����ʾ����״̬������Ping����  
					MQTT_PingREQ(); 					 //����Ping���ĵ����ͻ�����  
					break;
			case 1:										 //���Ping_flag����1��˵����һ�η��͵���ping���ģ�û���յ��������ظ�������1û�б����Ϊ0�������������쳣������Ҫ��������pingģʽ
					TIM3_ENABLE_2S(); 					 //���ǽ���ʱ��6����Ϊ2s��ʱ,���ٷ���Ping����
					MQTT_PingREQ();  					 //����Ping���ĵ����ͻ�����  
					break;
			case 2:										 //���Ping_flag����2��˵����û���յ��������ظ�
			case 3:				                         //���Ping_flag����3��˵����û���յ��������ظ�
			case 4:				                         //���Ping_flag����4��˵����û���յ��������ظ�	
					MQTT_PingREQ();  					 //����Ping���ĵ����ͻ����� 
					break;
			case 5:										 //���Ping_flag����5��˵�����Ƿ����˶��ping�����޻ظ���Ӧ�������������⣬������������
					Connect_flag = 0;                    //����״̬��0����ʾ�Ͽ���û���Ϸ�����
					TIM_Cmd(TIM3,DISABLE);               //��TIM3 				
					break;			
		}
		Ping_flag++;           		             		 //Ping_flag����1����ʾ�ַ�����һ��ping���ڴ��������Ļظ�
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);      //���TIM3����жϱ�־ 	
	}
}
/*-------------------------------------------------*/
/*����������ʱ��2�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM2_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){  //���TIM_IT_Update��λ����ʾTIM2����жϣ�����if	
		SystemTimer++;                                  //ȫ�ּ�ʱ�ı���+1,��ʾ��ȥ��1����
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     //���TIM2����жϱ�־ 	
	}
}
/*-------------------------------------------------*/
/*�����������������жϴ�������                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ӳ�������������жϴ�������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*���������ڴ�����жϴ�������                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ԥȡָʧ�ܣ��洢������ʧ���жϴ�������   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������δ�����ָ���Ƿ�״̬��������           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*�����������жϣ�SWI ָ����õĴ�������           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*�����������Լ������������                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*���������ɹ����ϵͳ����������                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*-------------------------------------------------*/
/*��������SysTicϵͳ��શ�ʱ����������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
//void SysTick_Handler(void)
//{
// 
//}