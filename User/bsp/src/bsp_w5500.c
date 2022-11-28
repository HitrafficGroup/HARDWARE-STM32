/**********************************************************************************
 * �ļ���  ��W5500.c
 * ����    ��W5500 ����������         
 * ��汾  ��ST_v3.5
**********************************************************************************/

#include "bsp.h"		
#include "bsp_w5500.h"	



/***************----- W5500 GPIO���� -----***************/
#define W5500_SCS_PORT	GPIOA
#define W5500_SCS		GPIO_Pin_8	//����W5500��CS����	 
#define W5500_SCS_Clr()	W5500_SCS_PORT->BRR = W5500_SCS
#define W5500_SCS_Set()	W5500_SCS_PORT->BSRR = W5500_SCS


#define W5500_RST_PORT	GPIOC
#define W5500_RST		GPIO_Pin_8	//����W5500��RST����
#define W5500_RST_Clr()	W5500_RST_PORT->BRR = W5500_RST
#define W5500_RST_Set()	W5500_RST_PORT->BSRR = W5500_RST


#define W5500_INT		GPIO_Pin_9	//����W5500��INT����
#define W5500_INT_PORT	GPIOC


/***************----- ��������������� -----***************/
NET             Net;
SOCKET_TYPE     Socket[8];

/***************----- �˿����ݻ����� -----***************/
unsigned char Rx_Buffer[2048];	//�˿ڽ������ݻ����� 
unsigned char Tx_Buffer[2048];	//�˿ڷ������ݻ����� 

unsigned char W5500_Interrupt;	//W5500�жϱ�־(0:���ж�,1:���ж�)




/*******************************************************************************
* ������  : W5500_GPIO_Configuration
* ����    : W5500 GPIO��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	
	/* W5500_RST���ų�ʼ������(PC8) */
	GPIO_InitStructure.GPIO_Pin  = W5500_RST;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
	W5500_RST_Clr();//��λ��������
	
	/* W5500_INT���ų�ʼ������(PC9) */
	GPIO_InitStructure.GPIO_Pin  = W5500_INT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);
    
    /* ��ʼ��CS����(PA8) */
	GPIO_InitStructure.GPIO_Pin = W5500_SCS;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);
    W5500_SCS_Set();
}

/*******************************************************************************
* ������  : Spi_Send_Short
* ����    : SPI1����2���ֽ�����(16λ)
* ����    : dat:�����͵�16λ����
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Spi_Send_Short(unsigned short dat)
{
	Spi_SendByte(dat>>8);//д���ݸ�λ
	Spi_SendByte(dat);	//д���ݵ�λ
}

/*******************************************************************************
* ������  : Write_W5500_1Byte
* ����    : ͨ��SPI��ָ����ַ�Ĵ���д1���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ,dat:��д�������
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_1Byte(unsigned short reg, unsigned char dat)
{
	W5500_SCS_Clr();

	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM1|RWB_WRITE|COMMON_R);//ͨ��SPIд�����ֽ�,1���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
	Spi_SendByte(dat);//д1���ֽ�����

	W5500_SCS_Set();
}

/*******************************************************************************
* ������  : Write_W5500_2Byte
* ����    : ͨ��SPI��ָ����ַ�Ĵ���д2���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ,dat:16λ��д�������(2���ֽ�)
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_2Byte(unsigned short reg, unsigned short dat)
{
	W5500_SCS_Clr();
		
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM2|RWB_WRITE|COMMON_R);//ͨ��SPIд�����ֽ�,2���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
	Spi_Send_Short(dat);//д16λ����

	W5500_SCS_Set();
}

/*******************************************************************************
* ������  : Write_W5500_nByte
* ����    : ͨ��SPI��ָ����ַ�Ĵ���дn���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ,*dat_ptr:��д�����ݻ�����ָ��,size:��д������ݳ���
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short i;

	W5500_SCS_Clr();
		
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(VDM|RWB_WRITE|COMMON_R);//ͨ��SPIд�����ֽ�,N���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���

	for(i=0;i<size;i++)//ѭ������������size���ֽ�����д��W5500
	{
		Spi_SendByte(*dat_ptr++);//дһ���ֽ�����
	}

    W5500_SCS_Set();
}

/*******************************************************************************
* ������  : Write_W5500_SOCK_1Byte
* ����    : ͨ��SPI1��ָ���˿ڼĴ���д1���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ,dat:��д�������
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat)
{
	W5500_SCS_Clr();
		
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM1|RWB_WRITE|(s*0x20+0x08));//ͨ��SPIд�����ֽ�,1���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���
	Spi_SendByte(dat);//д1���ֽ�����

    W5500_SCS_Set();
}

/*******************************************************************************
* ������  : Write_W5500_SOCK_2Byte
* ����    : ͨ��SPI��ָ���˿ڼĴ���д2���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ,dat:16λ��д�������(2���ֽ�)
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat)
{
	W5500_SCS_Clr();
			
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM2|RWB_WRITE|(s*0x20+0x08));//ͨ��SPIд�����ֽ�,2���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���
	Spi_Send_Short(dat);//д16λ����

	W5500_SCS_Set();
}

/*******************************************************************************
* ������  : Write_W5500_SOCK_4Byte
* ����    : ͨ��SPI��ָ���˿ڼĴ���д4���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ,*dat_ptr:��д���4���ֽڻ�����ָ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr)
{
	W5500_SCS_Clr();//��W5500��SCSΪ�͵�ƽ
    
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM4|RWB_WRITE|(s*0x20+0x08));//ͨ��SPIд�����ֽ�,4���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

	Spi_SendByte(*dat_ptr++);//д��1���ֽ�����
	Spi_SendByte(*dat_ptr++);//д��2���ֽ�����
	Spi_SendByte(*dat_ptr++);//д��3���ֽ�����
	Spi_SendByte(*dat_ptr++);//д��4���ֽ�����

	W5500_SCS_Set();//��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Read_W5500_1Byte
* ����    : ��W5500ָ����ַ�Ĵ�����1���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ��ȡ���Ĵ�����1���ֽ�����
* ˵��    : ��
*******************************************************************************/
unsigned char Read_W5500_1Byte(unsigned short reg)
{
	unsigned char temp;

	W5500_SCS_Clr();//��W5500��SCSΪ�͵�ƽ
			
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM1 | RWB_READ | COMMON_R);//ͨ��SPIд�����ֽ�,1���ֽ����ݳ���,������,ѡ��ͨ�üĴ���
	temp = Spi_SendByte(0x00);//����һ��������,��ȡ����

	W5500_SCS_Set();//��W5500��SCSΪ�ߵ�ƽ
	return temp;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_W5500_SOCK_1Byte
* ����    : ��W5500ָ���˿ڼĴ�����1���ֽ�����
* ����    : s:�˿ں� 0-7, reg:16λ�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ��ȡ���Ĵ�����1���ֽ�����
* ˵��    : ��
*******************************************************************************/
unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg)
{
	unsigned char temp;

	W5500_SCS_Clr();//��W5500��SCSΪ�͵�ƽ
			
	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM1 | RWB_READ | (s*0x20+0x08));//ͨ��SPIд�����ֽ�,1���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
	temp = Spi_SendByte(0x00);//����һ��������

	W5500_SCS_Set();//��W5500��SCSΪ�ߵ�ƽ
	return temp;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_W5500_SOCK_2Byte
* ����    : ��W5500ָ���˿ڼĴ�����2���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ��ȡ���Ĵ�����2���ֽ�����(16λ)
* ˵��    : ��
*******************************************************************************/
unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg)
{
	unsigned short temp;

	W5500_SCS_Clr();//��W5500��SCSΪ�͵�ƽ

	Spi_Send_Short(reg);//ͨ��SPIд16λ�Ĵ�����ַ
	Spi_SendByte(FDM2|RWB_READ|(s*0x20+0x08));//ͨ��SPIд�����ֽ�,2���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���

	temp = Spi_SendByte(0x00);//����һ��������
	temp <<= 8;
	temp |= Spi_SendByte(0x00);//��ȡ��λ����

	W5500_SCS_Set();//��W5500��SCSΪ�ߵ�ƽ
	return temp;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_SOCK_Data_Buffer
* ����    : ��W5500�������ݻ������ж�ȡ����
* ����    : s:�˿ں�,*dat_ptr:���ݱ��滺����ָ��
* ���    : ��
* ����ֵ  : ��ȡ�������ݳ���,rx_size���ֽ�
* ˵��    : ��
*******************************************************************************/
unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr)
{
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;

	rx_size = Read_W5500_SOCK_2Byte(s,Sn_RX_RSR);
	if(rx_size == 0) return 0;//û���յ������򷵻�
	if(rx_size > 1460) rx_size = 1460;

	offset = Read_W5500_SOCK_2Byte(s,Sn_RX_RD);
	offset1 = offset;
	offset &= (S_RX_SIZE-1);//����ʵ�ʵ������ַ

    W5500_SCS_Clr();//��W5500��SCSΪ�͵�ƽ

	Spi_Send_Short(offset);//д16λ��ַ
	Spi_SendByte(VDM|RWB_READ|(s*0x20+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
	
	if((offset + rx_size) < S_RX_SIZE)//�������ַδ����W5500���ջ������Ĵ���������ַ
	{
		for(i = 0; i < rx_size; i++)//ѭ����ȡrx_size���ֽ�����
		{
			*dat_ptr = Spi_SendByte(0x00);//����һ��������
			dat_ptr++;//���ݱ��滺����ָ���ַ����1
		}
	}
	else//�������ַ����W5500���ջ������Ĵ���������ַ
	{
		offset = S_RX_SIZE - offset;
		for(i = 0; i < offset; i++)//ѭ����ȡ��ǰoffset���ֽ�����
		{
			*dat_ptr = Spi_SendByte(0x00);//����һ��������
			dat_ptr++;//���ݱ��滺����ָ���ַ����1
		}
		W5500_SCS_Set(); //��W5500��SCSΪ�ߵ�ƽ

		W5500_SCS_Clr();//��W5500��SCSΪ�͵�ƽ

		Spi_Send_Short(0x00);//д16λ��ַ
		Spi_SendByte(VDM|RWB_READ|(s*0x20+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���

		for( ; i < rx_size; i++)//ѭ����ȡ��rx_size-offset���ֽ�����
		{
			*dat_ptr = Spi_SendByte(0x00);//����һ��������
			dat_ptr++;//���ݱ��滺����ָ���ַ����1
		}
	}
	W5500_SCS_Set(); //��W5500��SCSΪ�ߵ�ƽ

	offset1 += rx_size;//����ʵ�������ַ,���´ζ�ȡ���յ������ݵ���ʼ��ַ
	Write_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, RECV);//����������������
	return rx_size;//���ؽ��յ����ݵĳ���
}

/*******************************************************************************
* ������  : Write_SOCK_Data_Buffer
* ����    : ������д��W5500�����ݷ��ͻ�����
* ����    : s:�˿ں�,*dat_ptr:���ݱ��滺����ָ��,size:��д�����ݵĳ���
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short offset,offset1;
	unsigned short i;

	//�����ڴ�����Ŀ��������IP�Ͷ˿ں�
	if((Read_W5500_SOCK_1Byte(s, Sn_MR)&0x0f) != SOCK_UDP)//���Socket��ʧ��
	{
		Write_W5500_SOCK_4Byte(s, Sn_DIPR, Socket[s].UdpDIPR);//����Ŀ������IP  		
		Write_W5500_SOCK_2Byte(s, Sn_DPORTR, Socket[s].UdpDestPort);//����Ŀ�������˿ں�				
	}

	offset  = Read_W5500_SOCK_2Byte(s,Sn_TX_WR);
	offset1 = offset;
	offset &= (S_TX_SIZE-1);//����ʵ�ʵ������ַ

	W5500_SCS_Clr();

	Spi_Send_Short(offset);//д16λ��ַ
	Spi_SendByte(VDM|RWB_WRITE|(s*0x20+0x10));//д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

	if((offset + size) < S_TX_SIZE)//�������ַδ����W5500���ͻ������Ĵ���������ַ
	{
		for(i = 0; i < size; i++)//ѭ��д��size���ֽ�����
		{
			Spi_SendByte(*dat_ptr++);//д��һ���ֽڵ�����		
		}
	}
	else//�������ַ����W5500���ͻ������Ĵ���������ַ
	{
		offset = S_TX_SIZE - offset;
		for(i=0;i<offset;i++)//ѭ��д��ǰoffset���ֽ�����
		{
			Spi_SendByte(*dat_ptr++);//д��һ���ֽڵ�����
		}
		W5500_SCS_Set();

		W5500_SCS_Clr();

		Spi_Send_Short(0x00);//д16λ��ַ
		Spi_SendByte(VDM|RWB_WRITE|(s*0x20+0x10));//д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

		for(;i<size;i++)//ѭ��д��size-offset���ֽ�����
		{
			Spi_SendByte(*dat_ptr++);//д��һ���ֽڵ�����
		}
	}
	W5500_SCS_Set();

	offset1+=size;//����ʵ�������ַ,���´�д���������ݵ��������ݻ���������ʼ��ַ
	Write_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, SEND);//����������������
}

/*******************************************************************************
* ������  : W5500_Hardware_Reset
* ����    : Ӳ����λW5500
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : W5500�ĸ�λ���ű��ֵ͵�ƽ����500us����,���ܸ�λW5500
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	W5500_RST_Clr();//��λ��������
	bsp_DelayMS(50);
	W5500_RST_Set();//��λ��������
	bsp_DelayMS(200);
    
	//while((Read_W5500_1Byte(PHYCFGR) & LINK)==0);//�ȴ���̫���������
}

/*******************************************************************************
* ������  : W5500_Init
* ����    : ��ʼ��W5500�Ĵ�������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ʹ��W5500֮ǰ���ȶ�W5500��ʼ��
*******************************************************************************/
void W5500_Init(void)
{
	u8 i=0;
    
	Write_W5500_1Byte(MR, RST);//�����λW5500,��1��Ч,��λ���Զ���0
	bsp_DelayMS(10);//��ʱ10ms,�Լ�����ú���
    
	//��������(Gateway)��IP��ַ,Gateway_IPΪ4�ֽ�unsigned char����,�Լ����� 
	//ʹ�����ؿ���ʹͨ��ͻ�������ľ��ޣ�ͨ�����ؿ��Է��ʵ��������������Internet
	Write_W5500_nByte(GAR, Net.Gateway_IP, 4);
    
	//������������(MASK)ֵ,SUB_MASKΪ4�ֽ�unsigned char����,�Լ�����
	//��������������������
	Write_W5500_nByte(SUBR,Net.Sub_Mask,4);		
	
	//���������ַ,PHY_ADDRΪ6�ֽ�unsigned char����,�Լ�����,����Ψһ��ʶ�����豸�������ֵַ
	//�õ�ֵַ��Ҫ��IEEE���룬����OUI�Ĺ涨��ǰ3���ֽ�Ϊ���̴��룬�������ֽ�Ϊ��Ʒ���
	//����Լ����������ַ��ע���һ���ֽڱ���Ϊż��
	Write_W5500_nByte(SHAR,Net.Phy_Addr,6);		

	//���ñ�����IP��ַ,IP_ADDRΪ4�ֽ�unsigned char����,�Լ�����
	//ע�⣬����IP�����뱾��IP����ͬһ�����������򱾻����޷��ҵ�����
	Write_W5500_nByte(SIPR,Net.IP_Addr,4);		
	
	//���÷��ͻ������ͽ��ջ������Ĵ�С���ο�W5500�����ֲ�
	for(i=0;i<8;i++)
	{
		Write_W5500_SOCK_1Byte(i, Sn_RXBUF_SIZE, 0x02);//Socket Rx memory size=2k
		Write_W5500_SOCK_1Byte(i, Sn_TXBUF_SIZE, 0x02);//Socket Tx mempry size=2k
	}

	//��������ʱ�䣬Ĭ��Ϊ2000(200ms) 
	//ÿһ��λ��ֵΪ100΢��,��ʼ��ʱֵ��Ϊ2000(0x07D0),����200����
	Write_W5500_2Byte(REG_RTR, 0x07d0);

	//�������Դ�����Ĭ��Ϊ8�� 
	//����ط��Ĵ��������趨ֵ,�������ʱ�ж�(��صĶ˿��жϼĴ����е�Sn_IR ��ʱλ(TIMEOUT)�á�1��)
	Write_W5500_1Byte(RCR,8);
}

/*******************************************************************************
* ������  : Detect_Gateway
* ����    : ������ط�����
* ����    : ��
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ��
*******************************************************************************/
unsigned char Detect_Gateway(SOCKET s)
{
	unsigned char ip_adde[4];
	ip_adde[0] = Net.IP_Addr[0]+1;
	ip_adde[1] = Net.IP_Addr[1]+1;
	ip_adde[2] = Net.IP_Addr[2]+1;
	ip_adde[3] = Net.IP_Addr[3]+1;

	//������ؼ���ȡ���ص������ַ
	Write_W5500_SOCK_4Byte(s,Sn_DIPR,ip_adde);  //��Ŀ�ĵ�ַ�Ĵ���д���뱾��IP��ͬ��IPֵ
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);     //����socketΪTCPģʽ
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);       //��Socket	
	bsp_DelayMS(5);//��ʱ5ms 	
	
	if(Read_W5500_SOCK_1Byte(s,Sn_SR) != SOCK_INIT)//���socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}

	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);//����SocketΪConnectģʽ						

	do
	{
		u8 j=0;
		j=Read_W5500_SOCK_1Byte(s,Sn_IR);//��ȡSocket0�жϱ�־�Ĵ���
		if(j!=0)
		Write_W5500_SOCK_1Byte(s,Sn_IR,j);
		bsp_DelayMS(5);//��ʱ5ms 
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			return FALSE;	
		}
		else if(Read_W5500_SOCK_1Byte(s,Sn_DHAR) != 0xff)
		{
			Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�ر�Socket
			return TRUE;							
		}
	}while(1);
}

/*******************************************************************************
* ������  : Socket_Init
* ����    : ָ��Socket(0~7)��ʼ��
* ����    : s:����ʼ���Ķ˿�
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Socket_Init(SOCKET s)
{    
    //���÷�Ƭ���ȣ��ο�W5500�����ֲᣬ��ֵ���Բ��޸�	
    Write_W5500_SOCK_2Byte(Socket[s].Num, Sn_MSSR, 0x05b4);//����Ƭ�ֽ���=1460(0x5b4)
    //���ö˿�0�Ķ˿ں�
    Write_W5500_SOCK_2Byte(Socket[s].Num, Sn_PORT, Socket[s].LocalPort);
    
    if(Socket->Mode == TCP_SERVER)
    {
        //���ö˿�Ŀ��(Զ��)�˿ں�
        Write_W5500_SOCK_2Byte(Socket[s].Num, Sn_DPORTR, Socket[s].DestPort);
        //���ö˿�Ŀ��(Զ��)IP��ַ
        Write_W5500_SOCK_4Byte(Socket[s].Num, Sn_DIPR, Socket[s].DestIP);	
    }
}

/*******************************************************************************
* ������  : Socket_Connect
* ����    : ����ָ��Socket(0~7)Ϊ�ͻ�����Զ�̷���������
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ������Socket�����ڿͻ���ģʽʱ,���øó���,��Զ�̷�������������
*			����������Ӻ���ֳ�ʱ�жϣ��������������ʧ��,��Ҫ���µ��øó�������
*			�ó���ÿ����һ��,�������������һ������
*******************************************************************************/
unsigned char Socket_Connect(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);//����socketΪTCPģʽ
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//��Socket
	bsp_DelayMS(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)//���socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);//����SocketΪConnectģʽ
	return TRUE;//����TRUE,���óɹ�
}

/*******************************************************************************
* ������  : Socket_Listen
* ����    : ����ָ��Socket(0~7)��Ϊ�������ȴ�Զ������������
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ������Socket�����ڷ�����ģʽʱ,���øó���,�ȵ�Զ������������
*			�ó���ֻ����һ��,��ʹW5500����Ϊ������ģʽ
*******************************************************************************/
unsigned char Socket_Listen(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);//����socketΪTCPģʽ 
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//��Socket	
	bsp_DelayMS(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)//���socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}	
	Write_W5500_SOCK_1Byte(s,Sn_CR,LISTEN);//����SocketΪ����ģʽ	
	bsp_DelayMS(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_LISTEN)//���socket����ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//���ò��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}

	return TRUE;

	//���������Socket�Ĵ򿪺�������������,����Զ�̿ͻ����Ƿ�������������,����Ҫ�ȴ�Socket�жϣ�
	//���ж�Socket�������Ƿ�ɹ����ο�W5500�����ֲ��Socket�ж�״̬
	//�ڷ���������ģʽ����Ҫ����Ŀ��IP��Ŀ�Ķ˿ں�
}

/*******************************************************************************
* ������  : Socket_UDP
* ����    : ����ָ��Socket(0~7)ΪUDPģʽ
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ���Socket������UDPģʽ,���øó���,��UDPģʽ��,Socketͨ�Ų���Ҫ��������
*			�ó���ֻ����һ�Σ���ʹW5500����ΪUDPģʽ
*******************************************************************************/
unsigned char Socket_UDP(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_UDP); //����SocketΪUDPģʽ*/
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);   //��Socket*/
	bsp_DelayMS(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_UDP)//���Socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}
	else
		return TRUE;

	//���������Socket�Ĵ򿪺�UDPģʽ����,������ģʽ��������Ҫ��Զ��������������
	//��ΪSocket����Ҫ��������,�����ڷ�������ǰ����������Ŀ������IP��Ŀ��Socket�Ķ˿ں�
	//���Ŀ������IP��Ŀ��Socket�Ķ˿ں��ǹ̶���,�����й�����û�иı�,��ôҲ��������������
}

/*******************************************************************************
* ������  : W5500_Interrupt_Process
* ����    : W5500�жϴ��������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Interrupt_Process(void)
{
	uint8_t SIR_REG, SnIR_REG, n;
IntDispose:
	SIR_REG = Read_W5500_1Byte(SIR);//��ȡ�˿��жϱ�־�Ĵ���
	if(SIR_REG == 0) return;
	for(n=0; n<8; n++)
	{
		if((SIR_REG & Socket_Int(n)) == Socket_Int(n))//Socket n�¼����� 
		{
			SnIR_REG = Read_W5500_SOCK_1Byte(n,Sn_IR);//��ȡSocket n�жϱ�־�Ĵ���
			Write_W5500_SOCK_1Byte(n,Sn_IR, SnIR_REG);
			if(SnIR_REG & IR_CON)//��TCPģʽ��,Socket n�ɹ����� 
			{
				Socket[n].State |= S_CONN;//��������״̬0x02,�˿�������ӣ�����������������
			}
			if(SnIR_REG & IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
			{
				Write_W5500_SOCK_1Byte(n,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
                Socket_Init(n);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
				Socket[n].State = 0;//��������״̬0x00,�˿�����ʧ��
			}
			if(SnIR_REG & IR_SEND_OK)//Socket0���ݷ������,�����ٴ�����S_tx_process()������������ 
			{
				Socket[n].DataState |= S_TRANSMITOK;//�˿ڷ���һ�����ݰ���� 
			}
			if(SnIR_REG & IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
			{
				Socket[n].DataState |= S_RECEIVE;//�˿ڽ��յ�һ�����ݰ�
			}
			if(SnIR_REG & IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
			{
				Write_W5500_SOCK_1Byte(n,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 			
				Socket[n].State = 0;//��������״̬0x00,�˿�����ʧ��
			}
		}
	}
	if(Read_W5500_1Byte(SIR) != 0) 
		goto IntDispose;
}

/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket.State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(SOCKET s)
{
	if(Socket[s].State == 0)//�˿�0��ʼ������
	{
		if(Socket[s].Mode == TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(s) == TRUE)
				Socket[s].State = S_INIT;
			else
				Socket[s].State = 0;
		}
		else if(Socket[s].Mode == TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(s)==TRUE)
				Socket[s].State = S_INIT;
			else
				Socket[s].State = 0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(s) == TRUE)
				Socket[s].State = S_INIT | S_CONN;
			else
				Socket[s].State = 0;
		}
	}
}

/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Net.IP_Addr[0] = BasicInfo.IPv4.IP[0];       //���ر���IP��ַ
	Net.IP_Addr[1] = BasicInfo.IPv4.IP[1];
	Net.IP_Addr[2] = BasicInfo.IPv4.IP[2];
	Net.IP_Addr[3] = BasicInfo.IPv4.IP[3];
    
	Net.Gateway_IP[0] = BasicInfo.IPv4.GetWay[0];    //�������ز���
	Net.Gateway_IP[1] = BasicInfo.IPv4.GetWay[1];
	Net.Gateway_IP[2] = BasicInfo.IPv4.GetWay[2];
	Net.Gateway_IP[3] = BasicInfo.IPv4.GetWay[3];
    
	Net.Sub_Mask[0] = BasicInfo.IPv4.SubMask[0];      //������������
	Net.Sub_Mask[1] = BasicInfo.IPv4.SubMask[1];
	Net.Sub_Mask[2] = BasicInfo.IPv4.SubMask[2];
	Net.Sub_Mask[3] = BasicInfo.IPv4.SubMask[3];
    
	Net.Phy_Addr[0] = BasicInfo.IPv4.MAC[0];     //���������ַ
	Net.Phy_Addr[1] = BasicInfo.IPv4.MAC[1];
	Net.Phy_Addr[2] = BasicInfo.IPv4.MAC[2];
	Net.Phy_Addr[3] = BasicInfo.IPv4.MAC[3];
	Net.Phy_Addr[4] = BasicInfo.IPv4.MAC[4];
	Net.Phy_Addr[5] = BasicInfo.IPv4.MAC[5]; 
    
    Socket[0].Num = 0;
    Socket[0].Mode = UDP_MODE;	    //���ض˿�0�Ĺ���ģʽ,UDPģʽ
    Socket[0].LocalPort = (BasicInfo.IPv4.Socket[0] << 8) | BasicInfo.IPv4.Socket[1];     //���ض˿�0�Ķ˿ں�161 
    Socket[0].DestIP[0] = BasicInfo.IPv4Remote.RemoteIP[0];
    Socket[0].DestIP[1] = BasicInfo.IPv4Remote.RemoteIP[1];
    Socket[0].DestIP[2] = BasicInfo.IPv4Remote.RemoteIP[2];
    Socket[0].DestIP[3] = BasicInfo.IPv4Remote.RemoteIP[3];
    Socket[0].DestPort  =(BasicInfo.IPv4Remote.RemoteSocket[0] << 8) | BasicInfo.IPv4Remote.RemoteSocket[1];
    
    Socket[1].Num = 1;
    Socket[1].Mode = 3;//TCP_SERVER;             //TCP_SERVER
    Socket[1].LocalPort = 5001;     //���ض˿�1�Ķ˿ں�5001 
    
    //���ض˿�2����Ϊ�ͻ���ģʽ,��Ҫ���÷�������ַ�Ͷ˿�
    Socket[2].Num = 2;
    Socket[2].Mode = 3;//TCP_CLIENT;             //TCP_CLIENT
    Socket[2].LocalPort = 5002;     //���ض˿�2�Ķ˿ں�5002 
    
    Socket[2].DestIP[0] = BasicInfo.IPv4Remote.RemoteIP[0];
    Socket[2].DestIP[1] = BasicInfo.IPv4Remote.RemoteIP[1];
    Socket[2].DestIP[2] = BasicInfo.IPv4Remote.RemoteIP[2];
    Socket[2].DestIP[3] = BasicInfo.IPv4Remote.RemoteIP[3];
    Socket[2].DestPort  =(BasicInfo.IPv4Remote.RemoteSocket[0] << 8) | BasicInfo.IPv4Remote.RemoteSocket[1];
    
    Socket[3].Mode = 3;
    Socket[4].Mode = 3;
    Socket[5].Mode = 3;
    Socket[6].Mode = 3;
    Socket[7].Mode = 3;

    #if DEBUG
    printf("IP = %d.%d.%d.%d\r\n",BasicInfo.IPv4.IP[0],BasicInfo.IPv4.IP[1],BasicInfo.IPv4.IP[2],BasicInfo.IPv4.IP[3]);
    printf("GetWay = %d.%d.%d.%d\r\n",BasicInfo.IPv4.GetWay[0],BasicInfo.IPv4.GetWay[1],BasicInfo.IPv4.GetWay[2],BasicInfo.IPv4.GetWay[3]);
    printf("SubMask = %d.%d.%d.%d\r\n",BasicInfo.IPv4.SubMask[0],BasicInfo.IPv4.SubMask[1],BasicInfo.IPv4.SubMask[2],BasicInfo.IPv4.SubMask[3]);
    printf("LocalPort = %d\r\n",Socket[0].LocalPort);
    
    printf("RemoteIP = %d.%d.%d.%d\r\n",BasicInfo.IPv4Remote.RemoteIP[0],BasicInfo.IPv4Remote.RemoteIP[1],BasicInfo.IPv4Remote.RemoteIP[2],BasicInfo.IPv4Remote.RemoteIP[3]);
    printf("DestPort = %d\r\n",Socket[2].DestPort);    
    #endif
}

/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//��ʼ��W5500�Ĵ�������
	Detect_Gateway(0);	//������ط����� 
	//Detect_Gateway(1);	//������ط����� 
    //Detect_Gateway(2);	//������ط����� 
    
	Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
	//Socket_Init(1);
	//Socket_Init(2);
}


