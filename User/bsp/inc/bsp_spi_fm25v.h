/*
*********************************************************************************************************
*
*	ģ������ : SPI�ӿڴ���FLASH ��дģ��
*	�ļ����� : bsp_spi_fm25v.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FM25V_H
#define _BSP_SPI_FM25V_H

#include <stdint.h>

typedef struct
{
	uint8_t ChipID[9];		/* оƬID */
	char ChipName[16];		/* оƬ�ͺ��ַ�������Ҫ������ʾ */
	uint32_t TotalSize;		/* ������ */
}Fm25v_T;

void Fm25v_Init(void);
uint8_t Fm25v_ReadID(void);
void Fm25v_ReadInfo(void);

static void Fm25v_WriteEnable(void);
static void Fm25v_WriteDisable(void);

uint8_t Fm25v_Read(uint8_t * _pBuf, uint32_t _uiReadAddr);
uint8_t Fm25v_Write(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
uint8_t Fm25v_FastRead(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);

extern Fm25v_T fm25v;

#endif
