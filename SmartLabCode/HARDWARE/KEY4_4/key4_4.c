#include "stm32f10x.h"
#include "key4_4.h"
#include "sys.h" 
#include "delay.h"
#include "led.h"
//#include "usart.h"
#include "oled.h"

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化PE3,4

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下

//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		//else if(KEY2==0)return KEY2_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
}

//矩阵键盘引脚初始化         
//接线  PF0-PF7分别接  R4 R3 R2 R1  L4 L3 L2 L1 (old)
//接线  PA0-PA3分别接  R4 R3 R2 R1  PC0-PC3分别接  L4 L3 L2 L1 (new)
//薄膜键盘 ： 最左开始PA3 PA2 PA1 PA0 PC0 PC1 PC2 PC3
	static u8 key0_up=1;//按键按松开标志
	static u8 key1_up=1;//按键按松开标志
	static u8 key2_up=1;//按键按松开标志
	static u8 key3_up=1;//按键按松开标志
	static u8 key4_up=1;//按键按松开标志
	static u8 key5_up=1;//按键按松开标志
	static u8 key6_up=1;//按键按松开标志
	static u8 key7_up=1;//按键按松开标志
	static u8 key8_up=1;//按键按松开标志
	static u8 key9_up=1;//按键按松开标志
	static u8 keyA_up=1;//按键按松开标志
	static u8 keyB_up=1;//按键按松开标志
	static u8 keyC_up=1;//按键按松开标志
	static u8 keyD_up=1;//按键按松开标志
	static u8 keyX_up=1;//按键按松开标志
	static u8 keyJ_up=1;//按键按松开标志
	int num=0,warning=0,warning1=0;
	u8 scan_key[5] = {'0'};
	u8 row_clear[] ="               ";
void JZKEY_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);//使能PORTB,PORTD，PORTG时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;                        //键盘输出
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;                       //键盘输入
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;   //输入模式
	
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
}




//mode:0,不支持连续按;1,支持连续按;
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
	
    //第一行
	Hang_00_L;//把第一行输出低电平
	Hang_01_H;
	Hang_02_H;
	Hang_03_H;
		if(key1_up&&Lie_00_V==0)
		{    
		   delay_ms(jpys); //延时20秒，软件消抖
			
			
		   key1_up=0;
		   if(Lie_00_V==0) //如果第一列是低电平，说明有键被按下，如果没有直接退出if语句
		   {
			   return '1';
		   }
		}else if(Lie_00_V==1)key1_up=1;
	 
		if(key2_up&&Lie_01_V==0)//如果第二列是低电平，
		{    
		   delay_ms(jpys);//延时20秒，软件消抖
		   key2_up=0;
		   if(Lie_01_V==0)//如果第二列是低电平，说明有键被按下，如果没有直接退出if语句
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
	 
		if(keyA_up&&Lie_03_V==0)//如果第四列是低电平
		{    
		   delay_ms(jpys);
		   keyA_up=0;
		   if(Lie_03_V==0)//如果第四列是低电平，说明有键被按下，如果没有直接退出if语句
		   {
			  return 'A';
		   }
		}else if(Lie_03_V==1)keyA_up=1;
 
	//第二行
	Hang_00_H;
	Hang_01_L;//把第二行拉低
	Hang_02_H;
	Hang_03_H;
		if(key4_up&&Lie_00_V==0)//如果第一列是低电平
		{    
		   delay_ms(jpys);
		   key4_up=0;
		   if(Lie_00_V==0)//说明有键被按下，如果没有直接退出if语句
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
 
	//第三行
	Hang_00_H;
	Hang_01_H;
	Hang_02_L;//把第三行置低
	Hang_03_H;
		if(key7_up&&Lie_00_V==0) //如果第一列是低电平
		{    
		   delay_ms(jpys);//延时20秒
		   key7_up=0;
		   if(Lie_00_V==0)//说明有键被按下，如果没有直接退出if语句
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
 
	//第四行
	Hang_00_H;
	Hang_01_H;
	Hang_02_H;
	Hang_03_L;//把第四行置低
		if(keyX_up&&Lie_00_V==0)//如果第一列是低电平
		{    
		   delay_ms(jpys);
		   keyX_up=0;
		   if(Lie_00_V==0)//说明有键被按下，如果没有直接退出if语句
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
		 
		if(keyJ_up&&Lie_02_V==0)//如果第三列是低电平
		{    
		   delay_ms(jpys);
		   keyJ_up=0;
		   if(Lie_02_V==0)//说明有键被按下，如果没有直接退出if语句
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
		
		return 'N';//无按键按下，返回N
}

/***********************
输入密码开关门
思路：形参为 主函数中密码存放的数组的首地址
			在函数中声明一个数组，用来存放按键输入的密码
			输入4位密码后，自动使用指针位运算与密码表内容进行比对
			完全匹配则返回给主函数一个开门指令'R'
***********************/

u8 entry_passwd(u8 * lock_passwd)				//输入密码，开关门
{
		
		u8 temp = 'N';											//定义临时变量，用来存放按键键值

		if((temp=JZKEY_Scan(0))!='N')				//判断，按键按下时进入（不按按键返回N）
		{
			if(temp=='*')
			{
				
				for(num=0;num<4;num++)					//输入一半返回时，输入缓冲区清零
				scan_key[num] = '\0';
				num=0;
				return 0;
			}
				
			scan_key[num] = temp;							//把输入的密码存入scan_key数组
			//printf("scan_key=%c\n",scan_key[num]);
			num++;														//计数++，输入密码的位数
			OLED_ShowString(56,2,scan_key);		//显示屏显示输入的密码
		}
//		printf("num=%d",num);
		if(num==4)													//如果输入了四位密码
		{
																			//先将计数清零
			if(*(lock_passwd++)==scan_key[0]&&*(lock_passwd++)==scan_key[1]&&*(lock_passwd++)==scan_key[2]&&*(lock_passwd++)==scan_key[3])//逐位判断密码是否和密码表密码相同
			{
				OLED_ShowString(56,2,"    ");//OLED显示passwd区域清除
				for(num=0;num<4;num++)			//密码缓冲区清零
					scan_key[num]='\0';	
				num = 0;										//num归零，防止再进入if
				lock_passwd = lock_passwd-4;//密码表指针回到0位
				return 'R';									//返回开门指令
			}
			else
			{

				OLED_ShowString(0,4,"Wrong passwd!!");
				for(num=0;num<4;num++)			//密码缓冲区清零
				scan_key[num]='\0';
				num = 0;										//num归零，防止再进入if
				LED1 = 0;
				delay_ms(20000);						//红灯亮以示警告
				delay_ms(20000);
				LED1 = 1;
				OLED_ShowString(56,2,"    ");//OLED显示passwd区域清除
				if(++warning==3)
				{
					warning = 0;
					OLED_Clear();
					while(1)									//错误3次，死循环，led闪烁报警，只能按*返回主页面
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
		return 'W';											//返回关门指令
}
/***********************
输入密码开关门
***********************/


/***********************
修改密码
***********************/

u8 change_passwd(u8 * lock_passwd)				//输入密码，开关门
{
		u8 changePasswd=0;
		u8 temp = 'N';											//定义临时变量，用来存放按键键值
	
		OLED_ShowString(0,2,"OldPasswd:");		//显示屏显示输入的密码
	
		if((temp=JZKEY_Scan(0))!='N')				//判断，按键按下时进入（不按按键返回N）
		{
			if(temp=='*')
			{
				
				for(num=0;num<4;num++)					//输入一半返回时，输入缓冲区清零
				scan_key[num] = '\0';
				num=0;
				return 0;
			}
			
			scan_key[num] = temp;							//把输入的密码存入scan_key数组
			//printf("scan_key=%c\n",scan_key[num]);
			num++;														//计数++，输入密码的位数
			OLED_ShowString(80,2,scan_key);		//显示屏显示输入的密码
		}

		if(num==4)													//如果输入了四位密码
		{
																			//先将计数清零
			if(*(lock_passwd++)==scan_key[0]&&*(lock_passwd++)==scan_key[1]&&*(lock_passwd++)==scan_key[2]&&*(lock_passwd++)==scan_key[3])//逐位判断密码是否和密码表密码相同
			{
				OLED_ShowString(80,2,"    ");
				OLED_ShowString(0,2,"NewPasswd:");
				for(num=0;num<4;num++)					//输入缓冲区清零
					scan_key[num] = '\0';
				while(1)
				{
					
					if((temp=JZKEY_Scan(0))!='N')				//判断，按键按下时进入（不按按键返回N）
						{
							if(temp=='*')
								{
									
									for(num=0;num<4;num++)					//输入一半返回时，输入缓冲区清零
									scan_key[num] = '\0';
									num=0;
									changePasswd=0;
									return 0;
								}
									
							scan_key[changePasswd] = temp;							//把输入的密码存入scan_key数组
							//printf("scan_key=%c\n",scan_key[num]);
							changePasswd++;														//计数++，输入密码的位数
							OLED_ShowString(80,2,scan_key);		//显示屏显示输入的密码
						}
							if(changePasswd==4)
								{
									lock_passwd = lock_passwd-4;//密码表指针回到0位
									*(lock_passwd++)=scan_key[0];//修改密码
									*(lock_passwd++)=scan_key[1];
									*(lock_passwd++)=scan_key[2];
									*(lock_passwd++)=scan_key[3];
									for(num=0;num<4;num++)					//输入缓冲区清零
										scan_key[num] = '\0';
									num=0;
									return 'R';
								}
				}

			}
			else
			{

				OLED_ShowString(0,4,"Wrong passwd!!");
				for(num=0;num<4;num++)			//密码缓冲区清零
				scan_key[num]='\0';
				num = 0;										//num归零，防止再进入if
				LED1 = 0;
				delay_ms(20000);						//红灯亮以示警告
				delay_ms(20000);
				LED1 = 1;
				OLED_ShowString(80,2,"    ");//OLED显示passwd区域清除
				OLED_ShowString(0,4,row_clear);//OLED显示passwd区域清除
				if(++warning1==3)
				{
					warning1 = 0;
					OLED_Clear();
					while(1)									//错误3次，死循环，led闪烁报警，只能按*返回主页面
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
		return 'W';											//返回关门指令
}
/***********************
修改密码
***********************/


