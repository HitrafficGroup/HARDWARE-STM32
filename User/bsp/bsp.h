/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*********************************************************************************************************
*/

#ifndef _BSP_H
#define _BSP_H


/* 定义 BSP 版本号 */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU空闲时执行的函数 */
#define CPU_IDLE()		bsp_Idle()

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
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


/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);

#endif
