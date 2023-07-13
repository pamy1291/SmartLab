
#include <string.h>
#include "usart.h"
#include "syn6288.h"

/**************оƬ��������*********************/
/*�ⲿ����*/
extern unsigned char SYN_StopCom[];
extern unsigned char SYN_SuspendCom[];
extern unsigned char SYN_RecoverCom[];
extern unsigned char SYN_ChackCom[];
extern unsigned char SYN_PowerDownCom[];

//ѡ�񱳾�����2��(0���ޱ�������  1-15���������ֿ�ѡ)
//v[0~16]:0��������Ϊ������16���������������
//m[0~16]:0�ʶ�����Ϊ������16�ʶ��������
//t[0~5]:0�ʶ�����������5�ʶ��������
//���������ù�����ο������ֲ�
//SYN_FrameInfo(2, "[v10][m5][t4]��ӭʹ��SYN6288�����ϳ�ģ�飡");
//��ͣ�ϳɣ���ʱû���õ�������չʾ�����÷�
//YS_SYN_Set(SYN_SuspendCom);

//Music:  0:�ޱ�������  1~15:ѡ�񱳾�����
void SYN_FrameInfo(unsigned char Music, unsigned char *HZdata)
{
	/****************��Ҫ���͵��ı�**********************************/
	unsigned char  Frame_Info[50];
	unsigned char  HZ_Length;
	unsigned char  ecc  = 0;  			//����У���ֽ�
	unsigned int i = 0;
	HZ_Length = strlen((char*)HZdata); 			//��Ҫ�����ı��ĳ���

	/*****************֡�̶�������Ϣ**************************************/
	Frame_Info[0] = 0xFD ; 			//����֡ͷFD
	Frame_Info[1] = 0x00 ; 			//�������������ȵĸ��ֽ�
	Frame_Info[2] = HZ_Length + 3; 		//�������������ȵĵ��ֽ�
	Frame_Info[3] = 0x01 ; 			//���������֣��ϳɲ�������
	Frame_Info[4] = 0x01 | Music << 4 ; //����������������������趨

	/*******************У�������***************************************/
	for(i = 0; i < 5; i++)   				//���η��͹���õ�5��֡ͷ�ֽ�
	{
		ecc = ecc ^ (Frame_Info[i]);		//�Է��͵��ֽڽ������У��
	}

	for(i = 0; i < HZ_Length; i++)   		//���η��ʹ��ϳɵ��ı�����
	{
		ecc = ecc ^ (HZdata[i]); 				//�Է��͵��ֽڽ������У��
	}
	/*******************����֡��Ϣ***************************************/
	memcpy(&Frame_Info[5], HZdata, HZ_Length);
	Frame_Info[5 + HZ_Length] = ecc;
	UART4_SendString(Frame_Info,5 + HZ_Length + 1);
}
