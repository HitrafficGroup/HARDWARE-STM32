/*
*********************************************************************************************************
*
*	ģ������ : ͨ��ģ��
*	�ļ����� : Channel.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-30  wcx     �׷�
*
*********************************************************************************************************
*/
#include "Channel.h"
#include "bsp_io.h"
#include "public.h"

ChannelTable            ChannelTab;     //ͨ����
ChannelStatusType       ChannelStatus;  //ͨ��״̬��     �������Լ���ChannelStateTab
ChannelReadStatusType   ChannelReadStatus;

//Ĭ�ϵ�ͨ������
void ChannelInit(void)
{
    uint8_t i,groupNum,boardNum;
    ChannelTab.Maximum = ChannelMax;       //���ͨ���������32
    for(i = 0; i < ChannelTab.Maximum; i++)
    {
        ChannelTab.Channel[i].Num = i + 1;
        groupNum = i%4;
        boardNum = i/4;
        if(groupNum <= 2)
        {
            ChannelTab.Channel[i].ControlSource = boardNum + 1;   //��λ��
            ChannelTab.Channel[i].ControlType = CCT_VEHICLE;      //��������
            ChannelTab.Channel[i].Flash = CFM_Yellow;             //����ģʽ
            ChannelTab.Channel[i].Dim = CDM_Green;                //�Զ�ģʽ
        }
        else
        {
            ChannelTab.Channel[i].ControlSource = 5;              //��λ��
            ChannelTab.Channel[i].ControlType = CCT_PEDESTRIAN;   //��������
            ChannelTab.Channel[i].Flash = CFM_Red;                //����ģʽ
            ChannelTab.Channel[i].Dim = CDM_Green;                //�Զ�ģʽ
        }
        ChannelTab.Channel[i].Position = boardNum + 1;
        ChannelTab.Channel[i].Direction = groupNum + 1;
        ChannelTab.Channel[i].CountdownID = i;
    }
}

uint8_t isPedPhase(uint8_t phaseNum)
{
    uint8_t i;
    for(i = 0; i < ChannelTab.Maximum; i++)
    {
        //����Դ��ͬ��Ϊ������λ
        if(ChannelTab.Channel[i].ControlSource == phaseNum && 
           ChannelTab.Channel[i].ControlType == CCT_PEDESTRIAN)
        {
            return 1;
        }
    }
    return 0;
}

uint8_t isVehPhase(uint8_t phaseNum)
{
    uint8_t i;
    for(i = 0; i < ChannelTab.Maximum; i++)
    {
        //����Դ��ͬ��Ϊ������λ
        if(ChannelTab.Channel[i].ControlSource == phaseNum && 
           ChannelTab.Channel[i].ControlType == CCT_VEHICLE)
        {
            return 1;
        }
    }
    return 0;
}

uint8_t isVehChannel(uint8_t channelNum)
{
    if(ChannelTab.Channel[channelNum].ControlSource > 0 && ChannelTab.Channel[channelNum].ControlType == CCT_VEHICLE)
    {
        return 1;
    }
    return 0;
}



uint32_t GetAppointChannel(uint8_t Pos, uint8_t Dir)
{
    uint8_t i;
    uint32_t ChannelMask = 0x1;
    uint32_t AppointChannel = 0;

    for(i = 0; i < ChannelTab.Maximum; i++)
    {
        if(ChannelTab.Channel[i].ControlSource != 0)//ͨ������
        {
            if(ChannelTab.Channel[i].Position > 0 && ChannelTab.Channel[i].Direction > 0)
            {
                if(((0x01 << (ChannelTab.Channel[i].Position-1))&Pos) && ((0x01 << (ChannelTab.Channel[i].Direction-1))&Dir))//��ָ��������ͬ
                {
                    AppointChannel |= ChannelMask;
                }
            }
        }
        ChannelMask <<= 1;
    }
    return AppointChannel;
}




//����ģʽ���
void AutoFlashMode(void)
{
    uint8_t i;
    uint32_t ChannelMask = 0x1;
    ChannelStatus.Greens = 0;
    ChannelStatus.Yellows = 0;
    ChannelStatus.Reds = 0;
    ChannelStatus.Flash = 0;
    for(i = 0; i < ChannelMax; i++)
    {
        if(ChannelTab.Channel[i].ControlSource != 0)
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
        ChannelMask <<= 1;
    }
}

void AutoAllRedMode(void)
{
    uint8_t i;
    uint32_t ChannelMask = 0x1;
    ChannelStatus.Greens = 0;
    ChannelStatus.Yellows = 0;
    ChannelStatus.Reds = 0;
    ChannelStatus.Flash = 0;
    for(i = 0; i < ChannelMax; i++)
    {
        if(ChannelTab.Channel[i].ControlSource != 0)
        {
            ChannelStatus.Reds |= ChannelMask;
        }
        ChannelMask <<= 1;
    }
}

void AutoLampOffMode(void)
{
    ChannelStatus.Greens = 0;
    ChannelStatus.Yellows = 0;
    ChannelStatus.Reds = 0;
    ChannelStatus.Flash = 0;
}


//ͨ��״̬ˢ�µ�������
enum LampDrive_Type ChannelStatusToDrivereg(uint32_t ChannelMask, uint8_t tick10msCount)
{
    enum LampDrive_Type temp;
    if(ChannelStatus.Flash & ChannelMask)
    {
        if(ChannelStatus.Greens & ChannelMask)
        {
            if(tick10msCount < 50)
                temp = LD_BLACK;
            else
                temp = LD_GREEN;
        }
        else if(ChannelStatus.Yellows & ChannelStatus.Reds & ChannelMask)
        {
            if(tick10msCount < 50)
                temp = LD_YELLOW;
            else
                temp = LD_RED;
        }
        else if(ChannelStatus.Reds & ChannelMask)
        {
            if(tick10msCount < 50)
                temp = LD_BLACK;
            else
                temp = LD_RED;
        }
        else //if(ChannelStatus.Yellows & ChannelMask)//�����Ʊ�־,δְλ��ɫ,Ĭ�ϻƵ�
        {
            if(tick10msCount < 50)
                temp = LD_BLACK;
            else
                temp = LD_YELLOW;
        }
    }
    else
    {
        if(ChannelStatus.Greens & ChannelMask)
        {
            temp = LD_GREEN;
        }
        else if(ChannelStatus.Yellows & ChannelMask)
        {
            temp = LD_YELLOW;
        }
        else if(ChannelStatus.Reds & ChannelMask)
        {
            temp = LD_RED;
        }
        else temp = LD_BLACK;
    }
    
    return temp;
}

void LampControl(uint8_t tick10msCount)//10msˢ��һ��
{
    uint8_t i;
    uint32_t ChannelMask = 0X1;
    enum LampDrive_Type temp[4];
    
    for(i = 0; i < ChannelMax/4; i++)//��������ͨ������Դ
    {
        temp[0] = ChannelStatusToDrivereg(ChannelMask, tick10msCount);
        ChannelMask <<= 1;
        temp[1] = ChannelStatusToDrivereg(ChannelMask, tick10msCount);
        ChannelMask <<= 1;
        temp[2] = ChannelStatusToDrivereg(ChannelMask, tick10msCount);
        ChannelMask <<= 1;
        temp[3] = ChannelStatusToDrivereg(ChannelMask, tick10msCount);
        ChannelMask <<= 1;
        
        LampDriveReg[i].drivereg.group1 = temp[0];
        LampDriveReg[i].drivereg.group2 = temp[1];
        LampDriveReg[i].drivereg.group3 = temp[2];
        LampDriveReg[i].drivereg.group4 = temp[3];
        LampDriveReg[i].drivereg.run = (tick10msCount < 3)?1:0;
    }
    
    LampDriveOut();
}

