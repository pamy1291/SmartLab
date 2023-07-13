/*-----------------------------------------------------*/
/*                     质点团队开发                    */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数头文件              */
/*                                                     */
/*-----------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"

extern unsigned int SystemTimer;        //变量声明
extern unsigned int TEHUTimer;        //变量声明
extern u8 temp,humi,ppm;//温度，湿度，烟雾浓度百分比

#define  PRODUCTKEY           "ijarUSB40EL"                                        //产品ID
#define  DEVICENAME           "DHT11"                                               //设备名  
#define  DEVICESECRE          "df9ec084a0845c2f62873ca3390a60d6"                   //设备秘钥   
#define  S_TOPIC_NAME         "/sys/ijarUSB40EL/DHT11/thing/service/property/set"   //需要订阅的主题 DHT11为设备名称
#define  P_TOPIC_NAME         "/sys/ijarUSB40EL/DHT11/thing/event/property/post"    //需要发布的主题  

#define SSID   "CMCC"              //路由器SSID名称   2.4G   
#define PASS   "135141pm"                 //路由器密码



#endif











