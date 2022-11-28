#ifndef PATTERN_H
#define PATTERN_H

#include "public.h"

typedef struct
{
    uint8_t Num;               //1 ������
    uint8_t CycleTimeL;        //2 ����
    uint8_t CycleTimeH;        //
    uint8_t OffsetTime;        //3 ��λ��
    uint8_t SplitNum;          //4 ���űȺ�
    uint8_t SequenceNum;       //5 �����
    uint8_t WorkMode;          //6 ����ģʽ
}PatternType;             //������Ϣ

typedef struct
{
    uint8_t     Maximum;
    PatternType Pattern[PatternMax];  //7*100
    uint8_t     Reserve[3];
}PatternTable;           //������   704 = 16 * 44 =  0x02C0

extern uint16_t         NowCycleTime;
extern PatternType      PatternNow; //��ǰ���з��� 
extern PatternTable     PatternTab; //������


uint8_t GetPatternIndex(PatternTable* PatternTab, uint8_t PatternNum);
void PatternDefault(void);

#endif
