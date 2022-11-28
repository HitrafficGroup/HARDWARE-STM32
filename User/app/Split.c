/*
*********************************************************************************************************
*
*	ģ������ : ���ű�ģ��
*	�ļ����� : Split.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "public.h"
#include "Split.h"


SplitType       SplitNow;   //��ǰ���е� ���ű� ����
SplitTable      SplitTab;   //���űȱ�
PhaseSplitType  RingSplit[RingMax];    //������ǰ���ű�




//��λ�� + ���ű����� = ���ű���λ����
uint8_t GetSplitPhaseIndex(SplitType* Split, uint8_t PhaseNum)
{
    uint8_t     i;
    uint8_t     temp = 0xff;
    
    for(i = 0; i < PhaseMax; i++)
    {
        if(Split->Phase[i].PhaseNum == PhaseNum)
        {
            temp = i;
            break;
        }
    }
    
    return temp;
}

void SplitDefault(void)
{
    uint8_t i;
    
    memset(&SplitTab.Maximum,0x00,sizeof(SplitTab));
    SplitTab.Maximum = SplitMax;

    SplitTab.Split[0].Num = 1;
    for(i = 0; i <= 4; i++)//Ĭ������5����λ
    {
        SplitTab.Split[0].Phase[i].PhaseNum = i + 1;//
        SplitTab.Split[0].Phase[i].Time = 16;       //��λ�ķ���ʱ�䡣�����̵ơ��������Ƶơ�ȫ��ʱ��
        SplitTab.Split[0].Phase[i].Mode = SM_None;  //���ű�ģʽ 2-��
        SplitTab.Split[0].Phase[i].Coord = SC_FIXED;//Э������Coord bit2��1-��Ϊ�̶���λ
    }
}

//Ĭ�ϵ����ű�����
//���ű�1��Ĭ������
void SplitDataInit(uint8_t n)
{
    uint8_t i;
    SplitTab.Split[n].Num = n + 1;
    for(i = 0; i <= 4; i++)//Ĭ������5����λ
    {
        SplitTab.Split[n].Phase[i].PhaseNum = i + 1;//
        SplitTab.Split[n].Phase[i].Time = 18;       //��λ�ķ���ʱ�䡣�����̵ơ��������Ƶơ�ȫ��ʱ��
        SplitTab.Split[n].Phase[i].Mode = SM_None;  //���ű�ģʽ 2-��
        SplitTab.Split[n].Phase[i].Coord = SC_FIXED;//Э������Coord bit2��1-��Ϊ�̶���λ
    }
}

void SplitXDataInit(SplitType* Split)
{
    uint8_t n;
    Split->Num = 1;
    for(n=0;n<=4;n++)
    {
        Split->Phase[n].PhaseNum = n + 1;//
        Split->Phase[n].Time = 18;      //��λ�ķ���ʱ�䡣�����˻�������λ���̵ơ��������Ƶơ�ȫ��ʱ��
                                            //�Լ�������λ�ķ���ʱ������ʱ�䡣
        Split->Phase[n].Mode = SM_None;  //���ű�ģʽ 2-��
        Split->Phase[n].Coord = SC_FIXED;//Э������Coord bit2��1-��Ϊ�̶���λ
    }
}

