#include "PhaseStateControl.h"
#include "Channel.h"
#include "Phase.h"
#include "Split.h"
#include "Overlap.h"

/* ��ǰ���еĸ���������λ״̬���� */
PhaseStateType      PhaseState;

uint32_t GetPhaseNexts(void)
{
    uint32_t    PhaseNexts = 0;
    if(PhaseState.StateNum+1 < PhaseState.StateMax)
        PhaseStatus.PhaseNexts = PhaseState.State[PhaseState.StateNum+1];
    else
        PhaseStatus.PhaseNexts = PhaseState.State[0];
#if PhaseNextLog
    printf("PhaseOns = %04x, PhaseNexts = %04x\r\n",PhaseStatus.PhaseOns, PhaseStatus.PhaseNexts);
#endif
    return PhaseNexts;
}

/* 
 * ��λ�̵�ʱ��
 * ͨ��4��������Ч״̬����λ�ţ�����ʱ�䣬�ж�������λ��״̬
 * PhaseStatus
 */
void PhaseGreenCount(void)//1s����һ��
{
    uint8_t     i;
    uint32_t    PhaseMask = 0x1;
    
    for(i = 0; i < PhaseMax; i++)
    {
        if(PhaseStatus.Greens & PhaseMask)
        {
            PhaseTimes[i]++;
        }
        else PhaseTimes[i] = 0;
        PhaseMask <<= 1;
    }
}

/* 
 * ��λ״̬����
 * ͨ��4��������Ч״̬����λ�ţ�����ʱ�䣬�ж�������λ��״̬
 * PhaseStatus
 */
void PhaseStatusControl(void)//1s����һ��
{
    uint8_t     i,PhaseNum,PhaseIndex;
    uint32_t    PhaseMask;
    
    PhaseStatus.Reds = 0;
    PhaseStatus.Yellows = 0;
    PhaseStatus.Greens = 0;
    PhaseStatus.VehClears = 0;
    PhaseStatus.DontWalks = 0;
    PhaseStatus.PedClears = 0;
    PhaseStatus.Walks = 0;
    PhaseStatus.PhaseOns = 0;
    
    for(i = 0; i < RingMax; i++)//4��������λ���Ƿ��е�ǰͨ���Ŀ���Դ
    {
        if(PhaseState.Ring[i].SeqMax == 0) continue;//�û�δ����
        
        PhaseNum = RingPhase[i].PhaseNum;
        PhaseIndex = RingPhase[i].PhaseIndex;
        PhaseMask = (0x01 << (PhaseNum-1));
        PhaseStatus.PhaseOns |= PhaseMask;
        
        //����״̬�㷨
        if(PhaseState.Ring[i].SecondRemain > PhaseTab.Phase[PhaseIndex].YellowChange + PhaseTab.Phase[PhaseIndex].RedClear)//������
        {
            PhaseStatus.Greens  |=  PhaseMask;
            PhaseStatus.Yellows &= ~PhaseMask;
            PhaseStatus.Reds    &= ~PhaseMask;
            
            if(PhaseState.Ring[i].SecondRemain > PhaseTab.Phase[PhaseIndex].VehicleClear + PhaseTab.Phase[PhaseIndex].YellowChange + PhaseTab.Phase[PhaseIndex].RedClear)
                PhaseStatus.VehClears &= ~PhaseMask;
            else //��������
                PhaseStatus.VehClears |=  PhaseMask;
        }
        else if(PhaseState.Ring[i].SecondRemain > PhaseTab.Phase[PhaseIndex].RedClear)//������
        {
            PhaseStatus.Greens  &= ~PhaseMask;
            PhaseStatus.Yellows |=  PhaseMask;
            PhaseStatus.Reds    &= ~PhaseMask;
        }
        else//������
        {
            PhaseStatus.Reds        |=  PhaseMask;
            PhaseStatus.Greens      &= ~PhaseMask;
            PhaseStatus.Yellows     &= ~PhaseMask;
            PhaseStatus.VehClears   &= ~PhaseMask;
        }
        
        //����״̬�㷨
        if(PhaseTab.Phase[PhaseIndex].OptionsH & 0x20)//�������˷���
        {
            if(PhaseStatus.Greens & PhaseMask)
            {
                if(PhaseStatus.VehClears & PhaseMask) //����
                {
                    PhaseStatus.Walks       &= ~PhaseMask;
                    PhaseStatus.PedClears   |=  PhaseMask;
                    PhaseStatus.DontWalks   &= ~PhaseMask;
                }
                else
                {
                    PhaseStatus.Walks       |=  PhaseMask;
                    PhaseStatus.PedClears   &= ~PhaseMask;
                    PhaseStatus.DontWalks   &= ~PhaseMask;
                }
            }
            else if(PhaseStatus.Yellows & PhaseMask)
            {
                PhaseStatus.Walks       &= ~PhaseMask;
                PhaseStatus.PedClears   |=  PhaseMask;
                PhaseStatus.DontWalks   &= ~PhaseMask;
            }
            else if(PhaseStatus.Reds & PhaseMask)
            {
                PhaseStatus.Walks       &= ~PhaseMask;
                PhaseStatus.PedClears   &= ~PhaseMask;
                PhaseStatus.DontWalks   |=  PhaseMask;
            }
        }
        else //û�й�ѡ�������˷���
        {
            if(RingSplit[i].Time >= (PhaseTab.Phase[PhaseIndex].Walk + PhaseTab.Phase[PhaseIndex].PedestrianClear + PhaseTab.Phase[PhaseIndex].RedClear))
            {
                if((RingSplit[i].Time - PhaseState.Ring[i].SecondRemain) < PhaseTab.Phase[PhaseIndex].Walk) // <= ��Ϊ < �����1��
                {
                    PhaseStatus.Walks     |=  PhaseMask;
                    PhaseStatus.PedClears &= ~PhaseMask;
                    PhaseStatus.DontWalks &= ~PhaseMask;
                }
                else if((RingSplit[i].Time - PhaseState.Ring[i].SecondRemain) < (PhaseTab.Phase[PhaseIndex].Walk + PhaseTab.Phase[PhaseIndex].PedestrianClear))
                {
                    PhaseStatus.Walks     &= ~PhaseMask;
                    PhaseStatus.PedClears |=  PhaseMask;
                    PhaseStatus.DontWalks &= ~PhaseMask;
                }
                else
                {
                    PhaseStatus.Walks     &= ~PhaseMask;
                    PhaseStatus.PedClears &= ~PhaseMask;
                    PhaseStatus.DontWalks |=  PhaseMask;
                }
            }
            else //û�й�ѡ�������˷���ʱ,�趨�����˷���ʱ��̫��
            {
                if(PhaseState.Ring[i].SecondRemain > PhaseTab.Phase[PhaseIndex].PedestrianClear + PhaseTab.Phase[PhaseIndex].RedClear)
                {
                    PhaseStatus.Walks     |=  PhaseMask;
                    PhaseStatus.PedClears &= ~PhaseMask;
                    PhaseStatus.DontWalks &= ~PhaseMask;
                }
                else if(PhaseState.Ring[i].SecondRemain > PhaseTab.Phase[PhaseIndex].RedClear)
                {
                    PhaseStatus.Walks     &= ~PhaseMask;
                    PhaseStatus.PedClears |=  PhaseMask;
                    PhaseStatus.DontWalks &= ~PhaseMask;
                }
                else
                {
                    PhaseStatus.Walks     &= ~PhaseMask;
                    PhaseStatus.PedClears &= ~PhaseMask;
                    PhaseStatus.DontWalks |=  PhaseMask;
                }
            }
        }
    }
    PhaseGreenCount();
    PhaseStatus.PhaseNexts = GetPhaseNexts();
}

//������λ״̬����
void OverlapStatusControl(void) //1Sˢ��һ��
{
    uint8_t i;
    uint32_t    OverlapMask = 0x1;
    for(i = 0; i < OverlapMax; i++)//��������ͨ������Դ
    {
        if(OverlapTab.Overlap[i].Num > 0 && OverlapTab.Overlap[i].Num <= OverlapMax)
        {
            OverlapMask = (0x1<<(OverlapTab.Overlap[i].Num - 1));
            if(OverlapTab.Overlap[i].Type == OT_NORMAL || (OverlapTab.Overlap[i].Type == OT_MINUSGREENYELLOW && ModifierPhases[i] == 0))//������λ����Ϊ����
            {
                //printf("Overlap[%d].Type = OT_NORMAL",i);
                //printf("IncludedPhases[i] = %d",IncludedPhases[i]);
                //printf("PhaseOns = %d",PhaseStatus.PhaseOns);
                //printf("Overlap[%d] is active ",i);
                //printf("Overlap[%d].Num = %d is active ",i,OverlapTab.Overlap[i].Num);
                
                if(IncludedPhases[i] & PhaseStatus.PhaseOns)
                {
                    if(OverlapTab.Overlap[i].TrailGreen == 0)//��Ҫ�ж�PhaseNexts�Ƿ�Ҳ��ĸ��λ
                    {
                        //printf("PhaseNexts = %04x",PhaseStatus.PhaseNexts);
                        if(IncludedPhases[i] & PhaseStatus.PhaseNexts)
                        {
                            OverlapStatus.Greens    |= OverlapMask;
                            OverlapStatus.Yellows   &=~OverlapMask;
                            OverlapStatus.Reds      &=~OverlapMask;
                            OverlapStatus.Flashs    &=~OverlapMask;
                        }
                        else
                        {
                            if(IncludedPhases[i] & PhaseStatus.Greens)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                            }
                            else if(IncludedPhases[i] & PhaseStatus.Yellows)
                            {
                                OverlapStatus.Yellows   |= OverlapMask;
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                            }
                            else
                            {
                                OverlapStatus.Reds      |= OverlapMask;
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                            }
                            if(IncludedPhases[i] & PhaseStatus.VehClears)
                                OverlapStatus.Flashs    |= OverlapMask;
                            else
                                OverlapStatus.Flashs    &=~OverlapMask;
                        }
                    }
                    else//ĸ��λ����ʱ����������̵Ƶ��̵�ʱ��
                    {
                        if(IncludedPhases[i] & PhaseStatus.Greens)
                        {
                            OverlapCounter[i] = 1 + OverlapTab.Overlap[i].TrailGreen + OverlapTab.Overlap[i].TrailClear + OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed;
                            OverlapStatus.Greens    |= OverlapMask;
                            OverlapStatus.Yellows   &=~OverlapMask;
                            OverlapStatus.Reds      &=~OverlapMask;
                            OverlapStatus.Flashs    &=~OverlapMask;                        
                        }
                        else if(OverlapCounter[i])
                        {
                            OverlapCounter[i]--;
                            if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailClear + OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    |= OverlapMask;
                            }
                            else if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   |= OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else
                            {
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      |= OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                        }
                    }
                }
                else//ĸ��λ�ر�
                {
                    if(OverlapTab.Overlap[i].TrailGreen == 0)
                    {
                        OverlapStatus.Reds      |= OverlapMask;
                        OverlapStatus.Yellows   &=~OverlapMask;
                        OverlapStatus.Greens    &=~OverlapMask;
                    }
                    else
                    {
                        if(OverlapCounter[i])  
                        {
                            OverlapCounter[i]--;
                            if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailClear + OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    |= OverlapMask;
                            }
                            else if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   |= OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else
                            {
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      |= OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                        }
                    }
                }
            }
            else if(OverlapTab.Overlap[i].Type == OT_MINUSGREENYELLOW)
            {   //��ĸ��λΪ��ɫ,������λ��Ϊ��ɫ����ô������λΪ��ɫ
                if((ModifierPhases[i] & PhaseStatus.Greens)==0)//��������λ��Ϊ��ɫ
                {
                    if((IncludedPhases[i] & PhaseStatus.Greens))//��ĸ��λΪ��ɫ
                    {
                        if(OverlapTab.Overlap[i].TrailGreen == 0)
                        {
                            OverlapStatus.Greens    |= OverlapMask;
                            OverlapStatus.Yellows   &=~OverlapMask;
                            OverlapStatus.Reds      &=~OverlapMask;
                            if(IncludedPhases[i] & PhaseStatus.PhaseNexts)
                            {
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else
                            {
                                if(IncludedPhases[i] & PhaseStatus.VehClears)
                                    OverlapStatus.Flashs    |= OverlapMask;
                                else
                                    OverlapStatus.Flashs    &=~OverlapMask;
                            }
                        }
                        else
                        {
                            OverlapCounter[i] = 1 + OverlapTab.Overlap[i].TrailGreen + OverlapTab.Overlap[i].TrailClear + OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed;
                            OverlapStatus.Greens    |= OverlapMask;
                            OverlapStatus.Yellows   &=~OverlapMask;
                            OverlapStatus.Reds      &=~OverlapMask;
                            OverlapStatus.Flashs    &=~OverlapMask;
                        }
                    }
                    //��ĸ��λ�����κ�һ����λΪ��ɫ������λ��Ϊĸ��λ����������λ��Ϊ��ɫʱ��������λΪ��ɫ��
                    else if((IncludedPhases[i] & PhaseStatus.Yellows))//��ĸ��λΪ��ɫ
                    {
                        if((IncludedPhases[i] & PhaseStatus.PhaseNexts)==0 && (ModifierPhases[i] & PhaseStatus.Yellows)==0)
                        {
                            OverlapStatus.Greens    &=~OverlapMask;
                            OverlapStatus.Yellows   |= OverlapMask;
                            OverlapStatus.Reds      &=~OverlapMask;
                            OverlapStatus.Flashs    &=~OverlapMask;
                        }
                    }
                    else
                    {
                        if(OverlapTab.Overlap[i].TrailGreen>0 && OverlapCounter[i]>0)  
                        {
                            OverlapCounter[i]--;
                            if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailClear + OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailYellow + OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    |= OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    |= OverlapMask;
                            }
                            else if(OverlapCounter[i] > OverlapTab.Overlap[i].TrailRed)
                            {
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   |= OverlapMask;
                                OverlapStatus.Reds      &=~OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                            else
                            {
                                OverlapStatus.Greens    &=~OverlapMask;
                                OverlapStatus.Yellows   &=~OverlapMask;
                                OverlapStatus.Reds      |= OverlapMask;
                                OverlapStatus.Flashs    &=~OverlapMask;
                            }
                        }
                        else
                        {
                            OverlapStatus.Greens    &=~OverlapMask;
                            OverlapStatus.Yellows   &=~OverlapMask;
                            OverlapStatus.Reds      |= OverlapMask;
                            OverlapStatus.Flashs    &=~OverlapMask;
                        }
                    }
                }
                else
                {
                    OverlapStatus.Greens    &=~OverlapMask;
                    OverlapStatus.Yellows   &=~OverlapMask;
                    OverlapStatus.Reds      |= OverlapMask;
                    OverlapStatus.Flashs    &=~OverlapMask;
                }
            }
        }
    }
}

//ͨ��״̬ˢ��
void ChannelStatusControl(void)//1Sˢ��һ��
{
    uint8_t  i;
    uint32_t PhaseMask;
    uint32_t ChannelMask = 0x1;
    
    ChannelStatus.Reds      = 0;
    ChannelStatus.Yellows   = 0;
    ChannelStatus.Greens    = 0;
    ChannelStatus.Flash     = 0;
    
    for(i = 0; i < ChannelMax; i++)//��������ͨ������Դ
    {
        //��λ��Ϊ0 ���� ��������Ϊ���� ��ʾNo Control (Not In Use)
        //if(ChannelTab.Channel[i].ControlSource == 0 || ChannelTab.Channel[i].ControlType == CCT_OTHER) continue;
        if(ChannelTab.Channel[i].ControlSource == 0)
        {
            ChannelMask <<= 1;
            continue;
        }
        PhaseMask = (0x1 << (ChannelTab.Channel[i].ControlSource-1));
        
        if(ChannelTab.Channel[i].ControlType == CCT_VEHICLE) //Vehicle��������Ϊ������
        {
            if((PhaseStatus.PhaseOns & PhaseMask) == 0) //��ͨ����Ӧ�Ŀ���Դ��λ�ر�
            {
                ChannelStatus.Reds      |= ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
            else //��ͨ����Ӧ�Ŀ���Դ��λ����
            {
                if(PhaseStatus.Reds & PhaseMask) //��λ״̬Ϊ��
                {
                    ChannelStatus.Reds      |= ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
                else if(PhaseStatus.Yellows & PhaseMask) //��λ״̬Ϊ��
                {
                    ChannelStatus.Yellows   |= ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
                else if(PhaseStatus.Greens & PhaseMask) //��λ״̬Ϊ��
                {
                    ChannelStatus.Greens    |= ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;                
                    if(PhaseStatus.VehClears & PhaseMask)
                        ChannelStatus.Flash |= ChannelMask;
                    else
                        ChannelStatus.Flash &=~ChannelMask;
                }
                else //��λ״̬�ر�
                {
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
            }
        }
        else if(ChannelTab.Channel[i].ControlType == CCT_PEDESTRIAN)//Pedestrian
        {
            if((PhaseStatus.PhaseOns & PhaseMask) == 0)
            {
                ChannelStatus.Reds      |= ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
            else
            {
                if(PhaseStatus.DontWalks & PhaseMask)//��λ״̬Ϊ��
                {
                    ChannelStatus.Reds      |= ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
                else if(PhaseStatus.PedClears & PhaseMask)//��λ״̬Ϊ����
                {
                    ChannelStatus.Greens    |= ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;                
                    ChannelStatus.Flash     |= ChannelMask;
                }
                else if(PhaseStatus.Walks & PhaseMask)//��λ״̬Ϊ��
                {
                    ChannelStatus.Greens    |= ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
            }
        }
        else if(ChannelTab.Channel[i].ControlType == CCT_OVERLAP)
        {
            if(OverlapStatus.Reds & PhaseMask)//��λ״̬Ϊ��
            {
                ChannelStatus.Reds      |= ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
            else if(OverlapStatus.Yellows & PhaseMask)//��λ״̬Ϊ��
            {
                ChannelStatus.Yellows   |= ChannelMask;
                ChannelStatus.Reds      &=~ChannelMask;
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
            else if(OverlapStatus.Greens & PhaseMask)//��λ״̬Ϊ��
            {
                ChannelStatus.Greens    |= ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;  
                ChannelStatus.Reds      &=~ChannelMask;
                if(OverlapStatus.Flashs & PhaseMask)
                    ChannelStatus.Flash |= ChannelMask;
                else
                    ChannelStatus.Flash &=~ChannelMask;
            }
            else//��λ״̬�ر�
            {
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;
                ChannelStatus.Reds      |= ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
        }
        else if(ChannelTab.Channel[i].ControlType == CCT_GREEN)
        {
            ChannelStatus.Greens    |= ChannelMask;
            ChannelStatus.Reds      &=~ChannelMask;
            ChannelStatus.Yellows   &=~ChannelMask;
            ChannelStatus.Flash     &=~ChannelMask;
        }
        else if(ChannelTab.Channel[i].ControlType == CCT_RED)
        {
            ChannelStatus.Reds      |= ChannelMask;
            ChannelStatus.Greens    &=~ChannelMask;
            ChannelStatus.Yellows   &=~ChannelMask;
            ChannelStatus.Flash     &=~ChannelMask;
        }
        else if(ChannelTab.Channel[i].ControlType == CCT_FLASH)
        {
            if(ChannelTab.Channel[i].Flash == CFM_Yellow)          //����
            {
                ChannelStatus.Yellows |= ChannelMask;
            }
            else if(ChannelTab.Channel[i].Flash == CFM_Red)        //����
            {
                ChannelStatus.Reds |= ChannelMask;
            }
            else if(ChannelTab.Channel[i].Flash == CFM_Alternate)  //����
            {
                ChannelStatus.Reds |= ChannelMask;
                ChannelStatus.Yellows |= ChannelMask;
            }
            ChannelStatus.Flash |= ChannelMask;
        }
        else if(ChannelTab.Channel[i].ControlType == CCT_OTHER)
        {
            if((PhaseStatus.PhaseOns & PhaseMask) == 0)
            {
                ChannelStatus.Reds      |= ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
            else
            {
                if(PhaseStatus.Reds & PhaseMask)//��λ״̬Ϊ��
                {
                    ChannelStatus.Reds      |= ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
                else if(PhaseStatus.Yellows & PhaseMask)//��λ״̬Ϊ��
                {
                    ChannelStatus.Yellows   |= ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
                else if(PhaseStatus.Greens & PhaseMask)//��λ״̬Ϊ��
                {
                    ChannelStatus.Greens    |= ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;                
                    if(PhaseStatus.VehClears & PhaseMask)
                        ChannelStatus.Flash |= ChannelMask;
                    else
                        ChannelStatus.Flash &=~ChannelMask;
                }
                else//��λ״̬�ر�
                {
                    ChannelStatus.Greens    &=~ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Flash     &=~ChannelMask;
                }
            }
        }
        ChannelMask <<= 1;
    }
}

void ChannelStatusToLmap(void)
{
    LampControl(PhaseState.Phase10msCount); 
}





