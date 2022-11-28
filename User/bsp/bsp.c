/*
*********************************************************************************************************
*	模块名称 : BSP模块(For STM32F103)
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。主程序的每个c文件可以在开
*			  头	添加 #include "bsp.h" 来包含所有的外设驱动模块。
*********************************************************************************************************
*/

#include "bsp.h"
#include "PhaseStateControl.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。
		系统时钟缺省配置为72MHz，如果需要更改，可以修改 system_stm32f103.c 文件
	*/

	/* 优先级分组设置为4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	bsp_InitLed();		/* 配置LED的GPIO端口 */
    bsp_InitIO();
    LampDriveOff();
    W5500_GPIO_Configuration();
    
	bsp_InitTimer();	/* 初始化系统滴答定时器 (此函数会开中断) */
	bsp_InitUart();		/* 初始化串口驱动 */
	can_Init();         /* 初始化STM32 CAN硬件 */
	can_NVIC_Config();  /* 配置CAN中断 */
    
    bsp_DelayMS(100);
    
    RtcInit();
    bsp_InitSpiBus();   // 初始化spi接口
	Ch376t_Init();      // CS INT 管脚配置
	Fm25v_Init();	    // 配置 PE11-Fm25v_CS PE12-W25Q_CS PE15-W5500_CS
    
	Fm25v_ReadInfo();   /* 自动识别芯片型号 */
	
	bsp_Init_OLED_gpio();
    
	oled_Init();
	OLED_Clear(); 
}

volatile uint8_t reg1s_flag;
volatile uint8_t reg1ms_flag = 1;
volatile uint8_t reg10ms_flag;
volatile uint8_t reg100ms_flag;

volatile uint16_t reg1ms;
//volatile uint16_t reg1sCount;

void bsp_RunPer100ms(void)
{
    reg100ms_flag = 1;
    OP.Run100ms_flag = 1;
}


void bsp_RunPer10ms(void)
{
    reg10ms_flag = 1;
    OP.Run10ms_flag = 1;
    ChannelStatusToLmap();
    i2c_delay++;
}

void bsp_RunPer1ms(void)
{
    reg1ms_flag = 1;
	if(++reg1ms >= 1000)
    {
        reg1ms = 0;
        reg1s_flag = 1;
        OP.Run1s_flag = 1;
    }
    if(reg1ms%10 == 0)
        bsp_RunPer10ms();
    if(reg1ms%100 == 0)
        bsp_RunPer100ms();
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Idle
*	功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要插入 CPU_IDLE() 宏来调用本函数。
*			 本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Idle(void)
{
	/* --- 喂狗 */
	/* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */
	/* 例如 emWin 图形库，可以插入图形库需要的轮询函数 */
	/* GUI_Exec(); */
	/* 例如 uIP 协议，可以插入uip轮询函数 */
}
