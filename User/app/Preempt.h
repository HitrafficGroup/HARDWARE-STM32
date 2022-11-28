#ifndef PREEMPT_H
#define PREEMPT_H

#include "public.h"


typedef struct
{
    uint8_t Num;            //���Ⱥ�
    uint8_t Control;        //���ȿ���
    uint8_t Link;           //������·
    uint16_t Delay;         //�ӳ�
    uint16_t Duration;      //����ʱ��
    uint8_t MinimumGreen;   //��С�̵�ʱ��
    uint8_t MinimumWalk;    //��С����ʱ��
    uint8_t EnterPedClear;  //�������ʱ��
    uint8_t TrackGreen;     //����̵�ʱ��
    uint8_t DwellGreen;     //פ���̵�ʱ��
    uint8_t MaximumPresence;//�������ʱ��
    uint8_t TrackPhase[16]; //�����������λ
    uint8_t DwellPhase[16]; //������פ����λ
    uint8_t DwellPed[16];   //����פ����λ
    uint8_t ExitPhase[16];  //�˳���λ
    uint8_t State;          //���ȹ涨
    
    uint8_t TrackOverlap[16];   //���������λ
    uint8_t DwellOverlap[16];   //����פ����λ
    uint8_t CyclingPhase[16];   //������ѭ����λ
    uint8_t CyclingPed[16];     //����ѭ����λ
    uint8_t CyclingOverlap[16]; //����ѭ����λ
    
    uint8_t EnterYellowChange;  //����Ƶ�ʱ��
    uint8_t EnterRedClear;      //����ȫ��ʱ��
    uint8_t TrackYellowChange;  //���Ƶ�ʱ��
    uint8_t TrackRedClear;      //���ȫ��ʱ��
}Preempt_T;     //���Ȳ�����

typedef struct
{
    Preempt_T Preempt[8];
}Preempt_Tab;     //���Ȳ�����












#endif
