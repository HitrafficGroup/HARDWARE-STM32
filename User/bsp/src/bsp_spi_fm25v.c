/*
*********************************************************************************************************
*	ģ������ : SPI�ӿڴ���FLASH ��дģ��
*	�ļ����� : bsp_spi_fm25v.c
*	��    �� : V1.0
*	˵    �� : 
*
*********************************************************************************************************
*/

#include "bsp.h"


/* ���崮��Flash ID */
const char Fm25v_ID[9] = {0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0xC2,0x24,0x00};


/* Fm25v10-GƬѡ���� �õ�ѡ�� �ø߲�ѡ�� */
#define Fm25v_CS_PORT       GPIOC
#define Fm25v_CS_PIN        GPIO_Pin_4
#define Fm25v_CS_LOW()      Fm25v_CS_PORT->BRR  = Fm25v_CS_PIN
#define Fm25v_CS_HIGH()     Fm25v_CS_PORT->BSRR = Fm25v_CS_PIN

/* Fm25v10-GƬѡ���� �õ�ѡ�� �ø߲�ѡ�� */
#define W25Q_CS_PORT        GPIOC
#define W25Q_CS_PIN	        GPIO_Pin_5
#define W25Q_CS_LOW()       W25Q_CS_PORT->BRR  = W25Q_CS_PIN
#define W25Q_CS_HIGH()      W25Q_CS_PORT->BSRR = W25Q_CS_PIN

/* Fm25v10-GƬѡ���� �õ�ѡ�� �ø߲�ѡ�� */
#define W5500_CS_PORT       GPIOA
#define W5500_CS_PIN        GPIO_Pin_8
#define W5500_CS_LOW()      W5500_CS_PORT->BRR  = W5500_CS_PIN
#define W5500_CS_HIGH()     W5500_CS_PORT->BSRR = W5500_CS_PIN


#define Fm25v_WREN	0x06	/* Set write enable latch */
#define Fm25v_WRDI	0x04	/* Reset write enable latch */
#define Fm25v_RDSR	0x05	/* Read Status Register */
#define Fm25v_WRSR	0x01	/* Write Status Register */
#define Fm25v_READ	0x03	/* Read memory data */
#define Fm25v_FSTRD	0x0b	/* Fast read memory data */
#define Fm25v_WRITE	0x02	/* Write memory data */
#define Fm25v_SLEEP	0xb9	/* Enter sleep mode */
#define Fm25v_RDID	0x9F	/* Read device ID */
#define Fm25v_SNR	0xc3	/* Read S/N */

#define DUMMY_BYTE  0xA5	/* ���������Ϊ����ֵ�����ڶ����� */

Fm25v_T fm25v;

void Fm25v_ConfigGPIO(void);
void Fm25v_Init(void);
void Fm25v_Sleep(void);
uint8_t Fm25v_ReadID(void);

static void Fm25v_SetCS(uint8_t _level);

static void Fm25v_WriteEnable(void);
static void Fm25v_WriteDisable(void);
static void Fm25v_ReadStatus(uint8_t * _pBuf);
static void Fm25v_WriteStatus(uint8_t _ucValue);

uint8_t Fm25v_Read(uint8_t * _pBuf, uint32_t _uiReadAddr);
uint8_t Fm25v_Write(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
uint8_t Fm25v_FastRead(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);

/*
*********************************************************************************************************
*	�� �� ��: sf_ConfigGPIO
*	����˵��: ���ô���Flash��ƬѡGPIO�� ����Ϊ�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Fm25v_ConfigGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��GPIO ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);

	/* PC4-Fm25v_CS PC5-W25Q_CS PA8-W5500_CS ����Ƭѡ����Ϊ�������ģʽ */
	Fm25v_SetCS(1);		/* Ƭѡ�øߣ���ѡ�� */
	GPIO_SetBits(W25Q_CS_PORT,W25Q_CS_PIN);
	GPIO_SetBits(W5500_CS_PORT,W5500_CS_PIN);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  /* IO������ٶ� */	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Pin = Fm25v_CS_PIN | W25Q_CS_PIN | W5500_CS_PIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  /* IO������ٶ� */	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Pin = W5500_CS_PIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitSpiFlash
*	����˵��: ��ʼ������FlashӲ���ӿڣ�����STM32��SPIʱ�ӡ�GPIO)
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Fm25v_Init(void)
{
	Fm25v_ConfigGPIO();
    strcpy(fm25v.ChipName,"Fm25v10-G");
	fm25v.TotalSize = 0x1ffff;
    Fm25v_ReadID();
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_SetCS(0)
*	����˵��: ����CS�� ����������SPI����
*	��    ��: ��
	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Fm25v_SetCS(uint8_t _level)
{
	if (_level == 0)
	{
		//bsp_SpiBusEnter();	/* ռ��SPI���ߣ� �������߹��� */
		Fm25v_CS_LOW();
	}
	else
	{
		Fm25v_CS_HIGH();
		//bsp_SpiBusExit();	    /* �ͷ�SPI���ߣ� �������߹��� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_WriteEnable
*	����˵��: ����������дʹ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Fm25v_WriteEnable(void)
{
	Fm25v_SetCS(0);						/* ʹ��Ƭѡ */
	Spi_SendByte(Fm25v_WREN);			/* �������� */
	Fm25v_SetCS(1);					    /* ����Ƭѡ */
}
static void Fm25v_WriteDisable(void)
{
	Fm25v_SetCS(0);						/* ʹ��Ƭѡ */
	Spi_SendByte(Fm25v_WRDI);			/* �������� */
	Fm25v_SetCS(1);						/* ����Ƭѡ */
}

static void Fm25v_ReadStatus(uint8_t * _pBuf)
{
    Fm25v_SetCS(0);					/* ʹ��Ƭѡ */
    Spi_SendByte(Fm25v_RDSR);		/* ���������״̬�Ĵ��� */
    *_pBuf = Spi_SendByte(DUMMY_BYTE);
    Fm25v_SetCS(1);					/* ����Ƭѡ */
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_WriteStatus
*	����˵��: д״̬�Ĵ���
*	��    ��:  _ucValue : ״̬�Ĵ�����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Fm25v_WriteStatus(uint8_t _ucValue)
{
    Fm25v_SetCS(0);					/* ʹ��Ƭѡ */
    Spi_SendByte(Fm25v_WRSR);		/* �������д״̬�Ĵ��� */
    Spi_SendByte(_ucValue);		    /* �������ݣ�״̬�Ĵ�����ֵ */
    Fm25v_SetCS(1);					/* ����Ƭѡ */
}

void Fm25v_Sleep(void)
{
    Fm25v_SetCS(0);					/* ʹ��Ƭѡ */
    Spi_SendByte(Fm25v_SLEEP);		/* �������д״̬�Ĵ��� */
    Fm25v_SetCS(1);					/* ����Ƭѡ */
}

uint8_t Fm25v_Read (uint8_t * _pBuf, uint32_t _uiReadAddr)
{
	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if(_uiReadAddr > fm25v.TotalSize)
	{
		return 0;
	}

	Fm25v_SetCS(0);						        /* ʹ��Ƭѡ */
	Spi_SendByte(Fm25v_READ);			        /* Fast Read Operation */
	Spi_SendByte((_uiReadAddr&0xFF0000) >> 16); /* ����������ַ�ĸ�8bit */
	Spi_SendByte((_uiReadAddr&0xFF00) >> 8);	/* ����������ַ�м�8bit */
	Spi_SendByte(_uiReadAddr&0xFF);			    /* ����������ַ��8bit */
	*_pBuf = Spi_SendByte(DUMMY_BYTE);			/* ��һ���ֽڲ��洢��pBuf�������ָ���Լ�1 */
	Fm25v_SetCS(1);								/* ����Ƭѡ */
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_Write
*	����˵��:
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWriteAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Fm25v_Write(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i;
	if((_usSize == 0) ||(_uiWriteAddr + _usSize) > fm25v.TotalSize)
	{
		return 0;
	}
	Fm25v_WriteEnable();				/* ����дʹ������ */
	
	Fm25v_SetCS(0);						/* ʹ��Ƭѡ */
	Spi_SendByte(Fm25v_WRITE);			/* Write Operation */
	Spi_SendByte((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	Spi_SendByte((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	Spi_SendByte(_uiWriteAddr & 0xFF);				/* ����������ַ��8bit */

	for( i = 0; i < _usSize; i++)
	{
		Spi_SendByte(*_pBuf++);			/* �������� */
	}
	Fm25v_SetCS(1);						/* ��ֹƬѡ */
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_FastRead
*	����˵��: ���ٶ� ȡ�����ֽڡ��ֽڸ������ܳ���оƬ������
*	��    ��: _pBuf : ����Դ��������
*						_uiReadAddr ���׵�ַ
*						_usSize �����ݸ���,���ܳ���оƬ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Fm25v_FastRead(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if((_uiSize == 0) ||(_uiReadAddr + _uiSize) > fm25v.TotalSize)
	{
		return 0;
	}

	Fm25v_SetCS(0);									/* ʹ��Ƭѡ */
	Spi_SendByte(Fm25v_FSTRD);			/* Fast Read Operation */
	Spi_SendByte((_uiReadAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	Spi_SendByte((_uiReadAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	Spi_SendByte(_uiReadAddr & 0xFF);				/* ����������ַ��8bit */
	Spi_SendByte(DUMMY_BYTE);
	while (_uiSize--)
	{
		*_pBuf++ = Spi_SendByte(DUMMY_BYTE);			/* ��һ���ֽڲ��洢��pBuf�������ָ���Լ�1 */
	}
	Fm25v_SetCS(1);									/* ����Ƭѡ */
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_ReadID
*	����˵��: ��ȡ����ID
*	��    ��: ��
*	�� �� ֵ:
*********************************************************************************************************
*/
uint8_t Fm25v_ReadID(void)
{
	uint8_t i;

	Fm25v_SetCS(0);							/* ʹ��Ƭѡ */
	Spi_SendByte(Fm25v_RDID);				/* ���Ͷ�ID���� */
	for(i=0;i<=8;i++)
    {
		fm25v.ChipID[i] = Spi_SendByte(DUMMY_BYTE);		/* ��ID�ĵ�i���ֽ� */
    }
	Fm25v_SetCS(1);							/* ����Ƭѡ */
    
    //ID read finish, then check if it is ok;
    for(i=0;i<9;i++)
    {
        if(fm25v.ChipID[i] != Fm25v_ID[i]) return 0;
    }
    return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: Fm25v_ReadInfo
*	����˵��: ��ȡ����ID,�������������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Fm25v_ReadInfo(void)
{
	/* �Զ�ʶ����Flash�ͺ� */
    #if DEBUG
	printf("ChipID = %x%x%x%x%x%x%x%x%x ",
        fm25v.ChipID[0],
        fm25v.ChipID[1],
        fm25v.ChipID[2],
        fm25v.ChipID[3],
        fm25v.ChipID[4],
        fm25v.ChipID[5],
        fm25v.ChipID[6],
        fm25v.ChipID[7],
        fm25v.ChipID[8]
        );
    #endif
}

