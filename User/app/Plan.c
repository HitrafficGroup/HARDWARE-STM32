/*
*********************************************************************************************************
*
*	ģ������ : ����ģ��
*	�ļ����� : Plan.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "Plan.h"

PeriodType       Period;     //��ǰ����ʱ��
PlanType         Plan;       //��ǰ����ʱ�α�
PlanTable        PlanTab;    //ʱ�α�


//����ռƻ��Ƿ�Ϊ�գ�Ϊ�ջ��߲������� PlanMax������0����Ϊ�շ���1��
uint8_t PlanEmptyCheck(uint8_t n)
{
    uint8_t temp;
    
    if(n>=PlanMax) return 0;
    temp = PlanTab.Plan[n].Num;
    if(temp>0 && temp <=PlanMax)//�ռƻ��źϷ�
    {
        temp = PlanTab.Plan[n].Period[0].ActionNum;
        if(temp>0 == temp<=ActionMax)
            return 1;
        else
            return 0;
    }
    else return 0;
}

/* 
    ʱ�α� + ʱ�� = ʱ������
*/
uint8_t GetPeriodIndex(PlanType* DayPlan, TimeType* Time)
{
    uint8_t     i;
    uint8_t     temp = 0xff;
	uint16_t    day_by_mins;
	uint16_t    plan_by_mins0;
	uint16_t    plan_by_mins1;
    
    //��ǰʱ�� + ʱ�α� = ʱ�κ�
	day_by_mins = Time->Hour*60 + Time->Minute;     //������
    for(i=0;i<24;i++)
    {
        plan_by_mins0 = DayPlan->Period[i].Time.Hour * 60 + DayPlan->Period[i].Time.Minute;
        
        if(i < 23)
            plan_by_mins1 = DayPlan->Period[i+1].Time.Hour * 60 + DayPlan->Period[i+1].Time.Minute;
        else
            plan_by_mins1 = 0;
        
        if(plan_by_mins0 <= day_by_mins)
        {
            if(day_by_mins < plan_by_mins1 || plan_by_mins1 == 0)
            {
                temp = i;
                break;
            }
        }
        else
        {
            if(plan_by_mins1 == 0)
            {
                temp = i;
                break;
            }
        }
    }
    return temp;
}

void PlanDefault(void)
{
    memset(&PlanTab.Maximum,0x00,sizeof(PlanTab));
    
    PlanTab.Maximum = PlanMax;
    PlanTab.Plan[0].Num = 1;
    PlanTab.Plan[0].Period[0].Time.Hour = 0x00;
    PlanTab.Plan[0].Period[0].Time.Minute = 0x00;
    PlanTab.Plan[0].Period[0].ActionNum = 0x01;
    
    PlanTab.Plan[0].Period[1].Time.Hour = 4;
    PlanTab.Plan[0].Period[1].Time.Minute = 50;
    PlanTab.Plan[0].Period[1].ActionNum = 0x02;
}

