/*-------------------------------------------------*/
/*                     �ʵ��Ŷӿ���                */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�������ϴ���Դ�ļ�        */
/*                                                 */
/*-------------------------------------------------*/

#include "main.h"       //������Ҫ��ͷ�ļ�
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "dateup.h"      //������Ҫ��ͷ�ļ�
#include "delay.h"      //������Ҫ��ͷ�ļ�
#include "usart1.h"     //������Ҫ��ͷ�ļ�
#include "usart2.h"     //������Ҫ��ͷ�ļ�
#include "timer2.h"     //������Ҫ��ͷ�ļ�
#include "timer3.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�
#include "wifi.h"	      //������Ҫ��ͷ�ļ�
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "iwdg.h"       //������Ҫ��ͷ�ļ�



/*-------------------------------------------------*/
/*����������ʱ�ϴ��������ݵ�����                   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Data_State(void)
{

	char data[256];             //������
//	u8 tempdata = 0, humidata = 0;	//���ڱ�����ʪ�ȵı���	
//	u8 ppm = 0;
	if((SystemTimer - TEHUTimer)>=2){                                     //ȫ��ʱ�� �� ��ʪ�ȼ�ʱʱ�����ٲ�30s������if
		TEHUTimer = SystemTimer;                                           //�ѵ�ǰ��ȫ��ʱ�䣬��¼����ʪ�ȼ�ʱ����
		//DHT11_Read_Data(&tempdata, &humidata);	//��ȡ��ʪ��ֵ                                  
		u1_printf("�¶ȣ�%d  ʪ�ȣ�%d  ����Ũ�ȣ�%d\r\n",temp,humi,ppm);         //�����������
		sprintf(data,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"Humi\":%d,\"Temp\":%d,\"MQ2\":%d},\"version\":\"1.0.0\"}",humi,temp,ppm);  //��������
		MQTT_PublishQs0(P_TOPIC_NAME,data,strlen(data));                  //������ݵ����ͻ�����

	}

}
