/*
*********************************************************************************************************
*
*	ģ������ : ������λģ��
*	�ļ����� : Overlap.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "public.h"
#include "Overlap.h"

uint16_t    OverlapCounter[OverlapMax] = {0};     //������λʱ��ͳ��
uint16_t    IncludedPhases[OverlapMax];     //ĸ��λ ��������еĻ�������λ
uint16_t    ModifierPhases[OverlapMax];     //������λ 

OverlapTable        OverlapTab; //�����λ��
OverlapStatusType   OverlapStatus;

void OverlapInit(void)
{
    memset(&OverlapTab.Maximum,0x00,sizeof(OverlapTab));
    OverlapTab.Maximum = OverlapMax;
}

//������λ�� + ������λ�� = ������λ����
uint8_t GetOverlapIndex(OverlapTable* Overlap_tab, uint8_t OverlapNum)
{
    uint8_t     i;
    uint8_t     temp = 0xff;
    
    for(i = 0; i < OverlapMax; i++)
    {
        if(Overlap_tab->Overlap[i].Num == OverlapNum)
        {
            temp = i;
            break;
        }
    }
    
    return temp;
}


