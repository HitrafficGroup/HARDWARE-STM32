#ifndef PHASE_H
#define PHASE_H

#include "public.h"

/********************************************************/
#define FlashPhase      252
#define AllRedPhase     253
#define AllOffPhase     254


/********************************************************/
typedef struct
{
    uint8_t PhaseNum;
    uint8_t PhaseIndex;
    uint8_t SplitIndex;
    uint8_t PhaseNext;
    uint8_t PhaseNextIndex;
    uint8_t SplitNextIndex;
}PhaseIndexType;


/********************************************************/
typedef struct
{
    uint8_t Num;              //1-��λ��
    uint8_t Walk;             //2-������λ�̵�ʱ��
    uint8_t PedestrianClear;  //3-������λ����ʱ��
    uint8_t MinimumGreen;     //4-�ڸ�Ӧ�����£���λִ���̵Ƶ���Сʱ��
    uint8_t Passage;          //5-�ڸ�Ӧ�����£���λ�̵�ʱ���ӳ�һ�ε�ʱ��

    uint8_t Maximum1;         //6-�ڸ�Ӧ�����£���λִ���̵Ƶ����ʱ��1
    uint8_t Maximum2;         //7-�ڸ�Ӧ�����£���λִ���̵Ƶ����ʱ��2��һ������£����ʱ��ʹ�������1��ֻ����ǿ��ִ�������2ʱ��ʹ�������2
    uint8_t YellowChange;     //8-��������λ�̵ƽ���ת�����źŵĻƵ�ʱ��
    uint8_t RedClear;         //9-��������λ���н�������ͬ������һ����λ����֮ǰ�����еĺ��ʱ��
    uint8_t RedRevert;        //10-��λ�Ƶƽ����󣬾��ٴη����̵����辭������С���ʱ��

    uint8_t AddedInitial;     //11-ÿ�ӷ��̵�ʱ�����ڵĳ�����Ӧ�󣬿ɱ��ʼ������㿪ʼ������ֵ
    uint8_t MaximumInitial;   //12-�ɱ��ʼ��������ֵ
    uint8_t TimeBeforeReduction;//13-��ʼ���Լ���ǰ��ʱ��
    uint8_t CarsBeforeReduction;//14-��ʼ���Լ���ǰ�ĳ�����
    uint8_t TimeToReduce;     //15-�ö�����Գ�ΪNEMA TS 1�� TS 2���涨�������½������Ʒ�����ڼ�С�����ܶȼ��

    uint8_t ReduceBy;         //16-����ݼ���
    uint8_t MinimumGap;       //17-��С��λ�������������������һֱ�½���ֱ��������ڻ�С����С��������豸���涨����С���ֵ��Ȼ�󣬿�����������������С��������豸���涨��ֵ
    uint8_t DynamicMaxLimit;  //18-�涨�˶�̬����������ת�������޺�����
    uint8_t DynamicMaxStep;   //19-�涨����ת�����Զ�����
    uint8_t Startup;          //20-��ʼ״̬�������� 0-����  1-δ����  2-�̵�  3-�������̵�  4-�Ƶ�  5-ȫ��

    uint8_t Ring;             //21-��λ���ڻ�
    uint8_t VehicleClear;     //22-�Լ����ӵĻ�������ʱ������
    uint8_t OptionsL;         //23,24-����ѡ��
    uint8_t OptionsH;         
    uint8_t ConcurrencyL;     //25,26   ������λ�����Ժͱ���λͬʱ���е���λ,�Դ����жϳ�ͻ��λ
    uint8_t ConcurrencyH;     // �Ƿ��Ϊ��ͻ��λ����,Ϊ1��ʾ���Ӧ��λ��ͻ
    uint8_t Expand[6];        //6
}PhaseType;

typedef struct
{
    uint8_t     Maximum;
    PhaseType   Phase[32];
    uint8_t     Reserve[15];
}PhaseTable; //16 + 32*32 = 1040 = 0x0410


/********************************************************/
typedef struct
{
    uint32_t Reds;
    uint32_t Yellows;
    uint32_t Greens;
    uint32_t VehClears;
    uint32_t DontWalks;
    uint32_t PedClears;
    uint32_t Walks;
    uint32_t PhaseOns;
    uint32_t PhaseNexts;
}PhaseStatusType;

/********************************************************/
//1202V0219F PAGE51 ԭNTCIP���У�\
���ֽ�Ϊ��λ��8��һ���飬����Ϊһ��group��\
�ڴˣ�����32λ���������������.
typedef struct 
{
    uint32_t PhaseOmit;
    uint32_t PedOmit;
    uint32_t Hold;
    uint32_t ForceOff;
    uint32_t VehCall;
    uint32_t PedCall;
}PhaseControlType;//for remote control

/********************************************************/
extern  PhaseTable          PhaseTab;       //��λ��
extern  PhaseStatusType     PhaseStatus;    //
extern  PhaseControlType    PhaseControl;   //
extern  PhaseIndexType      RingPhase[RingMax]; //

extern  uint32_t            PhaseTimes[32];

uint8_t GetPhaseIndex(PhaseTable* Phase_Tab, uint8_t PhaseNum);
void PhaseDefault(void);








#endif
