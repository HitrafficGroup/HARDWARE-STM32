/*
*********************************************************************************************************
*
*	ģ������ : SPI��������
*	�ļ����� : bsp_spi_bus.h
*	��    �� : V1.0
*	˵    �� : SPI���ߵײ��������ṩSPI���á��շ����ݡ����豸����SPI֧�֡�
*	�޸ļ�¼ :
*		�汾��  ����        ����    ˵��
*   v1.0    2014-10-24      wcx     
*
*********************************************************************************************************
*/

#include "bsp.h"


#define SPI_peripheral		SPI1
#define SPI_BAUD			SPI_BaudRatePrescaler_8


/*
	SPI1 ���߷���
	PA5	SPI1_SCK
	PA6	SPI1_MISO
	PA7	SPI1_MOSI
*/

uint8_t g_spi_busy = 0;		/* SPI ���߹����־ */

static void bsp_InitSpi1GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ʹ��GPIO ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* PA5-SCK PA6-MISO PA7-MOSI */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  /* IO������ٶ� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitSPIBus
*	����˵��: ����STM32�ڲ�SPIӲ���Ĺ���ģʽ���ٶȵȲ��������ڷ���SPI�ӿڵĴ���Flash��ֻ���� SCK�� MOSI�� MISO���ߵ����á�������ƬѡCS��Ҳ����������оƬ���е�INT��BUSY��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitSpiBus(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	bsp_InitSpi1GPIO();
	
	/* ��SPIʱ�� */
	//ENABLE_SPI_RCC();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* ����SPIӲ������ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* ���ݷ���2��ȫ˫�� */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32��SPI����ģʽ ������ģʽ */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* ����λ���� �� 8λ */
	/* SPI_CPOL��SPI_CPHA���ʹ�þ���ʱ�Ӻ����ݲ��������λ��ϵ��
	   ����: ���߿����Ǹߵ�ƽ,��2�����أ������ز�������)
	*/
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			/* ʱ�������ز������� */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		/* ʱ�ӵĵ�2�����ز������� */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* Ƭѡ���Ʒ�ʽ��������� */

	/* ���ò�����Ԥ��Ƶϵ�� */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BAUD;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* ����λ������򣺸�λ�ȴ� */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC����ʽ�Ĵ�������λ��Ϊ7 */
	SPI_Init(SPI_peripheral, &SPI_InitStructure);

	SPI_Cmd(SPI_peripheral, DISABLE);			/* �Ƚ�ֹSPI  */
	SPI_Cmd(SPI_peripheral, ENABLE);			/* ʹ��SPI  */
}
/*
*********************************************************************************************************
*	�� �� ��: Spi_SendByte
*	����˵��: ����������һ���ֽڣ�ͬʱ��MISO���߲����������ص�����
*	��    ��:  _ucByte : ���͵��ֽ�ֵ
*	�� �� ֵ: ��MISO���߲����������ص�����
*********************************************************************************************************
*/
uint8_t Spi_SendByte(uint8_t _ucValue)
{
	/* �ȴ��ϸ�����δ������� */
	while(SPI_I2S_GetFlagStatus(SPI_peripheral, SPI_I2S_FLAG_TXE) == RESET);

	/* ͨ��SPIӲ������1���ֽ� */
	SPI_I2S_SendData(SPI_peripheral, _ucValue);

	/* �ȴ�����һ���ֽ�������� */
	while(SPI_I2S_GetFlagStatus(SPI_peripheral, SPI_I2S_FLAG_RXNE) == RESET);

	/* ���ش�SPI���߶��������� */
	return SPI_I2S_ReceiveData(SPI_peripheral);
}
