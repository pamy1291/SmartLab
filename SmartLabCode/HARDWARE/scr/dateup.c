/*-------------------------------------------------*/
/*                     质点团队开发                */
/*-------------------------------------------------*/
/*                                                 */
/*          实现数据上传的源文件        */
/*                                                 */
/*-------------------------------------------------*/

#include "main.h"       //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件
#include "dateup.h"      //包含需要的头文件
#include "delay.h"      //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart2.h"     //包含需要的头文件
#include "timer2.h"     //包含需要的头文件
#include "timer3.h"     //包含需要的头文件
#include "timer4.h"     //包含需要的头文件
#include "wifi.h"	      //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件
#include "iwdg.h"       //包含需要的头文件



/*-------------------------------------------------*/
/*函数名：定时上传各种数据的任务                   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Data_State(void)
{

	char data[256];             //缓冲区
//	u8 tempdata = 0, humidata = 0;	//用于保存温湿度的变量	
//	u8 ppm = 0;
	if((SystemTimer - TEHUTimer)>=2){                                     //全局时间 和 温湿度计时时间至少差30s，进入if
		TEHUTimer = SystemTimer;                                           //把当前的全局时间，记录到温湿度计时变量
		//DHT11_Read_Data(&tempdata, &humidata);	//读取温湿度值                                  
		u1_printf("温度：%d  湿度：%d  烟雾浓度：%d\r\n",temp,humi,ppm);         //串口输出数据
		sprintf(data,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"Humi\":%d,\"Temp\":%d,\"MQ2\":%d},\"version\":\"1.0.0\"}",humi,temp,ppm);  //构建数据
		MQTT_PublishQs0(P_TOPIC_NAME,data,strlen(data));                  //添加数据到发送缓冲区

	}

}
