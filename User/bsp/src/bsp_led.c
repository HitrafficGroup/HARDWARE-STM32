/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.c
*	��    �� : V1.0
*	˵    �� : ����LEDָʾ��
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    LED �ߵ�ƽ���� ���߷��䣺

    LED1       : PE14
    LED2       : PE13
    LED3       : PE12
    LED4       : PE11

    LED5       : PE10
    LED6       : PE9 
    LED7       : PE8
    LED8       : PE7
*/
	#define RCC_ALL_LED 	RCC_APB2Periph_GPIOE	/* �����ڶ�Ӧ��RCCʱ�� */

	#define GPIO_PORT_LED   GPIOE
	
	#define GPIO_PIN_LED1	GPIO_Pin_10
	#define GPIO_PIN_LED2	GPIO_Pin_9
	#define GPIO_PIN_LED3	GPIO_Pin_8
	#define GPIO_PIN_LED4	GPIO_Pin_7
	#define GPIO_PIN_LED5	GPIO_Pin_14
	#define GPIO_PIN_LED6	GPIO_Pin_13
	#define GPIO_PIN_LED7	GPIO_Pin_12
	#define GPIO_PIN_LED8	GPIO_Pin_11
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_ALL_LED, ENABLE);

	/*
		�������е�LEDָʾ��GPIOΪ�������ģʽ
		���ڽ�GPIO����Ϊ���ʱ��GPIO����Ĵ�����ֵȱʡ��0
	*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  /* IO������ٶ� */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1 | GPIO_PIN_LED2 | GPIO_PIN_LED3 | GPIO_PIN_LED4 | GPIO_PIN_LED5 | GPIO_PIN_LED6 | GPIO_PIN_LED7 | GPIO_PIN_LED8;
	GPIO_Init(GPIO_PORT_LED, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 8
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED2;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED3;
	}
	else if (_no == 4)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED4;
	}
	else if (_no == 5)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED5;
	}
	else if (_no == 6)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED6;
	}
	else if (_no == 7)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED7;
	}
	else if (_no == 8)
	{
		GPIO_PORT_LED->BSRR = GPIO_PIN_LED8;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 8
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED2;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED3;
	}
	else if (_no == 4)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED4;
	}
	else if (_no == 5)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED5;
	}
	else if (_no == 6)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED6;
	}
	else if (_no == 7)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED7;
	}
	else if (_no == 8)
	{
		GPIO_PORT_LED->BRR = GPIO_PIN_LED8;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 8
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED2;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED3;
	}
	else if (_no == 4)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED4;
	}
	else if (_no == 5)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED5;
	}
	else if (_no == 6)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED6;
	}
	else if (_no == 7)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED7;
	}
	else if (_no == 8)
	{
		GPIO_PORT_LED->ODR ^= GPIO_PIN_LED8;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED1) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 2)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED2) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 3)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED3) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 4)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED4) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 5)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED5) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 6)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED6) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 7)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED7) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 8)
	{
		if ((GPIO_PORT_LED->ODR & GPIO_PIN_LED8) == 0)
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*************************************************************************************************/
