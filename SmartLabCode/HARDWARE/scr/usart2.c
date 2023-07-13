/*-------------------------------------------------*/
/*                     质点团队开发                */
/*-------------------------------------------------*/
/*                                                 */
/*             实现串口2功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "usart2.h"     //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件

#if  USART2_RX_ENABLE                   //如果使能接收功能
char Usart2_RxCompleted = 0;            //定义一个变量 0：表示接收未完成 1：表示接收完成 
unsigned int Usart2_RxCounter = 0;      //定义一个变量，记录串口2总共接收了多少字节的数据
char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //定义一个数组，用于保存串口2接收到的数据   	
#endif

char DMA_flag = 0;                      // 0 DMA发送空闲   1 DMA发送中
/*-------------------------------------------------*/
/*函数名：初始化串口2                               */
/*参  数：bound：波特率                             */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Usart2_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //定义一个设置GPIO功能的变量
	USART_InitTypeDef USART_InitStructure;   //定义一个设置串口功能的变量
#if USART2_RX_ENABLE                         //如果使能接收功能
	NVIC_InitTypeDef NVIC_InitStructure;     //如果使能接收功能，定义一个设置中断的变量
#endif

#if USART2_RX_ENABLE                                 //如果使能接收功能
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断向量分组：第2组 抢先优先级：0 1 2 3 子优先级：0 1 2 3
#endif	
    USART_DeInit(USART2);                                  //串口2 所有寄存器 恢复默认值
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);  //使能串口2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //使能GPIOA时钟
	USART_DeInit(USART2);                                  //串口2寄存器重新设置为默认值
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;              //准备设置PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO速率50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //复用推挽输出，用于串口2的发送
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //设置PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              //准备设置PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入，用于串口2的接收
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //设置PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
#if USART2_RX_ENABLE               												   //如果使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //收发模式
#else                                                                              //如果不使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //只发模式
#endif        
    USART_Init(USART2, &USART_InitStructure);                                      //设置串口2	

#if USART2_RX_ENABLE  	         					        //如果使能接收模式
	USART_ClearFlag(USART2, USART_FLAG_RXNE);	            //清除接收标志位
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);          //开启接收中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       //设置串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//中断通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //设置串口2中断
#endif  

	USART_Cmd(USART2, ENABLE);                              //使能串口2
}
/*-------------------------------------------------*/
/*函数名：初始化串口2 开启DMA和空闲中断              */
/*参  数：bound：波特率                             */
/*返回值：无                                        */
/*-------------------------------------------------*/
void Usart2_IDELInit(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //定义一个设置GPIO功能的变量
	USART_InitTypeDef USART_InitStructure;   //定义一个设置串口功能的变量
#if USART2_RX_ENABLE                         //如果使能接收功能
	NVIC_InitTypeDef NVIC_InitStructure;     //如果使能接收功能，定义一个设置中断的变量
#endif

#if USART2_RX_ENABLE                                 //如果使能接收功能
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断向量分组：第2组 抢先优先级：0 1 2 3 子优先级：0 1 2 3
#endif	
    USART_DeInit(USART2);                                  //串口2 所有寄存器 恢复默认值
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);  //使能串口2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //使能GPIOA时钟
	USART_DeInit(USART2);                                  //串口2寄存器重新设置为默认值
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;              //准备设置PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO速率50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //复用推挽输出，用于串口2的发送
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //设置PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              //准备设置PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入，用于串口2的接收
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //设置PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
#if USART2_RX_ENABLE               												   //如果使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //收发模式
#else                                                                              //如果不使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //只发模式
#endif        
    USART_Init(USART2, &USART_InitStructure);                                      //设置串口2	

#if USART2_RX_ENABLE  	         					        //如果使能接收模式
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);          //开启空闲中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       //设置串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//中断通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //设置串口2中断
#endif  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);            //开启串口2 DMA接收
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);            //开启串口2 DMA发送
	Usart2DMA_init();                                       //初始化DMA
	USART_Cmd(USART2, ENABLE);                              //使能串口2
	
}

/*-------------------------------------------------*/
/*函数名：串口2 printf函数                         */
/*参  数：char* fmt,...  格式化输出字符串和参数    */
/*返回值：无                                       */
/*-------------------------------------------------*/

__align(8) char USART2_TxBuff[USART2_TXBUFF_SIZE];  

void u2_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(USART2_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)USART2_TxBuff);		
	while((USART2->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = USART2_TxBuff[i];
		while((USART2->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*函数名：串口2发送缓冲区中的数据                  */
/*参  数：data：数据                               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u2_TxData(unsigned char *data)
{
	DMA_flag = 1;                                                    //DMA标志=1  要开始发送了
	DMA1_Channel7->CNDTR = (unsigned int)(data[0]*256+data[1]);      //重新设置数据个数   
	DMA1_Channel7->CMAR =  (unsigned int)(&data[2]);                 //重新设内存地址
	DMA_Cmd(DMA1_Channel7,ENABLE);                                   //开启DMA
}
/*-------------------------------------------------*/
/*函数名：串口2 DMA初始化                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Usart2DMA_init(void) 
{   
	DMA_InitTypeDef    DMA_Initstructure;    
	NVIC_InitTypeDef   NVIC_InitStructure;   
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);                               //开启DMA1 时钟	
	//先配置通道6 串口2的接收
	DMA_Initstructure.DMA_PeripheralBaseAddr =  (unsigned int)(&USART2->DR);        //外设地址
	DMA_Initstructure.DMA_MemoryBaseAddr     = (unsigned int)Usart2_RxBuff;         //内存地址
	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC;                              //从外设到内存 
	DMA_Initstructure.DMA_BufferSize = USART2_RXBUFF_SIZE;                          //大小设置为串口2的缓冲区大小
	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                //外设不增
	DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;                          //内存地址寄存器递增
	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;         //外设数据宽度为8位
	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                 //内存数据宽度为8位
	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;                                   //工作在正常模式
	DMA_Initstructure.DMA_Priority = DMA_Priority_High;                             //拥有高优先级
	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;                                    //没有设置为内存到内存传输
	DMA_Init(DMA1_Channel6,&DMA_Initstructure);	                                    //设置通道6
	DMA_Cmd(DMA1_Channel6,ENABLE);                                                  //使能通道6
	
	//再配置通道7 串口2的发送
	DMA_Initstructure.DMA_PeripheralBaseAddr =(unsigned int)(&USART2->DR);          //外设地址
	DMA_Initstructure.DMA_MemoryBaseAddr     =(unsigned int)(&MQTT_TxDataOutPtr[2]);//内存地址
	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralDST;                              //从内存到外设 
	DMA_Initstructure.DMA_BufferSize = TBUFF_UNIT;                                  //大小设置为TBUFF_UNIT
	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                //外设不增
	DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;                          //内存地址寄存器递增
	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;         //外设数据宽度为8位
	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                 //内存数据宽度为8位
	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;                                   //工作在正常模式
	DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;                           //拥有中等优先级
	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;                                    //没有设置为内存到内存传输
	DMA_Init(DMA1_Channel7,&DMA_Initstructure);	                                    //设置通道7
	DMA_Cmd(DMA1_Channel7,DISABLE);                                                 //先不要使能通道7	
	
	DMA_ClearITPendingBit(DMA1_IT_TC7);                                             //清除DMA1通道7 发送完成中断标志
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);                                   //开启DMA1通道7 发送完成中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;                        //设置DMA中断 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; 					    //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;							    //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                 //打开中断 
	NVIC_Init(&NVIC_InitStructure);                                                 //配置中断
}
