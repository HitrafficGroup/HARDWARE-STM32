/*
*********************************************************************************************************
*
*	模块名称 : 行人检测器模块
*	文件名称 : Peddet.c
*	版    本 : V1.0
*	说    明 : 
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0    2019-12-30  wcx     首发
*
*********************************************************************************************************
*/
#include "public.h"
#include "Peddet.h"
#include "bsp_io.h"
#include "tsc.h"

PeddetTable  PeddetTab;  //行人检测器表
PeddetStateTable PeddetStateTab;

void PeddetInit(void)
{
    memset(&PeddetTab.Maximum,0x00,sizeof(PeddetTab));
    PeddetTab.Maximum = PeddetMax;
}

void PeddetStateInit(void)
{
    memset(&PeddetStateTab.peddet[0].Valid,0x00,sizeof(PeddetStateTab));
    PeddetStateTab.Maximum = PeddetMax;
}

void PeddetStateGet(void)//执行周期1s
{
	uint8_t i;
    static uint8_t reg60s = 0;

    if((peddet_hw.rising | peddet_hw.failing) & 0x80)
    {
        peddet_hw.rising &= 0x7f;
        peddet_hw.failing &= 0x7f;
        OP.SendDoorAlarm = 1;
    }
    for(i = 0; i < 8; i++)
    {
        if(PeddetStateTab.peddet[i].Valid) //有信号
        {
            if(PeddetStateTab.peddet[i].Presence < 0xffff)PeddetStateTab.peddet[i].Presence++;
        }
        else //无信号
        {
            if(PeddetStateTab.peddet[i].NoActivity < 0xffff)PeddetStateTab.peddet[i].NoActivity++;
        }
    }
    
    if(++reg60s >= 60)//60s检查一次
    {
        reg60s = 0;
        for(i = 0; i < 8; i++)
        {
            if(PeddetTab.peddet[i].NoActivity)
            {
                if(PeddetStateTab.peddet[i].NoActivity >= PeddetTab.peddet[i].NoActivity * 60)
                {
                    PeddetStateTab.peddet[i].Alarms |= PD_NoActivityFault;
                }
                else 
                {
                    PeddetStateTab.peddet[i].Alarms &= ~PD_NoActivityFault;
                }
            }
            
            if(PeddetTab.peddet[i].MaxPresence)
            {
                if(PeddetStateTab.peddet[i].Presence >= PeddetTab.peddet[i].MaxPresence * 60)
                {
                    PeddetStateTab.peddet[i].Alarms |= PD_MaxPresenceFault;
                }
                else 
                {
                    PeddetStateTab.peddet[i].Alarms &= ~PD_MaxPresenceFault;
                }
            }
            
            if(PeddetTab.peddet[i].Erratic)
            {
                if(PeddetStateTab.peddet[i].ErraticCounts > PeddetTab.peddet[i].Erratic)
                {
                    PeddetStateTab.peddet[i].Alarms |= PD_ErraticOutputFault;
                }
                else 
                {
                    PeddetStateTab.peddet[i].Alarms &= ~PD_ErraticOutputFault;
                }
            }
            PeddetStateTab.peddet[i].ErraticCounts = 0;
        }
    }
}










