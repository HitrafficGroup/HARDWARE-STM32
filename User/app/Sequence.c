/*
*********************************************************************************************************
*
*	ģ������ : ����ģ��
*	�ļ����� : Sequence.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "public.h"
#include "Sequence.h"

//��ǰ���е� �� ��һ������ ���� ���� 
uint8_t         RingStatus[RingMax];
SequenceType    SequenceNow;
SequenceTable   SeqTab;   //�����


/*
    ����������ָ��
    ���ܣ��жϸ������ж��ٸ���λ
          ����Ļ��Ų��Ϸ���������λ����0
*/
uint8_t GetSeqMax(RingType* Ring)
{
    uint8_t i,phaseMax = 0;
    if(IsRing(Ring->RingNum))
    {
        for(i=0;i<PhaseMax;i++)
        {
            if(IsPhase(Ring->Phase[i]))
                phaseMax++;
            else
                break;
        }
    }

    return phaseMax;
}

void SequenceDefault(void)
{
    memset(&SeqTab.Maximum,0x00,sizeof(SeqTab));
    SeqTab.Maximum = SequenceMax;
    
    SeqTab.Seq[0].Num = 1;
    SeqTab.Seq[0].Ring[0].RingNum = 1;
    SeqTab.Seq[0].Ring[1].RingNum = 0;
    SeqTab.Seq[0].Ring[2].RingNum = 0;
    SeqTab.Seq[0].Ring[3].RingNum = 0;
    
    SeqTab.Seq[0].Ring[0].Phase[0] = 1;
    SeqTab.Seq[0].Ring[0].Phase[1] = 2;
    SeqTab.Seq[0].Ring[0].Phase[2] = 3;
    SeqTab.Seq[0].Ring[0].Phase[3] = 4;
    SeqTab.Seq[0].Ring[0].Phase[4] = 5;
    
    
    SeqTab.Seq[1].Num = 2;
    SeqTab.Seq[1].Ring[0].RingNum = 1;
    SeqTab.Seq[1].Ring[1].RingNum = 0;
    SeqTab.Seq[1].Ring[2].RingNum = 0;
    SeqTab.Seq[1].Ring[3].RingNum = 0;
    
    SeqTab.Seq[1].Ring[0].Phase[0] = 1;
    SeqTab.Seq[1].Ring[0].Phase[1] = 2;
    SeqTab.Seq[1].Ring[0].Phase[2] = 5;
}

//����Sequence��Ĭ������
//ֻ��һ��������1����5����λ
void SequenceXDataInit(SequenceType* Sequence)
{
    Sequence->Num = 1;
    Sequence->Ring[0].RingNum = 1;//��1��Ч
    Sequence->Ring[1].RingNum = 0;//ֻ��1��������������Ч
    Sequence->Ring[2].RingNum = 0;
    Sequence->Ring[3].RingNum = 0;
    
    Sequence->Ring[0].Phase[0] = 1;
    Sequence->Ring[0].Phase[1] = 2;
    Sequence->Ring[0].Phase[2] = 3;
    Sequence->Ring[0].Phase[3] = 4;
    Sequence->Ring[0].Phase[4] = 5;
    Sequence->Ring[0].Phase[5] = 0;
}
