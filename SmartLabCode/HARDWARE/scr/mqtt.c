/*-------------------------------------------------*/
/*                   �ʵ��Ŷӿ���                */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "main.h"         //������Ҫ��ͷ�ļ�
#include "mqtt.h"         //������Ҫ��ͷ�ļ�
#include "string.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"        //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "utils_hmac.h"   //������Ҫ��ͷ�ļ�
#include "wifi.h"         //������Ҫ��ͷ�ļ�
#include "usart2.h"     //������Ҫ��ͷ�ļ�
#include "timer2.h"     //������Ҫ��ͷ�ļ�
#include "timer3.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�

unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];           //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_RxDataInPtr;                            //ָ����ջ�����������ݵ�λ��
unsigned char *MQTT_RxDataOutPtr;                           //ָ����ջ�������ȡ���ݵ�λ��
unsigned char *MQTT_RxDataEndPtr;                           //ָ����ջ�����������λ��

unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];           //���ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_TxDataInPtr;                            //ָ���ͻ�����������ݵ�λ��
unsigned char *MQTT_TxDataOutPtr;                           //ָ���ͻ�������ȡ���ݵ�λ��
unsigned char *MQTT_TxDataEndPtr;                           //ָ���ͻ�����������λ��

unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];              //�������ݵĽ��ջ�����
unsigned char *MQTT_CMDInPtr;                               //ָ���������������ݵ�λ��
unsigned char *MQTT_CMDOutPtr;                              //ָ�����������ȡ���ݵ�λ��
unsigned char *MQTT_CMDEndPtr;                              //ָ���������������λ��



char ClientID[128];                                          //��ſͻ���ID�Ļ�����
int  ClientID_len;                                           //��ſͻ���ID�ĳ���

char Username[128];                                          //����û����Ļ�����
int  Username_len;											 //����û����ĳ���

char Passward[128];                                          //�������Ļ�����
int  Passward_len;											 //�������ĳ���

char ServerIP[128];                                          //��ŷ�����IP��������
int  ServerPort;                                             //��ŷ������Ķ˿ں�

int   Fixed_len;                       					     //�̶���ͷ����
int   Variable_len;                     					 //�ɱ䱨ͷ����
int   Payload_len;                       					 //��Ч���ɳ���
unsigned char  temp_buff[TBUFF_UNIT];						 //��ʱ������������������

char Ping_flag;           //ping����״̬      0������״̬���ȴ���ʱʱ�䵽������Ping����
                          //ping����״̬      1��Ping�����ѷ��ͣ����յ� �������ظ����ĵĺ� ��1��Ϊ0
char Connect_flag;        //ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
char ConnectPack_flag;    //CONNECT����״̬   1��CONNECT���ĳɹ�
char SubcribePack_flag;   //���ı���״̬      1�����ı��ĳɹ�

/*----------------------------------------------------------*/
/*����������ʼ������,����,�������ݵ� ������ �Լ���״̬����  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_Init(void)
{	
	MQTT_RxDataInPtr=MQTT_RxDataBuf[0];               //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_RxDataOutPtr=MQTT_RxDataInPtr;               //ָ���ͻ�������ȡ���ݵ�ָ���λ
  MQTT_RxDataEndPtr=MQTT_RxDataBuf[R_NUM-1];        //ָ���ͻ�����������ָ���λ
	
	MQTT_TxDataInPtr=MQTT_TxDataBuf[0];               //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_TxDataOutPtr=MQTT_TxDataInPtr;               //ָ���ͻ�������ȡ���ݵ�ָ���λ
	MQTT_TxDataEndPtr=MQTT_TxDataBuf[T_NUM-1];        //ָ���ͻ�����������ָ���λ
	
	MQTT_CMDInPtr=MQTT_CMDBuf[0];                     //ָ���������������ݵ�ָ���λ
	MQTT_CMDOutPtr=MQTT_CMDInPtr;                     //ָ�����������ȡ���ݵ�ָ���λ
	MQTT_CMDEndPtr=MQTT_CMDBuf[C_NUM-1];              //ָ���������������ָ���λ

  MQTT_ConectPack();                                //���ͻ�����������ӱ���
	MQTT_Subscribe(S_TOPIC_NAME,0);	                  //���ͻ�������Ӷ���topic���ȼ�0	
  
	
	Ping_flag = ConnectPack_flag = SubcribePack_flag = 0;  //������������
}
/*----------------------------------------------------------*/
/*�������������Ƴ�ʼ���������õ��ͻ���ID���û���������      */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void AliIoT_Parameter_Init(void)
{	
	char temp[128];                                                       //������ܵ�ʱ����ʱʹ�õĻ�����

	memset(ClientID,0,128);                                               //�ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);  //�����ͻ���ID�������뻺����
	ClientID_len = strlen(ClientID);                                      //����ͻ���ID�ĳ���
	
	memset(Username,0,128);                                               //�û����Ļ�����ȫ������
	sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);                      //�����û����������뻺����
	Username_len = strlen(Username);                                      //�����û����ĳ���
	
	memset(temp,0,128);                                                                      //��ʱ������ȫ������
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);     //��������ʱ������   
	utils_hmac_sha1(temp,strlen(temp),Passward,DEVICESECRE,DEVICESECRE_LEN);                 //��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	Passward_len = strlen(Passward);                                                         //�����û����ĳ���
	
	memset(ServerIP,0,128);  
	sprintf(ServerIP,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",PRODUCTKEY);                  //��������������
	ServerPort = 1883;                                                                       //�������˿ں�1883
	
	u1_printf("�� �� ����%s:%d\r\n",ServerIP,ServerPort); //�������������Ϣ
	u1_printf("�ͻ���ID��%s\r\n",ClientID);               //�������������Ϣ
	u1_printf("�� �� ����%s\r\n",Username);               //�������������Ϣ
	u1_printf("��    �룺%s\r\n",Passward);               //�������������Ϣ
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                                                        //���ӱ����У��̶���ͷ������ʱ��=1
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //���ӱ����У����س���      
	Remaining_len = Variable_len + Payload_len;                           //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0]=0x10;                       //�̶���ͷ��1���ֽ� ���̶�0x01		
	do{                                      //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;            //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;   //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                    //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��
	
	temp_buff[Fixed_len+0]=0x00;    //�ɱ䱨ͷ��1���ֽ� ���̶�0x00	            
	temp_buff[Fixed_len+1]=0x04;    //�ɱ䱨ͷ��2���ֽ� ���̶�0x04
	temp_buff[Fixed_len+2]=0x4D;	//�ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	temp_buff[Fixed_len+3]=0x51;	//�ɱ䱨ͷ��4���ֽ� ���̶�0x51
	temp_buff[Fixed_len+4]=0x54;	//�ɱ䱨ͷ��5���ֽ� ���̶�0x54
	temp_buff[Fixed_len+5]=0x54;	//�ɱ䱨ͷ��6���ֽ� ���̶�0x54
	temp_buff[Fixed_len+6]=0x04;	//�ɱ䱨ͷ��7���ֽ� ���̶�0x04
	temp_buff[Fixed_len+7]=0xC2;	//�ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[Fixed_len+8]=0x00; 	//�ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	temp_buff[Fixed_len+9]=0x64;	//�ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  			    //�ͻ���ID���ȸ��ֽ�
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  			    //�ͻ���ID���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 			//���ƹ����ͻ���ID�ִ�	
	/*     �û���        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				  		    //�û������ȸ��ֽ�
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				 		    //�û������ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);                //���ƹ����û����ִ�	
	/*      ����        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;			    //���볤�ȸ��ֽ�
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;			    //���볤�ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len);   //���ƹ��������ִ�

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);                  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����                            */
/*��  ����QoS�����ĵȼ�                                     */
/*��  ����topic_name������topic��������                     */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                           //SUBSCRIBE�����У��ɱ䱨ͷ����=2	
	Payload_len = 2 + strlen(topic_name) + 1;   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�
	
	temp_buff[0]=0x82;                                    //��1���ֽ� ���̶�0x82                      
	temp_buff[1]=Variable_len + Payload_len;              //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	temp_buff[2]=0x00;                                    //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3]=0x01;		                              //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4]=strlen(topic_name)/256;                  //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5]=strlen(topic_name)%256;		          //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));  //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�		
	temp_buff[6+strlen(topic_name)]=QoS;                  //���1���ֽڣ����ĵȼ�
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������PING���ģ�������                                  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	temp_buff[0]=0xC0;              //��1���ֽ� ���̶�0xC0                      
	temp_buff[1]=0x00;              //��2���ֽ� ���̶�0x00 

	TxDataBuf_Deal(temp_buff, 2);   //�������ݵ�������
}
/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*��  ����data_len�����ݳ���                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                              //�̶���ͷ������ʱ�ȵ��ڣ�1�ֽ�
	Variable_len = 2 + strlen(topic);           //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                     //��Ч���ɳ��ȣ�����data_len
	Remaining_len = Variable_len + Payload_len; //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0]=0x30;                       //�̶���ͷ��1���ֽ� ���̶�0x30   	
	do{                                      //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;            //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;   //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                    //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��
		             
	temp_buff[Fixed_len+0]=strlen(topic)/256;                      //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	temp_buff[Fixed_len+1]=strlen(topic)%256;		               //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+2],topic,strlen(topic));           //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���	
	memcpy(&temp_buff[Fixed_len+2+strlen(topic)],data,data_len);   //��Ч���ɣ�����data����
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*����������������������ĵȼ�0������                       */
/*��  ����redata�����յ�����                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int  re_len;               	           //����һ����������Ž��յ������ܳ���
	int  pack_num;                         //����һ�����������������һ�����ʱ���������͵ĸ���
    int  temp,temp_len;                    //����һ���������ݴ�����
    int  totle_len;                        //����һ������������Ѿ�ͳ�Ƶ����͵���������
	int  topic_len;              	       //����һ���������������������ĳ���
	int  cmd_len;                          //����һ����������������а������������ݵĳ���
	int  cmd_loca;                         //����һ����������������а������������ʼλ��
	int  i;                                //����һ������������forѭ��
	int  local,multiplier;
	unsigned char tempbuff[RBUFF_UNIT];	   //��ʱ������
	unsigned char *data;                   //redata������ʱ�򣬵�һ���ֽ�������������data����ָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�ĵط�
		
	re_len = redata[0]*256+redata[1];                               //��ȡ���յ������ܳ���		
	data = &redata[2];                                              //dataָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�� 
	pack_num = temp_len = totle_len = temp = 0;                	    //������������
	local = 1;
	multiplier = 1;
	do{
		pack_num++;                                     			//��ʼѭ��ͳ�����͵ĸ�����ÿ��ѭ�����͵ĸ���+1	
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		totle_len += (temp_len + local);                          	//�ۼ�ͳ�Ƶ��ܵ����͵����ݳ���
		re_len -= (temp_len + local) ;                              //���յ������ܳ��� ��ȥ ����ͳ�Ƶ����͵��ܳ���      
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}while(re_len!=0);                                  			//������յ������ܳ��ȵ���0�ˣ�˵��ͳ�������
	u1_printf("���ν�����%d����������\r\n",pack_num);//���������Ϣ
	temp_len = totle_len = 0;                		            	//������������
	local = 1;
	multiplier = 1;
	for(i=0;i<pack_num;i++){                                        //�Ѿ�ͳ�Ƶ��˽��յ����͸�������ʼforѭ����ȡ��ÿ�����͵����� 		
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);				
		topic_len = data[local+totle_len]*256+data[local+1+totle_len] + 2;    //���㱾����������������ռ�õ�������
		cmd_len = temp_len-topic_len;                               //���㱾��������������������ռ�õ�������
		cmd_loca = totle_len + local +  topic_len;                  //���㱾�������������������ݿ�ʼ��λ��
		memcpy(tempbuff,&data[cmd_loca],cmd_len);                   //�������ݿ�������		                 
		CMDBuf_Deal(tempbuff, cmd_len);                             //�������������
		totle_len += (temp_len+local);                              //�ۼ��Ѿ�ͳ�Ƶ����͵����ݳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}	
}
/*----------------------------------------------------------*/
/*�������������ͻ�����                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_TxDataInPtr[2],data,size);      //�������ݵ����ͻ�����	
	MQTT_TxDataInPtr[0] = size/256;              //��¼���ݳ���
	MQTT_TxDataInPtr[1] = size%256;              //��¼���ݳ���
	MQTT_TxDataInPtr+=TBUFF_UNIT;                 //ָ������
	if(MQTT_TxDataInPtr==MQTT_TxDataEndPtr)      //���ָ�뵽������β����
		MQTT_TxDataInPtr = MQTT_TxDataBuf[0];    //ָ���λ����������ͷ
}
/*----------------------------------------------------------*/
/*�������������������                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[2],data,size);      //�������ݵ��������
	MQTT_CMDInPtr[0] = size/256;              //��¼���ݳ���
	MQTT_CMDInPtr[1] = size%256;              //��¼���ݳ���
	MQTT_CMDInPtr[size+2] = '\0';             //�����ַ���������
	MQTT_CMDInPtr+=CBUFF_UNIT;                 //ָ������
	if(MQTT_CMDInPtr==MQTT_CMDEndPtr)         //���ָ�뵽������β����
		MQTT_CMDInPtr = MQTT_CMDBuf[0];       //ָ���λ����������ͷ
}

/*��һ������  */
/*����ʼ����*/
void connet_first(void)
{
			u1_printf("׼�����ӷ�����\r\n");                  //���������Ϣ
			Usart2_Init(115200);                             //����2���ܳ�ʼ����������115200	
			TIM_Cmd(TIM4,DISABLE);                           //�ر�TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //�ر�TIM3  
			TIM_Cmd(TIM2,DISABLE);                           //�ر�TIM2    
			WiFi_RxCounter=0;                                //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
			if(WiFi_Connect_IoTServer()==0){   			     //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
				u1_printf("���ӷ������ɹ�\r\n");              //���������Ϣ
				Usart2_IDELInit(115200);                     //����2 ����DMA �� �����ж�
				Connect_flag = 1;                            //Connect_flag��1����ʾ���ӳɹ�	
				SystemTimer = 0;                             //ȫ�ּ�ʱʱ�������0
				WiFi_RxCounter=0;                            //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
				MQTT_Buff_Init();                            //��ʼ�����ͽ���������� 
				TIM3_ENABLE_30S();                           //������ʱ��3 30s��PING���ʱ��
				TIM2_ENABLE_1S();                            //������ʱ��2 1s�Ķ�ʱ��
			}		




}
/*�������ӳɹ�*/
void connet_on(void)
{
			/*-------------------------------------------------------------*/
			if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                    //if�����Ļ���˵�����ͻ�������������
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ���
				//��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
				//��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //������ʾ��Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);                       //��������
					MQTT_TxDataOutPtr += TBUFF_UNIT;                      //ָ������
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //���ָ�뵽������β����
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //ָ���λ����������ͷ
				} 				
			}//�����ͻ��������ݵ�else if��֧��β
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){                //if�����Ļ���˵�����ջ�������������														
				u1_printf("���յ�����:");                             //������ʾ��Ϣ
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}				
				}
				/*-----------------------------------------------------*/
				/*                    ����SUBACK����                   */
				/*-----------------------------------------------------*/				
				//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : u1_printf("���ĳɹ�\r\n");            //���������Ϣ
							        SubcribePack_flag = 1;                //SubcribePack_flag��1����ʾ���ı��ĳɹ���
									break;                                //������֧                                             
						default   : u1_printf("����ʧ�ܣ�׼������\r\n");  //���������Ϣ 
									Connect_flag = 0;                     //Connect_flag���㣬��������
									break;                                //������֧ 								
					}					
				}
				/*-----------------------------------------------------*/
				/*                  ����PINGRESP����                   */
				/*-----------------------------------------------------*/
				//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING���Ļظ�\r\n"); 		  //���������Ϣ 
					if(Ping_flag==1){                     //���Ping_flag=1����ʾ��һ�η���
						 Ping_flag = 0;    				  //Ҫ���Ping_flag��־
					}else if(Ping_flag>1){ 				  //���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						Ping_flag = 0;     				  //Ҫ���Ping_flag��־
						TIM3_ENABLE_30S(); 				  //PING��ʱ���ػ�30s��ʱ��
					}				
				}
				/*-----------------------------------------------------*/
				/*                  �����������ͱ���                   */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if(MQTT_RxDataOutPtr[2]==0x30){ 
					u1_printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
				
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     //����ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //�������ָ�뵽���ջ�����β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //����ָ���λ�����ջ�������ͷ                        
			}
			
			/*-------------------------------------------------------------*/
			/*                     ���������������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if�����Ļ���˵�����������������			       
				u1_printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);                 //���������Ϣ
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	 //ָ������
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //���ָ�뵽������β����
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //ָ���λ����������ͷ				
			}//��������������ݵ�else if��֧��β	



}
