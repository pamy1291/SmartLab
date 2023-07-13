
#include <string.h>
#include "usart.h"
#include "syn6288.h"

/**************芯片设置命令*********************/
/*外部声明*/
extern unsigned char SYN_StopCom[];
extern unsigned char SYN_SuspendCom[];
extern unsigned char SYN_RecoverCom[];
extern unsigned char SYN_ChackCom[];
extern unsigned char SYN_PowerDownCom[];

//选择背景音乐2。(0：无背景音乐  1-15：背景音乐可选)
//v[0~16]:0背景音乐为静音，16背景音乐音量最大
//m[0~16]:0朗读音量为静音，16朗读音量最大
//t[0~5]:0朗读语速最慢，5朗读语速最快
//其他不常用功能请参考数据手册
//SYN_FrameInfo(2, "[v10][m5][t4]欢迎使用SYN6288语音合成模块！");
//暂停合成，此时没有用到，用于展示函数用法
//YS_SYN_Set(SYN_SuspendCom);

//Music:  0:无背景音乐  1~15:选择背景音乐
void SYN_FrameInfo(unsigned char Music, unsigned char *HZdata)
{
	/****************需要发送的文本**********************************/
	unsigned char  Frame_Info[50];
	unsigned char  HZ_Length;
	unsigned char  ecc  = 0;  			//定义校验字节
	unsigned int i = 0;
	HZ_Length = strlen((char*)HZdata); 			//需要发送文本的长度

	/*****************帧固定配置信息**************************************/
	Frame_Info[0] = 0xFD ; 			//构造帧头FD
	Frame_Info[1] = 0x00 ; 			//构造数据区长度的高字节
	Frame_Info[2] = HZ_Length + 3; 		//构造数据区长度的低字节
	Frame_Info[3] = 0x01 ; 			//构造命令字：合成播放命令
	Frame_Info[4] = 0x01 | Music << 4 ; //构造命令参数：背景音乐设定

	/*******************校验码计算***************************************/
	for(i = 0; i < 5; i++)   				//依次发送构造好的5个帧头字节
	{
		ecc = ecc ^ (Frame_Info[i]);		//对发送的字节进行异或校验
	}

	for(i = 0; i < HZ_Length; i++)   		//依次发送待合成的文本数据
	{
		ecc = ecc ^ (HZdata[i]); 				//对发送的字节进行异或校验
	}
	/*******************发送帧信息***************************************/
	memcpy(&Frame_Info[5], HZdata, HZ_Length);
	Frame_Info[5 + HZ_Length] = ecc;
	UART4_SendString(Frame_Info,5 + HZ_Length + 1);
}
