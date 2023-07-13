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
 *�궨��
 */
#define APP_TASK 1 //�ú�Ϊ1ʱ�Ż�ִ��APP����
#define ALIYUN_TASK 1//����������
/**
* RFID
*   ����˵����
*   1--SDA  <----->PA6
*   2--SCK  <----->PA5
*   3--MOSI <----->PA7
*   4--MISO <----->PA4
*   5--����
*   6--GND <----->GND
*   7--RST <----->PB0
*   8--VCC <----->VCC
**/
/**************syn6288оƬ��������*********************/
unsigned char SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //ֹͣ�ϳ�
unsigned char SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //��ͣ�ϳ�
unsigned char SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //�ָ��ϳ�
unsigned char SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //״̬��ѯ
unsigned char SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //����POWER DOWN ״̬����

/*****************���ݼ�¼*****************/
char cp_time[11+4] ;//0�ֽڳ�ʼ��Ϊ'R',������ͷ������ʶ��8λʱ���ı���1λRFIDʶ�𵽵�ID��ţ�4λ�����¶����ݣ�
u8 USART3_RX_BUF[4];//�������ݽ�������
u8 USART3_TX_BUF[40];//�������ݷ�������
u8 num_device[3];//��������ʹ���豸��id,��ͷ'D'
float temp_mlx;
u8 temp,humi,ppm;//�¶ȣ�ʪ�ȣ�����Ũ�Ȱٷֱ�
u8 Rxnum;//����������ţ���0��ʼ
u8 card_num = 0;//��ǩid���
u8 card_num_device = 0;//��¼ʹ���豸��id
u8 face_num = 0;//����id���
u8 tempH,tempL,rhH,rhL,ppm_pre;//��ģ����ֵ

u8 led_temp_status,led2_status,led_humi_status,led4_status,led_ppm_status;//����ָʾ��ֵ1����
u8 alarm_num;//ָʾ�ĸ��ڵ㱨��
u8 set_flag = 1;//������־λ
u8 r_flag;//1 ����RFID������0�˳�
u8 y_flag;//1 ���������������0�˳�
u8 f_flag;//1 ��������ʶ������0�˳�
u8 n_flag;//1 ����RFID���������0�˳�
u8 d_flag;//1 ����RFID���豸��������0�˳�

//�����������ͷʶ�������
u8 mask[4],name[20];//����ʶ������ʶ��

unsigned int TEHUTimer=0;     //������ʪ�Ȳɼ��ļ�ʱ�ı���   ��λ��
unsigned int SystemTimer=0;   //��������ȫ��ʱ�����

//�Զ��庯��
void data_send(u8 *Tx_Buf);//��ʪ�ȵ�������������
void alarm(void);//���ڱ���
u8 face(const u8 *name);//����ʶ��
void new_card(u8* C_oled_use_array);//�����
void aliyun(void);


//�Զ����ȡ�޷����ַ����Ⱥ���
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
	
	u8 key_board=0;//���Ӱ���
	u8 TandH[10];
	u8 *p=TandH;
	
	cp_time[0] = 'R';//��ʼ����Ϣ��ͷ
	u8 C_oled_use_array[11] = {'0','0','0','0','0','0','0','0','0','0','\0'};//oled��ʾ�õ�����
	
	tempH=35;	//�¶���ֵ
	tempL=10;
	rhH=85;		//ʪ����ֵ
	rhL=22;
	ppm_pre=50;//����Ũ����ֵ
	mask[1] = '#';//��ʼ������ʶ�𣬷�����
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	//IWDG_Init(5,1*625);               //�������Ź� ʱ��1*2s
	delay_init();	    	 //��ʱ������ʼ��

	LED_Init();
	KEY_Init();
	RC522_Init();       //��ʼ����Ƶ��ģ��
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear(); 

  MQ2_Init();
	SMBus_Init();//�������ģ��mlx90614

  //�жϽ���˳��
	Usart2_Init(115200);	 	//esp8266
	USART3_Init(9600);	 	//����
	USART4_Init(9600);		//K210
	Usart1_Init(9600);  //WiFi����

	WiFi_ResetIO_Init();            //��ʼ��WiFi�ĸ�λIO
	AliIoT_Parameter_Init();	    //��ʼ�����Ӱ�����IoTƽ̨MQTT�������Ĳ���
	//u1_printf("����");
	DHT11_Init();
	
	delay_ms(500);
	// UIDΪ��Ҫ�޸ĵĿ���UID key_type��0ΪKEYA����0ΪKEYB KEYΪ��Կ RW:1�Ƕ���0��д data_addrΪ�޸ĵĵ�ַ dataΪ��������
	//IC_RW ( UID, 0, KEY_A, 1, 0x10, data );
	
    while(1)
    {

//MEAU:	
			DHT11_Read_Data(&temp,&humi);//��ȡ��ʪ��
			ppm = MQ2_GetPPM()/2;//Ũ�Ȱٷֱ�����

			#if APP_TASK //APP����ģ������
			
			if(set_flag == 1) 
			{	
			    alarm();//����
					data_send(USART3_TX_BUF);//��ʪ�ȵ����ݷ���
				  //delay_ms(200);
			}
			
			if(d_flag == 1)//�����豸�������񣬵��߳�
			{
				  d_flag = 1;
					num_device[0] = 'D';	
			    while(1)//RFIDʶ��
					{
							OLED_Clear();
							OLED_ShowString(0,0,"Device Manage");
              card_num_device = card_unlock();//RFID ���ػ�ȡ���ĵڼ��ſ�
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
							if(d_flag == 0)//�˳��豸����
							{					
									d_flag = 0;//�˳�����0����ֹ�ٴν���
									OLED_Clear();
									break;
							}						
					} 
			}
			
			//OLED_Clear();  //����
			OLED_ShowString(0,0,"A.Card unlock");		//�˵�ABC������A���ÿ�����������B��¼�뿨������C���������������D���޸�����
			OLED_ShowString(0,4,"B.New Card");
			key_board = KEY_Scan(0);
			if(key_board!=WKUP_PRES){//���˳����������񣬰���RFID�����֣�����
					if(key_board==KEY0_PRES || r_flag==1)//����A���ÿ�����
					{
//unlock:						
						  r_flag = 1;
							LED0 = !LED0;//���԰尴��0
							while(1)//RFIDʶ��
							{
									OLED_Clear();
									OLED_ShowString(0,0,"A.Card unlock");
								  if(y_flag == 1) {
									    //�����������
									    temp_mlx = SMBus_ReadTemp();  //��ȡ����
										  sprintf(cp_time+10,"%.1f",temp_mlx);
											OLED_Clear();
											OLED_ShowString(0,0,"epi pre");
											OLED_ShowString(0,6,"mlx=");
											OLED_ShowString(32,6,(u8*)(cp_time+10));
											OLED_ShowString(64,6,"C");
										
									    if(mask[1] == '1' && temp_mlx < 37.5) {
											    //��ӭ
												  OLED_ShowString(0,2,"welcome");
											    SYN_FrameInfo(0, "[v16][m5][t6]��ӭ��");
												  mask[1] = '#';//���־
											}else if(mask[1] == '0' && temp_mlx < 37.5) {
											    //����ÿ���
												  OLED_ShowString(0,2,"no mask");
											    SYN_FrameInfo(0, "[v16][m5][t6]����ÿ��֣�");
											    mask[1] = '#';//���־
											}else if(mask[1] != '#' && temp_mlx >= 37.5){
											    //�¶ȹ���
												  OLED_ShowString(0,2,"high temp");
											    SYN_FrameInfo(0, "[v16][m5][t6]��ע�⣬�����¹��ߣ�");
												  mask[1] = '#';//���־
											}
											delay_ms(500);
									}
									//RFID���� || ����ʶ��
									if(f_flag == 1) { //����ʶ��
									    face_num = face(name);//��ȡ����ʶ��ID
											cp_time[1] = face_num%10 + '0';											
											temp_mlx = SMBus_ReadTemp();  //��ȡ����
											sprintf(cp_time+10,"%.1f",temp_mlx);
										
											OLED_Clear();
											OLED_ShowString(0,0,"face recognition");
										  OLED_ShowString(0,2,"ID:");
										  OLED_ShowChar(24,2,face_num+'0');
										
											for(int i=0;i<3;i++)
											{
											    UART3_SendString((u8*)cp_time,14);//ͨ�����ڷ�������
											}
											switch(face_num)
											{
													case 0:
													break;
													
													case 1:
													SYN_FrameInfo(0, "[v16][m5][t6]��ӭ��������");
													break;
													
													case 2:
													SYN_FrameInfo(0, "[v16][m5][t6]��ӭ��");
													break;														
													
													default:
													break;
											}
									    name[1] = '0';//���ʶ����
											delay_ms(500);
									}

									card_num = card_unlock();//RFID ���ػ�ȡ���ĵڼ��ſ�
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
							
											default://ʶ��Ϊ���еĿ�
											real_time = __TIME__;//��ȡʱ��
											cp_time[1] = card_num%10 + '0';											
											strcat(cp_time+2,real_time);
											temp_mlx = SMBus_ReadTemp();  //��ȡ����
											sprintf(cp_time+10,"%.1f",temp_mlx);
											for(int i=0;i<3;i++)
											{
											    UART3_SendString((u8*)cp_time,14);//ͨ�����ڷ�������
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
//											//����Ũ������
//											ppm = MQ2_GetPPM()/2;//Ũ�Ȱٷֱ�����
//											OLED_ShowString(0,6,"ppm=");
//											OLED_ShowNum(32,6,ppm,2,8);
//											OLED_ShowString(56,6,"%");
											//�����������
											OLED_ShowString(0,6,"mlx=");
											OLED_ShowString(32,6,(u8*)(cp_time+10));
											OLED_ShowString(64,6,"C");
											delay_ms(1864);
											break;
									};

									if(n_flag == 1) //�����
									{
									    new_card(C_oled_use_array);
									}
									delay_ms(20);
							
									if((key_board = KEY_Scan(0))==KEY0_PRES || r_flag == 0)//�˳�A
									{
//quit:								
										  r_flag = 0;//�˳�����0
										  OLED_Clear();
											break;
									}
							}//while(1)RFIDʶ��
			}//if(KEY=='A'|| key_board==KEY0_PRES)//����A���ÿ�����

	    if(key_board==KEY1_PRES)//����B��¼�뿨
			{
					LED1 = !LED1;//���԰尴��1
					OLED_Clear();
					OLED_ShowString(0,0,"B.New Card");
					while(1)
					{
						switch(card_infor_entry(C_oled_use_array))
						{
								case 0:
							  OLED_ShowString(0,2,"New Card:");
								OLED_ShowString(0,4,C_oled_use_array);//��ʾ��ID
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
					
						if((key_board = KEY_Scan(0))==KEY1_PRES)//�˳�B
						{
								OLED_Clear();
								break;
						}
						
				}//while
			}//if(key_board==KEY1_PRES)//����B��¼�뿨
			
			delay_ms(100);
//			goto MEAU;

    }//if(key_board!=WKUP_PRES)
		#endif
		
	#if ALIYUN_TASK
	aliyun();
	#endif
	}//while
}//main

/*****************************************���ܺ���ʵ��***************************************************/
/*
 *���ݷ��͸�ʽת��
 */
void data_send(u8 *Tx_Buf)
{
		u8 len;
		Tx_Buf[0]='T';						//֡ͷ
	  Tx_Buf[1]=temp/10%10+'0';		 	//����ʪ������������������,�͸�����ģ�����ֻ�APP��ʾ
	  Tx_Buf[2]=temp%10+0x30;		
	  Tx_Buf[3]=humi/10%10+0x30;
	  Tx_Buf[4]=humi%10+0x30;
	
	  Tx_Buf[5]=tempL/10%10+0x30;		 	//�����¶���ֵ������������,�͸�����ģ�����ֻ�APP��ʾ
	 	Tx_Buf[6]=tempL%10+0x30;		
	  Tx_Buf[7]=rhL/10%10+0x30;
	 	Tx_Buf[8]=rhL%10+0x30;
	
	  Tx_Buf[9]=led_temp_status+0x30;			//���͵���Ԥ��״̬
		Tx_Buf[10]=led2_status+0x30;		  //����Ԥ��
		Tx_Buf[11]=led_humi_status+0x30;			//��ʪ��Ԥ��
		Tx_Buf[12]=led4_status+0x30;			//��ʪ��Ԥ��

		Tx_Buf[13]=tempH/10%10+0x30;		 	//������ֵ�¶�����������������,�͸�����ģ�����ֻ�APP��ʾ
	 	Tx_Buf[14]=tempH%10+0x30;	

		Tx_Buf[15]=rhH/10%10+0x30;				//������ֵʪ������������������,�͸�����ģ�����ֻ�APP��ʾ
	 	Tx_Buf[16]=rhH%10+0x30;
		
		Tx_Buf[17]=ppm/10%10+0x30;				//��ǰŨ��
	 	Tx_Buf[18]=ppm%10+0x30;
		Tx_Buf[19]=ppm_pre/10+0x30;		//Ũ����ֵ
		Tx_Buf[20]=ppm_pre%10+0x30;
		Tx_Buf[21]=led_ppm_status+0x30;   //Ũ�ȱ���λ

		Tx_Buf[22]=card_num%10+0x30;
		
		
		len = ac_strlen(Tx_Buf);
		UART3_SendString(Tx_Buf,len);
		delay_ms(500);
}

//��������
void alarm(void)
{
		if(temp<=tempL) 
    {
		    led_temp_status=1;
		    alarm_num=1;
		}				//���¶�Ԥ��
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
		
		if(humi<=rhL)	
    {
		    led_humi_status=1;
		    alarm_num=3;
		}			//��ʪ��Ԥ��
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
		
		if(ppm>ppm_pre)	
    {
			  led_ppm_status=1;
		    alarm_num=5;
		}		//Ũ��Ԥ��
		else 
    {
		    led_ppm_status=0;
		    alarm_num=0;
		}
}
/*
 *����ʶ�����������������
 */
u8 face(const u8 *name) {
		if(name[0] == 'P') {
				return name[1] - '0';
		}else {
				return 0;
		}
}
/*
 *����¿�
 */
void new_card(u8* C_oled_use_array) {
	while(1)
	{
		OLED_Clear();
		OLED_ShowString(0,0,"New Card");
		switch(card_infor_entry(C_oled_use_array))
		{
				case 0:
				OLED_ShowString(0,2,"New Card:");
				OLED_ShowString(0,4,C_oled_use_array);//��ʾ��ID
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
		if(n_flag == 0)//�˳�
		{
				OLED_Clear();
				break;
		}
		
	}//while
}
/*
 *���Ӱ�����
 */
void aliyun(void)
{		
//	while(1)                       //��ѭ��
//	{		
		//WiFi_printf("����");//u2_printf
		/*--------------------------------------------------------------------*/
		/*            Connect_flag=1ͬ����������������                        */
		/*--------------------------------------------------------------------*/
			if(Connect_flag==1){     
				connet_on();         //�������ӳɹ����մ������
				Data_State();         //�������ݵ�����
				delay_ms(1);          //��ʱ1ms�������У�������ι������Ȼ���Ź����������ɸ�λ				
			}	
		/*--------------------------------------------------------------------*/
		/*                         ׼�����ӷ�����                             */
		/*--------------------------------------------------------------------*/
			else{ 
				connet_first();
			}

//	}//while(1)
}

//����3�жϷ������,�û�����USART3_TXD(PB.10)USART3_RXD(PB.11)
void USART3_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���ձ�־Ϊ��Ϊ0
	{
	      Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		    USART3_RX_BUF[Rxnum] = Res;
		    if(USART3_RX_BUF[Rxnum]==0x0A)//���յ�'\n'
						Rxnum=0;
		    else
						Rxnum++;
		    USART_ClearFlag(USART3,USART_FLAG_RXNE);//���ձ�־λ��0
				if(USART3_RX_BUF[0]=='P')//����RFID����
				    r_flag = 1;
				if(USART3_RX_BUF[0]=='p')//�˳�RFID����
				    r_flag = 0;
				
				if(USART3_RX_BUF[0]=='Y')//���������������
				    y_flag = 1;
				if(USART3_RX_BUF[0]=='y')//�˳������������
				    y_flag = 0;
				
				if(USART3_RX_BUF[0]=='F')//��������ʶ������
				    f_flag = 1;
				if(USART3_RX_BUF[0]=='f')//�˳�����ʶ������
				    f_flag = 0;
				
			  if(USART3_RX_BUF[0]=='N')//����RFID���������
				    n_flag = 1;
				if(USART3_RX_BUF[0]=='n')//�˳�RFID���������
				    n_flag = 0;
			  if(USART3_RX_BUF[0]=='D')//����RFID���������
				    d_flag = 1;
				if(USART3_RX_BUF[0]=='d')//�˳�RFID���������
				    d_flag = 0;
								
		    if(USART3_RX_BUF[0]=='8')//���յ���APP��������״̬�������ָ��ͷ����
		    { 
				    if(USART3_RX_BUF[1]=='0')			  
				    {
					    set_flag=0;					//״̬���
				    }
				    if(USART3_RX_BUF[1]=='1')			  
			    	{
					    set_flag=1;					//״̬����
				    }
				    USART3_RX_BUF[1]=0;				  //����
		    }
/********************************��ʪ����ֵ����*************************************/
				if(USART3_RX_BUF[0]=='6')				   //���յ���APP�������ĵ��¶�����ָ��ͷ����..............
				{
					if(USART3_RX_BUF[1]=='0')			   //���յ���APP���������¶����ö�ֵ��1ָ��
					{
						tempL++;					   //�¶ȶ�ֵ��1
						if(tempL>99)
						tempL=99;
					}
					if(USART3_RX_BUF[1]=='1')			   //���յ���APP���������¶����ö�ֵ��1ָ��
					{
						tempL--;					   //�¶ȶ�ֵ��1
						if(tempL<1)
						tempL=1;
					}
					USART3_RX_BUF[1]=0;				   //����
				}
				
				if(USART3_RX_BUF[0]=='4')				   //���յ���APP�������ĸ��¶�����ָ��ͷ����................
				{
					if(USART3_RX_BUF[1]=='1')			   //���յ���APP���������¶����ö�ֵ��1ָ��
					{
						tempH++;					   //�¶ȶ�ֵ��1
						if(tempH>99)
						tempH=99;
					}
					if(USART3_RX_BUF[1]=='0')			   //���յ���APP���������¶����ö�ֵ��1ָ��
					{
						tempH--;					   //�¶ȶ�ֵ��1
						if(tempH<1)
						tempH=1;
					}
					USART3_RX_BUF[1]=0;				   //����
				}
				
				if(USART3_RX_BUF[0]=='7')				  //���յ���APP�������ĵ�ʪ������ָ��ͷ����..................
				{
						if(USART3_RX_BUF[1]=='0')			  //���յ���APP��������ʪ�����ö�ֵ��1ָ��
						{
							rhL++;					  //ʪ�ȶ�ֵ��1
							if(rhL>99)
							rhL=99;
						}
						if(USART3_RX_BUF[1]=='1')			  //���յ���APP��������ʪ�����ö�ֵ��1ָ��
						{
								rhL--;					  //ʪ�ȶ�ֵ��1
								if(rhL<1)
								rhL=1;
						}
						USART3_RX_BUF[1]=0;				  //����
				}
				
				if(USART3_RX_BUF[0]=='5')				  //���յ���APP�������ĸ�ʪ������ָ��ͷ����....................
				{
						if(USART3_RX_BUF[1]=='1')			  //���յ���APP��������ʪ�����ö�ֵ��1ָ��
						{
							rhH++;					  //ʪ�ȶ�ֵ��1
							if(rhH>99)
							rhH=99;
						}
						if(USART3_RX_BUF[1]=='0')			  //���յ���APP��������ʪ�����ö�ֵ��1ָ��
						{
								rhH--;					  //ʪ�ȶ�ֵ��1
								if(rhH<1)
								rhH=1;
						}
						USART3_RX_BUF[1]=0;				  //����
				}
				if(USART3_RX_BUF[0]=='9')				  //���յ���APP��������������ֵ����ָ��ͷ����
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
						USART3_RX_BUF[1]=0;				  //����
				}				
		
		
	}
}

//����4�жϷ�����򣬽���k210ʶ����UART4_TXD(PC.10)UART4_RXD(PC.11)
void UART4_IRQHandler(void)      
{
	static u8 mask_num, name_num;
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(UART4);	//��ȡ���յ�������
		USART_ClearFlag(UART4,USART_FLAG_RXNE);//������ձ�־

		//��ʼ������,��ͷ����
		if(Res=='M'){
			mask_num = 0;
			mask[mask_num] = Res;
		}else if(Res=='P'){
			name_num = 0;
			name[name_num] = Res;			
		}
		
		//ʵ������
		if('0' <= Res && Res <= '9'){
			mask[++mask_num] = Res;
		}
		if(('a' <= Res && Res <= 'z') || ('0' <= Res && Res <= '9')){
			name[++name_num] = Res;			
		}
		
	}
}

//����1�жϷ������	
//void USART1_IRQHandler(void)                	
//{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{
//		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
//		USART_ClearFlag(USART1,USART_FLAG_RXNE);
//			////printf("%s",&Res);
//		if(Res=='1'){
//			//usr code
//			
//		}
//		
//	}
//}
