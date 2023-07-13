#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"

//矩阵按键
#define Hang_00_L  GPIO_ResetBits(GPIOA, GPIO_Pin_3)//行00
#define Hang_00_H  GPIO_SetBits(GPIOA, GPIO_Pin_3)
 
#define Hang_01_L  GPIO_ResetBits(GPIOA, GPIO_Pin_2)//行01
#define Hang_01_H  GPIO_SetBits(GPIOA, GPIO_Pin_2)
 
#define Hang_02_L  GPIO_ResetBits(GPIOA, GPIO_Pin_1)//行02
#define Hang_02_H  GPIO_SetBits(GPIOA, GPIO_Pin_1)
 
#define Hang_03_L  GPIO_ResetBits(GPIOA, GPIO_Pin_0)//行03
#define Hang_03_H  GPIO_SetBits(GPIOA, GPIO_Pin_0)
 
#define Lie_00_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//列00
#define Lie_01_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//列01
#define Lie_02_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//列02
#define Lie_03_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//列03

#define  jpys   1   //宏定义，约定jpys==20，方便以后程序移植
//板子上的按键
//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define WK_UP PAin(0)	//PA0  WK_UP

#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 
#define KEY0_PRES 	1	//KEY0按下
#define KEY1_PRES	  2	//KEY1按下
#define WKUP_PRES   3	//KEY_UP按下(即WK_UP/KEY_UP)

//矩阵按键上的复位按键
#define KEYS1  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1)//读取按键0
#define KEYS2  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2)//读取按键0
#define KEYS3  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3)//读取按键0
#define KEYS4  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)//读取按键0

#define KEYS1_PRES 	1	//S1按下
#define KEYS2_PRES	2	//S2按下
#define KEYS3_PRES  3	//S3按下
#define KEYS4_PRES 4  //S4按下 

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函

void JZKEY_init(void);//IO初始化
u8 JZKEY_Scan(u8 mode);  	//按键扫描函数

void JZKEY_RED(char *p);
void lock_key_scan(void);
u8 entry_passwd(u8 * lock_passwd);
u8 change_passwd(u8 * lock_passwd);

#endif


