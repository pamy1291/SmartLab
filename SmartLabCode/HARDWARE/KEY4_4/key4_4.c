#include "stm32f10x.h"
#include "key4_4.h"
#include "sys.h" 
#include "delay.h"
#include "led.h"
//#include "usart.h"
#include "oled.h"

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��PE3,4

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0

}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����

//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		//else if(KEY2==0)return KEY2_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// �ް�������
}

//����������ų�ʼ��         
//����  PF0-PF7�ֱ��  R4 R3 R2 R1  L4 L3 L2 L1 (old)
//����  PA0-PA3�ֱ��  R4 R3 R2 R1  PC0-PC3�ֱ��  L4 L3 L2 L1 (new)
//��Ĥ���� �� ����ʼPA3 PA2 PA1 PA0 PC0 PC1 PC2 PC3
	static u8 key0_up=1;//�������ɿ���־
	static u8 key1_up=1;//�������ɿ���־
	static u8 key2_up=1;//�������ɿ���־
	static u8 key3_up=1;//�������ɿ���־
	static u8 key4_up=1;//�������ɿ���־
	static u8 key5_up=1;//�������ɿ���־
	static u8 key6_up=1;//�������ɿ���־
	static u8 key7_up=1;//�������ɿ���־
	static u8 key8_up=1;//�������ɿ���־
	static u8 key9_up=1;//�������ɿ���־
	static u8 keyA_up=1;//�������ɿ���־
	static u8 keyB_up=1;//�������ɿ���־
	static u8 keyC_up=1;//�������ɿ���־
	static u8 keyD_up=1;//�������ɿ���־
	static u8 keyX_up=1;//�������ɿ���־
	static u8 keyJ_up=1;//�������ɿ���־
	int num=0,warning=0,warning1=0;
	u8 scan_key[5] = {'0'};
	u8 row_clear[] ="               ";
void JZKEY_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTB,PORTD��PORTGʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;                        //�������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;                       //��������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;   //����ģʽ
	
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
}




//mode:0,��֧��������;1,֧��������;
u8 JZKEY_Scan(u8 mode)
{
	delay_init();
	
	if(mode)
	{
			key0_up=1;
			key1_up=1;
			key2_up=1;
			key3_up=1;
			key4_up=1;
			key5_up=1;
			key6_up=1;
			key7_up=1;
			key8_up=1;
			key9_up=1;
			keyA_up=1;
			keyB_up=1;
			keyC_up=1;
			keyD_up=1;
			keyX_up=1;
			keyJ_up=1;	
	}
	
    //��һ��
	Hang_00_L;//�ѵ�һ������͵�ƽ
	Hang_01_H;
	Hang_02_H;
	Hang_03_H;
		if(key1_up&&Lie_00_V==0)
		{    
		   delay_ms(jpys); //��ʱ20�룬�������
			
			
		   key1_up=0;
		   if(Lie_00_V==0) //�����һ���ǵ͵�ƽ��˵���м������£����û��ֱ���˳�if���
		   {
			   return '1';
		   }
		}else if(Lie_00_V==1)key1_up=1;
	 
		if(key2_up&&Lie_01_V==0)//����ڶ����ǵ͵�ƽ��
		{    
		   delay_ms(jpys);//��ʱ20�룬�������
		   key2_up=0;
		   if(Lie_01_V==0)//����ڶ����ǵ͵�ƽ��˵���м������£����û��ֱ���˳�if���
		   {
			  return '2'; 
		   }
		}else if(Lie_01_V==1)key2_up=1;
	 
		if(key3_up&&Lie_02_V==0)
		{    
		   delay_ms(jpys);
		   key3_up=0;
		   if(Lie_02_V==0)
		   {
			  return '3';
		   }
		}else if(Lie_02_V==1)key3_up=1;
	 
		if(keyA_up&&Lie_03_V==0)//����������ǵ͵�ƽ
		{    
		   delay_ms(jpys);
		   keyA_up=0;
		   if(Lie_03_V==0)//����������ǵ͵�ƽ��˵���м������£����û��ֱ���˳�if���
		   {
			  return 'A';
		   }
		}else if(Lie_03_V==1)keyA_up=1;
 
	//�ڶ���
	Hang_00_H;
	Hang_01_L;//�ѵڶ�������
	Hang_02_H;
	Hang_03_H;
		if(key4_up&&Lie_00_V==0)//�����һ���ǵ͵�ƽ
		{    
		   delay_ms(jpys);
		   key4_up=0;
		   if(Lie_00_V==0)//˵���м������£����û��ֱ���˳�if���
		   {
				return '4';
		   }
		}else if(Lie_00_V==1)key4_up=1;
		 
		if(key5_up&&Lie_01_V==0)
		{    
		   delay_ms(jpys);
		   key5_up=0;
		   if(Lie_01_V==0)
		   {
		       return '5';
		   }
		}else if(Lie_01_V==1)key5_up=1;
		 
		if(key6_up&&Lie_02_V==0)
		{    
		   delay_ms(jpys);
		   key6_up=0;
		   if(Lie_02_V==0)
		   {
			   return '6';
		   }
		}else if(Lie_02_V==1)key6_up=1;
		 
		if(keyB_up&&Lie_03_V==0)
		{    
		   delay_ms(jpys);
		   keyB_up=0;
		   if(Lie_03_V==0)
		   {
		       return 'B';  
		   }
		}else if(Lie_03_V==1)keyB_up=1;
 
	//������
	Hang_00_H;
	Hang_01_H;
	Hang_02_L;//�ѵ������õ�
	Hang_03_H;
		if(key7_up&&Lie_00_V==0) //�����һ���ǵ͵�ƽ
		{    
		   delay_ms(jpys);//��ʱ20��
		   key7_up=0;
		   if(Lie_00_V==0)//˵���м������£����û��ֱ���˳�if���
		   {
			   return '7';
		   }
		}else if(Lie_00_V==1)key7_up=1;
		 
		if(key8_up&&Lie_01_V==0)
		{    
		   delay_ms(jpys);
		   key8_up=0;
		   if(Lie_01_V==0)
		   {
		       return '8';
		   }
		}else if(Lie_01_V==1)key8_up=1;
		 
		if(key9_up&&Lie_02_V==0)
		{    
		   delay_ms(jpys);
		   key9_up=0;
		   if(Lie_02_V==0)
		   {
		       return '9';
		   }
		}else if(Lie_02_V==1)key9_up=1;
		 
		if(keyC_up&&Lie_03_V==0)
		{    
		   delay_ms(jpys);
		   keyC_up=0;
		   if(Lie_03_V==0)
		   {
		       return 'C'; 
		   }
		}else if(Lie_03_V==1)keyC_up=1;
 
	//������
	Hang_00_H;
	Hang_01_H;
	Hang_02_H;
	Hang_03_L;//�ѵ������õ�
		if(keyX_up&&Lie_00_V==0)//�����һ���ǵ͵�ƽ
		{    
		   delay_ms(jpys);
		   keyX_up=0;
		   if(Lie_00_V==0)//˵���м������£����û��ֱ���˳�if���
		   {
		       return '*';
		   }
		}else if(Lie_00_V==1)keyX_up=1;
		 
		if(key0_up&&Lie_01_V==0)
		{    
		   delay_ms(10);
		   key0_up=0;
		   if(Lie_01_V==0)
		   {
		       return '0';
		   }
		}else if(Lie_01_V==1)key0_up=1;
		 
		if(keyJ_up&&Lie_02_V==0)//����������ǵ͵�ƽ
		{    
		   delay_ms(jpys);
		   keyJ_up=0;
		   if(Lie_02_V==0)//˵���м������£����û��ֱ���˳�if���
		   {
		       return '#';
		   }
		}else if(Lie_02_V==1)keyJ_up=1;
		 
		if(keyD_up&&Lie_03_V==0)
		{    
		   delay_ms(jpys);
		   keyD_up=0;
		   if(Lie_03_V==0)
		   {
		       return 'D';
		   }
		}else if(Lie_03_V==1)keyD_up=1;
		
		return 'N';//�ް������£�����N
}

/***********************
�������뿪����
˼·���β�Ϊ �������������ŵ�������׵�ַ
			�ں���������һ�����飬������Ű������������
			����4λ������Զ�ʹ��ָ��λ��������������ݽ��бȶ�
			��ȫƥ���򷵻ظ�������һ������ָ��'R'
***********************/

u8 entry_passwd(u8 * lock_passwd)				//�������룬������
{
		
		u8 temp = 'N';											//������ʱ������������Ű�����ֵ

		if((temp=JZKEY_Scan(0))!='N')				//�жϣ���������ʱ���루������������N��
		{
			if(temp=='*')
			{
				
				for(num=0;num<4;num++)					//����һ�뷵��ʱ�����뻺��������
				scan_key[num] = '\0';
				num=0;
				return 0;
			}
				
			scan_key[num] = temp;							//��������������scan_key����
			//printf("scan_key=%c\n",scan_key[num]);
			num++;														//����++�����������λ��
			OLED_ShowString(56,2,scan_key);		//��ʾ����ʾ���������
		}
//		printf("num=%d",num);
		if(num==4)													//�����������λ����
		{
																			//�Ƚ���������
			if(*(lock_passwd++)==scan_key[0]&&*(lock_passwd++)==scan_key[1]&&*(lock_passwd++)==scan_key[2]&&*(lock_passwd++)==scan_key[3])//��λ�ж������Ƿ�������������ͬ
			{
				OLED_ShowString(56,2,"    ");//OLED��ʾpasswd�������
				for(num=0;num<4;num++)			//���뻺��������
					scan_key[num]='\0';	
				num = 0;										//num���㣬��ֹ�ٽ���if
				lock_passwd = lock_passwd-4;//�����ָ��ص�0λ
				return 'R';									//���ؿ���ָ��
			}
			else
			{

				OLED_ShowString(0,4,"Wrong passwd!!");
				for(num=0;num<4;num++)			//���뻺��������
				scan_key[num]='\0';
				num = 0;										//num���㣬��ֹ�ٽ���if
				LED1 = 0;
				delay_ms(20000);						//�������ʾ����
				delay_ms(20000);
				LED1 = 1;
				OLED_ShowString(56,2,"    ");//OLED��ʾpasswd�������
				if(++warning==3)
				{
					warning = 0;
					OLED_Clear();
					while(1)									//����3�Σ���ѭ����led��˸������ֻ�ܰ�*������ҳ��
					{
						OLED_ShowString(0,0,"Password has been disabled!Please contact the administrator!");
						LED1 = !LED1;
						LED0 = !LED0;
						delay_ms(50);
						
						if(JZKEY_Scan(0)=='*')
						{
							LED1=1;
							LED0=1;
							OLED_Clear();
							return 0;
						}
					}

				}
			}
		}
		return 'W';											//���ع���ָ��
}
/***********************
�������뿪����
***********************/


/***********************
�޸�����
***********************/

u8 change_passwd(u8 * lock_passwd)				//�������룬������
{
		u8 changePasswd=0;
		u8 temp = 'N';											//������ʱ������������Ű�����ֵ
	
		OLED_ShowString(0,2,"OldPasswd:");		//��ʾ����ʾ���������
	
		if((temp=JZKEY_Scan(0))!='N')				//�жϣ���������ʱ���루������������N��
		{
			if(temp=='*')
			{
				
				for(num=0;num<4;num++)					//����һ�뷵��ʱ�����뻺��������
				scan_key[num] = '\0';
				num=0;
				return 0;
			}
			
			scan_key[num] = temp;							//��������������scan_key����
			//printf("scan_key=%c\n",scan_key[num]);
			num++;														//����++�����������λ��
			OLED_ShowString(80,2,scan_key);		//��ʾ����ʾ���������
		}

		if(num==4)													//�����������λ����
		{
																			//�Ƚ���������
			if(*(lock_passwd++)==scan_key[0]&&*(lock_passwd++)==scan_key[1]&&*(lock_passwd++)==scan_key[2]&&*(lock_passwd++)==scan_key[3])//��λ�ж������Ƿ�������������ͬ
			{
				OLED_ShowString(80,2,"    ");
				OLED_ShowString(0,2,"NewPasswd:");
				for(num=0;num<4;num++)					//���뻺��������
					scan_key[num] = '\0';
				while(1)
				{
					
					if((temp=JZKEY_Scan(0))!='N')				//�жϣ���������ʱ���루������������N��
						{
							if(temp=='*')
								{
									
									for(num=0;num<4;num++)					//����һ�뷵��ʱ�����뻺��������
									scan_key[num] = '\0';
									num=0;
									changePasswd=0;
									return 0;
								}
									
							scan_key[changePasswd] = temp;							//��������������scan_key����
							//printf("scan_key=%c\n",scan_key[num]);
							changePasswd++;														//����++�����������λ��
							OLED_ShowString(80,2,scan_key);		//��ʾ����ʾ���������
						}
							if(changePasswd==4)
								{
									lock_passwd = lock_passwd-4;//�����ָ��ص�0λ
									*(lock_passwd++)=scan_key[0];//�޸�����
									*(lock_passwd++)=scan_key[1];
									*(lock_passwd++)=scan_key[2];
									*(lock_passwd++)=scan_key[3];
									for(num=0;num<4;num++)					//���뻺��������
										scan_key[num] = '\0';
									num=0;
									return 'R';
								}
				}

			}
			else
			{

				OLED_ShowString(0,4,"Wrong passwd!!");
				for(num=0;num<4;num++)			//���뻺��������
				scan_key[num]='\0';
				num = 0;										//num���㣬��ֹ�ٽ���if
				LED1 = 0;
				delay_ms(20000);						//�������ʾ����
				delay_ms(20000);
				LED1 = 1;
				OLED_ShowString(80,2,"    ");//OLED��ʾpasswd�������
				OLED_ShowString(0,4,row_clear);//OLED��ʾpasswd�������
				if(++warning1==3)
				{
					warning1 = 0;
					OLED_Clear();
					while(1)									//����3�Σ���ѭ����led��˸������ֻ�ܰ�*������ҳ��
					{
						OLED_ShowString(0,0,"Password has been disabled!Please contact the administrator!");
						LED1 = !LED1;
						LED0 = !LED0;
						delay_ms(50);
						
						if(JZKEY_Scan(0)=='*')
						{
							LED1=1;
							LED0=1;
							OLED_Clear();
							return 0;
						}
					}

				}
			}
		}
		return 'W';											//���ع���ָ��
}
/***********************
�޸�����
***********************/


