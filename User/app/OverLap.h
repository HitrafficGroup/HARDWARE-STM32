#ifndef OVERLAP_H
#define OVERLAP_H

#include "public.h"


#define OverlapTabSize  0x00B0


typedef enum{OT_OTHER = 1, OT_NORMAL, OT_MINUSGREENYELLOW} Overlap_Type;

typedef struct
{
    uint8_t  Num;                //编号 1-255
    uint8_t  Type;               //类型 1其他 2常规 3黄绿停止
    uint8_t  IncludedPhases[2];  //母相位 所跟随放行的机动车相位
    uint8_t  ModifierPhases[2];  //修正相位 
    uint8_t  TrailGreen;         //跟随绿灯 0-255 跟随相位跟随母相位结束放行绿灯后，继续放行的绿灯时间，单位：秒。
    uint8_t  TrailClear;
    uint8_t  TrailYellow;        //跟随黄灯 0-255 当跟随相位的绿灯或绿闪时间不为零时，放行的黄灯时间。绿灯和绿闪时间都为零时，黄灯时间使用母相位黄灯时间。
    uint8_t  TrailRed;           //跟随红灯 0-255 当跟随相位的绿灯或绿闪时间不为零时，放行的全红时间。绿灯和绿闪时间都为零时，全红时间使用母相位全红时间。
}OverlapType; //跟随相位 

typedef struct 
{
    uint8_t     Maximum;
    OverlapType Overlap[OverlapMax];    //10 byte * 16
    uint8_t     Reserve[15];
}OverlapTable; //跟随相位表      //176

typedef struct
{
    uint32_t Reds;
    uint32_t Yellows;
    uint32_t Greens;
    uint32_t Flashs;
}OverlapStatusType;


extern uint16_t     IncludedPhases[OverlapMax];     //母相位 所跟随放行的机动车相位
extern uint16_t     ModifierPhases[OverlapMax];     //修正相位 
extern uint16_t     OverlapCounter[OverlapMax];      //跟随相位时间统计
extern OverlapTable         OverlapTab;  //搭接相位表
extern OverlapStatusType    OverlapStatus;




void OverlapInit(void);
//跟随相位号 + 跟随相位表 = 跟随相位索引
uint8_t GetOverlapIndex(OverlapTable* Overlap_tab, uint8_t OverlapNum);

#endif
