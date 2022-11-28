/*
*********************************************************************************************************
*
*	ģ������ : �źŻ����п���
*	�ļ����� : tsc.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2019-12-04 wcx     �׷�
*
*********************************************************************************************************
*/

#include "bsp.h" 				/* �ײ�Ӳ������ */
#include "public.h"
#include "SchedulePlanMange.h"
#include "PhaseStateControl.h"

/*
    ������������
    ���ȼƻ��� + ���� �� ʱ�α�� 
    ʱ�α� + ʱ��     �� ������
    ������            �� ������ + ���� + ���� 
    ������            �� ����� + ���űȺ� + ��λ�� + ����������ڣ�+�� + ģʽ ����
    �����            �� ���еĻ� + �������� (����ǿ������,����λ��ѡ) 
    ���űȱ�          �� ����λʱ�� + ģʽ + Э��
    ��λ��            �� ��λ��ʱ������ + ��Ӧ���� + ���˲��� + ����ѡ��
    ͨ����            �� �������� + ����Դ + ���� + �Զ�
    
    ���ݸı������
    1�����ڸı䣺����ʱ�α���Ƿ�ı�        ��      ʱ�α�ı��־
    2��ʱ����Ӹı䣺�����������Ƿ�ı�      ��      �����ı��־
    3�������ı䣺�ж϶��������Ƿ�ı�        ��      
    
    4�����ں�ʱ�α������б仯��ֱ��Ӧ���µ����ݣ���Ϊ��Щ���ݱ仯���������źſ��Ƶ�ͻȻ�仯��
    5��ͨ�������ı䣬������Ч
    
    �źŻ��ڴ��д洢��Ϣ������
    1�����ȼƻ���       ����
    2��ʱ�α�           1��
    3��������           1��
    4��������           1��
    5�������           1��
    6�����ű�           1��
    7����λ��           ����
    8��ͨ����           ����
    
    �źŻ��������̷�����
    1��ÿ��Ŀ�ʼ �� �źŻ���������ʱ����ȡ���ڣ��ж����ڵ�׼ȷ�ԣ��� ���ȼƻ����ж����ȼ�����ȡ ʱ�α��;
    2����ȡʱ�α�����
*/

/* �������ݽṹ���� */
OPType              OP;
RtcType             Rtc;        //��ǰRTCʱ��
DateType            Date;       //��ǰNTCIP����
TimeType            Time;       //��ǰNTCIPʱ��
SeqTimeType         SeqTime;

MANUAL_CTRL_TYPE    ManualCtrl;



void GetPhaseStateSeqMax(void);
void GreenWaveTimeControl(void);
void GreenWaveTimeChange(void);






void RtcIrqCallback(void)
{
    PhaseState.Phase1sFlag = 1;
    OP.Seconds++;
    if((OP.Seconds%60)==0) OP.RtcIrqFlag = 1;
    #if RTCIRQ 
    printf("E5_IRQ \r\n"); 
    #endif
}

void ReadRealTime(void)
{
    static RtcType rtc;
    memcpy(&rtc, &Rtc, sizeof(rtc));//ǰһ��ʱ�䱸��
    
    Rtc.second++;
    if(OP.RtcIrqFlag == 1 || OP.StartFlag == 1 || OP.TimeChangeFlag == 1)
    {
        RtcReadTime();  //��ȡRTCʱ��
        OP.Seconds = (Time.Hour * 3600) + (Time.Minute * 60) + Rtc.second;
        if(OP.StartFlag) OP.StartFlag = 0;
        if(OP.RtcIrqFlag) OP.RtcIrqFlag = 0;
        if(OP.TimeChangeFlag) OP.TimeChangeFlag = 0;
    }
    
    OP.ShowTimeFlag = 1;
    if(Rtc.day != rtc.day)          //���ڽ�λ��ˢ��һ��
        OP.PlanRefreshFlag = 1;     //ʱ�α��ˢ��  ���� �� ÿ�죬ˢ��һ��
    if(Rtc.minute != rtc.minute)
        OP.ActionRefreshFlag = 1;   //����ˢ��      ÿ�η��ӣ�ˢ��һ��
}

/********************************************************/
//���ܣ�ͨ����ѯ���ȼƻ�����ȡʱ�α�ţ��ж�ʱ�α��Ƿ���Ҫ����
//���� �� ÿ����� ����һ��
void RunPlanRefresh(void)
{
    if(OP.PlanRefreshFlag)
    {
        OP.PlanRefreshFlag = 0;
        #if DEBUG
        printf("Plan Refresh \r\n");
        #endif
        if(SchedulePlanRefresh(&ScheduleTab, &Date))
            OP.ActionRefreshFlag = 1; //ʱ�α����֮����Ҫˢ��ʱ��
    }
}

/* �����󣬷���ֱ�����У����ж����в����ı仯��� */
void NewPeriodApply(PlanType* DayPlan, TimeType* Time)
{
    uint8_t ActionNum, ActionIndex, PatternIndex;
    uint8_t temp = GetPeriodIndex(DayPlan, Time); //ʱ�� + ʱ�α� = ʱ������
    if(temp < PeriodMax)//ʱ������
    {
        ActionNum = DayPlan->Period[temp].ActionNum;
        //����ʱ����Ϣ
        Period.Time.Hour = DayPlan->Period[temp].Time.Hour;
        Period.Time.Minute = DayPlan->Period[temp].Time.Minute;
        Period.ActionNum = ActionNum;
        ActionIndex = GetActionIndex(&ActionTab, ActionNum);
        
        if(ActionIndex < ActionMax)//�ǿն���
        {
            //���ƶ������ݣ�
            //���⹦�ܣ��������ܣ�ֱ�Ӹ���
            memcpy(&Action, &ActionTab.Action[ActionIndex], sizeof(Action));
            
            PatternIndex = GetPatternIndex(&PatternTab, Action.Pattern);
            if(PatternIndex < PatternMax)
            {
                memcpy(&PatternNow, &PatternTab.Pattern[PatternIndex], sizeof(PatternNow));
                
                memcpy(&SequenceNow, &SeqTab.Seq[PatternNow.SequenceNum - 1], sizeof(SequenceNow));
                memcpy(&SplitNow, &SplitTab.Split[PatternNow.SplitNum - 1], sizeof(SplitNow));
                OP.NewPatternFlag = 1;//�µķ���׼�����������������꣬����ʵʩ
            }
        }
    }
}

void NewPatternCheck(void)
{
    #if DEBUG 
    printf("NewPatternCheck\r\n");
    #endif
    if(SchedulePlanRefresh(&ScheduleTab, &Date))
    {
        #if DEBUG 
        printf("FindSchedulePlan\r\n");
        #endif
        NewPeriodApply(&Plan, &Time);
        OP.ActionRefreshFlag = 1;
    }
}

/********************************************************/
void RunPlanActionRefresh(void)
{
    if(OP.ActionRefreshFlag)
    {
        OP.ActionRefreshFlag = 0;
        #if DEBUG 
        printf("Action Refresh\r\n");
        #endif
        PlanActionRefresh(&Plan, &Time);
    }
}

//���ܣ���ѯʱ�α���ȡ��ǰʱ��Ķ�����
//���ݲ�����ʱ�α� + ʱ��
void PlanActionRefresh(PlanType* DayPlan, TimeType* Time)
{
    uint8_t ActionIndex, PatternIndex;
    //ʱ�� + ʱ�α� = ʱ������ 
    uint8_t PeriodIndex = GetPeriodIndex(DayPlan, Time);
    if(PeriodIndex < PeriodMax)
    {
        //ʱ�β����иı䣬�͸���ʱ�β���
        if(memcmp(&Period, &DayPlan->Period[PeriodIndex], sizeof(Period)) != 0)
        {
            #if DEBUG 
            printf("Period Changed\r\n");
            #endif
            memcpy(&Period, &DayPlan->Period[PeriodIndex], sizeof(Period));
        }
        
        ActionIndex = GetActionIndex(&ActionTab, Period.ActionNum);
        if(ActionIndex < ActionMax)//�ǿն���
        {
            if(memcmp(&Action, &ActionTab.Action[ActionIndex], sizeof(Action)) != 0)
            {
                #if DEBUG
                printf("Action Changed\r\n");
                #endif
                memcpy(&Action, &ActionTab.Action[ActionIndex], sizeof(Action));
            }
        }
        
        PatternIndex = GetPatternIndex(&PatternTab, Action.Pattern);
        if(PatternIndex < PatternMax)
        {
            //�����������űȲ����ı��ˣ��˴��������
            //������������űȲ����ı��ˣ��͸ı�Pattern��������űȺ�,�Ը������в���
            #if DEBUG
            printf_fifo_dec(&PatternNow.Num, sizeof(PatternNow));
            printf_fifo_dec(&PatternTab.Pattern[PatternIndex].Num, sizeof(PatternNow));
            #endif
            if(memcmp(&PatternNow, &PatternTab.Pattern[PatternIndex], sizeof(PatternNow)) != 0)
            {
                #if DEBUG
                printf("Action.Pattern = %d\r\n", Action.Pattern);
                printf("PatternIndex = %d\r\n", PatternIndex);
                printf("Pattern Changed\r\n");
                printf("Prepare New Pattern\r\n");
                #endif
                
                memcpy(&PatternNow, &PatternTab.Pattern[PatternIndex], sizeof(PatternNow));
                memcpy(&SequenceNow, &SeqTab.Seq[PatternNow.SequenceNum - 1], sizeof(SequenceNow));
                memcpy(&SplitNow, &SplitTab.Split[PatternNow.SplitNum - 1], sizeof(SplitNow));
                OP.NewPatternFlag = 1;
            }
        }
    }
}

/********************************************************/
void NewPatternApply(void)
{
    #if DEBUG
    printf("0New Pattern Apply\r\n");
    #endif

    OP.WorkMode = PatternNow.WorkMode;
    OP.WorkMode_Reason = WMR_Normal;
    if(OP.WorkMode == Flashing)AutoFlashMode();
    else if(OP.WorkMode == AllRed)AutoAllRedMode();
    else if(OP.WorkMode == LampOff)AutoLampOffMode();
    else if(OP.WorkMode == FixedTime || OP.WorkMode == LineCtrl)
    {
        GetPhaseStateSeqMax();//ͨ���������ȡ�����������λ�����Լ���λ������ֵ�Ͷ�Ӧ����
        GetSeqTime();//ͨ�����������űȻ�ȡ����ʱ�������
        
        if(OP.WorkMode == LineCtrl) GreenWaveTimeControl();//LineCtrl
        RunPhaseInit(&SequenceNow,  &SplitNow);
        GetPhaseStatusMap();
    }
    else if(OP.WorkMode == VehicleSense)//��Ӧģʽ
    {
        GetPhaseStateSeqMax();  //ͨ���������ȡ�����������λ��,�Լ���λ������ֵ�Ͷ�Ӧ����
        GetSeqTime();           //ͨ�����������űȻ�ȡ����ʱ�������
        
        RunPhaseInit(&SequenceNow,  &SplitNow);
        GetPhaseStatusMap();
    }
    bsp_LedToggle(LED_NEWPLAN);
}

void RunGetVehDetState(void)
{
    if(OP.GetVehDetStaFlag)
    {
        OP.GetVehDetStaFlag = 0;
        GetVehDetSta(); 
        VehDetStaCount();
    }
}

void RunModeProcess(void)
{
    uint8_t temp;
    if(OP.ScheduleDataChangeFlag)//���ȼƻ�������иı�
    {
        OP.ScheduleDataChangeFlag = 0;
        OP.PlanRefreshFlag = 1;
    }
    if(OP.PlanDataChangeFlag)
    {
        OP.PlanDataChangeFlag = 0;
        #if DEBUG
        printf("Plan Data Changed \r\n");
        #endif
        temp = ScheduleNow.PlanNum - 1;
        memcpy(&Plan, &PlanTab.Plan[temp], sizeof(Plan));
        //ʱ�α����֮����Ҫˢ��ʱ��
        OP.ActionRefreshFlag = 1;
    }
    if(OP.ActionDataChangeFlag)
    {
        OP.ActionDataChangeFlag = 0;
        OP.ActionRefreshFlag = 1;
    }
    if(OP.PatternDataChangeFlag)
    {
        OP.PatternDataChangeFlag = 0;
        OP.ActionRefreshFlag = 1;
    }
    if(OP.SequenceDataChangeFlag)
    {
        OP.SequenceDataChangeFlag = 0;
        PatternNow.SequenceNum = 0;
        OP.ActionRefreshFlag = 1;
    }
    if(OP.SplitDataChangeFlag)
    {
        OP.SplitDataChangeFlag = 0;
        PatternNow.SplitNum = 0;
        OP.ActionRefreshFlag = 1;
    }
    RunPlanRefresh();
    RunPlanActionRefresh();
}

void manual_mange(void)//10ms����һ��
{
    static uint8_t Value = 0xff, Count = 0;
    uint8_t cmd;
    uint8_t temp = manual_scan();
    if(Value != temp)
    {
        if(++Count > 10)
        {
            Count = 0;
            cmd = Value^temp;
            Value = temp;
            if(cmd & Value)//�����ͷ�
            {
                if(cmd == 0x01)//�ر��ֶ�
                {
                    ManualCtrl.KeyCmd = 0;
                }
            }
            else//��������
            {
                ManualCtrl.KeyCmd = cmd;
            }
            #if DEBUG
            printf("temp = %x, cmd = %x, ManualCmd = %x \r\n", temp, cmd, ManualCtrl.KeyCmd);
            #endif
        }
    }
    else
    {
        Count = 0;
    }
}

void manual_control(void)
{
    //ManualCtrl.KeyCmd
    //bit0Ϊ�ֶ�����, 0�����ֶ�, 1�ر��ֶ�
    //bit1����
    if(ManualCtrl.KeyCmd < 0xff)//�ֿ��ж���
    {
        if(ManualCtrl.LocalCtrlFlag == 0) //���ֶ�ģʽ
        {
            if(ManualCtrl.KeyCmd == MANUAL_ON)//ҡͷ����,���Զ����ֶ�
            {
                ManualCtrl.LocalCtrlFlag = 1;
                //if(OP.WorkMode < SPECIAL_MODE) OP.WorkModeBK = OP.WorkMode;
                if(OP.WorkMode < Flashing)//
                {
                    OP.WorkModeBK = OP.WorkMode;
                    OP.WorkMode = ManualStep;
                    ManualCtrl.Time = defaultAutoExitRemoteTime;
                }
                OP.WorkMode_Reason = WMR_LocalManual;
            }
            else if(ManualCtrl.KeyCmd == MANUAL_ClearError)//�ֶ��������
            {
                if(OP.WorkMode_Reason)
                {
                    if(WMR_RedGreenConflict == OP.WorkMode_Reason)
                    {
                        OP.red_green_conflict_reg = 0;//�������
                        OP.WorkMode_Reason = WMR_Normal;//ʹ�źŻ���������,���ǹ��ϻ���
                    }
                    if(WMR_GreenConflict == OP.WorkMode_Reason)
                    {
                        OP.green_conflict_reg = 0;//�������
                        OP.WorkMode_Reason = WMR_Normal;//ʹ�źŻ���������,���ǹ��ϻ���
                    }
                    if(WMR_RedFailed == OP.WorkMode_Reason)
                    {
                        OP.red_install_reg = red_install_fail_detect(0);//�������
                        OP.WorkMode_Reason = WMR_Normal;//ʹ�źŻ���������,���ǹ��ϻ���
                    }
                }
                #if DEBUG
                printf("MANUALClearError\r\n");
                #endif
            }
/*
            else if(ManualCtrl.KeyCmd == MANUAL_K1)//K1
            {
                if(OP.WorkMode != ManualAppoint)
                {
                    if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                    OP.WorkMode = ManualAppoint;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.StartFlag = 1;
                    ManualCtrl.LocalCtrlFlag = 1;
                    ManualCtrl.EnforceFlag = 0;
                    ManualCtrl.OrderFlag = 0;
                    ManualCtrl.ExitFlag = 0;
                    ManualCtrl.AutoTime = defaultAutoExitRemoteTime;
                }
                #if DEBUG
                printf("MANUAL_K1\r\n");
                #endif
            }
            else if(ManualCtrl.KeyCmd == MANUAL_K2)//
            {
                if(OP.WorkMode != ManualAppoint)
                {
                    if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                    OP.WorkMode = ManualAppoint;
                    ManualCtrl.StartFlag = 1;
                }
                ManualCtrl.LocalCtrlFlag = 1;
                OP.WorkMode_Reason = WMR_LocalManual;
                ManualCtrl.Pos = 0x01;
                ManualCtrl.Dir = 0x07;
                ManualCtrl.EnforceFlag = 1;
                ManualCtrl.OrderFlag = 1;
                ManualCtrl.ExitFlag = 0;
                ManualCtrl.AutoTime = defaultAutoExitRemoteTime;
                ManualCtrl.ChannelOnsNext = GetAppointChannel(ManualCtrl.Pos, ManualCtrl.Dir);
                #if DEBUG
                printf("MANUAL_K2\r\n");
                #endif
            }
            else if(ManualCtrl.KeyCmd == MANUAL_K5)//
            {
                if(OP.WorkMode == ManualAppoint)
                {
                    ManualCtrl.ExitFlag = 1;
                }
                #if DEBUG
                printf("MANUAL_K5\r\n");
                #endif
            }
*/
        }
        else //�ֶ�ģʽ
        {
            if(ManualCtrl.KeyCmd)
            {
                if(ManualCtrl.KeyCmd == MANUAL_FLASH)//����
                {
                    OP.WorkMode = ManualFlashing;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.Time = defaultAutoExitRemoteTime;
                    #if DEBUG
                    printf("ManualFlashing\r\n");
                    #endif
                }
                else if(ManualCtrl.KeyCmd == MANUAL_AllRed)//ȫ��
                {
                    OP.WorkMode = ManualAllRead;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.Time = defaultAutoExitRemoteTime;
                    #if DEBUG
                    printf("ManualAllRead\r\n");
                    #endif
                }
                else if(ManualCtrl.KeyCmd == MANUAL_NextStep)//��һ��
                {
                    OP.WorkMode = ManualStep;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.Time = defaultAutoExitRemoteTime;
                    ManualCtrl.NextStepFlag = 1;
                    #if DEBUG
                    printf("ManualNextStep\r\n");
                    #endif
                }
//                else if(ManualCtrl.KeyCmd == MANUAL_LampOff)//�ص�
//                {
//                    OP.WorkMode = ManualLampOff;
//                    OP.WorkMode_Reason = WMR_LocalManual;
//                    #if DEBUG
//                    printf("ManualLampOff\r\n");
//                    #endif
//                }
                else if(ManualCtrl.KeyCmd == MANUAL_K4)//
                {
                    if(OP.WorkMode != ManualAppoint)
                    {
                        if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                        ManualCtrl.StartFlag = 1;
                    }
                    OP.WorkMode = ManualAppoint;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.LocalCtrlFlag = 1;
                    ManualCtrl.Pos = 0x01;
                    ManualCtrl.Dir = 0x07;
                    ManualCtrl.EnforceFlag = 1;
                    ManualCtrl.OrderFlag = 1;
                    ManualCtrl.ExitFlag = 0;
                    //ManualCtrl.Time = 0;
                    ManualCtrl.AutoTime = defaultAutoExitRemoteTime;
                    ManualCtrl.ChannelOnsNext = GetAppointChannel(ManualCtrl.Pos, ManualCtrl.Dir);
                    #if DEBUG
                    printf("MANUAL_K3\r\n");
                    #endif
                }
                else if(ManualCtrl.KeyCmd == MANUAL_K5)//
                {
                    if(OP.WorkMode != ManualAppoint)
                    {
                        if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                        ManualCtrl.StartFlag = 1;
                    }
                    OP.WorkMode = ManualAppoint;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.LocalCtrlFlag = 1;
                    ManualCtrl.Pos = 0x02;
                    ManualCtrl.Dir = 0x07;
                    ManualCtrl.EnforceFlag = 1;
                    ManualCtrl.OrderFlag = 1;
                    ManualCtrl.ExitFlag = 0;
                    //ManualCtrl.Time = 0;
                    ManualCtrl.AutoTime = defaultAutoExitRemoteTime;
                    ManualCtrl.ChannelOnsNext = GetAppointChannel(ManualCtrl.Pos, ManualCtrl.Dir);
                    #if DEBUG
                    printf("MANUAL_K3\r\n");
                    #endif
                }
                else if(ManualCtrl.KeyCmd == MANUAL_K6)//
                {
                    if(OP.WorkMode != ManualAppoint)
                    {
                        if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                        ManualCtrl.StartFlag = 1;
                    }
                    OP.WorkMode = ManualAppoint;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.LocalCtrlFlag = 1;
                    ManualCtrl.Pos = 0x04;
                    ManualCtrl.Dir = 0x07;
                    ManualCtrl.EnforceFlag = 1;
                    ManualCtrl.OrderFlag = 1;
                    ManualCtrl.ExitFlag = 0;
                    //ManualCtrl.Time = 0;
                    ManualCtrl.AutoTime = defaultAutoExitRemoteTime;
                    ManualCtrl.ChannelOnsNext = GetAppointChannel(ManualCtrl.Pos, ManualCtrl.Dir);
                    #if DEBUG
                    printf("MANUAL_K3\r\n");
                    #endif
                }
                else if(ManualCtrl.KeyCmd == MANUAL_K7)//
                {
                    if(OP.WorkMode != ManualAppoint)
                    {
                        if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                        ManualCtrl.StartFlag = 1;
                    }
                    OP.WorkMode = ManualAppoint;
                    OP.WorkMode_Reason = WMR_LocalManual;
                    ManualCtrl.LocalCtrlFlag = 1;
                    ManualCtrl.Pos = 0x08;
                    ManualCtrl.Dir = 0x07;
                    ManualCtrl.EnforceFlag = 1;
                    ManualCtrl.OrderFlag = 1;
                    ManualCtrl.ExitFlag = 0;
                    //ManualCtrl.Time = 0;
                    ManualCtrl.AutoTime = defaultAutoExitRemoteTime;
                    ManualCtrl.ChannelOnsNext = GetAppointChannel(ManualCtrl.Pos, ManualCtrl.Dir);
                    #if DEBUG
                    printf("MANUAL_K3\r\n");
                    #endif
                }
            }
            else//ҡͷ����,���ֶ����Զ�
            {
                if(OP.WorkMode == ManualAppoint)
                {
                    ManualCtrl.ExitFlag = 1;
                }
                else
                {
                    ManualCtrl.LocalCtrlFlag = 0;
                    if(OP.WorkMode >= SPECIAL_MODE) OP.WorkMode = OP.WorkModeBK;
                    OP.WorkMode_Reason = WMR_Normal;
                }
            }
        }
        ManualCtrl.KeyCmd = 0xff;
    }
}

void rf315m_mange(void)
{
    if(rf_state > 0)
    {
        if(rf_state == 0x08)
        {
            ManualCtrl.LocalCtrlFlag = 1;
            if(OP.WorkMode < SPECIAL_MODE) OP.WorkModeBK = OP.WorkMode;
            ManualCtrl.KeyCmd = MANUAL_FLASH;
        }
        else if(rf_state == 0x04)
        {
            ManualCtrl.LocalCtrlFlag = 1;
            if(OP.WorkMode < SPECIAL_MODE) OP.WorkModeBK = OP.WorkMode;
            ManualCtrl.KeyCmd = MANUAL_AllRed;
        }
        else if(rf_state == 0x01)
        {
            ManualCtrl.LocalCtrlFlag = 1;
            if(OP.WorkMode < SPECIAL_MODE) OP.WorkModeBK = OP.WorkMode;
            ManualCtrl.KeyCmd = MANUAL_NextStep;
        }
        else if(rf_state == 0x02)
        {
            ManualCtrl.KeyCmd = MANUAL_OFF;
        }
    }
}

//ͬһ������,���̳�ͻ�ļ��
//���أ�0-15 bit ��Ӧ�����г�ͻ, 0�޳�ͻ
uint16_t Red_Green_conflict_detect(uint16_t red_state, uint16_t green_state)//200ms
{
    static uint8_t c200ms[16] = {0};
    static uint16_t conflict_reg = 0;
    uint16_t i = 0, temp_group = 0x1, temp_var = 0x8000, lamp_state;

    lamp_state = red_state & green_state;
    for(i = 0; i < 16; i++)
    {
        if((lamp_state & temp_var) == temp_var)
        {
            if(++c200ms[i] >= 10)//��������2��
            {
                c200ms[i] = 0;
                conflict_reg |= temp_group;
            }
        }
        else
        {
            c200ms[i] = 0;
            conflict_reg &= ~temp_group;
        }
        temp_group <<= 1;
        temp_var >>= 1;
    }

    return conflict_reg;
}

//phase 1  1111 0111 
//         0000 1000
//         0000 1001
uint16_t Green_conflict_detect(void)
{
    static uint8_t c200ms[16] = {0};
    static uint16_t conflict_reg = 0;
    uint32_t i = 0, phaseindex, temp_phase = 0x1, temp_concurrency, temp_release;

    for(i = 0; i < 16;i++)
    {
        if(PhaseStatus.Greens & temp_phase)//�õ����ѷ���
        {
            temp_release = PhaseStatus.Greens & (~temp_phase);
            phaseindex = GetPhaseIndex(&PhaseTab, i+1);
            temp_concurrency = PhaseTab.Phase[phaseindex].ConcurrencyL | (PhaseTab.Phase[phaseindex].ConcurrencyH<<8);
            temp_concurrency = (~temp_concurrency)&0xffff;
            if(temp_concurrency & temp_release)
            {
                if(++c200ms[i] >= 10)//��������2��
                {
                    c200ms[i] = 0;
                    conflict_reg |= temp_phase;
                }
            }
            else
            {
                c200ms[i] = 0;
                conflict_reg &= ~temp_phase;
            }
        }
        else 
        {
            c200ms[i] = 0;
            conflict_reg &= ~temp_phase;
        }
        temp_phase <<= 1;
    }

    return conflict_reg;
}

uint16_t red_install_fail_detect(uint16_t installs)
{
    static uint8_t c200ms[16] = {0};
    static uint16_t red_install_reg = 0;
    uint16_t i = 0, temp_group = 0x1, temp_var = 0x8000;
    
    if(installs == 0) { red_install_reg = 0; OP.red_failed_reg = 0; return 0;}
    for(i = 0; i < 16; i++)
    {
        if(red_state & temp_var)//ĳһ·��������
        {
            if(current_stab & temp_var)//�����е���
            {
                if(++c200ms[i] >= 10)//��������2��
                {
                    c200ms[i] = 0;
                    red_install_reg |= temp_group;
                }
            }
            else if(red_install_reg & temp_group)//û�е��� �� �Ѱ�װ
            {
                if(++c200ms[i] >= 10)//ʧЧ��������2��
                {
                    c200ms[i] = 0;
                    OP.red_failed_reg |= temp_group;
                }
            }
            else c200ms[i] = 0;
        }
        else c200ms[i] = 0;
        
        temp_group <<= 1;
        temp_var >>= 1;
    }
    
    return red_install_reg;
}

void fail_conflict_detect(void)
{
    red_state = ((red_state_stab&0xf) << 12) | 
                ((red_state_stab&0xf0) << 4) |
                ((red_state_stab&0xf00) >> 4) |
                ((red_state_stab&0xf000) >> 12);
    green_state =   ((green_state_stab&0xf) << 12) | 
                    ((green_state_stab&0xf0) << 4) |
                    ((green_state_stab&0xf00) >> 4) |
                    ((green_state_stab&0xf000) >> 12);
    current_stab = ((current_state_stab&0x3333) << 2) |
                   ((current_state_stab&0xcccc) >> 2);
    current_stab =  ((current_stab&0xf) << 12) | 
                    ((current_stab&0xf0) << 4) |
                    ((current_stab&0xf00) >> 4) |
                    ((current_stab&0xf000) >> 12);

    if(Unit.RedGreenConflictDetectFlag)
    {
        OP.red_green_conflict_reg = Red_Green_conflict_detect(red_state, green_state);
        if(OP.red_green_conflict_reg)
        {
            if(OP.WorkMode_Reason == WMR_Normal)
            {
                OP.WorkMode_Reason = WMR_RedGreenConflict;
                OP.WorkModeBK = OP.WorkMode;
                OP.WorkMode = Flashing;
            }
        }
    }
    
    if(Unit.GreenConflictDetectFlag)
    {
        OP.green_conflict_reg = Green_conflict_detect();
        if(OP.green_conflict_reg)
        {
            if(OP.WorkMode_Reason == WMR_Normal)
            {
                OP.WorkMode_Reason = WMR_GreenConflict;
                OP.WorkModeBK = OP.WorkMode;
                OP.WorkMode = Flashing;
            }
        }
    }
    
    if(Unit.RedFailedDetectFlag)
    {
        OP.red_install_reg = red_install_fail_detect(0xff);
        if(OP.red_failed_reg)
        {
            if(OP.WorkMode_Reason == WMR_Normal)
            {
                OP.WorkMode_Reason = WMR_RedFailed;
                OP.WorkModeBK = OP.WorkMode;
                OP.WorkMode = Flashing;
            }
        }
    }
}

void Input_mange(void)
{
    static uint8_t reg1s = 5;
    static uint8_t reg10ms = 10;
    static uint8_t reg200ms = 20;
    
    if(reg1ms_flag) //1ms��Ϣ���� 
    {
        reg1ms_flag = 0;
        lamp_state_detect();//1ms
        if(++reg10ms >= 10)//10ms
        {
            reg10ms = 0;
            OP.GetVehDetStaFlag = 1;
            peddet_scan(&peddet_hw);    //8·��ť����״̬���
            rf315m_scan();              //rf315M�ֿ�����
            rf315m_mange();
            manual_mange();

            if(++reg200ms >= 20)//200ms
            {
                reg200ms = 0;
                fail_conflict_detect();
                if(++reg1s >= 5)//1s
                {
                    reg1s = 0;
                    PeddetStateGet();//�����״̬ͳ��
                }
            }
        }
    }
}

void Auto_adjust_time(void)
{
    if(OP.sync_with_gps_flag)//wcxmask
    {
        RtcWrite(&OP.Gps.local);
        OP.TimeChangeFlag = 1;
        OP.sync_with_gps_flag = 0;
        #if DEBUG > 2
        printf("sync_with_gps, Seconds = %d, gps_seconds = %d\n", OP.Seconds, OP.gps_seconds);
        #endif
    }
}

void LampControlProcess(void)
{
    if(OP.Run10ms_flag) //10ms��Ϣ���� 
    {
        OP.Run10ms_flag = 0;
        RunPhaseTimeCalc();
    }
    
    if(OP.Run1s_flag)
    {
        OP.Run1s_flag = 0;
        OP.Reg1sCount++;
        Auto_adjust_time();
        ReadRealTime();
        bsp_LedToggle(LED_RUN);
        PPS_Toggle();
    }
    LampStateControl();
}

//�̲����ƣ���λ��ʱ�����Ϳ���
void GreenWaveTimeControl(void)
{
    OP.OffsetTimeModifier.OffsetTime = ((OP.Seconds + NowCycleTime - PatternNow.OffsetTime)%NowCycleTime);//��λƫ��ʱ��
    if(OP.OffsetTimeModifier.OffsetTime==0)
    {
        OP.OffsetTimeModifier.ModifierFlag = 0;
    }
    else
    {
        OP.OffsetTimeModifier.ModifierTime = NowCycleTime - OP.OffsetTimeModifier.OffsetTime;
        if(OP.OffsetTimeModifier.OffsetTime <= (NowCycleTime * 4 / 10))
        {
            OP.OffsetTimeModifier.ModifierFlag = 1;
            if(OP.OffsetTimeModifier.OffsetTime <= (NowCycleTime * 2 / 10))
                OP.OffsetTimeModifier.StepTime = OP.OffsetTimeModifier.OffsetTime/PhaseState.CycleStepMax + 1;
            else
                OP.OffsetTimeModifier.StepTime = OP.OffsetTimeModifier.OffsetTime/PhaseState.CycleStepMax/2 + 1;
        }
        else
        {
            OP.OffsetTimeModifier.ModifierFlag = 2;
            if(OP.OffsetTimeModifier.ModifierTime <= (NowCycleTime * 3 / 10))
                OP.OffsetTimeModifier.StepTime = OP.OffsetTimeModifier.ModifierTime/PhaseState.CycleStepMax + 1;
            else
                OP.OffsetTimeModifier.StepTime = OP.OffsetTimeModifier.ModifierTime/PhaseState.CycleStepMax/2 + 1;
        }
    }

    #if DEBUG 
    printf("OP.Seconds = %d, NowCycleTime = %d \r\n",OP.Seconds,NowCycleTime);
    printf("OffsetTime = %d \r\n", OP.OffsetTimeModifier.OffsetTime);
    printf("ModifierTime = %d \r\n", OP.OffsetTimeModifier.ModifierTime);
    printf("ModifierFlag = %d \r\n", OP.OffsetTimeModifier.ModifierFlag);
    printf("CycleStepMax = %d \r\n", PhaseState.CycleStepMax);
    printf("StepTime = %d \r\n", OP.OffsetTimeModifier.StepTime);
    #endif
}

void GreenWaveTimeChange(void)
{
    uint8_t ring;
    if(OP.OffsetTimeModifier.ModifierFlag == 1)
    {
        if(OP.OffsetTimeModifier.OffsetTime > OP.OffsetTimeModifier.StepTime)
        {
            for(ring = 0; ring < RingMax; ring++)
            {
                if(PhaseState.Ring[ring].SeqMax) PhaseState.Ring[ring].SecondRemain -= OP.OffsetTimeModifier.StepTime;
            }
            OP.OffsetTimeModifier.OffsetTime -= OP.OffsetTimeModifier.StepTime;
            #if DEBUG
            printf("greenware -%d\r\n",OP.OffsetTimeModifier.StepTime);
            printf("OffsetTime = %d\r\n",OP.OffsetTimeModifier.OffsetTime);
            #endif
        }
        else
        {
            for(ring = 0; ring < RingMax; ring++)
            {
                if(PhaseState.Ring[ring].SeqMax) PhaseState.Ring[ring].SecondRemain -= OP.OffsetTimeModifier.OffsetTime;
            }
            OP.OffsetTimeModifier.OffsetTime = 0;
            OP.OffsetTimeModifier.ModifierTime = 0;
            OP.OffsetTimeModifier.ModifierFlag = 0;
            OP.OffsetTimeModifier.StepTime = 0;
            #if DEBUG
            printf("greenware matched\r\n");
            printf("OffsetTime = %d\r\n",OP.OffsetTimeModifier.OffsetTime);
            #endif
        }
    }
    else if(OP.OffsetTimeModifier.ModifierFlag == 2)
    {
        if(OP.OffsetTimeModifier.ModifierTime > OP.OffsetTimeModifier.StepTime)
        {
            for(ring = 0; ring < RingMax; ring++)
            {
                if(PhaseState.Ring[ring].SeqMax) PhaseState.Ring[ring].SecondRemain += OP.OffsetTimeModifier.StepTime;
            }
            OP.OffsetTimeModifier.ModifierTime -= OP.OffsetTimeModifier.StepTime;
            #if DEBUG
            printf("greenware +%d\r\n",OP.OffsetTimeModifier.StepTime);
            printf("ModifierTime = %d\r\n",OP.OffsetTimeModifier.ModifierTime);
            #endif
        }
        else
        {
            for(ring = 0; ring < RingMax; ring++)
            {
                if(PhaseState.Ring[ring].SeqMax) PhaseState.Ring[ring].SecondRemain += OP.OffsetTimeModifier.ModifierTime;
            }
            OP.OffsetTimeModifier.OffsetTime = 0;
            OP.OffsetTimeModifier.ModifierTime = 0;
            OP.OffsetTimeModifier.ModifierFlag = 0;
            OP.OffsetTimeModifier.StepTime = 0;
            #if DEBUG
            printf("greenware matched\r\n");
            printf("ModifierTime = %d\r\n",OP.OffsetTimeModifier.ModifierTime);
            #endif
        }
    }
}

/*
 * ��ǰ���е���λ�ı�
 * ���¸�ֵ��λ �� ���ű� ����
 */
uint8_t RingPhaseChange(uint8_t ring)
{
    uint8_t ChangeFlag = 0,SeqNum,PhaseNum,PhaseIndex,PhaseNext,SplitPhaseIndex,SplitNextPhaseIndex;
    if(PhaseState.Ring[ring].PhaseChangeFlag)
    {
        PhaseState.Ring[ring].PhaseChangeFlag = 0;
        SeqNum = PhaseState.Ring[ring].SeqNum;
        
        //�˴�������λ�ţ�����ȡ��λ���� �� ���ű���λ����
        PhaseNum = SequenceNow.Ring[ring].Phase[SeqNum];
        if(SeqNum+1 < PhaseState.Ring[ring].SeqMax)
            PhaseNext = SequenceNow.Ring[ring].Phase[SeqNum+1];
        else
            PhaseNext = SequenceNow.Ring[ring].Phase[0];
        
        RingPhase[ring].PhaseIndex = GetPhaseIndex(&PhaseTab, PhaseNum);
        RingPhase[ring].PhaseNextIndex = GetPhaseIndex(&PhaseTab, PhaseNext);
        PhaseIndex = RingPhase[ring].PhaseIndex;
        
        //ͨ���������������е���λ�����ű�
        RingPhase[ring].PhaseNum = PhaseNum;
        RingPhase[ring].PhaseNext = PhaseNext;
        
        SplitPhaseIndex = GetSplitPhaseIndex(&SplitNow, PhaseNum);
        SplitNextPhaseIndex = GetSplitPhaseIndex(&SplitNow, PhaseNext);
        memcpy(&RingSplit[ring], &SplitNow.Phase[SplitPhaseIndex], sizeof(PhaseSplitType));
        
        RingPhase[ring].SplitIndex = SplitPhaseIndex;
        RingPhase[ring].SplitNextIndex = SplitNextPhaseIndex;
        
        PhaseState.Ring[ring].SecondRemain = RingSplit[ring].Time;
        PhaseState.Ring[ring].VehicleTransitionTime = PhaseTab.Phase[PhaseIndex].VehicleClear + 
                                                      PhaseTab.Phase[PhaseIndex].YellowChange + 
                                                      PhaseTab.Phase[PhaseIndex].RedClear;
        if(PhaseState.StepMaxRing == ring && OP.OffsetTimeModifier.ModifierFlag > 0)
        {
            ChangeFlag = 1;
            #if DEBUG
            printf("ChangeFlag = 1\r\n");
            #endif
        }
    }
    return ChangeFlag;
}

//Ҫ�� PhaseState ���е�һЩ���������ȥ, ��Ӧ�Ĺ��ܺ��������ȥ wcx
void GetPhaseStateSeqMax(void)
{
    uint8_t ring;
    PhaseState.ValidRings = 0;
    PhaseState.CycleStepMax = 0;
    for(ring = 0; ring < RingMax; ring++)
    {
        PhaseState.Ring[ring].SeqMax = GetSeqMax(&SequenceNow.Ring[ring]);//��ȡ�˻�����λ����
        if(PhaseState.Ring[ring].SeqMax)
        {
            PhaseState.ValidRings++;
            if(PhaseState.Ring[ring].SeqMax > PhaseState.CycleStepMax)
            {
                PhaseState.CycleStepMax = PhaseState.Ring[ring].SeqMax;
                PhaseState.StepMaxRing = ring;
            }
        }
    }
    #if DEBUG
    printf("ValidRings = %d, StepMaxRing = %d, CycleStepMax = %d\r\n",\
            PhaseState.ValidRings, PhaseState.StepMaxRing, PhaseState.CycleStepMax);
    #endif
}

/*
    ���е���λ���ݳ�ʼ��������������ű������л�ȡ���е�4��������λ���ݺ����ű�
*/
void RunPhaseInit(SequenceType* Sequence, SplitType* SplitX)
{
    uint8_t ring, ChangeFlag = 0;
    
    for(ring = 0; ring < RingMax; ring++)
    {
        PhaseState.Ring[ring].SeqNum = 0;
        if(PhaseState.Ring[ring].SeqMax > 0)//�ǿջ�
        {
            PhaseState.Ring[ring].PhaseChangeFlag = 1;
            if(RingPhaseChange(ring)) ChangeFlag = 1;
            PhaseState.Ring[ring].RingNum = Sequence->Ring[ring].RingNum;
            PhaseState.Ring[ring].CycleOverFlag = 0;
        }
        else
        {
            PhaseState.Ring[ring].RingNum = 0;
            PhaseState.Ring[ring].CycleOverFlag = 1;
        }
    }
    PhaseState.Phase10msCount = 0;
    PhaseState.Phase1sFlag = 0;
    if(ChangeFlag == 1 && OP.WorkMode == LineCtrl) GreenWaveTimeChange();
}


/*
    �����������κ�Ӳ�������ϵͳ
*/
void RunDataInit(void)
{
    ChannelReadStatus.Greens = 0;
    ChannelReadStatus.Yellows = 0;
    ChannelReadStatus.Reds = 0;
    
    OP.RestartFlag = 0;
    OP.StartFlag = 1;
    OP.TimeChangeFlag = 1;
    OP.ConnectFlag = 0;
    OP.SendWorkModeAutoFlag = 0;
    OP.SendWorkModeFlag = 0;
    OP.SendDoorAlarm = 0;
    OP.sync_with_gps_flag = 0;

    LampDriveDataInit();//��ʼ�����������ݼ�13H
}


/*
 * ��������� PhaseState ��ֵ
 */
void RunPhaseStateStartup(void)
{
    uint8_t ring;
    for(ring = 0; ring < RingMax; ring++)
    {
        PhaseState.Ring[ring].RingNum = 0;
        PhaseState.Ring[ring].CycleOverFlag = 1;
    }
    
    PhaseState.Phase1sFlag = 0;
    PhaseState.Phase10msCount = 0;
    PhaseState.Ring[0].SeqNum = 0;
    PhaseState.Ring[0].SeqMax = 2;
    PhaseState.Ring[0].RingNum = 1;
    PhaseState.Ring[0].CycleOverFlag = 0;
    PhaseState.Ring[0].SecondRemain = Unit.StartupFlash;
    AutoFlashMode();
    OP.WorkMode = StarupMode;
    OP.red_install_reg = 0;
    OP.red_failed_reg = 0;
    
    #if DEBUG
    printf("Startup Flashing\r\n");
    #endif
}

void StartupProcess(void)//����ģʽ
{
    if(PhaseState.Ring[0].SecondRemain) PhaseState.Ring[0].SecondRemain--;
    if(PhaseState.Ring[0].SecondRemain == 0)
    {
        if(++PhaseState.Ring[0].SeqNum <= PhaseState.Ring[0].SeqMax)
        {
            if(PhaseState.Ring[0].SeqNum == 1)
            {
                if(OP.PlanRefreshFlag)
                {
                    OP.PlanRefreshFlag = 0;
                    NewPatternCheck();
                }
                if(OP.NewPatternFlag)
                {
                    #if DEBUG
                    printf("Startup All Red\r\n");
                    #endif
                    PhaseState.Ring[0].SeqNum = 1;
                    PhaseState.Ring[0].SecondRemain = Unit.StartupAllRed;
                    AutoAllRedMode(); 
                }
                else//���������з���
                {
                    #if DEBUG
                    printf("Startup No Pattern\r\n");
                    #endif
                    PhaseState.Ring[0].SeqNum = 0;
                    OP.PlanRefreshFlag = 1;//���¼�����޷���
                    OLED_ShowString(48,0,"NP");
                }
            }
            else if(PhaseState.Ring[0].SeqNum == 2)
            {
                if(OP.NewPatternFlag)
                {
                    OP.NewPatternFlag = 0;
                    NewPatternApply();
                }
            }
        }
    }
}

uint8_t FindMiniTransitionRing(void)
{
    uint8_t i, ring = 0, MiniSecondRemainTime = 0xff;
    for(i = 0; i<RingMax; i++)
    {
        if(PhaseState.Ring[i].SeqMax > 0 && PhaseState.Ring[i].SecondRemain > 0)//�ǿջ�
        {
            if(MiniSecondRemainTime > PhaseState.Ring[i].SecondRemain)
            {
                ring = i;
                MiniSecondRemainTime = PhaseState.Ring[i].SecondRemain;
            }
        }
    }
    return ring;
}

uint8_t isInTransitionStep(void)
{
    //�ҵ���Сʣ��ʱ��Ļ�
    uint8_t MiniRemainTimeRing = FindMiniTransitionRing();
    //ʣ��ʱ��С�ڻ���ת��ʱ��,�Զ��л�����һ��λ״̬
    if(PhaseState.Ring[MiniRemainTimeRing].SecondRemain <= PhaseState.Ring[MiniRemainTimeRing].VehicleTransitionTime) return PhaseState.Ring[MiniRemainTimeRing].SecondRemain;
    return 0;
}

//����0-�����л�  ����1-�����л��������л��ź�
uint8_t ManualStepProcess(void)//�ֶ�ģʽ
{
    uint8_t i, k=0, PhaseChangeBit = 0, MiniRemainTimeRing;
    
    //�ҵ���Сʣ��ʱ��Ļ�
    MiniRemainTimeRing = FindMiniTransitionRing();
    //ʣ��ʱ��С�ڻ���ת��ʱ��,�Զ��л�����һ��λ״̬
    if(PhaseState.Ring[MiniRemainTimeRing].SecondRemain <= PhaseState.Ring[MiniRemainTimeRing].VehicleTransitionTime) ManualCtrl.NextStepFlag = 1;
    if(ManualCtrl.NextStepFlag == 0)return 0;
    
    if(PhaseState.Ring[MiniRemainTimeRing].SecondRemain > PhaseState.Ring[MiniRemainTimeRing].VehicleTransitionTime + 1)
    {
        while(1)
        {
            for(i = 0; i < RingMax; i++)
            {
                if(PhaseState.Ring[i].SeqMax)
                {
                    if(PhaseState.Ring[i].SecondRemain) PhaseState.Ring[i].SecondRemain--;
                    if(i == MiniRemainTimeRing && PhaseState.Ring[i].SecondRemain <= (PhaseState.Ring[i].VehicleTransitionTime + 1)) k = 0x55;
                }
            }
            if(k)break;
        }
    }
    
    for(i = 0; i < RingMax; i++)
    {
        if(PhaseState.Ring[i].SeqMax)
        {
            if(PhaseState.Ring[i].SecondRemain) PhaseState.Ring[i].SecondRemain--;
            if(PhaseState.Ring[i].SecondRemain == 0)
            {
                if(PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].SeqNum++;
                }
                if(PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    PhaseChangeBit = 1;
                    RingPhaseChange(i);
                }
                else 
                {
                    if(PhaseState.Ring[i].CycleOverFlag==0)
                    {
                        PhaseState.Ring[i].CycleOverFlag = 1;
                        PhaseChangeBit = 1;
                    }
                }
            }
        }
    }
    
    if(PhaseChangeBit)
    {
        PhaseState.StateNum++;
        ManualCtrl.NextStepFlag = 0;
    }
    
    if( PhaseState.Ring[0].CycleOverFlag & \
        PhaseState.Ring[1].CycleOverFlag & \
        PhaseState.Ring[2].CycleOverFlag & \
        PhaseState.Ring[3].CycleOverFlag)  //4������λ���������
    {
        PhaseState.NewCycleFlag = 1; //�µ�����
    }

    if(PhaseState.NewCycleFlag)//�������з���
    {
        PhaseState.NewCycleFlag = 0;
        for(i = 0; i < RingMax; i++)
        {
            if(PhaseState.Ring[i].SeqMax)
            {
                PhaseState.StateNum = 0;
                PhaseState.Ring[i].SeqNum = 0;
                PhaseState.Ring[i].CycleOverFlag = 0;
                PhaseState.Ring[i].PhaseChangeFlag = 1;
                RingPhaseChange(i);
            }
        }
    }
    return 1;
}


//�ݶ���ָ������ֻ���ǻ����������ת��ʱ��,�Ժ��ٿ������˵İ�ȫ����ʱ��
void GetMaxiTransitionTime(uint32_t ChannelOns)
{
    uint8_t i;
    uint8_t ChannelTransitionTime = 0;
    uint8_t MaxiChannelTransitionPhase = 0;
    
    uint32_t ChannelMask = 0x1;
    ManualCtrl.MaxiChannelTransitionTime = 0;

    for(i = 0; i < ChannelTab.Maximum; i++)
    {
        if(ChannelOns & ChannelMask)//ͨ������
        {
            uint8_t phaseNum, phaseIndex;
            phaseNum = ChannelTab.Channel[i].ControlSource;
            phaseIndex = GetPhaseIndex(&PhaseTab, phaseNum);
            if(ChannelTab.Channel[i].ControlType == CCT_VEHICLE)
            {
                ChannelTransitionTime = PhaseTab.Phase[phaseIndex].VehicleClear + PhaseTab.Phase[phaseIndex].YellowChange + PhaseTab.Phase[phaseIndex].RedClear;
                if(ManualCtrl.MaxiChannelTransitionTime < ChannelTransitionTime)
                {
                    ManualCtrl.MaxiChannelTransitionTime = ChannelTransitionTime;
                    MaxiChannelTransitionPhase = phaseNum;
                    #if DEBUG
                    printf("MaxiChannelTransitionPhase = %d\n",MaxiChannelTransitionPhase);
                    printf("ManualCtrl.MaxiChannelTransitionTime = %d\n",ManualCtrl.MaxiChannelTransitionTime);
                    #endif
                }
            }
            else if(ChannelTab.Channel[i].ControlType == CCT_PEDESTRIAN)
            {
                ChannelTransitionTime = PhaseTab.Phase[phaseIndex].PedestrianClear + PhaseTab.Phase[phaseIndex].RedClear;
                if(ManualCtrl.MaxiChannelTransitionTime < ChannelTransitionTime)
                {
                    ManualCtrl.MaxiChannelTransitionTime = ChannelTransitionTime;
                    MaxiChannelTransitionPhase = phaseNum;
                    #if DEBUG
                    printf("MaxiChannelTransitionPhase = %d\n",MaxiChannelTransitionPhase);
                    printf("ManualCtrl.MaxiChannelTransitionTime = %d\n",ManualCtrl.MaxiChannelTransitionTime);
                    #endif
                }
            }
        }
        ChannelMask <<= 1;
    }
}

void RemoteChannelStatusCtrl(void)
{
    uint8_t i;
    uint32_t ChannelMask = 0x1;

    for(i = 0; i < ChannelTab.Maximum; i++)
    {
        if(ChannelTab.Channel[i].ControlSource)
        {
            if(ManualCtrl.ChannelOnsNow & ChannelMask)//�������̵�״̬
            {
                if((ManualCtrl.ChannelOnsNext & ChannelMask)==0)//��һ�������
                {
                    uint8_t phaseNum, phaseIndex;
                    phaseNum = ChannelTab.Channel[i].ControlSource;
                    phaseIndex = GetPhaseIndex(&PhaseTab, phaseNum);
                    
                    if(ChannelTab.Channel[i].ControlType == CCT_PEDESTRIAN)
                    {
                        if(ManualCtrl.Time > PhaseTab.Phase[phaseIndex].PedestrianClear + PhaseTab.Phase[phaseIndex].RedClear)
                        {
                            ChannelStatus.Greens    |= ChannelMask;
                            ChannelStatus.Reds      &=~ChannelMask;
                            ChannelStatus.Yellows   &=~ChannelMask; 
                            ChannelStatus.Flash     &=~ChannelMask;
                        }
                        else if(ManualCtrl.Time > PhaseTab.Phase[phaseIndex].RedClear)
                        {
                            ChannelStatus.Greens    |= ChannelMask;
                            ChannelStatus.Reds      &=~ChannelMask;
                            ChannelStatus.Yellows   &=~ChannelMask; 
                            ChannelStatus.Flash     |= ChannelMask;
                        }
                        else//ȫ�����
                        {
                            ChannelStatus.Reds      |= ChannelMask;
                            ChannelStatus.Yellows   &=~ChannelMask;
                            ChannelStatus.Greens    &=~ChannelMask;
                            ChannelStatus.Flash     &=~ChannelMask;
                        }
                    }
                    else //if(ChannelTab.Channel[i].ControlType == CCT_VEHICLE)//������������ȫ���ջ�����������
                    {
                        if(ManualCtrl.Time > PhaseTab.Phase[phaseIndex].VehicleClear + PhaseTab.Phase[phaseIndex].YellowChange + PhaseTab.Phase[phaseIndex].RedClear)
                        {
                            ChannelStatus.Greens    |= ChannelMask;
                            ChannelStatus.Reds      &=~ChannelMask;
                            ChannelStatus.Yellows   &=~ChannelMask; 
                            ChannelStatus.Flash     &=~ChannelMask;
                        }
                        else if(ManualCtrl.Time > PhaseTab.Phase[phaseIndex].YellowChange + PhaseTab.Phase[phaseIndex].RedClear)
                        {
                            ChannelStatus.Greens    |= ChannelMask;
                            ChannelStatus.Reds      &=~ChannelMask;
                            ChannelStatus.Yellows   &=~ChannelMask; 
                            ChannelStatus.Flash     |= ChannelMask;
                        }
                        else if(ManualCtrl.Time > PhaseTab.Phase[phaseIndex].RedClear)
                        {
                            ChannelStatus.Yellows   |= ChannelMask;
                            ChannelStatus.Reds      &=~ChannelMask;
                            ChannelStatus.Greens    &=~ChannelMask;
                            ChannelStatus.Flash     &=~ChannelMask;
                        }
                        else//ȫ�����
                        {
                            ChannelStatus.Reds      |= ChannelMask;
                            ChannelStatus.Yellows   &=~ChannelMask;
                            ChannelStatus.Greens    &=~ChannelMask;
                            ChannelStatus.Flash     &=~ChannelMask;
                        }
                    }
                }
                else//��һ�����������
                {
                    ChannelStatus.Greens    |= ChannelMask;
                    ChannelStatus.Reds      &=~ChannelMask;
                    ChannelStatus.Yellows   &=~ChannelMask; 
                    ChannelStatus.Flash     &=~ChannelMask;
                }
            }
            else//���
            {
                ChannelStatus.Reds      |= ChannelMask;
                ChannelStatus.Yellows   &=~ChannelMask;
                ChannelStatus.Greens    &=~ChannelMask;
                ChannelStatus.Flash     &=~ChannelMask;
            }
        }
        else//δ���úڵ�
        {
            ChannelStatus.Greens    &=~ChannelMask;
            ChannelStatus.Reds      &=~ChannelMask;
            ChannelStatus.Yellows   &=~ChannelMask; 
            ChannelStatus.Flash     &=~ChannelMask;
        }
        ChannelMask <<= 1;
    }
}

/*
20220728: WorkPoint
1��������е�ǰ������λ
2�����ָ�����ж�Ӧ����λ

֮ǰ˼·������ͨ���ķ�λ����λ���ٸ�����λ��ͨ�����ͻ������ͬ��λ��ͬ��λ�ĵ�Ҳ�����ˣ�����˼·����
���Ϊ������ͨ���ķ�λ��ͨ����֮���л�ͨ����
*/
void AppointCtrlProcess(void)//ָ������ģʽ
{
    #if DEBUG
    printf("ManualCtrl.ManualCtrlTFlag = %d\n",ManualCtrl.RemoteCtrlFlag);
    printf("ManualCtrl.StartFlag = %d\n",ManualCtrl.StartFlag);
    printf("ManualCtrl.EnforceFlag = %d\n",ManualCtrl.EnforceFlag);
    printf("ManualCtrl.Time = %d\n",ManualCtrl.Time);
    #endif
    if(ManualCtrl.Time)ManualCtrl.Time--;
    
    if(ManualCtrl.EnforceFlag == 0)//
    {
        if(ManualCtrl.StartFlag == 1)//�տ�ʼָ������,��ִ��ָ��
        {
            ManualCtrl.Time = ManualCtrl.AutoTime;
            ManualCtrl.NextStepFlag = 0;
            if(ManualStepProcess() == 0)//����ʱ��֮ǰ,���л�����һ��,���ȴ�ָ��
            {
                ManualCtrl.StartFlag = 0;
                ManualCtrl.ChannelOnsNow = ChannelStatus.Greens;
                ManualCtrl.ChannelOnsBackup = ChannelStatus.Greens;
            }
            else//�����ڰ�ȫ����ʱ��
            {
                OP.LampStateRefreshFlag = 1; //ÿ1sˢ��һ��ͨ��״̬
                return;
            }
        }
        else
        {
            if(ManualCtrl.Time == ManualCtrl.MaxiChannelTransitionTime + 1)//��ִ��ָ��,�Զ��ݼ������ɲ���,���Զ��˳��ֶ�״̬
            {
                ManualCtrl.ChannelOnsNext = ManualCtrl.ChannelOnsBackup;
            }
            if(ManualCtrl.Time == 0)
            {
                uint8_t i;
                ManualCtrl.RemoteCtrlFlag = 0;
                ManualCtrl.LocalCtrlFlag = 0;
                OP.WorkMode = OP.WorkModeBK;
                OP.WorkMode_Reason = WMR_Normal;
                for(i = 0; i<RingMax; i++)
                {
                    if(PhaseState.Ring[i].SeqMax > 0)//�ǿջ�
                    {
                        PhaseState.Ring[i].SecondRemain += 20;
                    }
                }
            }
        }
    }
    else//ִ��ָ������ָ��
    {
        if(ManualCtrl.StartFlag == 1)//�տ�ʼָ������,��ִ��ָ�������ֱ�Ӵ��Զ�����ת��ָ������
        {
            ManualCtrl.StartFlag = 0;
            ManualCtrl.Time = isInTransitionStep();
            if(ManualCtrl.Time==0)//�ǹ���ģʽ
            {
                ManualCtrl.ChannelOnsBackup = ChannelStatus.Greens;
                ManualCtrl.Time = ManualCtrl.AutoTime;
            }
            else //����ģʽ
            {
                ManualCtrl.ChannelOnsBackup = ChannelStatus.Greens | ChannelStatus.Yellows;
            }
        }
        
        if(ManualCtrl.OrderFlag)
        {
            ManualCtrl.OrderFlag = 0;
            ManualCtrl.ChannelOnsNow = ChannelStatus.Greens | ChannelStatus.Yellows;
            if(ManualCtrl.ChannelOnsNow != ManualCtrl.ChannelOnsNext)
            {
                ManualCtrl.MaxiChannelTransitionTime = 0;
                GetMaxiTransitionTime(ManualCtrl.ChannelOnsNow);
                if(ManualCtrl.Time > ManualCtrl.MaxiChannelTransitionTime + 1)
                {
                    ManualCtrl.Time = ManualCtrl.MaxiChannelTransitionTime + 1;
                }
            }
        }
        
        if(ManualCtrl.Time == 0)
        {
            ManualCtrl.Time = ManualCtrl.AutoTime;//ʱ�䵽0
            ManualCtrl.EnforceFlag = 0;
            ManualCtrl.ChannelOnsNow = ManualCtrl.ChannelOnsNext;
        }
    }

    if(ManualCtrl.ExitFlag)
    {
        ManualCtrl.ExitFlag = 0;
        ManualCtrl.EnforceFlag = 0;
        ManualCtrl.ChannelOnsNow = ChannelStatus.Greens | ChannelStatus.Yellows;
        ManualCtrl.ChannelOnsNext = ManualCtrl.ChannelOnsBackup;
        if(ManualCtrl.ChannelOnsNow != ManualCtrl.ChannelOnsNext)
        {
            ManualCtrl.MaxiChannelTransitionTime = 0;
            GetMaxiTransitionTime(ManualCtrl.ChannelOnsNow);
            if(ManualCtrl.Time > ManualCtrl.MaxiChannelTransitionTime + 1)
            {
                ManualCtrl.Time = ManualCtrl.MaxiChannelTransitionTime + 1;
            }
        }
    }

    RemoteChannelStatusCtrl();  
}

void FixedTimeProcess(void)//������ģʽ
{
    uint8_t i, PhaseChangeBit = 0;
    //����������λ�Ƿ����н�����������λ���� PhaseChangeFlag
    //�������Ƿ����н�����������������λ���� CycleOverFlag 
    //��һ����� ���л���CycleOverFlag�����ж��Ƿ������µ�����
    for(i = 0; i < RingMax; i++)
    {
        if(PhaseState.Ring[i].SeqMax)
        {
            if(PhaseState.Ring[i].SecondRemain) PhaseState.Ring[i].SecondRemain--;
            if(PhaseState.Ring[i].SecondRemain == 0)
            {
                if(PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].SeqNum++;
                }
                if(PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    PhaseChangeBit = 1;
                    RingPhaseChange(i);
                }
                else 
                {
                    if(PhaseState.Ring[i].CycleOverFlag==0)
                    {
                        PhaseState.Ring[i].CycleOverFlag = 1;
                        PhaseChangeBit = 1;
                    }
                }
            }
        }
    }
    if(PhaseChangeBit) PhaseState.StateNum++;

    if( PhaseState.Ring[0].CycleOverFlag & \
        PhaseState.Ring[1].CycleOverFlag & \
        PhaseState.Ring[2].CycleOverFlag & \
        PhaseState.Ring[3].CycleOverFlag)  //4������λ���������
    {
        PhaseState.NewCycleFlag = 1; //�µ�����
    }

    if(PhaseState.NewCycleFlag)//�������з���
    {
        PhaseState.NewCycleFlag = 0;
        RunModeProcess();//�µ������жϷ����Ƿ�ı�
        
        if(OP.NewPatternFlag) //�µķ���
        {
            OP.NewPatternFlag = 0;
            NewPatternApply();
        }
        else //�����µķ���
        {
            for(i = 0; i < RingMax; i++)
            {
                if(PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.StateNum = 0;
                    PhaseState.Ring[i].SeqNum = 0;
                    PhaseState.Ring[i].CycleOverFlag = 0;
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    RingPhaseChange(i);
                }
            }
        }
    }
}

void LineCtrlProcess(void)//������ģʽ
{
    uint8_t i, PhaseChangeBit = 0, ChangeFlag = 0;
    //����������λ�Ƿ����н�����������λ���� PhaseChangeFlag
    //�������Ƿ����н�����������������λ���� CycleOverFlag 
    //��һ����� ���л���CycleOverFlag�����ж��Ƿ������µ�����
    for(i = 0; i < RingMax; i++)
    {
        if(PhaseState.Ring[i].SeqMax)
        {
            if(PhaseState.Ring[i].SecondRemain) PhaseState.Ring[i].SecondRemain--;
            if(PhaseState.Ring[i].SecondRemain == 0)
            {
                if(PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].SeqNum++;
                }
                if(PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    PhaseChangeBit = 1;
                    if(RingPhaseChange(i))ChangeFlag = 1;
                }
                else 
                {
                    if(PhaseState.Ring[i].CycleOverFlag==0)
                    {
                        PhaseState.Ring[i].CycleOverFlag = 1;
                        PhaseChangeBit = 1;
                    }
                }
            }
        }
    }
    if(ChangeFlag) GreenWaveTimeChange();
    if(PhaseChangeBit) PhaseState.StateNum++;
    
    if(PhaseState.Ring[0].CycleOverFlag&PhaseState.Ring[1].CycleOverFlag&
       PhaseState.Ring[2].CycleOverFlag&PhaseState.Ring[3].CycleOverFlag)  //4������λ���������
    {
        PhaseState.NewCycleFlag = 1; //�µ�����
    }

    if(PhaseState.NewCycleFlag)//�������з���
    {
        PhaseState.NewCycleFlag = 0;
        RunModeProcess();//�µ������жϷ����Ƿ�ı�
        
        if(OP.NewPatternFlag) //�µķ���
        {
            OP.NewPatternFlag = 0;
            NewPatternApply();
        }
        else //�����µķ���
        {
            for(i = 0; i < RingMax; i++)
            {
                if(PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.StateNum = 0;
                    PhaseState.Ring[i].SeqNum = 0;
                    PhaseState.Ring[i].CycleOverFlag = 0;
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    if(RingPhaseChange(i))ChangeFlag = 1;
                }
            }
            if(ChangeFlag) GreenWaveTimeChange();
            else GreenWaveTimeControl(); //�������,�����¼����Ƿ�ƥ����λ��
        }
    }
}

void FlashingProcess(void)//����ģʽ
{
    PhaseState.NewCycleFlag = 0;
    //������㷨ʹ�ó�ͻ������,�źŻ��Զ��ָ�.
    //�ı��㷨,��Ϊ��ͻ��,�źŻ�����,��ͻ�Ͳ�������.
    if(OP.WorkMode_Reason == WMR_Normal)
    {
        RunModeProcess();//�µ������жϷ����Ƿ�ı�
        if(OP.WorkMode != PatternNow.WorkMode) OP.NewPatternFlag = 1;
        if(OP.NewPatternFlag) //�µķ���
        {
            OP.NewPatternFlag = 0;
            NewPatternApply();
        }
    }
    AutoFlashMode();
}

void AllRedProcess(void)//ȫ��ģʽ
{
    PhaseState.NewCycleFlag = 0;
    RunModeProcess();//�µ������жϷ����Ƿ�ı�
    if(OP.NewPatternFlag) //�µķ���
    {
        OP.NewPatternFlag = 0;
        NewPatternApply();
    }
    AutoAllRedMode();
}

void LampOffProcess(void)//�ص�ģʽ
{
    PhaseState.NewCycleFlag = 0;
    RunModeProcess();//�µ������жϷ����Ƿ�ı�
    if(OP.NewPatternFlag) //�µķ���
    {
        OP.NewPatternFlag = 0;
        NewPatternApply();
    }
    AutoLampOffMode();
}

void SplitModeManage(void)//��Ӧģʽ�����ű�ģʽ����
{
    uint8_t i, OmittedFlag;
    for(i = 0; i < RingMax; i++)
    {
        if((RingSplit[i].Coord & SC_FIXED) == 0)//�ǹ̶���λ
        {
            if(RingSplit[i].Mode == SM_MinVehRecall)//��С��Ӧ
            {
                RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].MinimumGreen;
            }
            else if(RingSplit[i].Mode == SM_MaxVehRecall)//�����Ӧ
            {
                RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
            }
            else if(RingSplit[i].Mode == SM_MaxVehPedRecall)//��С��Ӧ
            {
                RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
            }
            else if(RingSplit[i].Mode == SM_Omitted)//��С��Ӧ
            {
                RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
            }
            else if(RingSplit[i].Mode == SM_PedRecall)//������Ӧ
            {
                RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
            }
        }
    }
}

//��Ӧģʽ�£�
//��������
//1��ʣ��ʱ��С��[��λ�ӳ���+��ȫʱ��]����ôʣ��ʱ��[���ӵ�][��λ�ӳ���+��ȫʱ��],����[����][��λ�ӳ���]+[��ȫʱ��]
//2�����������[�������˷���]����ô��������ʱ�� Ӧ���� ������ȫʱ��
//3��

//���˲���
//1����ʵ�ֵ���ģʽ�µĿ����߼������������󣬲�ִ�У������󣬲�ִ�С�
//2��˼���໷����ģʽ�£�����ʱ������λ��ͻ��ִ�в���
//

//�ҳ��������IO����ţ�Ȼ������˼�������Ӧ�ļ�����ţ���ȡ�������λ�š�
//���ж���λ���Ƿ��ڻ����У�����λ��ģʽΪ��������ģʽ�����������������λ�����־��
//
//PhaseStatus.PhaseNexts
//
//
//
//
//
//1������ִ�е���λʱ����ɣ������һ��λ�Ƿ�Ϊ����������λ
//2���жϸ���λ�Ƿ�����˻�������,�����˻����������ջ�������ִ��,û��,���鰴ť��Ч���.
//2���ж�����λ����������λ��Ӧ�İ�ť�Ƿ���Ч
//3����ť��Ч��ִ����λ����Ч����������λ��
//

uint8_t CheckNextPhaseSplitMode(uint8_t ring)//����Ӧ������λ�����ű�ģʽ
{

    return 0;
}

//0����ִ��,1��������λ
uint8_t peddet_Control(uint8_t ring)
{
    uint8_t i, temp;

    //����λ�Ƿ������� pedestrianRecall
    #if DEBUG
    printf("Num = %d, Next = %d, Index = %d, NextIndex = %d, SplitIndex = %d, SplitNextIndex = %d\n",
            RingPhase[ring].PhaseNum,
            RingPhase[ring].PhaseNext,
            RingPhase[ring].PhaseIndex,
            RingPhase[ring].PhaseNextIndex,
            RingPhase[ring].SplitIndex,
            RingPhase[ring].SplitNextIndex
    );
    
    printf("Next Phase split.mode = %d\n",SplitNow.Phase[RingPhase[ring].SplitNextIndex].Mode);
    #endif

    if(SplitNow.Phase[RingPhase[ring].SplitNextIndex].Mode != SM_PedRecall) return 0;
    //����λ�Ƿ�Ϊ������λ����Ϊ������λ
    if(isVehPhase(RingPhase[ring].PhaseNext) == 1 || isPedPhase(RingPhase[ring].PhaseNext) == 0) return 0;
    //����,˵������λΪ������λ(�޻���channel),�������� pedestrianRecall
    
    if(peddet_hw.rising & 0x7f)//��8���������ڿ��ż��
    {
        temp = 0x01;
        for(i = 0; i < 8; i++)
        {
            if(peddet_hw.rising & temp)
            {
                uint8_t PeddetIndex, CallPhase;                
                if(PeddetTab.peddet[i].Num == (i + 1)) PeddetIndex = i;
                else { temp <<= 1; continue;}
                
                CallPhase = PeddetTab.peddet[PeddetIndex].CallPhase;
                if(CallPhase == 0){ temp <<= 1; continue;}

                if(RingPhase[ring].PhaseNext == CallPhase) 
                {
                    peddet_hw.rising &= ~temp;
                    return 0;
                }
            }
            temp <<= 1;
        }
    }
    //����,˵������λ��Ӧ��ť�޶���
    return 1;
}


void VehicleSenseProcess(void)//��Ӧģʽ
{
    uint8_t i, PhaseChangeBit = 0;
    uint8_t VehicleClear, YellowChange, RedClear;
    uint8_t Walk, PedestrianClear;
    uint8_t RemainTime;
    for(i = 0; i < RingMax; i++)
    {
        if(PhaseState.Ring[i].SeqMax)
        {
            if(PhaseState.Ring[i].SecondRemain) PhaseState.Ring[i].SecondRemain--;
            if(PhaseState.Ring[i].SecondRemain == 0)
            {
                while(PhaseState.Ring[i].SeqNum + 1 < PhaseState.Ring[i].SeqMax)
                {
                    if(peddet_Control(i) == 1)//1��������λ
                    {
                        PhaseState.Ring[i].SeqNum++;
                        PhaseState.Ring[i].PhaseChangeFlag = 1;
                        RingPhaseChange(i);
                        PhaseState.Ring[i].SecondRemain = 0;
                    }
                    else break;
                }
                
                if(++PhaseState.Ring[i].SeqNum < PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    PhaseChangeBit = 1;
                    #if DEBUG
                    printf("SeqNum = %d\n", PhaseState.Ring[i].SeqNum);
                    #endif
                    RingPhaseChange(i);
                }
                else 
                {
                    if(PhaseState.Ring[i].CycleOverFlag==0)
                    {
                        PhaseState.Ring[i].CycleOverFlag = 1;
                        PhaseChangeBit = 1;
                    }
                }
            }
            else if(0)
            {
                VehicleClear = PhaseTab.Phase[RingPhase[i].PhaseIndex].VehicleClear;
                YellowChange = PhaseTab.Phase[RingPhase[i].PhaseIndex].YellowChange;
                RedClear = PhaseTab.Phase[RingPhase[i].PhaseIndex].RedClear;
                Walk = PhaseTab.Phase[RingPhase[i].PhaseIndex].Walk;
                PedestrianClear = PhaseTab.Phase[RingPhase[i].PhaseIndex].PedestrianClear;
                
                RemainTime = PhaseState.Ring[i].SecondRemain;
                if((RingSplit[i].Coord & SC_FIXED) == 0)//�ǹ̶���λ
                {
                    if(RingSplit[i].Mode == SM_MinVehRecall)//��С��Ӧ
                    {
                        RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].MinimumGreen;
                    }
                    else if(RingSplit[i].Mode == SM_MaxVehRecall)//�����Ӧ
                    {
                        RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
                    }
                    else if(RingSplit[i].Mode == SM_MaxVehRecall)//��С��Ӧ
                    {
                        RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
                    }
                    else if(RingSplit[i].Mode == SM_PedRecall)//������Ӧ
                    {
                        RingSplit[i].Time = PhaseTab.Phase[RingPhase[i].PhaseIndex].Maximum1;
                    }
                }
                if(RemainTime > VehicleClear)
                    ;
            }
        }
    }
    
    if(PhaseChangeBit) PhaseState.StateNum++;
    if( PhaseState.Ring[0].CycleOverFlag & \
        PhaseState.Ring[1].CycleOverFlag & \
        PhaseState.Ring[2].CycleOverFlag & \
        PhaseState.Ring[3].CycleOverFlag)  //4������λ���������
    {
        PhaseState.NewCycleFlag = 1; //�µ�����
    }
    
    if(PhaseState.NewCycleFlag)//�������з���
    {
        PhaseState.NewCycleFlag = 0;
        RunModeProcess();//�µ������жϷ����Ƿ�ı�
        #if DEBUG
        printf("NewCycle\n");
        #endif
        if(OP.NewPatternFlag) //�µķ���
        {
            OP.NewPatternFlag = 0;
            NewPatternApply();
        }
        else //�����µķ���
        {
            for(i = 0; i < RingMax; i++)
            {
                if(PhaseState.Ring[i].SeqMax)
                {
                    PhaseState.StateNum = 0;
                    //PhaseState.Ring[i].SeqNum = 0;
                    PhaseState.Ring[i].CycleOverFlag = 0;
                    //printf("SeqNum = %d\n", PhaseState.Ring[i].SeqNum);
                    while(1)
                    {
                        if(peddet_Control(i) == 1)//1��������λ
                        {
                            if(++PhaseState.Ring[i].SeqNum >= PhaseState.Ring[i].SeqMax)PhaseState.Ring[i].SeqNum = 0;
                            PhaseState.Ring[i].PhaseChangeFlag = 1;
                            RingPhaseChange(i);
                            PhaseState.Ring[i].SecondRemain = 0;
                            //printf("SeqNum = %d\n", PhaseState.Ring[i].SeqNum);
                        }
                        else break;
                    }
                    //printf("SeqNum = %d\n", PhaseState.Ring[i].SeqNum);
                    if(++PhaseState.Ring[i].SeqNum >= PhaseState.Ring[i].SeqMax)PhaseState.Ring[i].SeqNum = 0;
                    //printf("SeqNum = %d\n", PhaseState.Ring[i].SeqNum);
                    PhaseState.Ring[i].PhaseChangeFlag = 1;
                    RingPhaseChange(i);
                }
            }
        }
    }
}

/*
ErrorProcess: ϵͳ���������
���ϵͳ���ڽ�������ģʽ,��ô�жϽ�����ԭ��,,

*/
void ErrorProcess(void)
{


}


/*
    ��Ҫ�ж���λ�����ű�ģʽ���ж��Ƿ��ӳ�,�Ƿ��л���
*/
void RunPhaseTimeCalc(void)//10ms����һ��
{
    PhaseState.Phase10msCount++;
    if(OP.SendWorkModeAutoFlag == 1)//������
    {
        if(PhaseState.Phase10msCount == 1)
        {
            OP.SendWorkModeFlag = 1;
        }
        else if(PhaseState.Phase10msCount == 51 && ChannelStatus.Flash > 0)
        {
            OP.SendWorkModeFlag = 1;
        }
    }
    
    if(PhaseState.Phase1sFlag)
    {
        PhaseState.Phase1sFlag = 0;
        PhaseState.Phase10msCount = 0;
        manual_control();//�ֶ����ƹ���
//        if(ManualCtrl.LocalCtrlFlag)//�ֶ�ģʽ
//        {
//            if(OP.WorkMode == ManualFlashing) //�ֶ�����
//            {
//                AutoFlashMode();
//            }
//            else if(OP.WorkMode == ManualAllRead)//�ֶ�ȫ��
//            {
//                AutoAllRedMode();
//            }
//            else if(OP.WorkMode == ManualLampOff)//�ֶ��ص�
//            {
//                AutoLampOffMode();
//            }
//            else if(OP.WorkMode == ManualStep)//�ֶ�
//            {
//                ManualStepProcess();
//                OP.LampStateRefreshFlag = 1;    //ÿ1sˢ��һ��ͨ��״̬
//            }
//            return;
//        }
        
        if(ManualCtrl.RemoteCtrlFlag || ManualCtrl.LocalCtrlFlag)
        {
            if(OP.WorkMode == ManualAppoint)//�ֶ�
            {
                AppointCtrlProcess();
            }
            else
            {
                if(ManualCtrl.Time)
                {
                    if(--ManualCtrl.Time == 0)
                    {
                        ManualCtrl.RemoteCtrlFlag = 0;
                        ManualCtrl.LocalCtrlFlag = 0;
                        OP.WorkMode = OP.WorkModeBK;
                        OP.WorkMode_Reason = WMR_Normal;
                    }
                }
                if(OP.WorkMode == ManualFlashing) //�ֶ�����
                {
                    AutoFlashMode();
                }
                else if(OP.WorkMode == ManualAllRead)//�ֶ�ȫ��
                {
                    AutoAllRedMode();
                }
                else if(OP.WorkMode == ManualLampOff)//�ֶ��ص�
                {
                    AutoLampOffMode();
                }
                else if(OP.WorkMode == ManualStep)//�ֶ���һ��
                {
                    ManualStepProcess();
                    OP.LampStateRefreshFlag = 1;    //ÿ1sˢ��һ��ͨ��״̬
                }
            }
            return;
        }
        
        if(OP.WorkMode == FixedTime)    //�����ڵ�ִ�в���
        {
            FixedTimeProcess();
            OP.LampStateRefreshFlag = 1;    //ÿ1sˢ��һ��ͨ��״̬
        }
        else if(OP.WorkMode == LineCtrl)    //��Э��(�̲�)��ִ�в���
        {
            LineCtrlProcess();
            OP.LampStateRefreshFlag = 1;    //ÿ1sˢ��һ��ͨ��״̬
        }
        else if(OP.WorkMode == VehicleSense)//��Ӧ��ִ�в���
        {
            VehicleSenseProcess();
            OP.LampStateRefreshFlag = 1;    //ÿ1sˢ��һ��ͨ��״̬
        }
        else if(OP.WorkMode == Flashing)//�����ִ�в���
        {
            FlashingProcess();
        }
        else if(OP.WorkMode == AllRed)//ȫ���ִ�в���
        {
            AllRedProcess();
        }
        else if(OP.WorkMode == LampOff)//ȫ���ִ�в���
        {
            LampOffProcess();
        }
        else if(OP.WorkMode == StarupMode)//����ģʽ
        {
            StartupProcess();
        }
    }
}

void GetSeqTime(void)
{
    uint8_t     i = 0,step = 0,PhaseNum = 0,SplitPhaseIndex = 0,PhaseIndex = 0;
    uint16_t    CycleTime = 0;
    for(i = 0; i < RingMax; i++)
    {
        SeqTime.Ring[i].CycleTime = 0;
        for(step = 0; step < PhaseMax; step++)
        {
            PhaseNum = SequenceNow.Ring[i].Phase[step];
            if(IsPhase(PhaseNum))
            {
                PhaseIndex = GetPhaseIndex(&PhaseTab, PhaseNum);
                SplitPhaseIndex = GetSplitPhaseIndex(&SplitNow, PhaseNum);
                if(SplitPhaseIndex < PhaseMax)
                {
                    SeqTime.Ring[i].Time[step] = SplitNow.Phase[SplitPhaseIndex].Time;
                }
                else//���űȱ���δ�������λ,ʹ����С��λʱ��
                {
                    SeqTime.Ring[i].Time[step] = PhaseTab.Phase[PhaseIndex].MinimumGreen+PhaseTab.Phase[PhaseIndex].YellowChange+PhaseTab.Phase[PhaseIndex].RedClear;
                }
                PhaseState.Ring[i].VehicleTransitionTime = PhaseTab.Phase[PhaseIndex].VehicleClear + PhaseTab.Phase[PhaseIndex].YellowChange + PhaseTab.Phase[PhaseIndex].RedClear;
                SeqTime.Ring[i].CycleTime += SeqTime.Ring[i].Time[step];
            }
            else break;
        }
        #if DEBUG
            printf("SeqTime.Ring[%d].CycleTime = %d\r\n",i,SeqTime.Ring[i].CycleTime);
        #endif
        if(CycleTime < SeqTime.Ring[i].CycleTime) CycleTime = SeqTime.Ring[i].CycleTime;
    }

    NowCycleTime = CycleTime;
    //PatternNow.CycleTimeL = CycleTime&0xff;
    //PatternNow.CycleTimeH =(CycleTime>>8);
    #if DEBUG
    printf("NowCycleTime = %d\r\n",CycleTime);
    #endif
}

void GetPhaseStatusMap(void)
{
    uint8_t i;
    uint8_t ringTime[4] = {0};
    uint8_t ringStep[4] = {0};
    uint8_t ringPhase[4] = {0};
    uint16_t t;
    uint32_t PhaseMask = 0;
    uint32_t PhaseStatus = 0;
    
    for(i = 0; i < RingMax; i++)
    {
        if(PhaseState.Ring[i].SeqMax)
        {
            ringTime[i] = SeqTime.Ring[i].Time[0];
        }
    }
    
    PhaseState.StateNum = 0;
    PhaseState.State[0] = 0;//��λ״̬1
    
    for(t=1;t<NowCycleTime;t++)
    {
        PhaseMask = 0;
        for(i = 0; i < RingMax; i++)
        {
            if(PhaseState.Ring[i].SeqMax)
            {
                if(ringTime[i]) ringTime[i]--;
                if(ringTime[i] == 0)
                {
                    if(ringStep[i] < PhaseState.Ring[i].SeqMax)ringStep[i]++;
                    ringTime[i] = SeqTime.Ring[i].Time[ringStep[i]];
                }
                ringPhase[i] = SequenceNow.Ring[i].Phase[ringStep[i]];
                PhaseMask |= (0x1<<(ringPhase[i]-1));
            }
        }
        if(PhaseStatus != PhaseMask)
        {
            PhaseStatus = PhaseMask;
            PhaseState.State[PhaseState.StateNum] = PhaseMask;
            #if DEBUG
            printf("PhaseState[%d] = %04x\r\n",PhaseState.PhaseStatusNum,PhaseState.State[PhaseState.PhaseStatusNum]);
            #endif
            PhaseState.StateNum++;
        }
    }
    PhaseState.StateMax = PhaseState.StateNum;
    PhaseState.StateNum = 0;
    #if DEBUG
    printf("PhaseStatusMax = %d\r\n",PhaseState.StateMax);
    #endif
}

/*
    ͨ����ǰ���е���λ�� ��λ���� �� �ѷ���ʱ��, �жϼ����ͨ��״̬
*/
void LampStateControl(void)//1s����һ��
{
    if(OP.LampStateRefreshFlag)//��̬ˢ��
    {
        OP.LampStateRefreshFlag = 0;
        PhaseStatusControl();
        OverlapStatusControl();
        ChannelStatusControl();//����PhaseStatus,����ChannelStatus
    }
}
