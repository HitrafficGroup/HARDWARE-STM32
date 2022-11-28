/*
*********************************************************************************************************
*
*	ģ������ : 
*	�ļ����� : BasicInfo.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "BasicInfo.h"


DeviceInfo_TypeDef  DeviceInfo;
BasicInfo_TypeDef   BasicInfo;


void get_cpuid(uint8_t *pdata)
{
    uint32_t Device_Serial[3];
    Device_Serial[0] = *(volatile uint32_t*)(0x1FFFF7E8);
    Device_Serial[1] = *(volatile uint32_t*)(0x1FFFF7EC);
    Device_Serial[2] = *(volatile uint32_t*)(0x1FFFF7F0);
    
    pdata[2] = (Device_Serial[0]&0xff);
    pdata[1] = (Device_Serial[0]>>8);
    pdata[0] = (Device_Serial[0]>>16);
    printf("_cpuid %08x %08x %08x\n\r",Device_Serial[0],Device_Serial[1],Device_Serial[2]);
}


void DeviceInfoInit(void)
{
	const uint8_t ManufacturerInfo[128] = "\0S\0i\0n\0o\0w\0a\0t\0c\0h\0e\0r\0 \0T\0e\0c\0h\0n\0o\0l\0o\0g\0y\0 \0C\0o\0.\0,\0 \0L\0t\0d\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";// 1.1 ���쳧����Ϣ128�ֽ�
    const uint8_t DeviceInfo_buf[48] = 
    {
        0x19,0x01,0x20,0x01,                    //1.2 �豸�汾,4byte,����λӲ��,����λ���
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,//1.3 ���Ҷ��źŻ���Ψһ���16byte,����16byte,���ֽ�Ϊ0
        0x20,0x22,0x04,0x02,0x00,0x00,          //1.4 ��������,6byte
        0x20,0x22,0x04,0x01,0x00,0x00,          //1.5 ��������,6byte
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//����16�ֽ�
    };

    memcpy(DeviceInfo.ManufacturerInfo, ManufacturerInfo, 128);
    memcpy(DeviceInfo.DeviceVersion, DeviceInfo_buf, 48);
}

void BasicInfoInit(void)
{
	const uint8_t IntersectionInfo[128]={0};// 2.1 ��װ·����Ϣ,128byte 
    uint8_t BasicInfo_buf[48]=
    {
        192,168,  1,123,//IP,4byte
        255,255,255,  0,//SubMask,4byte
        192,168,  1,  1,//Gateway,4byte
          0,161,        //Socket,2byte
        ':','S','W',0x00,0x00,0x01,     //Phy,6byte 
        
        192,168,  1, 100,// 2.3.1 RemoteIP,4byte
        0x17,0x78,      // 2.3.2 RemoteSocket,2byte 6008
          0,            // 2.3.3 ͨ������,1byte
        0x01,0x00,0x70,0x80,    // 2.4 ʱ��,value -43200~43200,4byte
        0x00,0x00,0x00,0x01,    // 2.5 �źŻ����,�϶�ϵͳ�е�Ψһ���,4byte
        0x01,           // 2.6 �źŻ����Ƶ�·������,1byte,value 1-8
        0x00,           // 2.7 GPSʱ�ӱ�־,1byte,bool����,0��Ч,1��Ч
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//�����ռ� 11�ֽ�
    };
    get_cpuid(&BasicInfo_buf[17]);//MAC������cpuid�ҹ�,�ɱ�֤Ψһ.
    memcpy(BasicInfo.IntersectionInfo, IntersectionInfo, 128);
    memcpy(BasicInfo.IPv4.IP, BasicInfo_buf, 48);
}



