/*-----------------------------------------------------*/
/*                     �ʵ��Ŷӿ���                    */
/*-----------------------------------------------------*/
/*                     ����ṹ                        */
/*-----------------------------------------------------*/
/*USER     �����������main��������������������      */
/*HARDWARE ��������������ֹ����������������          */
/*CORE     ������STM32�ĺ��ĳ��򣬹ٷ��ṩ�����ǲ��޸� */
/*STLIB    ���ٷ��ṩ�Ŀ��ļ������ǲ��޸�              */
/*-----------------------------------------------------*/
/*                                                     */
/*           ����main��������ں���ͷ�ļ�              */
/*                                                     */
/*-----------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"

extern unsigned int SystemTimer;        //��������
extern unsigned int TEHUTimer;        //��������
extern u8 temp,humi,ppm;//�¶ȣ�ʪ�ȣ�����Ũ�Ȱٷֱ�

#define  PRODUCTKEY           "ijarUSB40EL"                                        //��ƷID
#define  DEVICENAME           "DHT11"                                               //�豸��  
#define  DEVICESECRE          "df9ec084a0845c2f62873ca3390a60d6"                   //�豸��Կ   
#define  S_TOPIC_NAME         "/sys/ijarUSB40EL/DHT11/thing/service/property/set"   //��Ҫ���ĵ����� DHT11Ϊ�豸����
#define  P_TOPIC_NAME         "/sys/ijarUSB40EL/DHT11/thing/event/property/post"    //��Ҫ����������  

#define SSID   "CMCC"              //·����SSID����   2.4G   
#define PASS   "135141pm"                 //·��������



#endif











