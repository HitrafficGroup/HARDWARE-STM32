#ifndef SPLIT_H
#define SPLIT_H
#include "public.h"


/*
    一、相位时间SplitTime 0-255
        相位的放行时间。包含了机动车相位的绿灯、绿闪、黄灯、全红时间
        以及行人相位的放行时间和清空时间。
    二、绿信比模式SplitMode
    1-可变标志：该相位在此绿信比模式下作为可变标志输出
    2-无
    3-最小车辆响应：
        感应控制时，机动车相位被强制执行最小绿。
        此属性优先级高于相位参数中的“机动车自动请求”属性。
    4-最大车辆响应：
        感应控制时，机动车相位被强制执行最大绿。
        此属性优先级高于相位参数中的“机动车自动请求”属性。
    5-行人响应：
        感应控制时，行人相位被强制获取放行权。
        此属性优先级高于相位参数中的“行人自动请求”属性。
    6-最大车辆/行人响应：
        感应控制时，机动车相位被强制执行最大绿，行人相位被强制获取放行权。
        此属性优先级高于相位参数中的“机动车自动请求”属性和“行人自动请求”属性。
    7-忽略相位
        该相位在此绿信比模式下，从方案中去除。
    三、协调配置Coord
        bit0：1-当协调控制时，该相位作为协调相位，与其他路口进行协调。
        bit1：1-作为关键相位
        bit2：1-作为固定相位
*/
typedef enum 
{
    SM_Other = 1,
    SM_None = 2,
    SM_MinVehRecall = 3,
    SM_MaxVehRecall = 4,
    SM_PedRecall = 5,
    SM_MaxVehPedRecall = 6,
    SM_Omitted = 7,
}SplitMode;

#define SC_NONE       0x00
#define SC_COORD      0x01
#define SC_KEY        0x02
#define SC_FIXED      0x04

//相位时间  相位的放行时间。
//包含了机动车相位的绿灯、绿闪、黄灯、全红时间
//以及行人相位的放行时间和清空时间。
typedef struct
{
    uint8_t PhaseNum;           //相位号
    uint8_t Time;               //相位时间
    uint8_t Mode;               //相位模式
    uint8_t Coord;              //协调配置 0-1 
}PhaseSplitType; //相位绿信比定义

typedef struct
{
    uint8_t         Num;     //绿信比号
    PhaseSplitType  Phase[PhaseMax];    //16
}SplitType;    //绿信比数据

typedef struct
{
    uint8_t     Maximum;
    SplitType   Split[SplitMax];                //20
    uint8_t     Reserve[11];
}SplitTable;     //绿信比表 65 * 20 + 12 = 1312 = 82 * 16 = 0x0520 


extern SplitType        SplitNow;
extern SplitTable       SplitTab;  //绿信比表 
extern PhaseSplitType   RingSplit[RingMax];      //绿信比

uint8_t GetSplitPhaseIndex(SplitType* Split, uint8_t PhaseNum);

void SplitDefault(void); 
void SplitDataInit(uint8_t n);
void SplitXDataInit(SplitType* Split);


//coordPatternStatus    //2.5.10
//localFreeStatus       //2.5.11
//coordCycleStatus      //2.5.12
//coordSyncStatus       //2.5.13
//systemPatternControl  //2.5.14
//systemSyncControl     //2.5.15


#endif
