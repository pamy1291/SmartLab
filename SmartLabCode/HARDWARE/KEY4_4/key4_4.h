#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"

//���󰴼�
#define Hang_00_L  GPIO_ResetBits(GPIOA, GPIO_Pin_3)//��00
#define Hang_00_H  GPIO_SetBits(GPIOA, GPIO_Pin_3)
 
#define Hang_01_L  GPIO_ResetBits(GPIOA, GPIO_Pin_2)//��01
#define Hang_01_H  GPIO_SetBits(GPIOA, GPIO_Pin_2)
 
#define Hang_02_L  GPIO_ResetBits(GPIOA, GPIO_Pin_1)//��02
#define Hang_02_H  GPIO_SetBits(GPIOA, GPIO_Pin_1)
 
#define Hang_03_L  GPIO_ResetBits(GPIOA, GPIO_Pin_0)//��03
#define Hang_03_H  GPIO_SetBits(GPIOA, GPIO_Pin_0)
 
#define Lie_00_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//��00
#define Lie_01_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//��01
#define Lie_02_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//��02
#define Lie_03_V GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//��03

#define  jpys   1   //�궨�壬Լ��jpys==20�������Ժ������ֲ
//�����ϵİ���
//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define WK_UP PAin(0)	//PA0  WK_UP

#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//��ȡ����1
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����3(WK_UP) 
#define KEY0_PRES 	1	//KEY0����
#define KEY1_PRES	  2	//KEY1����
#define WKUP_PRES   3	//KEY_UP����(��WK_UP/KEY_UP)

//���󰴼��ϵĸ�λ����
#define KEYS1  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1)//��ȡ����0
#define KEYS2  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2)//��ȡ����0
#define KEYS3  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3)//��ȡ����0
#define KEYS4  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)//��ȡ����0

#define KEYS1_PRES 	1	//S1����
#define KEYS2_PRES	2	//S2����
#define KEYS3_PRES  3	//S3����
#define KEYS4_PRES 4  //S4���� 

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯

void JZKEY_init(void);//IO��ʼ��
u8 JZKEY_Scan(u8 mode);  	//����ɨ�躯��

void JZKEY_RED(char *p);
void lock_key_scan(void);
u8 entry_passwd(u8 * lock_passwd);
u8 change_passwd(u8 * lock_passwd);

#endif


