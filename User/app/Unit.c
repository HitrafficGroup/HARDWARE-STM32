/*
*********************************************************************************************************
*
*	ģ������ : ����ģ��
*	�ļ����� : Plan.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "Unit.h"

UnitTab   Unit;    //��Ԫ����

void UnitInit(void)
{
    memset(&Unit.StartupFlash,0x00,sizeof(Unit));
    Unit.StartupFlash = 6;
    Unit.StartupAllRed = 5;
    Unit.AutomaticPedClear = 2;
    Unit.RedRevert = 6;
    Unit.BackupTimeH = 2;
    Unit.BackupTimeL = 0;
    Unit.GreenConflictDetectFlag = 0;
    Unit.RedGreenConflictDetectFlag = 1;
    Unit.RedFailedDetectFlag = 0;
}

