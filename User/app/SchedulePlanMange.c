#include "Schedule.h"
#include "Plan.h"

/*
 * ���ܣ�ͨ����ѯ���ȼƻ�����ȡʱ�α�ţ��ж�ʱ�α��Ƿ�ı䣬�иı�ֱ�Ӹ���
 * ���أ���Plan���ã�����0�����򷵻�1
 * ִ��Ƶ�ʣ�����ʱ���У�ÿ��������У�����ʱ���޸�ʱ���У����ȼƻ����޸�ʱ����
 * 
 */
uint8_t SchedulePlanRefresh(ScheduleTable* ScheduleTab, DateType* Date)
{
    uint8_t temp = GetScheduleIndex(ScheduleTab, Date);   //���� + ���ȼƻ��� = ʱ�α��
    #if DEBUG 
    printf("ScheduleIndex = %d \r\n",temp);
    #endif
    if(temp < ScheduleMax)
    {
        if(memcmp(&ScheduleNow, &ScheduleTab->Schedule[temp], sizeof(ScheduleNow)) != 0)
        {
            #if DEBUG 
            printf("Schedule Changed \r\n");
            #endif
            memcpy(&ScheduleNow, &ScheduleTab->Schedule[temp], sizeof(ScheduleNow));
        }
        
        if(ScheduleNow.PlanNum != Plan.Num)
        {
            #if DEBUG 
            printf("Plan Changed \r\n");
            #endif
            memcpy(&Plan, &PlanTab.Plan[ScheduleNow.PlanNum - 1], sizeof(Plan));
            return 1;
        }
    }
    return 0;
}
