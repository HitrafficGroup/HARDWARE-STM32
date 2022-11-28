/*
*********************************************************************************************************
*
*	ģ������ : ��λģ��
*	�ļ����� : Phase.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "public.h"
#include "Phase.h"

PhaseTable          PhaseTab;       //��λ��
PhaseStatusType     PhaseStatus;
PhaseControlType    PhaseControl;
PhaseIndexType      RingPhase[RingMax];

uint32_t            PhaseTimes[32];

//��λ�� + ��λ�� = ��λ����
uint8_t GetPhaseIndex(PhaseTable* Phase_Tab, uint8_t PhaseNum)
{
    uint8_t     i;
    uint8_t     temp = 0xff;

    for(i = 0; i < PhaseMax; i++)
    {
        if(Phase_Tab->Phase[i].Num == PhaseNum)
        {
            temp = i;
            break;
        }
    }
    
    return temp;   //PhaseTab.Phase[temp].PhaseNum
}

//Ĭ�ϵ���λ����
void PhaseDefault(void)
{
    uint8_t i;
    memset(&PhaseTab.Maximum,0x00,sizeof(PhaseTab));
    PhaseTab.Maximum = 32;
    
    for(i = 0; i <= 4; i++)
    {
        PhaseTab.Phase[i].Num = i + 1;
        PhaseTab.Phase[i].Walk = 10;
        PhaseTab.Phase[i].PedestrianClear = 6;
        PhaseTab.Phase[i].MinimumGreen = 6;
        PhaseTab.Phase[i].Passage = 3;
        PhaseTab.Phase[i].Maximum1 = 30;
        PhaseTab.Phase[i].Maximum2 = 40;
        PhaseTab.Phase[i].YellowChange = 3;
        PhaseTab.Phase[i].RedClear = 2;
        PhaseTab.Phase[i].RedRevert = 0;
        PhaseTab.Phase[i].AddedInitial = 0;
        PhaseTab.Phase[i].MaximumInitial = 0;
        PhaseTab.Phase[i].TimeBeforeReduction = 0;
        PhaseTab.Phase[i].CarsBeforeReduction = 0;
        
        PhaseTab.Phase[i].TimeToReduce = 0;
        PhaseTab.Phase[i].ReduceBy = 0;
        PhaseTab.Phase[i].MinimumGap = 0;
        PhaseTab.Phase[i].DynamicMaxLimit = 0;
        PhaseTab.Phase[i].DynamicMaxStep = 0;
        
        PhaseTab.Phase[i].Startup = 1;//δ����
        PhaseTab.Phase[i].OptionsH = 0;
        PhaseTab.Phase[i].OptionsL = 0;
        PhaseTab.Phase[i].Ring = 1;
        PhaseTab.Phase[i].ConcurrencyH = 0xff;
        PhaseTab.Phase[i].ConcurrencyL = 0xff;
        PhaseTab.Phase[i].VehicleClear = 3;//26-�Լ����ӵĻ�������ʱ������
    }
}

void PhaseStatusInit(void)
{
    memset(&PhaseStatus,0x00,sizeof(PhaseStatus));
}
