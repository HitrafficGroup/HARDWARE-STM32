/*
*********************************************************************************************************
*
*	ģ������ : SPI��������
*	�ļ����� : bsp_spi_bus.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_BUS_H
#define _BSP_SPI_BUS_H
#include <stdint.h>



/*
	��SPIʱ�������2��Ƶ����֧�ֲ���Ƶ��
	�����SPI1��2��ƵʱSCKʱ�� = 42M��4��ƵʱSCKʱ�� = 21M
	�����SPI3, 2��ƵʱSCKʱ�� = 21M
*/
#define SPI_SPEED_42M		SPI_BaudRatePrescaler_2
#define SPI_SPEED_21M		SPI_BaudRatePrescaler_4
#define SPI_SPEED_5_2M	SPI_BaudRatePrescaler_8
#define SPI_SPEED_2_6M	SPI_BaudRatePrescaler_16
#define SPI_SPEED_1_3M	SPI_BaudRatePrescaler_32
#define SPI_SPEED_0_6M	SPI_BaudRatePrescaler_64

void bsp_InitSpiBus(void);
uint8_t Spi_SendByte(uint8_t _ucValue);


#endif
