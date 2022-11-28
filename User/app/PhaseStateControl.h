#ifndef PHASESTATECONTROL_H
#define PHASESTATECONTROL_H
#include "stdint.h"
#include "public.h"

typedef struct
{    
    uint8_t         RingNum;        //����
    uint8_t         SeqNum;         //���еĽ׶κ�
    uint8_t         SeqMax;         //�������
    uint8_t         PhaseChangeFlag;//��λ�л���־
    uint8_t         CycleOverFlag;  //�����н�����־
    uint8_t         VehicleTransitionTime;
    uint16_t        SecondRemain;   //��ݼ�����
}RingStateType;

typedef struct
{
    uint8_t         Phase1sFlag;
    uint8_t         Phase10msCount; //10ms������
    uint8_t         miniRemainTime;
    uint8_t         NewCycleFlag;   //�����ڱ�־λ
    uint8_t         StepMaxRing;    //��λ��������ߵĻ���
    uint8_t         CycleStepMax;   //����λ�������ֵ
    uint8_t         ValidRings;     //��Ч(�ǿ�)����
    RingStateType   Ring[RingMax];
    uint8_t         StateNum; //��ǰ����(����)
    uint8_t         StateMax; //��λ״̬(����)���ֵ
    uint32_t        State[64];//��λ״̬�б�
}PhaseStateType;




extern PhaseStateType       PhaseState;

uint32_t GetPhaseNexts(void);
void PhaseStatusControl(void);//1s����һ��
void OverlapStatusControl(void);//1Sˢ��һ��
void ChannelStatusControl(void);//1Sˢ��һ��
void ChannelStatusToLmap(void);

#endif
