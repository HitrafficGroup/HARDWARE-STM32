#ifndef PUBLIC_H
#define PUBLIC_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define RELEASE     1   //1���� 0����


#if RELEASE
    #define DEBUG           0
    #define RTCIRQ          0
    #define PhaseNextLog    0
#else
    #define DEBUG           1
    #define RTCIRQ          1
    #define PhaseNextLog    1
#endif


/* �������ݽṹ���� */
typedef struct  //���ݸ�ʽ BCD��
{
	uint8_t second;     //00-59 
	uint8_t minute;     //00-59 
	uint8_t hour;       //00-23 
	uint8_t day;        //00-31 
	uint8_t month;      //01-12 
	uint8_t year;       //00-99 
	uint8_t week;       //00-06 00���� 01��һ ���� 06����
}RtcType;



#define FILTER_TIME     5       //�����������˲�ʱ��
/********************************************************/
#define ScheduleMax     40
#define PlanMax         16
#define PeriodMax       24
#define ActionMax       100
#define PatternMax      100
#define SequenceMax     16
#define SplitMax        20
#define OverlapMax      16


#define RingMax         4
#define PhaseMax        16

#define IsRing(n)       ((n) > 0 && (n) <= RingMax)
#define IsPhase(n)      ((n) > 0 && (n) <= PhaseMax)

#define ChannelMax      16  //�������ͨ������
#define VehdetMax       32
#define PeddetMax       8

#define IsVehdet(n)     ((n) > 0 && (n) <= VehdetMax)
#define IsPeddet(n)     ((n) > 0 && (n) <= PeddetMax)



/********************************************************/
//uint8_t �����ݣ�DEC��BCD�໥ת��
#define DEC_to_BCD(x)   ((((x)/10)<<4)+((x)%10))
#define BCD_to_DEC(x)   ((((x)>>4)*10)+((x)&0x0f))
#define MAX(a,b)        (a>=b?:a,b)

#define defaultAutoExitRemoteTime      ((Unit.BackupTimeH<<8) | Unit.BackupTimeL)

/********************************************************/
#define SPECIAL_MODE    0xA0

typedef enum 
{
    StarupMode  = 0,        //��ʱ�� 
    FixedTime,              //01�̶���ʱ 
    LineCtrl,               //02�����޵����߿�
    VehicleSense,           //03�����Ӧ 
    Flashing,               //04ʱ������
    AllRed,                 //ʱ���趨ȫ�� 
    LampOff,                //ʱ���趨�ص� 
    
    /*****************************************/
    //�����ֶ�
    ManualStep = 0xA0,      //�ֶ�����
    ManualAppoint,          //�ֶ�ָ������ 
    ManualFlashing,         //�ֶ����� 
    ManualAllRead,          //�ֶ����� 
    ManualLampOff,          //�ֶ�����
    
    NoPatternFlash,         //�޷������� 
    ConflictFlash,          //��ͻ���� 
    ErrorOff,               //���Ϲص� 
    
    LampTestMode,           //����ר��ָ��,�����ڲ�����������ڲ���Ӳ������.
    DegradingFlashing = 0xD0,
    DegradingLampOff,
}RUN_MODE;


typedef enum 
{
    WMR_Normal = 0,
    WMR_LocalManual,
    WMR_RemoteManual,
    WMR_RemoteGuard,
    
    WMR_NoDayPlan = 0xA0,
    WMR_NoAction,
    WMR_NoPattern,
    WMR_NoSequence,
    WMR_NoSplit,
    WMR_RedGreenConflict,
    WMR_GreenConflict,
    WMR_RedFailed,
    
    WMR_WMR_NoPar,
    WMR_Hardware,
    WMR_Other = 0xff,
}WorkMode_Reason_Type;




#endif
