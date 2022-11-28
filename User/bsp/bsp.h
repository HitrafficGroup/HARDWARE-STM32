/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_H
#define _BSP_H


/* ���� BSP �汾�� */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU����ʱִ�еĺ��� */
#define CPU_IDLE()		bsp_Idle()

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

/* ���������ڵ��Խ׶��Ŵ� */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#include "stm32f10x.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#include "bsp_io.h"
#include "bsp_led.h"
#include "bsp_oled.h"
#include "bsp_timer.h"
#include "bsp_i2c.h"
#include "bsp_spi_bus.h"
#include "bsp_spi_fm25v.h"
#include "bsp_spi_ch376t.h"
#include "bsp_uart_fifo.h"
#include "bsp_w5500.h"
#include "can_network.h"
#include "bsp_iwdg.h"


#include "sd2405.h"
#include "tsc.h"

#include "BasicInfo.h"
#include "Pattern.h"
#include "Sequence.h"
#include "Channel.h"
#include "Phase.h"
#include "Split.h"
#include "DefaultParameter.h"
#include "gb25280.h"
#include "Overlap.h"
#include "Peddet.h"

//extern volatile uint16_t reg1ms;
//extern volatile uint16_t reg1sCount;
extern volatile uint8_t reg1s_flag;
extern volatile uint8_t reg1ms_flag;
extern volatile uint8_t reg10ms_flag;
extern volatile uint8_t reg100ms_flag;


/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);

#endif
