#ifndef PEDDET_H
#define PEDDET_H

#include "public.h"

#define PeddetTabSize  0x0040


#define PD_NoActivityFault      0x01
#define PD_MaxPresenceFault     0x02
#define PD_ErraticOutputFault   0x04
#define PD_CommunicationsFault  0x08
#define PD_ConfigurationFaultt  0x10
#define PD_OtherFaultt          0x80

typedef struct
{
    uint8_t  Valid;
    uint8_t  Alarms;
    uint16_t NoActivity;
    uint16_t Presence;
    uint16_t ErraticCounts;
}PeddetState;

typedef struct
{
    PeddetState peddet[8];
    uint8_t     Maximum;
}PeddetStateTable;


typedef struct
{
    uint8_t Num;            //�������
    uint8_t CallPhase;      //������λ
    uint8_t NoActivity;     //����Ӧʱ��(����),�����ж�ʧЧ,0�����
    uint8_t MaxPresence;    //����ʱ��,�����ж�ʧЧ,0�����
    uint8_t Erratic;        //��/����,�����ж�ʧЧ,0�����
    uint8_t Alarms;         //detector alarms.
}Peddet;

typedef struct
{
    uint8_t     Maximum;
    Peddet      peddet[PeddetMax];  //6 byte * 8
    uint8_t     Reserve[15];
}PeddetTable; //48 + 16 = 64 = 0x40

extern PeddetTable  PeddetTab;  //���˼������
extern PeddetStateTable PeddetStateTab;

void PeddetInit(void);
void PeddetStateInit(void);
void PeddetStateGet(void);//ִ������1s

#endif
