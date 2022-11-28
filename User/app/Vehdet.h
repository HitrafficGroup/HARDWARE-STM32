#ifndef DETECTORVEHICLE_H
#define DETECTORVEHICLE_H

#include "public.h"


typedef struct
{
    uint8_t  GetVehicleDetectorStateFlag;
    uint8_t  DelayCount;
}VehicleDetectorTimer_Type;


typedef struct
{
    uint8_t  Num;           //��������
    uint8_t  Index;         //��������ò����ڼ�������е�����
    uint8_t  Valid;         //1��Ч 0��Ч
    uint8_t  State;         //1��Ч 0��Ч
    uint32_t DelayCount;    //��λ10ms��
    uint32_t Time;          //��ǰ״̬����ʱ�䣬��λ10ms
    uint16_t CarsByCycle;   //ÿ���ڹ�����
    uint16_t CarsByMinute;  //ÿ���ӳ�����  �Ƿ�ͳ��ÿ���ڹ�������������ÿ���ӣ�
}VehDetState_Type;

typedef struct
{
    VehDetState_Type VehDet[VehdetMax];
}VehDetStateTable;
//�ļ���֯��ÿ��һ���Է���Ϊ��λ��������¼ �ļ�/��Ϣ������ͳ������һ����


//Options bits
#define VDO_require             0x80
#define VDO_queue               0x40
#define VDO_strategy            0x20
#define VDO_extend              0x10
#define VDO_redRequireLock      0x08
#define VDO_yellowRequireLock   0x04
#define VDO_recordOccupancy     0x02
#define VDO_recordFlow          0x01

//Alarms bits
#define VDA_otherError          0x80
#define VDA_reserve6            0x40
#define VDA_reserve5            0x20
#define VDA_configError         0x10
#define VDA_communicationError  0x08
#define VDA_senseError          0x04
#define VDA_overlimitError      0x02
#define VDA_noResponseError     0x01

//ReportedAlarms bits
#define VDR_reserve7            0x80
#define VDR_reserve6            0x40
#define VDR_reserve5            0x20
#define VDR_floatingOverlimit   0x10
#define VDR_shortCircuit        0x08
#define VDR_openCircuit         0x04
#define VDR_watchdogFailure     0x02
#define VDR_other               0x01




typedef struct  //14
{
    uint8_t Num;            //��������
    uint8_t Options;        //�����ѡ��
    uint8_t CallPhase;      //������λ  �����������Ӧ�Ļ�������λ�����ü������⵽����ͨ�����ź�ʱ����Ӧ��������λ��������Ӧ��
    uint8_t SwitchPhase;    //������λ  �����ת����λ��������ָ����λ�ǻƵƻ�����ת����λ���̵�ʱ�������������Ӧ��ת��������λ
    uint8_t Extend;         //�ӳ���    �ڸ�Ӧ�����£�������λ�̵�ʱ���ӳ�һ�ε�ʱ�䡣��������λ�����ĵ�λ�ӳ�����ͬ������ֻ��ѡ��ʹ�ü�������ӳ��̡�ѡ������Ч����֮ʹ�á�������λ���ĵ�λ�ӳ���
    uint8_t QueueLimit;     //��������  ������Ŷ����Ʋ�������λ���롣
    uint8_t NoActivity;     //����Ӧʱ��         ������ڸö�ʱ���ڣ�û�м�⵽����ͨ������ʾ�ü�������ֹ��ϣ��ù��ϱ��źŻ����������־����ֵΪ��ʱ������ܲ����á���λΪ�����ӡ�
    uint8_t MaxPresence;    //��������Ӧʱ��   ������ڸö�ʱ���ڣ�һֱ��⵽����ͨ������ʾ�ü�������ֹ��ϣ��ù��ϱ��źŻ����������־����ֵΪ��ʱ������ܲ����á���λΪ�����ӡ�
    uint8_t ErraticCounts;  //�������    �������ȷ��������ϲ��������һ�����������ָ�Ӧ��ǿ������豸����Ϊ���ڹ��ϣ��ü��������Ϊ��ʧ�ܵġ��������������ֵ��Ϊ�㣬���رնԸü��������ϣ���λ����/���ӡ�
    uint8_t FailTime;       //ʧ��ʱ��      �������������û�еõ���Ӧ֮ǰ�����ʧ��ʱ����û���κ�������ȡ�����μ�¼�����󣬲���������Ӧ��
    uint8_t Alarms;         //����    
    uint8_t ReportedAlarms; //����    
    uint8_t Reset;          //����0-1       ���ò����ŵ������״̬����?
    uint8_t Reserve;
    uint16_t Delay;         //�ӳ�      �����������ʱ��ﵽ�ӳ�ʱ�����õĳ��ȣ��ż�¼һ������,��׼��λ10S
}Vehdet;

typedef struct
{
    Vehdet vehdet[VehdetMax];
}VehdetTable;


typedef struct
{
    uint32_t Active;
    uint32_t Alarms;
}VehdetStatusTable;

typedef struct
{
    uint8_t Volume;     //��ֵ�ķ�ΧӦΪ0��254����ʾ���ռ��ڼ�ͨ����ؼ������ŵĽ�ͨ����
    uint8_t Occupancy;  //̽����ռ����ռ�ռ����ݻ�̽������Ԫ�����Ϣ�����ռ���ʵİٷֱȡ�
    //Range     Meaning
    //0-200     Detector Occupancy in 0.5% Increments
    //201-209   Reserved
    //210       Max Presence Fault
    //211       No Activity Fault
    //212       Open loop Fault
    //213       Shorted loop Fault
    //214       Excessive Change Fault
    //215       Reserved
    //216       Watchdog Fault
    //217       Erratic Output Fault
    //218-255   Reserved
}VolumeOccupancy_Type;

typedef struct
{
    VolumeOccupancy_Type VehDet[VehdetMax];
}VolumeOccupancyTable;

extern VehdetTable              VehdetTab;          //��������
extern VehdetStatusTable        VehdetStatusTab;    //����״̬��
extern VolumeOccupancyTable     VolumeOccupancyTab; //������ռ���ʱ�
extern VehDetStateTable         VehDetStateTab;       //����״̬��

void VehicleDetectorInit(void);
void GetVehDetSta(void);//1msִ��һ�Σ������״̬��ȡ
void VehDetStaCount(void);    //10msִ��һ�Σ������״̬ʱ��ͳ��




#endif
