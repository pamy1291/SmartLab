#include "sys.h"
#include "usart.h"
#include "rc522.h"
#include "led.h"
#include "oled.h"
#include "key4_4.h"
#include "mq2.h"
#include "mlx90614.h"
#include "syn6288.h"

#include "main.h"
#include "delay.h"
#include "iwdg.h"
#include "usart1.h"
#include "usart2.h"
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"
#include "wifi.h"
#include "mqtt.h"
#include "iwdg.h"
#include "dateup.h"
#include "dht11.h"

#include <stdio.h>
#include <string.h>

/*
 *宏定义
 */
#define APP_TASK 1 //该宏为1时才会执行APP任务
#define ALIYUN_TASK 0//阿里云任务
/**
* RFID
*   连线说明：
*   1--SDA  <----->PA6
*   2--SCK  <----->PA5
*   3--MOSI <----->PA7
*   4--MISO <----->PA4
*   5--悬空
*   6--GND <----->GND
*   7--RST <----->PB0
*   8--VCC <----->VCC
**/
/**************syn6288芯片设置命令*********************/
unsigned char SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //停止合成
unsigned char SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
unsigned char SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //恢复合成
unsigned char SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //状态查询
unsigned char SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状态命令

/*****************数据记录*****************/
char cp_time[11+4] ;//0字节初始化为'R',数据字头，便于识别；8位时间文本；1位RFID识别到的ID组号；4位红外温度数据；
u8 USART3_RX_BUF[4];//蓝牙数据接收数组
u8 USART3_TX_BUF[40];//蓝牙数据发送数组
u8 num_device[3];//发送数组使用设备的id,字头'D'
float temp_mlx;
u8 temp,humi,ppm;//温度，湿度，烟雾浓度百分比
u8 Rxnum;//接收数组序号，从0开始
u8 card_num = 0;//标签id序号
u8 card_num_device = 0;//记录使用设备的id
u8 face_num = 0;//人脸id序号
u8 tempH,tempL,rhH,rhL,ppm_pre;//各模块阈值

u8 led_temp_status,led2_status,led_humi_status,led4_status,led_ppm_status;//报警指示，值1报警
u8 alarm_num;//指示哪个节点报警
u8 set_flag = 1;//报警标志位
u8 r_flag;//1 进入RFID读卡，0退出
u8 y_flag;//1 进入疫情防控任务，0退出
u8 f_flag;//1 进入人脸识别任务，0退出
u8 n_flag;//1 进入RFID卡添加任务，0退出
u8 d_flag;//1 进入RFID卡设备管理任务，0退出

//定义接收摄像头识别的数组
u8 mask[4],name[20];//口罩识别，人脸识别

unsigned int TEHUTimer=0;     //用于温湿度采集的计时的变量   单位秒
unsigned int SystemTimer=0;   //变量声明全局时间变量

//自定义函数
void data_send(u8 *Tx_Buf);//温湿度等数据蓝牙发送
void alarm(void);//用于报警
u8 face(const u8 *name);//人脸识别
void new_card(u8* C_oled_use_array);//卡添加
void aliyun(void);


//自定义获取无符号字符长度函数
size_t ac_strlen(unsigned char* p)
{
	int count = 0;
	while((*p) != '\0')
	{
		count++;
		p++;
	}
	return count;
}


int main(void)
{
	char* real_time;
	
	u8 key_board=0;//板子按键
	u8 TandH[10];
	u8 *p=TandH;
	
	cp_time[0] = 'R';//初始化信息字头
	u8 C_oled_use_array[11] = {'0','0','0','0','0','0','0','0','0','0','\0'};//oled显示用的数组
	
	tempH=40;	//温度阈值
	tempL=10;
	rhH=70;		//湿度阈值
	rhL=30;
	ppm_pre=50;//烟雾浓度阈值
	mask[1] = '#';//初始化口罩识别，防进入
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	
	//IWDG_Init(5,1*625);               //独立看门狗 时间1*2s
	delay_init();	    	 //延时函数初始化

	LED_Init();
	KEY_Init();
	RC522_Init();       //初始化射频卡模块
	OLED_Init();			//初始化OLED  
	OLED_Clear(); 

	MQ2_Init();
	SMBus_Init();//红外测温模块mlx90614

	//中断接收顺序
	Usart2_Init(115200);	 	//esp8266
	USART3_Init(9600);	 	//蓝牙
	USART4_Init(9600);		//K210
	Usart1_Init(9600);  //WiFi调试

	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
	AliIoT_Parameter_Init();	    //初始化连接阿里云IoT平台MQTT服务器的参数
	//u1_printf("调试");
	DHT11_Init();
	
	delay_ms(500);
	
    while(1)
    {

//MEAU:	
		DHT11_Read_Data(&temp,&humi);//获取温湿度
		ppm = MQ2_GetPPM()/2;//浓度百分比量化

		#if APP_TASK //APP及各模块任务
			
		if(set_flag == 1) 
		{	
			alarm();//警报
			data_send(USART3_TX_BUF);//温湿度等数据发送
			//delay_ms(200);
		}
			
		if(d_flag == 1)//进入设备管理任务，单线程
		{
			d_flag = 1;
			num_device[0] = 'D';	
			while(1)//RFID识别
			{
				OLED_Clear();
				OLED_ShowString(0,0,"Device Manage");
				card_num_device = card_unlock();//RFID 返回获取到的第几张卡
				num_device[1] = card_num_device%10 + '0';
				switch(card_num_device)
				{
					case 0:
					OLED_Clear();
					OLED_ShowString(0,2,"no card");
					OLED_ShowString(0,4,"Please Swipe Again");
					delay_ms(1000);
					OLED_Clear();
					OLED_ShowString(0,0,"Device Manage");
					break;
								
					case 255:
					break;	

					default:
					UART3_SendString(num_device,ac_strlen(num_device));	
					OLED_Clear();
					OLED_ShowString(0,2,"welcome");
					OLED_ShowChar(64,2,num_device[1]);
					delay_ms(1000);		
					break;								
				}
				if(d_flag == 0)//退出设备管理
				{					
					d_flag = 0;//退出后清0，防止再次进入
					OLED_Clear();
					break;
				}						
			} 
		}
			
		//OLED_Clear();  //清屏
		OLED_ShowString(0,0,"A.Card unlock");		//菜单ABC，键入A：用卡解锁，键入B：录入卡，键入C：密码解锁，键入D：修改密码
		OLED_ShowString(0,4,"B.New Card");

		key_board = KEY_Scan(0);
		if(key_board!=WKUP_PRES)
		{//非退出，进入任务，包含RFID，口罩，人脸
			if(key_board==KEY0_PRES || r_flag==1)//键入A：用卡解锁
			{
//unlock:						
				r_flag = 1;
				LED0 = !LED0;//调试板按键0
				while(1)//RFID识别
				{
					OLED_Clear();
					OLED_ShowString(0,0,"A.Card unlock");
					if(y_flag == 1) 
					{
					//疫情防控任务
					temp_mlx = SMBus_ReadTemp();  //获取体温
					sprintf(cp_time+10,"%.1f",temp_mlx);
					OLED_Clear();
					OLED_ShowString(0,0,"epi pre");
					OLED_ShowString(0,6,"mlx=");
					OLED_ShowString(32,6,(u8*)(cp_time+10));
					OLED_ShowString(64,6,"C");
											
					if(mask[1] == '1' && temp_mlx < 37.5) {
					//欢迎
					OLED_ShowString(0,2,"welcome");
					SYN_FrameInfo(0, "[v16][m5][t6]欢迎！");
					mask[1] = '#';//清标志
					}
					else if(mask[1] == '0' && temp_mlx < 37.5) 
					{
					//请戴好口罩
					OLED_ShowString(0,2,"no mask");
					SYN_FrameInfo(0, "[v16][m5][t6]请戴好口罩！");
					mask[1] = '#';//清标志
					}
					else if(mask[1] != '#' && temp_mlx >= 37.5)
					{
					//温度过高
					OLED_ShowString(0,2,"high temp");
					SYN_FrameInfo(0, "[v16][m5][t6]请注意，您体温过高！");
					mask[1] = '#';//清标志
					}
					delay_ms(500);
					}
					//RFID任务 || 人脸识别
					if(f_flag == 1) 
					{ //人脸识别
						face_num = face(name);//获取人脸识别ID
						cp_time[1] = face_num%10 + '0';											
						temp_mlx = SMBus_ReadTemp();  //获取体温
						sprintf(cp_time+10,"%.1f",temp_mlx);
											
						OLED_Clear();
						OLED_ShowString(0,0,"face recognition");
						OLED_ShowString(0,2,"ID:");
						OLED_ShowChar(24,2,face_num+'0');
											
						for(int i=0;i<3;i++)
						{
							UART3_SendString((u8*)cp_time,14);//通过串口发送数据
						}
						switch(face_num)
						{
							case 0:
							break;
														
							case 1:
							SYN_FrameInfo(0, "[v16][m5][t6]欢迎庞先生。");
							break;
														
							case 2:
							SYN_FrameInfo(0, "[v16][m5][t6]欢迎。");
							break;														
														
							default:
							break;
						}
						name[1] = '0';//清除识别结果
						delay_ms(500);
					}

					card_num = card_unlock();//RFID 返回获取到的第几张卡
					switch(card_num)
					{
						case 0:
						OLED_Clear();
						OLED_ShowString(0,2,"no card");
						OLED_ShowString(0,4,"Please Swipe Again");
						delay_ms(1864);
						OLED_Clear();
						OLED_ShowString(0,0,"A.Card unlock");
						break;

						case 255:
						break;
								
						default://识别为库中的卡
						real_time = __TIME__;//获取时间
						cp_time[1] = card_num%10 + '0';											
						strcat(cp_time+2,real_time);
						temp_mlx = SMBus_ReadTemp();  //获取体温
						sprintf(cp_time+10,"%.1f",temp_mlx);
						for(int i=0;i<3;i++)
						{
							UART3_SendString((u8*)cp_time,14);//通过串口发送数据
						}
						OLED_Clear();
						OLED_ShowString(0,2,"welcome,card");
						OLED_ShowChar(96,2,card_num+'0');
						OLED_ShowString(0,4,"Door is open");
						delay_ms(1864);								
//dht11_mq2:						
						OLED_Clear();
						OLED_ShowString(0,0,"Welcome");
						OLED_ShowString(0,2,"Data show");

						OLED_ShowString(0,4,"T=");
						OLED_ShowString(48,4,"H=");
						//DHT11_Read_Data(&temp,&humi);
						TandH[0] = temp/10+'0';
						TandH[1] = temp%10+'0';
						TandH[2] = '%';
						TandH[3] = '\0';
						TandH[4] = humi/10+'0';
						TandH[5] = humi%10+'0';
						TandH[6] = '%';
						TandH[7] = '\0';
						p = TandH;
						OLED_ShowString(16,4,p);
						p = TandH+4;
						OLED_ShowString(64,4,p);
		//					烟雾浓度设置
		//					ppm = MQ2_GetPPM()/2;//浓度百分比量化
		//					OLED_ShowString(0,6,"ppm=");
		//					OLED_ShowNum(32,6,ppm,2,8);
		//					OLED_ShowString(56,6,"%");
		//					红外测温设置
						OLED_ShowString(0,6,"mlx=");
						OLED_ShowString(32,6,(u8*)(cp_time+10));
						OLED_ShowString(64,6,"C");
						delay_ms(1864);
						break;
					};

					if(n_flag == 1) //卡添加
					{
						new_card(C_oled_use_array);
					}
					delay_ms(20);
								
					if((key_board = KEY_Scan(0))==KEY0_PRES || r_flag == 0)//退出A
					{
//quit:								
						r_flag = 0;//退出后清0
						OLED_Clear();
						break;
					}
				}//while(1)RFID识别
			}//if(KEY=='A'|| key_board==KEY0_PRES)//键入A：用卡解锁

			if(key_board==KEY1_PRES)//键入B：录入卡
			{
				LED1 = !LED1;//调试板按键1
				OLED_Clear();
				OLED_ShowString(0,0,"B.New Card");
				while(1)
				{
					switch(card_infor_entry(C_oled_use_array))
					{
						case 0:
						OLED_ShowString(0,2,"New Card:");
						OLED_ShowString(0,4,C_oled_use_array);//显示卡ID
						LED0 = 0;
						delay_ms(1864);
						delay_ms(1864);
						LED0 = 1;
						OLED_Clear();
						OLED_ShowString(0,0,"B.New Card");
						break;
								
						case 255:
						//printf("case1\r\n");
						break;
								
						default:
						//printf("default\r\n");
						OLED_ShowString(0,2,"error");
						OLED_ShowString(0,4,"card is already exist");
						LED1 = 0;
						delay_ms(1864);
						delay_ms(1864);
						LED1 = 1;
						OLED_Clear();
						OLED_ShowString(0,0,"B.New Card");
						break;
								
					};
					delay_ms(20);
							
					if((key_board = KEY_Scan(0))==KEY1_PRES)//退出B
					{
						OLED_Clear();
						break;
					}
								
				}//while
			}//if(key_board==KEY1_PRES)//键入B：录入卡
			
			delay_ms(100);
//			goto MEAU;

		}//if(key_board!=WKUP_PRES)
		#endif
		
		#if ALIYUN_TASK
		aliyun();
		#endif
	}//while
}//main

/*****************************************功能函数实现***************************************************/
/*
 *数据发送格式转换
 */
void data_send(u8 *Tx_Buf)
{
	u8 len;
	Tx_Buf[0]='T';						//帧头
	Tx_Buf[1]=temp/10%10+'0';		 	//将温湿度数据送往发送数组,送给蓝牙模块让手机APP显示
	Tx_Buf[2]=temp%10+0x30;		
	Tx_Buf[3]=humi/10%10+0x30;
	Tx_Buf[4]=humi%10+0x30;
	
	Tx_Buf[5]=tempL/10%10+0x30;		 	//将低温度阈值送往发送数组,送给蓝牙模块让手机APP显示
	Tx_Buf[6]=tempL%10+0x30;		
	Tx_Buf[7]=rhL/10%10+0x30;
	Tx_Buf[8]=rhL%10+0x30;
	
	Tx_Buf[9]=led_temp_status+0x30;			//发送低温预警状态
	Tx_Buf[10]=led2_status+0x30;		  //高温预警
	Tx_Buf[11]=led_humi_status+0x30;			//低湿度预警
	Tx_Buf[12]=led4_status+0x30;			//高湿度预警

	Tx_Buf[13]=tempH/10%10+0x30;		 	//将高阈值温度数据送往发送数组,送给蓝牙模块让手机APP显示
	Tx_Buf[14]=tempH%10+0x30;	

	Tx_Buf[15]=rhH/10%10+0x30;				//将高阈值湿度数据送往发送数组,送给蓝牙模块让手机APP显示
	Tx_Buf[16]=rhH%10+0x30;
		
	Tx_Buf[17]=ppm/10%10+0x30;				//当前浓度
	Tx_Buf[18]=ppm%10+0x30;
	Tx_Buf[19]=ppm_pre/10+0x30;		//浓度阈值
	Tx_Buf[20]=ppm_pre%10+0x30;
	Tx_Buf[21]=led_ppm_status+0x30;   //浓度报警位

	Tx_Buf[22]=card_num%10+0x30;
		
		
	len = ac_strlen(Tx_Buf);
	UART3_SendString(Tx_Buf,len);
	delay_ms(500);
}

//报警函数
void alarm(void)
{
	//低温度预警
	if(temp<=tempL) 
    {
		led_temp_status=1;
		alarm_num=1;
	}				
	else if(temp>=tempH)
    {
		led_temp_status=2;
		alarm_num=2;
	}
	else 
	{
		led_temp_status=0;
		alarm_num=0;
	}
	//低湿度预警	
	if(humi<=rhL)	
    {
		led_humi_status=1;
		alarm_num=3;
	}			
	else if(humi>=rhH)
    {
		led_humi_status=2;
		alarm_num=4;
	}
	else 
    {
		led_humi_status=0;
		alarm_num=0;
	}
	//浓度预警	
	if(ppm>ppm_pre)	
    {
		led_ppm_status=1;
		alarm_num=5;
	}		
	else 
    {
		led_ppm_status=0;
		alarm_num=0;
	}
}
/*
 *人脸识别函数，返回名字序号
 */
u8 face(const u8 *name) 
{
	if(name[0] == 'P') 
	{
		return name[1] - '0';
	}
	else 
	{
		return 0;
	}
}
/*
 *添加新卡
 */
void new_card(u8* C_oled_use_array) 
{
	while(1)
	{
		OLED_Clear();
		OLED_ShowString(0,0,"New Card");
		switch(card_infor_entry(C_oled_use_array))
		{
			case 0:
			OLED_ShowString(0,2,"New Card:");
			OLED_ShowString(0,4,C_oled_use_array);//显示卡ID
			delay_ms(1000);
			OLED_Clear();
			OLED_ShowString(0,0,"New Card");
			break;
						
			case 255:
			break;
						
			default:
			OLED_ShowString(0,2,"error");
			OLED_ShowString(0,4,"card is already exist");
			delay_ms(1000);
			OLED_Clear();
			OLED_ShowString(0,0,"New Card");
			break;	
		};
		delay_ms(20);
		if(n_flag == 0)//退出
		{
			OLED_Clear();
			break;
		}
		
	}//while
}
/*
 *连接阿里云
 */
void aliyun(void)
{		
//	while(1)                       //主循环
//	{		
		//WiFi_printf("调试");//u2_printf
		/*--------------------------------------------------------------------*/
		/*            Connect_flag=1同服务器建立了连接                        */
		/*--------------------------------------------------------------------*/
	if(Connect_flag==1)
	{     
		connet_on();         //程序连接成功接收处理程序
		Data_State();         //各种数据的任务
		delay_ms(1);          //延时1ms，必须有，里面有喂狗，不然看门狗会溢出，造成复位				
	}	
	/*--------------------------------------------------------------------*/
	/*                         准备连接服务器                             */
	/*--------------------------------------------------------------------*/
	else
	{ 
		connet_first();
	}

//	}//while(1)
}

//串口3中断服务程序,用户蓝牙USART3_TXD(PB.10)USART3_RXD(PB.11)
void USART3_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收标志为不为0
	{
	    Res =USART_ReceiveData(USART3);	//读取接收到的数据
		USART3_RX_BUF[Rxnum] = Res;
		if(USART3_RX_BUF[Rxnum]==0x0A)//接收到'\n'
			Rxnum=0;
		else
			Rxnum++;
		USART_ClearFlag(USART3,USART_FLAG_RXNE);//接收标志位清0
		if(USART3_RX_BUF[0]=='P')//进入RFID任务
			r_flag = 1;
		if(USART3_RX_BUF[0]=='p')//退出RFID任务
			r_flag = 0;
				
		if(USART3_RX_BUF[0]=='Y')//进入疫情防控任务
			y_flag = 1;
		if(USART3_RX_BUF[0]=='y')//退出疫情防控任务
			y_flag = 0;
				
		if(USART3_RX_BUF[0]=='F')//进入人脸识别任务
			f_flag = 1;
		if(USART3_RX_BUF[0]=='f')//退出人脸识别任务
			f_flag = 0;
				
		if(USART3_RX_BUF[0]=='N')//进入RFID卡添加任务
			n_flag = 1;
		if(USART3_RX_BUF[0]=='n')//退出RFID卡添加任务
			n_flag = 0;
		if(USART3_RX_BUF[0]=='D')//进入RFID卡添加任务
			d_flag = 1;
		if(USART3_RX_BUF[0]=='d')//退出RFID卡添加任务
			d_flag = 0;
								
		if(USART3_RX_BUF[0]=='8')//接收到的APP传送来的状态清除设置指令头数据
		{ 
			if(USART3_RX_BUF[1]=='0')			  
			{
				set_flag=0;					//状态清除
			}
			if(USART3_RX_BUF[1]=='1')			  
			{
				set_flag=1;					//状态开启
			}
				USART3_RX_BUF[1]=0;				  //清零
		    }
/********************************温湿度阈值设置*************************************/
			if(USART3_RX_BUF[0]=='6')				   //接收到的APP传送来的低温度设置指令头数据..............
			{
				if(USART3_RX_BUF[1]=='0')			   //接收到的APP传送来的温度设置定值加1指令
				{
					tempL++;					   //温度定值加1
					if(tempL>99)
					tempL=99;
				}
				if(USART3_RX_BUF[1]=='1')			   //接收到的APP传送来的温度设置定值减1指令
				{
					tempL--;					   //温度定值减1
					if(tempL<1)
					tempL=1;
				}
					USART3_RX_BUF[1]=0;				   //清零
				}
				
				if(USART3_RX_BUF[0]=='4')				   //接收到的APP传送来的高温度设置指令头数据................
				{
					if(USART3_RX_BUF[1]=='1')			   //接收到的APP传送来的温度设置定值加1指令
					{
						tempH++;					   //温度定值加1
						if(tempH>99)
						tempH=99;
					}
					if(USART3_RX_BUF[1]=='0')			   //接收到的APP传送来的温度设置定值减1指令
					{
						tempH--;					   //温度定值减1
						if(tempH<1)
						tempH=1;
					}
					USART3_RX_BUF[1]=0;				   //清零
				}
				
				if(USART3_RX_BUF[0]=='7')				  //接收到的APP传送来的低湿度设置指令头数据..................
				{
					if(USART3_RX_BUF[1]=='0')			  //接收到的APP传送来的湿度设置定值加1指令
					{
						rhL++;					  //湿度定值加1
						if(rhL>99)
						rhL=99;
					}
					if(USART3_RX_BUF[1]=='1')			  //接收到的APP传送来的湿度设置定值减1指令
					{
							rhL--;					  //湿度定值减1
							if(rhL<1)
							rhL=1;
					}
					USART3_RX_BUF[1]=0;				  //清零
				}
				
				if(USART3_RX_BUF[0]=='5')				  //接收到的APP传送来的高湿度设置指令头数据....................
				{
					if(USART3_RX_BUF[1]=='1')			  //接收到的APP传送来的湿度设置定值加1指令
					{
						rhH++;					  //湿度定值加1
						if(rhH>99)
						rhH=99;
					}
					if(USART3_RX_BUF[1]=='0')			  //接收到的APP传送来的湿度设置定值减1指令
					{
							rhH--;					  //湿度定值减1
							if(rhH<1)
							rhH=1;
					}
					USART3_RX_BUF[1]=0;				  //清零
				}
				if(USART3_RX_BUF[0]=='9')				  //接收到的APP传送来的烟雾阈值设置指令头数据
				{
					if(USART3_RX_BUF[1]=='0')			  
					{
						ppm_pre++;
						if(ppm_pre>99)
						ppm_pre=99;
					}
					if(USART3_RX_BUF[1]=='1')			  
					{
						ppm_pre--;
						if(ppm_pre<1)
						ppm_pre=1;
					}
					USART3_RX_BUF[1]=0;				  //清零
				}				
		
	}
}

//串口4中断服务程序，接收k210识别结果UART4_TXD(PC.10)UART4_RXD(PC.11)
void UART4_IRQHandler(void)      
{
	static u8 mask_num, name_num;
	u8 Res;
	u8 mask_flag = 0;	//口罩识别接收标志
	u8 name_flag = 0; //姓名识别接收标志
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(UART4);	//读取接收到的数据
		USART_ClearFlag(UART4,USART_FLAG_RXNE);//清除接收标志

		//初始化数组,字头处理
		if(Res=='M')
		{
			name_flag = 0;	//关闭姓名数据接收
			mask_flag = 1; //准备开始接收口罩识别结果
			mask_num = 0;
			mask[mask_num] = Res;
		}
		else if(Res=='P')
		{
			mask_flag = 0;	//关闭口罩识别结果数据接收
			name_flag = 1;	//准备开始接收姓名识别结果
			name_num = 0;
			name[name_num] = Res;			
		}
		
		//实际数据
		if(mask_flag && '0' <= Res && Res <= '9')
		{
			mask[++mask_num] = Res;
		}
		if(name_flag && (('a' <= Res && Res <= 'z') || ('0' <= Res && Res <= '9')))
		{
			name[++name_num] = Res;			
		}
		
	}
}

//串口1中断服务程序	
//void USART1_IRQHandler(void)                	
//{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//		USART_ClearFlag(USART1,USART_FLAG_RXNE);
//		printf("%s",&Res);
//		if(Res=='1')
//		{
//			//usr code
//		}
//		
//	}
//}
