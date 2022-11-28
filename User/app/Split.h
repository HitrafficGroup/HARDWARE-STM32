#ifndef SPLIT_H
#define SPLIT_H
#include "public.h"


/*
    һ����λʱ��SplitTime 0-255
        ��λ�ķ���ʱ�䡣�����˻�������λ���̵ơ��������Ƶơ�ȫ��ʱ��
        �Լ�������λ�ķ���ʱ������ʱ�䡣
    �������ű�ģʽSplitMode
    1-�ɱ��־������λ�ڴ����ű�ģʽ����Ϊ�ɱ��־���
    2-��
    3-��С������Ӧ��
        ��Ӧ����ʱ����������λ��ǿ��ִ����С�̡�
        ���������ȼ�������λ�����еġ��������Զ��������ԡ�
    4-�������Ӧ��
        ��Ӧ����ʱ����������λ��ǿ��ִ������̡�
        ���������ȼ�������λ�����еġ��������Զ��������ԡ�
    5-������Ӧ��
        ��Ӧ����ʱ��������λ��ǿ�ƻ�ȡ����Ȩ��
        ���������ȼ�������λ�����еġ������Զ��������ԡ�
    6-�����/������Ӧ��
        ��Ӧ����ʱ����������λ��ǿ��ִ������̣�������λ��ǿ�ƻ�ȡ����Ȩ��
        ���������ȼ�������λ�����еġ��������Զ��������Ժ͡������Զ��������ԡ�
    7-������λ
        ����λ�ڴ����ű�ģʽ�£��ӷ�����ȥ����
    ����Э������Coord
        bit0��1-��Э������ʱ������λ��ΪЭ����λ��������·�ڽ���Э����
        bit1��1-��Ϊ�ؼ���λ
        bit2��1-��Ϊ�̶���λ
*/
typedef enum 
{
    SM_Other = 1,
    SM_None = 2,
    SM_MinVehRecall = 3,
    SM_MaxVehRecall = 4,
    SM_PedRecall = 5,
    SM_MaxVehPedRecall = 6,
    SM_Omitted = 7,
}SplitMode;

#define SC_NONE       0x00
#define SC_COORD      0x01
#define SC_KEY        0x02
#define SC_FIXED      0x04

//��λʱ��  ��λ�ķ���ʱ�䡣
//�����˻�������λ���̵ơ��������Ƶơ�ȫ��ʱ��
//�Լ�������λ�ķ���ʱ������ʱ�䡣
typedef struct
{
    uint8_t PhaseNum;           //��λ��
    uint8_t Time;               //��λʱ��
    uint8_t Mode;               //��λģʽ
    uint8_t Coord;              //Э������ 0-1 
}PhaseSplitType; //��λ���űȶ���

typedef struct
{
    uint8_t         Num;     //���űȺ�
    PhaseSplitType  Phase[PhaseMax];    //16
}SplitType;    //���ű�����

typedef struct
{
    uint8_t     Maximum;
    SplitType   Split[SplitMax];                //20
    uint8_t     Reserve[11];
}SplitTable;     //���űȱ� 65 * 20 + 12 = 1312 = 82 * 16 = 0x0520 


extern SplitType        SplitNow;
extern SplitTable       SplitTab;  //���űȱ� 
extern PhaseSplitType   RingSplit[RingMax];      //���ű�

uint8_t GetSplitPhaseIndex(SplitType* Split, uint8_t PhaseNum);

void SplitDefault(void); 
void SplitDataInit(uint8_t n);
void SplitXDataInit(SplitType* Split);


//coordPatternStatus    //2.5.10
//localFreeStatus       //2.5.11
//coordCycleStatus      //2.5.12
//coordSyncStatus       //2.5.13
//systemPatternControl  //2.5.14
//systemSyncControl     //2.5.15


#endif
