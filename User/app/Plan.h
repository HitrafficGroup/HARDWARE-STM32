#ifndef PLAN_H
#define PLAN_H
#include "public.h"


typedef struct
{
    uint8_t Hour;       //1
    uint8_t Minute;     //2
}TimeType;

typedef struct
{
    //uint8_t   EventNum;   //ʱ�κ�     ��ʡ�洢�ռ䣬�����������
    TimeType    Time;       //1,2 ʱ��
    uint8_t     ActionNum;  //3 �������
}PeriodType;//ʱ�α�

typedef struct 
{
    uint8_t     Num;         //1  ʱ�μƻ����
    PeriodType  Period[PeriodMax]; //96 ʱ�ζ���,����24��ʱ��� 3*24 = 72
}PlanType;    //ʱ�α�

typedef struct
{
    uint8_t     Maximum;
    PlanType    Plan[PlanMax];       //16 * 73
    uint8_t     Reserve[15];
}PlanTable;  //ʱ�α� 16*74 = 1184 = 0x04A0


extern PeriodType   Period;     //��ǰ����ʱ��
extern PlanType     Plan;
extern PlanTable    PlanTab;    //ʱ�α�




void PlanDefault(void);
uint8_t GetPeriodIndex(PlanType* DayPlan, TimeType* Time);


#endif
