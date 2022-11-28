#ifndef ACTION_H
#define ACTION_H

#include "public.h"

/* ����NTCIP������ҪWorkMode��ֻ��Ҫһ��Status�����޸� */
typedef struct
{
    uint8_t Num;                  //1 ������1-255
    uint8_t Pattern;              //2 ������0-255
    uint8_t AuxillaryFunction;    //3 ��������
    uint8_t SpecialFunction;      //4 ���⹦��
}ActionType;    //������ ��Ϣ

typedef struct
{
    uint8_t Maximum;
    ActionType Action[ActionMax];     //4 * 100
    uint8_t Reserve[15];
}ActionTable;    //������ ��Ϣ  416 = 0x01A0

extern ActionType   Action;         //��ǰ���ж���
extern ActionTable  ActionTab;      //������
extern uint8_t      ActionStatus;

uint8_t GetActionIndex(ActionTable* Action_tab, uint8_t ActionNum);
void ActionDataInit(uint8_t n);
void ActionDefault(void);

#endif
