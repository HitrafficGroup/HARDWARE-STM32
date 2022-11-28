/*
*********************************************************************************************************
*	ģ������ : SPI�ӿ��ļ�����оƬ
*	�ļ����� : bsp_spi_ch376t.c
*	��    �� : V1.0
*	˵    �� : 
*********************************************************************************************************
*/

#include "bsp.h"


uint8_t	ExchgBuf[4];
USB_STATUS Usb_status;

/* CH376T Ƭѡ���� �õ�ѡ�� �ø߲�ѡ�� */
#define Ch376t_CS_PORT		GPIOC
#define Ch376t_CS_PIN		GPIO_Pin_7
#define Ch376t_CS_LOW()     Ch376t_CS_PORT->BRR  = Ch376t_CS_PIN
#define Ch376t_CS_HIGH()    Ch376t_CS_PORT->BSRR = Ch376t_CS_PIN

/* CH376T Int */
#define Ch376t_INT_PORT		GPIOC
#define Ch376t_INT_PIN		GPIO_Pin_6
#define Ch376t_INT          (Ch376t_INT_PORT->IDR & Ch376t_INT_PIN)

/*
*********************************************************************************************************
*	�� �� ��: sf_ConfigGPIO
*	����˵��: ���ô���Flash��ƬѡGPIO�� ����Ϊ�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Ch376t_ConfigGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ʹ��GPIO ʱ�� */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* Ch376t_CS-PC7 ��Ϊ��� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = Ch376t_CS_PIN;
	GPIO_Init(Ch376t_CS_PORT, &GPIO_InitStructure);
	Ch376t_CS_HIGH();	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* Ch376t_INT-PC6 ����Ϊ���� */
	GPIO_InitStructure.GPIO_Pin = Ch376t_INT_PIN;
	GPIO_Init(Ch376t_INT_PORT, &GPIO_InitStructure);
}

void Ch376t_Init(void)
{
	Ch376t_ConfigGPIO();    //��ʼ�� CS INT ����
//    Ch376s_CheckExist();    //CheckExist
//    Ch376s_SetUsbMode(0x06);//����6ΪUSBģʽ 3ΪSD��ģʽ
//    Ch376s_SetSdoInt(0x10); //����SDO�����жϹ���
    
    Usb_status.fox = CheckConnect;
    Usb_status.Cmd = 0;
    Usb_status.Status = 0;
    Usb_status.Count = 0;
    Usb_status.MaxCount = 0; 
    Usb_status.SendCmdFlag = FALSE;
    Usb_status.OperFlag = FALSE;
}

void Ch376s_WrCmd( uint8_t Cmd )
{
	Ch376t_CS_LOW();
	Spi_SendByte(Cmd);
	Ch376t_CS_HIGH();
}

uint8_t Ch376s_CheckExist(void)
{
	uint8_t data = 0xAA;
	Ch376t_CS_LOW();
	Spi_SendByte(CMD_CHECK_EXIST);
	Spi_SendByte(0x65);  //��������������ݰ�λȡ��
	data = Spi_SendByte(0xff);
	Ch376t_CS_HIGH();
    
	if(data == 0x9A) 
    {
        printf("Ch376s_CheckExist ok!\n");
        return 1;
    }
	else 
    {
        printf("Ch376s_CheckExist not ok!\n");
        return 0;
    }
}
uint8_t Ch376s_SetUsbMode(uint8_t Mode)
{
	uint8_t data;
	Ch376t_CS_LOW();
	Spi_SendByte(CMD_SET_USB_MODE);
	Spi_SendByte(Mode);
    for(data=0; data<20;)data++;
	data = Spi_SendByte(0xff);
	Ch376t_CS_HIGH();
    
	if(data == CMD_RET_SUCESS) 
    {
        printf("Ch376s_SetUsbMode ok! \n");
        return 1;
    }
	else 
    {
        printf("Ch376s_SetUsbMode not ok! \n");
        return 0;
    }
}
// Ch376s_SetSdoInt������SDO�жϷ�ʽ
// ������0x10 ��ֹSDO�ж������0x90 Ƭѡ��Чʱ�ж��������
void Ch376s_SetSdoInt(uint8_t Cmd)
{
	Ch376t_CS_LOW();
	Spi_SendByte(CMD_SET_SDO_INT);
	Spi_SendByte(0x16);
    Spi_SendByte(Cmd);
	Ch376t_CS_HIGH();
}

uint8_t Ch376s_SendCmd(uint8_t Cmd)
{
    uint8_t data;
	Ch376t_CS_LOW();
	data = Spi_SendByte(Cmd);
	Ch376t_CS_HIGH();
    return data;
}

uint8_t Ch376s_GetStatus(void)
{
    uint8_t data;
    Ch376t_CS_LOW();
    Spi_SendByte(CMD_GET_STATUS);   //��״̬����
    data = Spi_SendByte(0xff);      //״̬����
    Ch376t_CS_HIGH();
    return data;
}

void Ch376s_StatusProcess(void)     //100MS ִ��һ��
{
    uint8_t Status;
    if(Usb_status.fox == CheckConnect)//���USB����
    {
        if(Usb_status.SendCmdFlag == FALSE)//����û�з���
        {
            Ch376s_SendCmd(CMD_DISK_CONNECT);//��������
            Usb_status.SendCmdFlag = TRUE;
            Usb_status.OperFlag = FALSE;
            Usb_status.Cmd = CMD_DISK_CONNECT;
            Usb_status.Status = USB_INT_SUCCESS;
            Usb_status.Count = 0;
            Usb_status.MaxCount = 100; 
        }
        else //�����Ѿ����ͣ���ô���״ֵ̬
        {
            if(Usb_status.OperFlag == FALSE)//�������ʧ��
            {
                if(Usb_status.Count < Usb_status.MaxCount)
                {
                    Usb_status.Count++;
                    Status = Ch376s_GetStatus();
                    if(Status == Usb_status.Status)//����ֵ��ȷ
                    {
                        printf("Ch376s_Check_Usb_Connect ok! ");
                        Usb_status.OperFlag = TRUE;
                    }
                }
                else //�˴������������Ͳ����������Ƿ��Ѿ�����
                {
                    Usb_status.SendCmdFlag = FALSE;
                    Usb_status.Count = 0; 
                }
            }
            else//��������ɹ�
            {
                Usb_status.fox = DiskMount;
                Usb_status.SendCmdFlag = FALSE;
                Usb_status.OperFlag = FALSE;
                Usb_status.Cmd = CMD_DISK_MOUNT;
                Usb_status.Status = USB_INT_SUCCESS;
                Usb_status.Count = 0;
                Usb_status.MaxCount = 200; 
            }
        }
    }
    else if(Usb_status.fox == DiskMount)
    {
        if(Usb_status.SendCmdFlag == FALSE)//����û�з���
        {
            Ch376s_SendCmd(CMD_DISK_MOUNT);//��������
            Usb_status.SendCmdFlag = TRUE;
            Usb_status.OperFlag = FALSE;
            Usb_status.Cmd = CMD_DISK_MOUNT;
            Usb_status.Status = USB_INT_SUCCESS;
            Usb_status.Count = 0;
            Usb_status.MaxCount = 10; 
        }
        else //�����Ѿ����ͣ���ô���״ֵ̬
        {
            if(Usb_status.OperFlag == FALSE)
            {
                if(Usb_status.Count < Usb_status.MaxCount)
                {
                    Usb_status.Count++;
                    Status = Ch376s_GetStatus();
                    if(Status == Usb_status.Status)//����ֵ��ȷ
                    {
                        printf("Ch376s_DISK_MOUNT ok! ");
                        Usb_status.OperFlag = TRUE;
                    }
                }
                else//�˴������������Ͳ����������Ƿ��Ѿ�����
                {
                    Usb_status.SendCmdFlag = FALSE;
                    Usb_status.Count = 0; 
                }
            }
            else
            {
                Usb_status.fox = SetFileName;
                Usb_status.SendCmdFlag = FALSE;
                Usb_status.OperFlag = FALSE;
                Usb_status.Cmd = CMD_SET_FILE_NAME;
                Usb_status.Status = USB_INT_SUCCESS;
                Usb_status.Count = 0;
                Usb_status.MaxCount = 200; 
            }
        }
    }
    else if(Usb_status.fox == SetFileName)
    {
        if(Usb_status.SendCmdFlag == FALSE)//����û�з���
        {
            Ch376s_SendCmd(CMD_SET_FILE_NAME);//��������
            Usb_status.SendCmdFlag = TRUE;
            Usb_status.OperFlag = FALSE;
            Usb_status.Cmd = CMD_SET_FILE_NAME;
            Usb_status.Status = USB_INT_SUCCESS;
            Usb_status.Count = 0;
            Usb_status.MaxCount = 10; 
        }
    }
}

uint32_t Ch376s_Read32bitDat(void)
{
	ExchgBuf[3] = Spi_SendByte(0xff);
	ExchgBuf[2] = Spi_SendByte(0xff);
	ExchgBuf[1] = Spi_SendByte(0xff);
	ExchgBuf[0] = Spi_SendByte(0xff);
	Ch376t_CS_HIGH();
	return(*(uint32_t*)ExchgBuf);
}

uint8_t	Ch376s_ReadVar8(uint8_t var)
{
	uint8_t	data;
    
    Ch376t_CS_LOW();
	Spi_SendByte(CMD11_READ_VAR8);
	Spi_SendByte(var);
	data = Spi_SendByte(0xff);
	Ch376t_CS_HIGH();
	return(data);
}
void Ch376s_WriteVar8(uint8_t var, uint8_t dat)
{
    Ch376t_CS_LOW();
	Spi_SendByte(CMD20_WRITE_VAR8);
	Spi_SendByte(var);
	Spi_SendByte(dat);
	Ch376t_CS_HIGH();
}

uint32_t Ch376s_ReadVar32(uint8_t var)
{
    Ch376t_CS_LOW();
	Spi_SendByte(CMD14_READ_VAR32);
	Spi_SendByte(var);
	return(Ch376s_Read32bitDat());
}

void CH376WriteVar32(uint8_t var, uint32_t dat)
{
    *(uint32_t*)ExchgBuf = dat;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD50_WRITE_VAR32);
	Spi_SendByte(var);
	Spi_SendByte(ExchgBuf[3]);
	Spi_SendByte(ExchgBuf[2]);
	Spi_SendByte(ExchgBuf[1]);
	Spi_SendByte(ExchgBuf[0]);
	Ch376t_CS_HIGH();
}

uint8_t	CH376ReadBlockI(uint8_t* buf)
{
	uint8_t	s,l;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD_RD_USB_DATA0);
	s = l = Spi_SendByte(0xff);
	if(l)
    {
		do{
			*buf = Spi_SendByte(0xff);
			buf++;
		}while (--l);
	}
	Ch376t_CS_HIGH();
	return(s);
}

uint8_t CH376ReadBlockX(uint8_t* buf)
{
	uint8_t	s,l;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD_RD_USB_DATA0);
	s = l = Spi_SendByte(0xff);
	if(l)
    {
		do{
			*buf = Spi_SendByte(0xff);
			buf++;
		} while (--l);
	}
	Ch376t_CS_HIGH();
	return(s);
}

uint8_t	CH376ReadBlock( uint8_t* buf )
{
	if(*(uint8_t*) & buf) return(CH376ReadBlockX(buf));
	else return( CH376ReadBlockI( buf ) );
}

uint8_t	CH376WriteReqBlockI( uint8_t* buf )
{
	uint8_t	s, l;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD_WR_REQ_DATA);
	s = l = Spi_SendByte(0xff);
	if(l)
    {
		do{
			Spi_SendByte(*buf);
			buf++;
		}while(--l);
	}
	Ch376t_CS_HIGH();
	return( s );
}

uint8_t	CH376WriteReqBlockX( uint8_t* buf )
{
	uint8_t	s,l;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD01_WR_REQ_DATA);
	s = l = Spi_SendByte(0xff);
	if(l){
		do{
			Spi_SendByte(*buf);
			buf++;
		}while(--l);
	}
	Ch376t_CS_HIGH();
	return(s);
}

uint8_t	CH376WriteReqBlock( uint8_t* buf )
{
	if (*(uint8_t*)&buf) return( CH376WriteReqBlockX(buf));
	else return( CH376WriteReqBlockI( buf ) );
}

void CH376WriteHostBlockX(uint8_t* buf, uint8_t len)
{
    Ch376t_CS_LOW();
	Spi_SendByte(CMD10_WR_HOST_DATA);
	Spi_SendByte(len);
	if(len){
		do{
			Spi_SendByte(*buf);
			buf++;
		}while (--len);
	}
	Ch376t_CS_HIGH();
}

void CH376WriteHostBlock(uint8_t* buf, uint8_t len)
{
    Ch376t_CS_LOW();
	Spi_SendByte(CMD10_WR_HOST_DATA);
	Spi_SendByte(len);  /* ?? */
	if(len)
    {
		do{
			Spi_SendByte(*buf);
			buf ++;
		}while(--len);
	}
	Ch376t_CS_HIGH();
}

void CH376WriteOfsBlock(uint8_t* buf, uint8_t ofs, uint8_t len)
{
    Ch376t_CS_LOW();
	Spi_SendByte(CMD20_WR_OFS_DATA);
	Spi_SendByte(ofs);
	Spi_SendByte(len);
	if(len){
		do{
			Spi_SendByte(*buf);
			buf ++;
		}while(--len);
	}
	Ch376t_CS_HIGH();
}

void CH376EndDirInfo(void)
{
	Ch376s_WriteVar8(0x0D,0x00);
}

uint32_t CH376GetFileSize(void)
{
	return(Ch376s_ReadVar32(VAR_FILE_SIZE));
}

uint8_t	CH376GetDiskStatus(void)
{
	return(Ch376s_ReadVar8(VAR_DISK_STATUS));
}
uint8_t	CH376GetIntStatus(void)
{
	uint8_t	s;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD01_GET_STATUS);
	s = Spi_SendByte(0xff);
	Ch376t_CS_HIGH();
	return(s);
}

uint8_t	Query376Interrupt( void )
{
	return( Ch376t_INT? FALSE : TRUE);
}

uint8_t	Wait376Interrupt(void)
{
	uint32_t i,t;
	for(i = 0; i < DEF_INT_TIMEOUT; i++) 
    {
        for(t = 0; t < 200; t++);
		if(Query376Interrupt()) return(Ch376s_GetStatus());
	}
	return(ERR_USB_UNKNOWN);
}

uint8_t	CH376SendCmdWaitInt(uint8_t Cmd)
{
    Ch376t_CS_LOW();
	Spi_SendByte(Cmd);
	Ch376t_CS_HIGH();
	return(Wait376Interrupt());
}
uint8_t	CH376SendCmdDatWaitInt(uint8_t mCmd, uint8_t mDat)
{
    Ch376t_CS_LOW();
	Spi_SendByte(mCmd);
	Spi_SendByte(mDat);
	Ch376t_CS_HIGH();
	return(Wait376Interrupt());
}
uint8_t	CH376DiskReqSense(void)
{
	uint8_t	s;
	bsp_DelayMS(5);
	s = CH376SendCmdWaitInt(CMD0H_DISK_R_SENSE);
	bsp_DelayMS(5);
	return(s);
}

uint8_t	CH376DiskConnect(void)
{
	if(Query376Interrupt()) CH376GetIntStatus();
	return(CH376SendCmdWaitInt(CMD_DISK_CONNECT));
}

uint8_t	CH376DiskMount(void)
{
	return(CH376SendCmdWaitInt(CMD_DISK_MOUNT));
}

uint8_t	CH376FileCreate(uint8_t* name)
{
	if(name)Ch376s_SetFileName(name);
	return(CH376SendCmdWaitInt(CMD0H_FILE_CREATE));
}

uint8_t	CH376DirCreate(uint8_t* name)
{
	Ch376s_SetFileName(name);
	return(CH376SendCmdWaitInt(CMD0H_DIR_CREATE));
}

uint8_t	CH376SeparatePath(uint8_t* path)
{
	uint8_t* pName;
	for(pName = path; *pName != 0; ++ pName);
	while(*pName != DEF_SEPAR_CHAR1 && *pName != DEF_SEPAR_CHAR2 && pName != path) pName--;
	if(pName != path) pName++;
	return(pName - path);
}

void Ch376s_SetFileName(uint8_t* name) /* ���ý�Ҫ�������ļ��� */
{
	uint8_t	c;
    uint8_t	*p;
    p = name;
    printf("Ch376s_SetFileName = %s \n",p);
    
    Ch376t_CS_LOW();
	Spi_SendByte(CMD_SET_FILE_NAME);
	c = *p;
	Spi_SendByte(c);
    printf("%c",c);
	while(c)
    {
		p++;
		c = *p;
		if (c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2) c = 0;
		Spi_SendByte(c);
        printf("%c",c);
	}
	Ch376t_CS_HIGH();
    printf("Ch376s_SetFileName\n");
}
uint8_t	CH376FileOpen(uint8_t* name)
{
	Ch376s_SetFileName(name);
	return(CH376SendCmdWaitInt(CMD_FILE_OPEN));
}
/*
uint8_t	CH376FileOpenDir(uint8_t* PathName, uint8_t StopName)
{
	uint8_t	i,s;
	s = 0;
	i = 1;

    s = CH376FileOpen("/C51\0");
    printf("\n\rOpen /C51 S = %x\n",s);
    if(s != ERR_OPEN_DIR)//0x41 ������ǳɹ�����Ŀ¼��˵�������� 
    {
        if(s == USB_INT_SUCCESS) {printf("USB_INT_SUCCESS");return(ERR_FOUND_NAME);}
        else if(s == ERR_MISS_FILE) { printf("ERR_MISS_FILE");return(ERR_MISS_DIR);}
        else return(s); // �������� 
    }

    s = CH376FileOpen("CH376HFT.C\0");
    printf("\n\rOpen CH376HFT.C S = %x\n",s);
    return(s);
}
*/
uint8_t	CH376FileOpenDir(uint8_t* PathName, uint8_t StopName)
{
	uint8_t	i,s;
	s = 0;
	i = 1;
    while(1)
    {
		while(PathName[i] != DEF_SEPAR_CHAR1 && PathName[i] != DEF_SEPAR_CHAR2 && PathName[i] != 0) ++i;
		if(PathName[i])i++;
		else i = 0;
        printf("CH376FileOpenDir i = %d\n",i);
		s = CH376FileOpen(&PathName[s]);
        printf("CH376FileOpenDir s = %d\n",s);
		if(i && i!=StopName)
        {
			if(s != ERR_OPEN_DIR)//0x41 ������ǳɹ�����Ŀ¼��˵�������� 
            {
				if(s == USB_INT_SUCCESS) return(ERR_FOUND_NAME); // �ɹ����ļ� printf("USB_INT_SUCCESS");
				else if(s == ERR_MISS_FILE) return(ERR_MISS_DIR); // �ļ�����Ŀ¼������ printf("ERR_MISS_FILE");
				else return(s); // �������� printf("�������� %x",s);
			}
			s = i;
		}
		else return(s);
	}
}

//Ŀ¼�������Գ���8���ֽڣ������Ǵ�д��ĸ�����֣������ַ�
uint8_t	CH376FileOpenPath(uint8_t* PathName)
{
	return(CH376FileOpenDir(PathName, 0xFF));
}

uint8_t	CH376FileCreatePath(uint8_t* PathName)
{
	uint8_t	s;
	uint8_t	Name;
	Name = CH376SeparatePath(PathName);
	if(Name)
    {
		s = CH376FileOpenDir(PathName, Name);
		if(s != ERR_OPEN_DIR)
        {
			if(s == USB_INT_SUCCESS) return(ERR_FOUND_NAME);
			else if (s == ERR_MISS_FILE) return(ERR_MISS_DIR);
			else return(s);
		}
	}
	return( CH376FileCreate(&PathName[Name]));
}

uint8_t	CH376DirCreatePath(uint8_t* PathName)
{
	uint8_t	s;
	uint8_t	Name;
	uint8_t	ClustBuf[4];
	Name = CH376SeparatePath(PathName);
	if(Name)
    {
		s = CH376FileOpenDir(PathName, Name);
		if(s != ERR_OPEN_DIR)
        {
			if(s == USB_INT_SUCCESS) return(ERR_FOUND_NAME);
			else if (s == ERR_MISS_FILE) return(ERR_MISS_DIR);
			else return(s);
		}
        Ch376t_CS_LOW();
		Spi_SendByte(CMD14_READ_VAR32);
		Spi_SendByte(VAR_START_CLUSTER);
		for(s = 0; s != 4; s++) ClustBuf[s] = Spi_SendByte(0xff);
		Ch376t_CS_HIGH();
        
		s = CH376DirCreate(&PathName[Name]);
		if (s != USB_INT_SUCCESS) return(s);
		CH376WriteVar32(VAR_FILE_SIZE, sizeof(FAT_DIR_INFO) * 2);
        
		s = CH376ByteLocate( sizeof(FAT_DIR_INFO) + STRUCT_OFFSET( FAT_DIR_INFO, DIR_FstClusHI));
		if (s != USB_INT_SUCCESS ) return(s);
		s = CH376ByteWrite( &ClustBuf[2], 2, NULL);
		if (s != USB_INT_SUCCESS ) return(s);
		s = CH376ByteLocate( sizeof(FAT_DIR_INFO) + STRUCT_OFFSET( FAT_DIR_INFO, DIR_FstClusLO));
		if (s != USB_INT_SUCCESS ) return(s);
		s = CH376ByteWrite( ClustBuf, 2, NULL);
		if (s != USB_INT_SUCCESS ) return(s);
		s = CH376ByteLocate(0);
		if (s != USB_INT_SUCCESS ) return(s);
		CH376WriteVar32(VAR_FILE_SIZE, 0);
		return( s );
	}
	else
    {
		if(PathName[0] == DEF_SEPAR_CHAR1 || PathName[0] == DEF_SEPAR_CHAR2)
            return(CH376DirCreate(PathName));
		else return(ERR_MISS_DIR);
	}
}

uint8_t	CH376FileErase( uint8_t* PathName)
{
	uint8_t	s;
	if(PathName)
    {
		for(s = 1; PathName[s] != DEF_SEPAR_CHAR1 && PathName[s] != DEF_SEPAR_CHAR2 && PathName[s] != 0; ++s);
		if(PathName[s])
        {
			s = CH376FileOpenPath(PathName);
			if(s != USB_INT_SUCCESS && s != ERR_OPEN_DIR) return(s);
		}
		else Ch376s_SetFileName(PathName);
	}
	return( CH376SendCmdWaitInt(CMD0H_FILE_ERASE));
}

uint8_t	CH376FileClose(uint8_t UpdateSz)
{
	return(CH376SendCmdDatWaitInt(CMD_FILE_CLOSE, UpdateSz));
}

uint8_t	CH376DirInfoRead(void)
{
	return(CH376SendCmdDatWaitInt(CMD_DIR_INFO_READ, 0xFF));
}

uint8_t	CH376DirInfoSave(void)
{
	return(CH376SendCmdWaitInt(CMD_DIR_INFO_SAVE));
}

uint8_t	CH376ByteLocate(uint32_t offset)
{
	*(uint32_t*)ExchgBuf = offset;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD_BYTE_LOCATE);
	Spi_SendByte(ExchgBuf[3]);
	Spi_SendByte(ExchgBuf[2]);
	Spi_SendByte(ExchgBuf[1]);
	Spi_SendByte(ExchgBuf[0]);
	Ch376t_CS_HIGH();
	return(Wait376Interrupt());
}

uint8_t	CH376ByteRead(uint8_t* buf, uint16_t ReqCount, uint16_t* RealCount)
{
	uint8_t	s;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD_BYTE_READ);
	Spi_SendByte((uint8_t)ReqCount);
	Spi_SendByte((uint8_t)(ReqCount>>8));
	Ch376t_CS_HIGH();
	if(RealCount) *RealCount = 0;
	while(1)
    {
		s = Wait376Interrupt();
		if(s == USB_INT_DISK_READ)
        {
            //s = CH376ReadBlock(buf);
			if(*(uint8_t*) & buf) s = CH376ReadBlockX(buf);
			else s = CH376ReadBlockI(buf);
            Ch376t_CS_LOW();
			Spi_SendByte(CMD_BYTE_RD_GO);
			Ch376t_CS_HIGH();
			buf += s;
			if(RealCount) *RealCount += s;
		}
        else if(s == USB_INT_SUCCESS) return(s);
		else return(s);  /* ���� */
	}
}

uint8_t	CH376ByteWrite(uint8_t* buf, uint16_t ReqCount, uint16_t* RealCount)
{
	uint8_t	s;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD2H_BYTE_WRITE);
	Spi_SendByte((uint8_t)ReqCount);
	Spi_SendByte((uint8_t)(ReqCount>>8));
	Ch376t_CS_HIGH();
	if(RealCount) *RealCount = 0;
	while(1)
    {
		s = Wait376Interrupt( );
		if(s == USB_INT_DISK_WRITE)
        {
			if(*(uint8_t*)&buf) s = CH376WriteReqBlockX( buf );
			else s = CH376WriteReqBlockI(buf);
            Ch376t_CS_LOW();
			Spi_SendByte(CMD0H_BYTE_WR_GO);
			Ch376t_CS_HIGH();
			buf += s;
			if(RealCount) *RealCount += s;
		}
		else return(s);
	}
}

#ifdef	EN_DISK_QUERY
uint8_t CH376DiskCapacity(uint32_t* DiskCap)
{
	uint8_t	s;
	s = CH376SendCmdWaitInt(CMD_DISK_CAPACITY);
	if(s == USB_INT_SUCCESS)
    {
        Ch376t_CS_LOW();
        Spi_SendByte(CMD_RD_USB_DATA0);
        Spi_SendByte(0xff);
        Ch376t_CS_HIGH();
        *DiskCap = CH376Read32bitDat();
	}
	else *DiskCap = 0;
	return(s);
}

uint8_t	CH376DiskQuery(uint32_t* DiskFre)
{
	uint8_t	s;

	s = CH376SendCmdWaitInt(CMD0H_DISK_QUERY);
	if(s == USB_INT_SUCCESS)
    {
        Ch376t_CS_LOW();
		Spi_SendByte(CMD_RD_USB_DATA0);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		ExchgBuf[3] = Spi_SendByte(0xff);
		ExchgBuf[2] = Spi_SendByte(0xff);
		ExchgBuf[1] = Spi_SendByte(0xff);
		ExchgBuf[0] = Spi_SendByte(0xff);
		*DiskFre = *(uint32_t*)ExchgBuf;
		Spi_SendByte(0xff);
		Ch376t_CS_HIGH();
	}
	else *DiskFre = 0;
	return(s);
}
#endif

uint8_t	CH376SecLocate(uint32_t offset)
{
	*(uint32_t*)ExchgBuf = offset;
    Ch376t_CS_LOW();
	Spi_SendByte(CMD4H_SEC_LOCATE );
	Spi_SendByte(ExchgBuf[3]);
	Spi_SendByte(ExchgBuf[2]);
	Spi_SendByte(ExchgBuf[1]);
	Spi_SendByte(ExchgBuf[0]);
	Ch376t_CS_HIGH();
	return(Wait376Interrupt());
}

#ifdef	EN_SECTOR_ACCESS
uint8_t	CH376DiskReadSec(uint8_t* buf, uint32_t iLbaStart, uint8_t iSectorCount)
{
	uint8_t	s,err;
	uint16_t mBlockCount;
	*(uint32_t)ExchgBuf = iLbaStart;
	for (err = 0; err != 3; ++ err)
    {
        Ch376t_CS_LOW();
		Spi_SendByte(CMD5H_DISK_READ);
		Spi_SendByte(ExchgBuf[3]);
		Spi_SendByte(ExchgBuf[2]);
		Spi_SendByte(ExchgBuf[1]);
		Spi_SendByte(ExchgBuf[0]);
		Spi_SendByte(iSectorCount);
		Ch376t_CS_HIGH();
		for(mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN; mBlockCount != 0; -- mBlockCount)
        {
			s = Wait376Interrupt();
			if(s == USB_INT_DISK_READ)
            {
				s = CH376ReadBlockX(buf);
                Ch376t_CS_LOW();
				Spi_SendByte(CMD0H_DISK_RD_GO);
				Ch376t_CS_HIGH();
				buf += s;
			}
			else break;
		}
		if(mBlockCount == 0)
        {
			s = Wait376Interrupt();
			if(s == USB_INT_SUCCESS)return(USB_INT_SUCCESS);
		}
		if(s == USB_INT_DISCONNECT)return(s);
		CH376DiskReqSense();
	}
	return(s);
}
uint8_t	CH376DiskWriteSec(uint8_t* buf, uint32_t iLbaStart, uint8_t iSectorCount)
{
	uint8_t	 s,err;
	uint16_t mBlockCount;
	*(uint32_t*)ExchgBuf = iLbaStart;
	for(err = 0; err != 3; ++err)
    {
        Ch376t_CS_LOW();
		Spi_SendByte(CMD5H_DISK_WRITE);
		Spi_SendByte(ExchgBuf[3]);
		Spi_SendByte(ExchgBuf[2]);
		Spi_SendByte(ExchgBuf[1]);
		Spi_SendByte(ExchgBuf[0]);
		Spi_SendByte(iSectorCount);
		Ch376t_CS_HIGH();
		for(mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN; mBlockCount != 0; --mBlockCount)
        {
			s = Wait376Interrupt();
			if(s == USB_INT_DISK_WRITE)
            {
				CH376WriteHostBlockX(buf, CH376_DAT_BLOCK_LEN);
                Ch376t_CS_LOW();
				Spi_SendByte(CMD0H_DISK_WR_GO);
				Ch376t_CS_HIGH();
				buf += CH376_DAT_BLOCK_LEN;
			}
			else break;
		}
		if(mBlockCount == 0)
        {
			s = Wait376Interrupt();
			if(s == USB_INT_SUCCESS) return(USB_INT_SUCCESS);
		}
		if(s == USB_INT_DISCONNECT) return(s);
		CH376DiskReqSense();
	}
	return(s);
}
uint8_t	CH376SecRead(uint8_t* buf, uint8_t ReqCount, uint8_t* RealCount)
{
	uint8_t	s;
	uint8_t	cnt;
	uint32_t	StaSec;

	if(RealCount) *RealCount = 0;
	do{
        Ch376t_CS_LOW();
		Spi_SendByte(CMD1H_SEC_READ);
		Spi_SendByte(ReqCount);
		Ch376t_CS_HIGH();
		s = Wait376Interrupt();

		if (s != USB_INT_SUCCESS) return(s);
        Ch376t_CS_LOW();
		Spi_SendByte(CMD01_RD_USB_DATA0);
		Spi_SendByte(0xff);
		cnt = Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		StaSec = CH376Read32bitDat();
		if(cnt == 0) break;
		s = CH376DiskReadSec(buf, StaSec, cnt);
		if (s != USB_INT_SUCCESS) return(s);
		buf += cnt * DEF_SECTOR_SIZE;
		if(RealCount) *RealCount += cnt;
		ReqCount -= cnt;
	}while(ReqCount);
	return(s);
}
uint8_t	CH376SecWrite(uint8_t* buf, uint8_t ReqCount, uint8_t* RealCount)
{
	uint8_t	s;
	uint8_t	cnt;
	uint32_t StaSec;
	if(RealCount) *RealCount = 0;
	do{
        Ch376t_CS_LOW();
		Spi_SendByte(CMD1H_SEC_WRITE);
		Spi_SendByte(ReqCount);
		Ch376t_CS_HIGH( );
		s = Wait376Interrupt( );
		if(s != USB_INT_SUCCESS) return(s);
        Ch376t_CS_LOW();
		Spi_SendByte(CMD01_RD_USB_DATA0);
		Spi_SendByte(0xff);
		cnt = Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		Spi_SendByte(0xff);
		StaSec = CH376Read32bitDat();
		if(cnt == 0) break;
		s = CH376DiskWriteSec(buf, StaSec, cnt);
		if(s != USB_INT_SUCCESS) return(s);
		buf += cnt * DEF_SECTOR_SIZE;
		if(RealCount) *RealCount += cnt;
		ReqCount -= cnt;
	}while(ReqCount);
	return(s);
}
#endif

/*
#ifdef	EN_LONG_NAME
uint8_t	CH376LongNameWrite(uint8_t* buf, uint16_t ReqCount)
{
	uint8_t	s;
#ifndef	DEF_IC_V43_U
	uint8_t	c;
	c = CH376ReadVar8( VAR_DISK_STATUS );
	if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, DEF_DISK_OPEN_DIR );
#endif
    Ch376t_CS_LOW();
	Spi_SendByte(CMD2H_BYTE_WRITE);
	Spi_SendByte((uint8_t)ReqCount);
	Spi_SendByte((uint8_t)(ReqCount>>8));
	Ch376t_CS_HIGH();
	while(1) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE ) {
			if ( buf ) buf += CH376WriteReqBlock( buf ); 
			else {
                Ch376t_CS_LOW();
                Spi_SendByte(CMD01_WR_REQ_DATA);
				s = Spi_SendByte(0xff);
				while (s--) Spi_SendByte(0);
			}
            Ch376t_CS_LOW();
            Spi_SendByte(CMD0H_BYTE_WR_GO);
			Ch376t_CS_HIGH( );
		}
		else {
#ifndef	DEF_IC_V43_U
			if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, c );
#endif
			return( s );
		}
	}
}

uint8_t	CH376CheckNameSum(uint8_t* DirName)
{
	uint8_t	NameLen;
	uint8_t	CheckSum;
	CheckSum = 0;
	for(NameLen = 0; NameLen != 11; NameLen++)
        CheckSum = (CheckSum & 1 ? 0x80 : 0x00) + (CheckSum >> 1) + *DirName++;
	return( CheckSum );
}

uint8_t	CH376LocateInUpDir(uint8_t* PathName)
{
	uint8_t	s;
    Ch376t_CS_LOW();
    Spi_SendByte(CMD14_READ_VAR32);
	Spi_SendByte(VAR_FAT_DIR_LBA);
	for(s = 4; s != 8; s++) GlobalBuf[s] = Spi_SendByte(0xff);
	Ch376t_CS_HIGH( );
	s = CH376SeparatePath( PathName );
	if ( s ) s = CH376FileOpenDir( PathName, s );
	else s = CH376FileOpen( "/" );
	if ( s != ERR_OPEN_DIR ) return( s );
	*(PUINT32)(&GlobalBuf[0]) = 0; 
	while ( 1 ) {
		s = CH376SecLocate( *(PUINT32)(&GlobalBuf[0]) );
		if ( s != USB_INT_SUCCESS ) return( s );
		CH376ReadBlock( &GlobalBuf[8] );
		if ( *(PUINT32)(&GlobalBuf[8]) == *(PUINT32)(&GlobalBuf[4]) ) return( USB_INT_SUCCESS );
        Ch376t_CS_LOW();
        Spi_SendByte(CMD50_WRITE_VAR32);
		Spi_SendByte(VAR_FAT_DIR_LBA);
		for(s = 8; s != 12; s ++) Spi_SendByte(GlobalBuf[s]);
		Ch376t_CS_HIGH( );
		++ *(uint32_t*)(&GlobalBuf[0]);
	}
}

uint8_t	CH376GetLongName( uint8_t* PathName, uint8_t* LongName )
{
	uint8_t	s;
	uint16_t	NameCount;
	s = CH376FileOpenPath( PathName );
	if ( s != USB_INT_SUCCESS && s != ERR_OPEN_DIR ) return( s );
	s = CH376DirInfoRead( ); 
	if ( s != USB_INT_SUCCESS ) return( s );
	CH376ReadBlock( GlobalBuf );
	CH376EndDirInfo( );
	GlobalBuf[32] = CH376CheckNameSum( GlobalBuf );
	GlobalBuf[33] = CH376ReadVar8( VAR_FILE_DIR_INDEX );
	NameCount = 0;
	while ( 1 ) {
		if ( GlobalBuf[33] == 0 ) {  
			s = CH376LocateInUpDir( PathName ); 
			if ( s != USB_INT_SUCCESS ) break;
			if ( CH376ReadVar32( VAR_CURRENT_OFFSET ) == 0 ) {
				s = ERR_LONG_NAME_ERR;
				break;
			}
			GlobalBuf[33] = DEF_SECTOR_SIZE / sizeof( FAT_DIR_INFO );
		}
		GlobalBuf[33] --;
		s = CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, GlobalBuf[33] ); 
		if ( s != USB_INT_SUCCESS ) break;
		CH376ReadBlock( GlobalBuf );
		CH376EndDirInfo( );
		if ( ( GlobalBuf[11] & ATTR_LONG_NAME_MASK ) != ATTR_LONG_NAME || GlobalBuf[13] != GlobalBuf[32] ) { 
			s = ERR_LONG_NAME_ERR;
			break; 
		}
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {
			if ( s == 1 + 5 * 2 ) s = 14;
			else if ( s == 14 + 6 * 2 ) s = 28;
			LongName[ NameCount++ ] = GlobalBuf[ s ];
			LongName[ NameCount++ ] = GlobalBuf[ s + 1 ];
			if ( GlobalBuf[ s ] == 0 && GlobalBuf[ s + 1 ] == 0 ) break;
			if ( NameCount >= LONG_NAME_BUF_LEN ) {
				s = ERR_LONG_BUF_OVER;
				goto CH376GetLongNameE;
			}
		}
		if ( GlobalBuf[0] & 0x40 ) {
			if ( s >= sizeof( FAT_DIR_INFO ) ) *(PUINT16)( &LongName[ NameCount ] ) = 0x0000;
			s = USB_INT_SUCCESS;
			break;
		}
	}
CH376GetLongNameE:
	CH376FileClose( FALSE );
	return( s );
}

uint8_t	CH376CreateLongName( uint8_t* PathName, PUINT8X LongName )

{
	uint8_t	s, i;
	uint8_t	DirBlockCnt;
	uint16_t	count;		
	uint16_t	NameCount;
	UINT32	NewFileLoc;	
	for ( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) if ( *(PUINT16)(&LongName[count]) == 0 ) break;
	if ( count == 0 || count >= LONG_NAME_BUF_LEN || count > LONE_NAME_MAX_CHAR ) return( ERR_LONG_NAME_ERR );
	DirBlockCnt = count / LONG_NAME_PER_DIR;
	i = count - DirBlockCnt * LONG_NAME_PER_DIR;
	if ( i ) {
		if ( ++ DirBlockCnt * LONG_NAME_PER_DIR > LONG_NAME_BUF_LEN ) return( ERR_LONG_BUF_OVER );
		count += 2;
		i += 2;
		if ( i < LONG_NAME_PER_DIR ) {
			while ( i++ < LONG_NAME_PER_DIR ) LongName[count++] = 0xFF;
		}
	}
	s = CH376FileOpenPath( PathName );
	if ( s == USB_INT_SUCCESS ) {
		s = ERR_NAME_EXIST;
		goto CH376CreateLongNameE;
	}
	if ( s != ERR_MISS_FILE ) return( s );
	s = CH376FileCreatePath( PathName ); 
	if ( s != USB_INT_SUCCESS ) return( s );
	i = CH376ReadVar8( VAR_FILE_DIR_INDEX );
	s = CH376LocateInUpDir( PathName );
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET ) + i * sizeof(FAT_DIR_INFO);
	s = CH376ByteLocate( NewFileLoc );
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	s = CH376ByteRead( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO), NULL );
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	for ( i = DirBlockCnt; i != 0; -- i ) {
		s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
		if ( count == 0 ) break;
		if ( GlobalBuf[0] && GlobalBuf[0] != 0xE5 ) {
			s = CH376ByteLocate( NewFileLoc ); 
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			GlobalBuf[ 0 ] = 0xE5; 
			for ( s = 1; s != sizeof(FAT_DIR_INFO); s ++ ) GlobalBuf[ s ] = GlobalBuf[ sizeof(FAT_DIR_INFO) + s ];
			s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			do { 
				s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count ); 
				if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			} while ( count && GlobalBuf[0] );
			NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET );
			i = DirBlockCnt + 1;
			if ( count == 0 ) break;
			NewFileLoc -= sizeof(FAT_DIR_INFO);
		}
	}
	if ( i ) { 
		s = CH376ReadVar8( VAR_SEC_PER_CLUS );
		if ( s == 128 ) { 
			s = ERR_FDT_OVER;
			goto CH376CreateLongNameE;
		}
		count = s * DEF_SECTOR_SIZE; 
		if ( count < i * sizeof(FAT_DIR_INFO) ) count <<= 1;  
		s = CH376LongNameWrite( NULL, count );
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376ByteLocate( NewFileLoc );
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	GlobalBuf[11] = ATTR_LONG_NAME;
	GlobalBuf[12] = 0x00;
	GlobalBuf[13] = CH376CheckNameSum( &GlobalBuf[ sizeof(FAT_DIR_INFO) ] ); 
	GlobalBuf[26] = 0x00;
	GlobalBuf[27] = 0x00;
	for ( s = 0; DirBlockCnt != 0; ) {
		GlobalBuf[0] = s ? DirBlockCnt : DirBlockCnt | 0x40;
		DirBlockCnt --;
		NameCount = DirBlockCnt * LONG_NAME_PER_DIR;
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {
			if ( s == 1 + 5 * 2 ) s = 14; 
			else if ( s == 14 + 6 * 2 ) s = 28;
			GlobalBuf[ s ] = LongName[ NameCount++ ];
			GlobalBuf[ s + 1 ] = LongName[ NameCount++ ];
		}
		s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376LongNameWrite( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO) );
CH376CreateLongNameE:
	CH376FileClose( FALSE );
	return( s );
}
#endif
*/
void mStopIfError(uint8_t iError)
{
	if(iError == USB_INT_SUCCESS) return;
	printf("Error: %02X\n",(uint16_t)iError);
	while(1)
    {
        bsp_LedOn(LED_ERROR);
		bsp_DelayMS(200);
        bsp_LedOff(LED_ERROR);
        bsp_DelayMS(200);
	}
}

char buf[128] = {0};

void list_file(void)
{
    uint8_t status;
    P_FAT_DIR_INFO pDir;
    
    strcpy(buf,"/*");
    printf("List file %s\n", buf);
    status = CH376FileOpenPath((uint8_t*)buf);
    while(status == USB_INT_DISK_READ)
    {
        CH376ReadBlock((uint8_t*)buf);
        pDir = (P_FAT_DIR_INFO)buf;
        if(pDir -> DIR_Name[0] != '.')
        {
            if(pDir -> DIR_Name[0] == 0x05) pDir -> DIR_Name[0] = 0xE5;
            pDir -> DIR_Attr = 0;
            printf( "*** EnumName: %s\n", pDir -> DIR_Name );
        }
        Ch376t_CS_LOW();
        Spi_SendByte(CMD_FILE_ENUM_GO);
        Ch376t_CS_HIGH();
        status = Wait376Interrupt();
    }
    if(status != ERR_MISS_FILE) mStopIfError(status);
}

void host(void)
{
    uint8_t status;
    uint16_t i,s;
	uint16_t TotalCount;
	uint16_t RealCount;

	while(1)
    {
		printf("Wait Udisk\n");
		while(CH376DiskConnect() != USB_INT_SUCCESS)
        {
			bsp_DelayMS(100);
		}
		bsp_LedOn(LED_ERROR);
        bsp_DelayMS(200);

		for(i = 0; i < 100; i++)
        {
			bsp_DelayMS(50);
            
			printf("CH376DiskMount %d\n",i);
			status = CH376DiskMount();
			if(status == USB_INT_SUCCESS) 
            {
                printf( "Ready!\n" );break;
            }
			else if(status == ERR_DISK_DISCON) 
            {
                printf( "Disconnect!\n" );break;
            }
			if(CH376GetDiskStatus() >= DEF_DISK_MOUNTED && i >= 90) break;
		}
		if(status == ERR_DISK_DISCON)
        {
			printf("Device gone\n");
			continue;
		}
		if(CH376GetDiskStatus() < DEF_DISK_MOUNTED)
        {
			printf("Unknown device\n");
			goto UnknownUsbDevice;
		}
		i = CH376ReadBlock((uint8_t*)buf);
		if(i == sizeof(INQUIRY_DATA))
        {
			buf[i] = 0;
			printf("UdiskInfo: %s\n",((P_INQUIRY_DATA)buf)->VendorIdStr);
		}
        
        //ͨ�����ݶ�ȡ
		strcpy(buf,"\\GROUP.BIN");//����CH376T�ֲ�, �ļ����������ֻ��ߴ�д��ĸ,����Сд��ĸ!
		status = CH376FileOpenPath((uint8_t*)buf);
        printf("CH376FileOpenPath = %x\n\r",status);
		if(status == ERR_MISS_DIR || status == ERR_MISS_FILE)
        {
            list_file();
		}
		else
        {
			mStopIfError(status);
			TotalCount = 128;
			printf( "���ļ��ж�����ǰ%d���ַ���:\n",TotalCount);
			while(TotalCount)
            {
				i = TotalCount;
				status = CH376ByteRead(&ChannelTab.Channel[0].Num, i, &RealCount);
				mStopIfError(status);
				TotalCount -= RealCount;
				for(s=0; s!=RealCount; s++) printf("%x ",(&ChannelTab.Channel[0].Num)[s]);
				if(RealCount < i)
                {
					printf("\n�ļ��Ѿ�����\n");
					break;
				}
			}
			printf( "\nClose group.bin\n" );
			status = CH376FileClose(FALSE);
			mStopIfError(status);
		}
        //��λ���ݶ�ȡ
		strcpy(buf,"\\PHASE.BIN");
		status = CH376FileOpenPath((uint8_t*)buf);
        printf("CH376FileOpenPath = %x\n\r",status);
		if(status == ERR_MISS_DIR || status == ERR_MISS_FILE)
        {
            list_file();
		}
		else
        {
			mStopIfError(status);
			TotalCount = 1024;
			printf( "���ļ��ж�����ǰ%d���ַ���:\n",TotalCount);
			while(TotalCount)
            {
				i = TotalCount;
				status = CH376ByteRead(&ChannelTab.Channel[0].Num, i, &RealCount);
				mStopIfError(status);
				TotalCount -= RealCount;
				for(s=0; s!=96; s++) printf("%x ",(&ChannelTab.Channel[0].Num)[s]);   //1024
				if(RealCount < i)
                {
					printf( "\n" );
					printf( "�ļ��Ѿ�����\n" );
					break;
				}
			}
			printf( "\nClose phase.bin\n" );
			status = CH376FileClose(FALSE);
			mStopIfError(status);
		}
        
UnknownUsbDevice:
        printf("Wait U Disk Take out\n");
		while(CH376DiskConnect() == USB_INT_SUCCESS) /* ���U���Ƿ����ӣ��ȴ�U�̰γ� */
        {
			bsp_DelayMS(1000);
		}
		bsp_LedOff(LED_ERROR);
        bsp_DelayMS(100);
	}
}



