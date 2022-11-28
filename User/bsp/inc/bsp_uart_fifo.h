/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_
#include <stdint.h>
#include "bsp.h"

/*	
	串口分配：
	【串口1】 RS232	--- 打印调试口，上位机
		PA9/USART1_TX
		PA10/USART1_RX

	【串口2】GPS 模块  --- 卫星定位，授时
		PA2/USART2_TX
		PA3/USART2_RX 

	【串口3】WiFi 模块
		PB10/USART3_TX
		PB11/USART3_RX

	【串口4】 RS485 通信
		PC10/UART4_TX
		PC11/UART4_RX
		PD0/BOOT1/RS485_TX_EN
		
	【串口5】 RS232 检测器输入
		PC12/UART5_TX
		PD2/UART5_RX
*/

#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	1
#define	UART4_FIFO_EN	1
#define	UART5_FIFO_EN	1

/* RS485芯片发送使能GPIO, PB2 */
#define RCC_RS485_DIR 	 RCC_APB2Periph_GPIOD
#define RS485_DIR_PORT   GPIOD
#define RS485_DIR_PIN	 GPIO_Pin_0

//高电平接收，低电平发射
#define RS485_RX_EN()	RS485_DIR_PORT->BSRR = RS485_DIR_PIN
#define RS485_TX_EN()	RS485_DIR_PORT->BRR = RS485_DIR_PIN


/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint16_t _byte);	/* 串口收到数据的回调函数指针 */
}UART_T;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
	extern UART_T g_tUart1;
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			9600
	#define UART2_TX_BUF_SIZE	128
	#define UART2_RX_BUF_SIZE	1024
    extern UART_T Uart2Gps;
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			115200
	#define UART3_TX_BUF_SIZE	1024
	#define UART3_RX_BUF_SIZE	1024
    extern UART_T g_tUart3;
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			9600
	#define UART4_TX_BUF_SIZE	1024
	#define UART4_RX_BUF_SIZE	1024
    extern UART_T g_tUart4;
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200
	#define UART5_TX_BUF_SIZE	1024
	#define UART5_RX_BUF_SIZE	1024
    extern UART_T g_tUart5;
#endif


void bsp_InitUart(void);

void UartSendBuf(UART_T *uart_t, uint8_t *_ucaBuf, uint16_t _usLen);
void UartSendChar(UART_T *uart_t, uint8_t _ucByte);

uint16_t UartGetCount(UART_T *uart_t);

void UartClearTxFifo(UART_T *uart_t);
void UartClearRxFifo(UART_T *uart_t);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(char *_pBuf);

void bsp_Set485Baud(uint32_t _baud);

void bsp_SetUart1Baud(uint32_t _baud);
void bsp_SetUart2Baud(uint32_t _baud);

void Gps_ReciveNew(uint16_t RxCount);

void printf_fifo_dec(uint8_t* tx, uint8_t len);
void printf_fifo_hex(uint8_t* tx, uint8_t len);


uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);


#endif
