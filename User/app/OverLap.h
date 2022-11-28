#ifndef OVERLAP_H
#define OVERLAP_H

#include "public.h"


#define OverlapTabSize  0x00B0


typedef enum{OT_OTHER = 1, OT_NORMAL, OT_MINUSGREENYELLOW} Overlap_Type;

typedef struct
{
    uint8_t  Num;                //��� 1-255
    uint8_t  Type;               //���� 1���� 2���� 3����ֹͣ
    uint8_t  IncludedPhases[2];  //ĸ��λ ��������еĻ�������λ
    uint8_t  ModifierPhases[2];  //������λ 
    uint8_t  TrailGreen;         //�����̵� 0-255 ������λ����ĸ��λ���������̵ƺ󣬼������е��̵�ʱ�䣬��λ���롣
    uint8_t  TrailClear;
    uint8_t  TrailYellow;        //����Ƶ� 0-255 ��������λ���̵ƻ�����ʱ�䲻Ϊ��ʱ�����еĻƵ�ʱ�䡣�̵ƺ�����ʱ�䶼Ϊ��ʱ���Ƶ�ʱ��ʹ��ĸ��λ�Ƶ�ʱ�䡣
    uint8_t  TrailRed;           //������ 0-255 ��������λ���̵ƻ�����ʱ�䲻Ϊ��ʱ�����е�ȫ��ʱ�䡣�̵ƺ�����ʱ�䶼Ϊ��ʱ��ȫ��ʱ��ʹ��ĸ��λȫ��ʱ�䡣
}OverlapType; //������λ 

typedef struct 
{
    uint8_t     Maximum;
    OverlapType Overlap[OverlapMax];    //10 byte * 16
    uint8_t     Reserve[15];
}OverlapTable; //������λ��      //176

typedef struct
{
    uint32_t Reds;
    uint32_t Yellows;
    uint32_t Greens;
    uint32_t Flashs;
}OverlapStatusType;


extern uint16_t     IncludedPhases[OverlapMax];     //ĸ��λ ��������еĻ�������λ
extern uint16_t     ModifierPhases[OverlapMax];     //������λ 
extern uint16_t     OverlapCounter[OverlapMax];      //������λʱ��ͳ��
extern OverlapTable         OverlapTab;  //�����λ��
extern OverlapStatusType    OverlapStatus;




void OverlapInit(void);
//������λ�� + ������λ�� = ������λ����
uint8_t GetOverlapIndex(OverlapTable* Overlap_tab, uint8_t OverlapNum);

#endif
