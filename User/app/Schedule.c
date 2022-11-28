/*
*********************************************************************************************************
*
*	ģ������ : ���ȼƻ�ģ��
*	�ļ����� : Schedule.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "public.h"
#include "Schedule.h"
//#include "bsp.h"




ScheduleType     ScheduleNow;      //��ǰ���еĵ��ȼƻ�
ScheduleTable    ScheduleTab;   //���ȼƻ���



void ScheduleDefault(void)
{
    memset(&ScheduleTab.Maximum,0x00,sizeof(ScheduleTab));
    
    ScheduleTab.Maximum = ScheduleMax;
    ScheduleTab.Schedule[0].Num = 1;
    ScheduleTab.Schedule[0].PlanNum = 1;
    ScheduleTab.Schedule[0].Date.MonthH = 0x1f;
    ScheduleTab.Schedule[0].Date.MonthL = 0xfe;
    ScheduleTab.Schedule[0].Date.Date[0] = 0xfe;
    ScheduleTab.Schedule[0].Date.Date[1] = 0xff;
    ScheduleTab.Schedule[0].Date.Date[2] = 0xff;
    ScheduleTab.Schedule[0].Date.Date[3] = 0xff;
    ScheduleTab.Schedule[0].Date.Day = 0xfe;
}

/*
 *  ͨ�����ڲ�ѯ�����ȼƻ�����ȡ���������ĵ��ȼƻ�������
 *  
 *  
*/
uint8_t GetScheduleIndex(ScheduleTable* ScheduleTab, DateType* Date)
{
    uint8_t  i;
    uint8_t  temp = 0xff;
    uint16_t SchMonth,NowMonth;
    uint32_t SchDate, NowDate;
    
    NowMonth = (Date->MonthH<<8) | Date->MonthL;
    NowDate  = (Date->Date[3]<<24)|(Date->Date[2]<<16)|(Date->Date[1]<<8)|Date->Date[0];
    
#if DEBUG 
    printf("Month = %x, Date = %x \r\n",NowMonth, NowDate);
#endif
    
    for(i = 0; i < ScheduleMax; i++)
    {
        if(ScheduleTab->Schedule[i].Num > 0 && ScheduleTab->Schedule[i].PlanNum > 0)
        {
            SchMonth = (ScheduleTab->Schedule[i].Date.MonthH<<8)  | \
                        ScheduleTab->Schedule[i].Date.MonthL;
            SchDate  = (ScheduleTab->Schedule[i].Date.Date[3]<<24)| \
                       (ScheduleTab->Schedule[i].Date.Date[2]<<16)| \
                       (ScheduleTab->Schedule[i].Date.Date[1]<<8 )| \
                        ScheduleTab->Schedule[i].Date.Date[0];
#if DEBUG 
            printf("SchMonth = %x, SchDate = %x \r\n", SchMonth, SchDate);
#endif
            
            if((SchMonth & NowMonth) && (SchDate & NowDate) && (ScheduleTab->Schedule[i].Date.Day & Date->Day))//���㼴ȷ��
            {
                temp = i;
                break;
                //if(NowMonth == SchMonth || SchMonth != SM_ALL)  //ָ���·�Ϊ������ ���ȼ����
            }
        }
    }
    return temp;
}



