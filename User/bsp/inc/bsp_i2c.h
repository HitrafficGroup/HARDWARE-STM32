/*
*********************************************************************************************************
*
*	ģ������ : I2C��������ģ��
*	�ļ����� : bsp_i2c_gpio.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ���
*********************************************************************************************************
*/

#ifndef _BSP_I2C_H
#define _BSP_I2C_H
#include <stdint.h>

#define LM75_ADDR   0x90



void bsp_InitI2C(void);
//void I2C_ByteWrite(uint8_t Addr,uint8_t addr,uint8_t dataValue);
//uint8_t I2C_ByteRead(uint8_t Addr,uint8_t addr);
//uint8_t I2C_Master_BufferWrite(uint8_t* pBuffer, uint16_t NumByteToWrite, uint16_t DataAddress, uint8_t SlaveAddress);


void Temperature_Read(void);

#endif
