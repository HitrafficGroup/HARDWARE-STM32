#ifndef PUBLIC_H
#define PUBLIC_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define RELEASE     1   //1发布 0调试


#if RELEASE
    #define DEBUG           0
    #define RTCIRQ          0
    #define PhaseNextLog    0
#else
    #define DEBUG           1
    #define RTCIRQ          1
    #define PhaseNextLog    1
#endif


/* 基础数据结构定义 */
typedef struct  //数据格式 BCD码
{
	uint8_t second;     //00-59 
	uint8_t minute;     //00-59 
	uint8_t hour;       //00-23 
	uint8_t day;        //00-31 
	uint8_t month;      //01-12 
	uint8_t year;       //00-99 
	uint8_t week;       //00-06 00周天 01周一 …… 06周六
}RtcType;



#define FILTER_TIME     5       //检测器输入的滤波时间
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

#define ChannelMax      16  //本机最大通道数量
#define VehdetMax       32
#define PeddetMax       8

#define IsVehdet(n)     ((n) > 0 && (n) <= VehdetMax)
#define IsPeddet(n)     ((n) > 0 && (n) <= PeddetMax)



/********************************************************/
//uint8_t 型数据，DEC与BCD相互转换
#define DEC_to_BCD(x)   ((((x)/10)<<4)+((x)%10))
#define BCD_to_DEC(x)   ((((x)>>4)*10)+((x)&0x0f))
#define MAX(a,b)        (a>=b?:a,b)

#define defaultAutoExitRemoteTime      ((Unit.BackupTimeH<<8) | Unit.BackupTimeL)

/********************************************************/
#define SPECIAL_MODE    0xA0

typedef enum 
{
    StarupMode  = 0,        //起动时序 
    FixedTime,              //01固定配时 
    LineCtrl,               //02本地无电缆线控
    VehicleSense,           //03单点感应 
    Flashing,               //04时段闪光
    AllRed,                 //时段设定全红 
    LampOff,                //时段设定关灯 
    
    /*****************************************/
    //本地手动
    ManualStep = 0xA0,      //手动步进
    ManualAppoint,          //手动指定放行 
    ManualFlashing,         //手动黄闪 
    ManualAllRead,          //手动黄闪 
    ManualLampOff,          //手动黄闪
    
    NoPatternFlash,         //无方案黄闪 
    ConflictFlash,          //冲突黄闪 
    ErrorOff,               //故障关灯 
    
    LampTestMode,           //测试专用指令,可用于测试输出或者内部软硬件测试.
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
