/*
*********************************************************************************************************
*	                                  
*	ģ������ : CAN������ʾ����
*	�ļ����� : can_network.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-09-01 armfly  ST�̼���V3.5.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/


#ifndef _CAN_NETWORK_H
#define _CAN_NETWORK_H

#include "bsp.h"



extern CanTxMsg CanTxMsgStruct;
extern CanRxMsg CanRxMsgStruct;

/* ���ⲿ���õĺ������� */
void can_Init(void);					/* ��ʼ��STM32 CANӲ�� */
void can_NVIC_Config(void);				/* ����CAN�ж� */
void SendCanMsg(uint8_t *p, uint8_t length);

#endif
