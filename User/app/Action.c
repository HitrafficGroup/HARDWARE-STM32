

/*
*********************************************************************************************************
*
*	ģ������ : ����ģ��
*	�ļ����� : Action.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "public.h"
#include "Action.h"
#include "Tsc.h"
#include "bsp.h"


ActionType  Action;         //��ǰ���ж��� 
ActionTable ActionTab;     //������
uint8_t     ActionStatus;


//������ + ������ = �����������
uint8_t GetActionIndex(ActionTable* Action_tab, uint8_t ActionNum)
{
    uint8_t     i;
    uint8_t     temp = 0xff;
    
    for(i = 0; i < ActionMax; i++)
    {
        if(Action_tab->Action[i].Num == ActionNum)
        {
            temp = i;
            break;
        }
    }
    return temp;   //ActionTab.Action[temp].Pattern;
}

//Ĭ�ϵĶ�������
//������1ָ�������з���1���޸������ܣ������⹦��
void ActionDataInit(uint8_t n)
{
    ActionTab.Action[n].Num = n + 1;
    ActionTab.Action[n].Pattern = 1;
    ActionTab.Action[n].AuxillaryFunction = 1;
    ActionTab.Action[n].SpecialFunction = 1;
}

void ActionDefault(void)
{
    memset(&ActionTab.Maximum,0x00,sizeof(ActionTab));
    
    ActionTab.Maximum = ActionMax;
    ActionTab.Action[0].Num = 1;
    ActionTab.Action[0].Pattern = 1;
    ActionTab.Action[0].AuxillaryFunction = 1;
    ActionTab.Action[0].SpecialFunction = 1;
    
    ActionTab.Action[1].Num = 2;
    ActionTab.Action[1].Pattern = 2;
    ActionTab.Action[1].AuxillaryFunction = 1;
    ActionTab.Action[1].SpecialFunction = 1;
}

