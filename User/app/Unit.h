#ifndef UNIT_H
#define UNIT_H
#include "public.h"


typedef struct
{
    uint8_t     StartupFlash;       //��������ʱ�� 0-255
    uint8_t     StartupAllRed;      //����ȫ��ʱ�� 0-255
    uint8_t     AutomaticPedClear;  //1���� 2��    �����Զ��������ʱ��
    uint8_t     RedRevert;          //��С���ʱ�� 0-255
    uint8_t     BackupTimeL;
    uint8_t     BackupTimeH;        //ϵͳ���õı���ʱ�� 0-65535
    uint8_t     FlowCycle;          //�����ɼ�����
    uint8_t     FlashStatus;
    uint8_t     Status;
    uint8_t     GreenConflictDetectFlag;
    uint8_t     RedGreenConflictDetectFlag;
    uint8_t     RedFailedDetectFlag;
    uint8_t     Reserve[4];
}UnitTab;   //16


extern UnitTab    Unit;


void UnitInit(void);



#endif
