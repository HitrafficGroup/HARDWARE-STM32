#include "bsp.h"
#include "oled_font.h" 

#define OLED_CMD    0	//д����
#define OLED_DATA   1	//д����

#define SIZE 		16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	  

/*
SPI2 ����OLED��ʾ��,ʵ�ʳ���ʹ��ģ��SPI
	OLED CS			PE15  Ƭѡ
	OLED RES		PB12 ��λ
	OLED SCK		PB13 
	OLED DC 		PB14 ����&���� ����
	OLED DIN		PB15 ����
*/
#define OLED_CS_PIN		GPIO_Pin_15

#define OLED_RES_PIN	GPIO_Pin_12
#define OLED_SCK_PIN	GPIO_Pin_13
#define OLED_DIN_PIN	GPIO_Pin_15
#define OLED_DC_PIN		GPIO_Pin_14


#define OLED_CS_Clr()	GPIOE->BRR = OLED_CS_PIN
#define OLED_CS_Set()	GPIOE->BSRR = OLED_CS_PIN

#define OLED_RES_Clr()	GPIOB->BRR =  OLED_RES_PIN
#define OLED_RES_Set()	GPIOB->BSRR = OLED_RES_PIN

#define OLED_DC_Clr()	GPIOB->BRR =  OLED_DC_PIN
#define OLED_DC_Set()	GPIOB->BSRR = OLED_DC_PIN

#define OLED_SCK_Clr()	GPIOB->BRR =  OLED_SCK_PIN
#define OLED_SCK_Set()	GPIOB->BSRR = OLED_SCK_PIN

#define OLED_DIN_Clr()	GPIOB->BRR =  OLED_DIN_PIN
#define OLED_DIN_Set()	GPIOB->BSRR = OLED_DIN_PIN


/*
*********************************************************************************************************
*	�� �� ��: bsp_Init_OLED_gpio
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init_OLED_gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOB , ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  /* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;	//E15
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void OLED_WriteCmd(uint8_t _ucCmd)
{
	uint8_t i;
	OLED_DC_Clr(); //����
	OLED_CS_Clr();
	for (i = 0; i < 8; i++)
	{
		if (_ucCmd & 0x80)
		{
			OLED_DIN_Set();
		}
		else
		{
			OLED_DIN_Clr();
		}

		OLED_SCK_Clr();
		_ucCmd <<= 1;
		OLED_SCK_Set();
	}
	
	OLED_CS_Set();
}

static void OLED_WriteData(uint8_t _ucData)
{
	uint8_t i;
	OLED_DC_Set(); //����
	OLED_CS_Clr();
	for (i = 0; i < 8; i++)
	{
		if (_ucData & 0x80)
		{
			OLED_DIN_Set();
		}
		else
		{
			OLED_DIN_Clr();
		}

		OLED_SCK_Clr();
		_ucData <<= 1;
		OLED_SCK_Set();
	}
	
	OLED_CS_Set();
}

void OLED_WR_Byte(uint8_t _ucData, uint8_t cmd)
{
	uint8_t i;	
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();
	
	OLED_CS_Clr();

	for (i = 0; i < 8; i++)
	{
		if (_ucData & 0x80)
		{
			OLED_DIN_Set();
		}
		else
		{
			OLED_DIN_Clr();
		}
		OLED_SCK_Clr();
		_ucData <<= 1;
		OLED_SCK_Set();
	}
	
	OLED_CS_Set();
	OLED_DC_Set();   	  
}
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
	OLED_WriteCmd(0xb0+y);
	OLED_WriteCmd(((x&0xf0)>>4)|0x10);
	OLED_WriteCmd((x&0x0f)|0x01); 
}
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WriteCmd(0X8D);  //SET DCDC����
	OLED_WriteCmd(0X14);  //DCDC ON
	OLED_WriteCmd(0XAF);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WriteCmd(0X8D);  //SET DCDC����
	OLED_WriteCmd(0X10);  //DCDC OFF
	OLED_WriteCmd(0XAE);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WriteCmd (0xb0+i);    //����ҳ��ַ��0~7��
		OLED_WriteCmd (0x00);      //������ʾλ�á��е͵�ַ
		OLED_WriteCmd (0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WriteData(0); 
	}
}

void oled_Init(void)
{
	OLED_RES_Set();
    bsp_DelayMS(100);
	OLED_RES_Clr();
    bsp_DelayMS(200);
	OLED_RES_Set();
	
	OLED_WriteCmd(0xAE);//--turn off oled panel
	OLED_WriteCmd(0x00);//---set low column address
	OLED_WriteCmd(0x10);//---set high column address
	OLED_WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WriteCmd(0x81);//--set contrast control register
	OLED_WriteCmd(0xCF); // Set SEG Output Current Brightness
	OLED_WriteCmd(0xA1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WriteCmd(0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WriteCmd(0xA6);//--set normal display
	OLED_WriteCmd(0xA8);//--set multiplex ratio(1 to 64)
	OLED_WriteCmd(0x3f);//--1/64 duty
	OLED_WriteCmd(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WriteCmd(0x00);//-not offset
	OLED_WriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WriteCmd(0xD9);//--set pre-charge period
	OLED_WriteCmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WriteCmd(0xDA);//--set com pins hardware configuration
	OLED_WriteCmd(0x12);
	OLED_WriteCmd(0xDB);//--set vcomh
	OLED_WriteCmd(0x40);//Set VCOM Deselect Level
	OLED_WriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WriteCmd(0x02);//
	OLED_WriteCmd(0x8D);//--set Charge Pump enable/disable
	OLED_WriteCmd(0x14);//--set(0x10) disable
	OLED_WriteCmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WriteCmd(0xA6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WriteCmd(0xAF);//--turn on oled panel
	
	OLED_WriteCmd(0xAF); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
	uint8_t c=0, i=0;	
    c = chr - ' ';//�õ�ƫ�ƺ��ֵ 
    if(x > Max_Column-1){x=0;y=y+2;}
    if(SIZE ==16)
    {
        OLED_Set_Pos(x,y);	
        for(i=0;i<8;i++)
        OLED_WriteData(F8X16[c*16+i]);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++)
        OLED_WriteData(F8X16[c*16+i+8]);
    }
    else 
    {	
        OLED_Set_Pos(x,y+1);
        for(i=0;i<6;i++)
        OLED_WriteData(F6x8[c][i]);
    }
}

//��ʾһ���ַ��Ŵ�
void OLED_ShowString(uint8_t x, uint8_t y, char *chr)
{
	uint8_t j=0;
	while (chr[j]!='\0')
	{
        OLED_ShowChar(x,y,chr[j]);
		x+=8;
		if(x>120){x=0;y+=2;}
		j++;
	}
}
