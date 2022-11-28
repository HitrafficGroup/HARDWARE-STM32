#ifndef SEQUENCE_H
#define SEQUENCE_H
#include "public.h"


/* RingStatus��״̬���� */
#define ForceOFF    0x20
#define MaxOut      0x10
#define GapOut      0x08

typedef enum
{
    MinGreen = 0, 
    Extension, 
    Maximum, 
    GreenRest, 
    YellowChange, 
    RedClearance, 
    RedRest
}RingStatus_Type;

/******************************************************************************/
typedef struct 
{
    uint8_t RingNum;            //����
    uint8_t Phase[PhaseMax];    //������
}RingType;  //��������   17 byte

typedef struct
{
    uint8_t     Num;            //�����
    RingType    Ring[RingMax];  //�������� 4����
}SequenceType;        //�������Ϣ 69 byte

typedef struct
{
    uint8_t         Maximum;
    SequenceType    Seq[SequenceMax];     //16
    uint8_t         Reserve[15];
}SequenceTable;      //�����  16 + (17*4+1)*16 = 1120 = 0x0460

/******************************************************************************/
typedef struct
{
    uint8_t StopTime;   //bit1,ֹͣ��ʱ
    uint8_t ForceOff;   //bit1,ǿ�ƹ�ͣ
    uint8_t Max2;       //
    uint8_t MaxInhibit;
    uint8_t PedRecycle;
    uint8_t RedRest;
    uint8_t OmitRedClear;
}RingControlType;
/******************************************************************************/







extern uint8_t          RingStatus[RingMax];
extern SequenceType     SequenceNow;
extern SequenceTable    SeqTab;   //�����


uint8_t GetSeqMax(RingType* Ring);

void SequenceDefault(void);

#endif
