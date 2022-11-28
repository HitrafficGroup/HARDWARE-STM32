/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_
#include <stdint.h>
#include "bsp.h"

/*	
	���ڷ��䣺
	������1�� RS232	--- ��ӡ���Կڣ���λ��
		PA9/USART1_TX
		PA10/USART1_RX

	������2��GPS ģ��  --- ���Ƕ�λ����ʱ
		PA2/USART2_TX
		PA3/USART2_RX 

	������3��WiFi ģ��
		PB10/USART3_TX
		PB11/USART3_RX

	������4�� RS485 ͨ��
		PC10/UART4_TX
		PC11/UART4_RX
		PD0/BOOT1/RS485_TX_EN
		
	������5�� RS232 ���������
		PC12/UART5_TX
		PD2/UART5_RX
*/

#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	1
#define	UART4_FIFO_EN	1
#define	UART5_FIFO_EN	1

/* RS485оƬ����ʹ��GPIO, PB2 */
#define RCC_RS485_DIR 	 RCC_APB2Periph_GPIOD
#define RS485_DIR_PORT   GPIOD
#define RS485_DIR_PIN	 GPIO_Pin_0

//�ߵ�ƽ���գ��͵�ƽ����
#define RS485_RX_EN()	RS485_DIR_PORT->BSRR = RS485_DIR_PIN
#define RS485_TX_EN()	RS485_DIR_PORT->BRR = RS485_DIR_PIN


/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;	/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;	/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint16_t _byte);	/* �����յ����ݵĻص�����ָ�� */
}UART_T;

/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
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
