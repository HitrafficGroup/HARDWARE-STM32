/*
*********************************************************************************************************
*	ģ������ : BSPģ��(For STM32F103)
*	�ļ����� : bsp.c
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ���������ģ������ļ�����Ҫ�ṩ bsp_Init()��������������á��������ÿ��c�ļ������ڿ�
*			  ͷ	��� #include "bsp.h" ���������е���������ģ�顣
*********************************************************************************************************
*/

#include "bsp.h"
#include "PhaseStateControl.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ��Ӳ���豸��ֻ��Ҫ����һ�Ρ��ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����
*			 ȫ�ֱ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/*
		����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����
		ϵͳʱ��ȱʡ����Ϊ72MHz�������Ҫ���ģ������޸� system_stm32f103.c �ļ�
	*/

	/* ���ȼ���������Ϊ4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	bsp_InitLed();		/* ����LED��GPIO�˿� */
    bsp_InitIO();
    LampDriveOff();
    W5500_GPIO_Configuration();
    
	bsp_InitTimer();	/* ��ʼ��ϵͳ�δ�ʱ�� (�˺����Ὺ�ж�) */
	bsp_InitUart();		/* ��ʼ���������� */
	can_Init();         /* ��ʼ��STM32 CANӲ�� */
	can_NVIC_Config();  /* ����CAN�ж� */
    
    bsp_DelayMS(100);
    
    RtcInit();
    bsp_InitSpiBus();   // ��ʼ��spi�ӿ�
	Ch376t_Init();      // CS INT �ܽ�����
	Fm25v_Init();	    // ���� PE11-Fm25v_CS PE12-W25Q_CS PE15-W5500_CS
    
	Fm25v_ReadInfo();   /* �Զ�ʶ��оƬ�ͺ� */
	
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
*	�� �� ��: bsp_Idle
*	����˵��: ����ʱִ�еĺ�����һ����������for��whileѭ������������Ҫ���� CPU_IDLE() �������ñ�������
*			 ������ȱʡΪ�ղ������û��������ι��������CPU��������ģʽ�Ĺ��ܡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Idle(void)
{
	/* --- ι�� */
	/* --- ��CPU�������ߣ���Systick��ʱ�жϻ��ѻ��������жϻ��� */
	/* ���� emWin ͼ�ο⣬���Բ���ͼ�ο���Ҫ����ѯ���� */
	/* GUI_Exec(); */
	/* ���� uIP Э�飬���Բ���uip��ѯ���� */
}
