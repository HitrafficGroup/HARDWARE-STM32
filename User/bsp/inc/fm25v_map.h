#ifndef _FM25V_MAP_H
#define _FM25V_MAP_H
#include <stdint.h>


//���ļ�����ײ�����
//����洢���ĵ�ַ����
//fm25v.TotalSize = 0x20000 = 131072�ֽ� 
//range 0x00000~0x1ffff   
//2048 = 0x800  0x20000/0x800 = 25


//����ʱ�жϣ�����0-1�ŵ�ַ�ռ�������룬�洢���ݲ�Ϊ0x5A,0xCC,
//��ô���ûָ�Ĭ�ϲ����ĳ���Ȼ����0�ŵ�ַ����Ϊ0x5A,0xCC
//ע���ȳ�ʼ���������޸������롣
//���������У���Ļ��ʾ�״ο�������ȴ���ʼ����
//��ʼ������������������1�롣

//0-1������ 2-15����ռ�
#define StartCodeAddr       0x00000000      //0x0010
#define DeviceInfoAddr      0x00000010      //0x00B0
#define BasicInfoAddr       0x000000C0      //0x00B0
#define UnitAddr            0x00000170      //0x0010
#define CoordAddr           0x00000180      //0x0010
#define ScheduleAddr        0x00000190      //0x0170
#define PlanAddr            0x00000300      //0x04A0
#define ActionAddr          0x000007A0      //0x01A0
#define PatternAddr         0x00000940      //0x02C0
#define SplitAddr           0x00000C00      //0x0520
#define SequenceAddr        0x00001120      //0x0460
#define PhaseAddr           0x00001580      //0x0410
#define ChannelAddr         0x00001990      //0x0110
#define OverlapAddr         0x00001AA0      //0x00B0
#define PeddetAddr          0x00001B50      //0x0040
#define VehdetAddr          0x00001B90      //0x0200
#define PreemptAddr         0x00001D90      //






//������Ϣ*************************************************************************

//typedef struct
//{
//    uint8_t IPV6[16];               // 2.8.1 �źŻ�IPV6,16byte,0x141-0x150
//    uint8_t SubMaskV6[16];          // 2.8.2 �źŻ�IPV6����ǰ׺,16byte,0x151-0x160
//    uint8_t GetWayV6[16];           // 2.8.3 IPV6����,16byte,0x161-0x170
//}IPv6Info_TypeDef;                  // 2.8 IPv6��Ϣ��

//typedef struct
//{
//    uint8_t RemoteIPV6[16];         // 2.9.1 �źŻ�IPV6,16byte,0x171-0x180
//    uint16_t RemoteSocketV6;        // 2.9.2 RemoteSocketV6,2byte,0x181-0x182
//    uint8_t CommunicationTypeV6[16];// 2.9.3 ͨ������,1byte,value 1-3,0x183
//}IPv6RemoteInfo_TypeDef;            // 2.9 IPv6���Ļ���Ϣ��


//������Ϣ********************************************************************************************
//typedef enum { Vehicle=1, NoneVehicle, Pedestrian, VehicleLane, VariableTrafficSigns, BusLane, TramLane, SpecialLane} GroupType;

//typedef struct
//{
//    uint8_t Number;     //��� - 1-64
//    uint8_t Mask;       //���� - bool 0�������1ǿ�ƺ��,
//    uint8_t Prohibit;   //��ֹ - bool 0�������1ǿ�ƹص�,
//}Control_TypeDef;       //��λ ���� ���� ���Ʊ�

//typedef struct
//{
//    uint8_t Number;     // 3.2.1 ������ 
//    uint8_t Type;       // 3.2.2 �������� 
//}GroupConfig_TypeDef;   // 3.2 �������ñ�

//typedef struct
//{
//    uint8_t Number; // 3.3.1 ������ 
//    uint8_t State;  // 3.3.2 ����״̬ 
//}GroupState_TypeDef;// 3.3 ����״̬��

//typedef struct //main 3
//{
//    uint8_t TotalNumber;            // 3.1 ��������,value 1-64,1byte,0x186,
//    GroupConfig_TypeDef Config[64]; // 3.2 �������ñ�,128byte,0x187-0x206
//    GroupState_TypeDef Status[64];  // 3.3 ����״̬��,128byte,0x207-0x286
//    Control_TypeDef Control[64];    // 3.4 ������Ʊ�,192byte,0x287-0x346
//}GroupInfo_TypeDef;                 // 3 ������Ϣ��

//��λ*********************************************************************************************
//typedef struct
//{
//    uint8_t State;  // 4.2.1 ���е�״̬
//    uint8_t Time;   // 4.2.2 ���е�ʱ��,��λΪS,ʱ��Ϊ0��ʾ��ִ�� 
//}Transition_TypeDef;// 4.2 ���ɲ���

//��λ���ñ� phase-config-table
//�ܹ� 1+8+24+8+8 = 49 byte
//typedef struct
//{
//    uint8_t Number;                 // 4.2.1 ��λ��,1byte
//    uint8_t Group[8];               // 4.2.2 ����λ���еĵ��� byte8-byte1 ~ bit63-bit0,8byte
//    Transition_TypeDef LoseTrans[3];// 4.2.3 - 4.2.8   ʧȥ ·Ȩ
//    Transition_TypeDef GetTrans[3]; // 4.2.9 - 4.2.14  ��� ·Ȩ
//    Transition_TypeDef GetStartUpTrans[3]; // 4.2.15 - 4.2.20 �������·Ȩ
//    Transition_TypeDef LoseStartUpTrans[3];// 4.2.21 - 4.2.26 ����ʧȥ·Ȩ
//    uint16_t MinimumGreenTime;      // 4.2.27 ��С�̵�ʱ��
//    uint16_t MaximumGreenTime1;     // 4.2.28 ����̵�ʱ��1
//    uint16_t MaximumGreenTime2;     // 4.2.29 ����̵�ʱ��2
//    uint16_t ExtendedGreenTime;     // 4.2.30 ��λ�ӳ���ʱ��
//    uint8_t Requirement[8];         // 4.2.31 ��λ���� byte8-byte1 ~ bit63-bit0 ��������64-1,Ϊ1��ʾ�ü�����������λ
//}PhaseConfig_TypeDef;               // 4.2 ��λ���ñ� 

//typedef struct //main 4
//{
//    uint8_t TotalNumber;               // 4.1 ��λ����, value 1-64,
//    PhaseConfig_TypeDef Config[64];    // 4.2 ��λ���ñ� 
//    Control_TypeDef Control[64];       // 4.3 ��λ���Ʊ� 
//}PhaseInfo_TypeDef;                    // 4 ��λ��Ϣ

//�����*********************************************************************************************
//��������     0�޳�;   1С�ͳ�;  2���ͳ�;  3���ͳ�;4������;5�й�糵;6���ֳ���;
//typedef enum { None=0, SmallCar, MediumCar, LargeCar, Bus, Tram, SpecialCar} CarType;

//typedef struct
//{
//    uint8_t Number;                 // 5.2.1 �������,1byte
//    uint8_t Type;                   // 5.2.2 ���������,1byte
//    uint16_t FlowCollectPeriod;     // 5.2.3 �����ɼ�����,2byte
//    uint16_t OccupyCollectCycle;    // 5.2.4 ռ���ʲɼ�����,2byte
//    uint8_t Position[128];          // 5.2.5 ��װλ����Ϣ
//}DetectorConfig_TypeDef;            // 5.2 ��������ñ�

//typedef struct
//{
//    uint8_t Number;                 // 5.3.1 �������,1byte
//    uint8_t Occupy;                 // 5.3.2 ��������״̬,bool 0û��,�г� 1byte
//    uint8_t Speed;                  // 5.3.3 ��ǰ����,1byte
//    CarType Type;                   // 5.3.4 ��ǰ�������� 
//    uint8_t PlateNumber[16];        // 5.3.5 ���ƺ�,16byte
//    uint16_t QueueLength;           // 5.3.6 �Ŷӳ���,��λ��,2byte
//}DetectorData_TypeDef;              // 5.3 ��������ݱ�

//main 5
//typedef struct
//{
//    uint8_t TotalNumber;               // 5.1 ���������, value Э�鷶Χ1-128,ʵ�ʹ淶�� 1-32
//    DetectorConfig_TypeDef Config[32]; // 5.2 ��������ñ� 
//    DetectorData_TypeDef Data[32];     // 5.3 ��������ݱ� 
//}DetectorInfo_TypeDef;                 // 5 �������Ϣ 

////��λ�׶�*********************************************************************************************
//typedef struct
//{
//    uint8_t Number;         // 6.2.1��λ�׶α��, 1-64, 1byte
//    uint8_t Phase[8];       // 6.2.2 byte8-byte1 ~ bit63-bit0 ��������64-1,1��ʾ����λ�ڸ���λ�׶��� 
//    uint8_t DelayUp[64];    // 6.2.3 byte1-byte64 ������λ1-��λ64 �������������� 
//    uint8_t EarlyClose[64]; // 6.2.4 byte1-byte64 ������λ1-��λ64 ������������� 
//}PhaseStageConfig_TypeDef;  // 6.2 ��λ�׶����ñ�

//typedef struct
//{
//    uint8_t Number;         // 6.3.1 ��λ�׶α��, 1-64, 1byte
//    uint8_t State;          // 6.3.2 ��λ�׶�״̬, , 1byte
//    uint16_t RunTime;       // 6.3.3 ��λ�׶�����ʱ��,��λ��, 2byte
//    uint16_t RemainTime;    // 6.3.4 ��λ�׶�ʣ��ʱ��, 2byte
//}PhaseStageStatus_TypeDef;  // 6.3 ��λ�׶�״̬��

//typedef struct
//{
//    uint8_t Number;         // 6.4.1 ��λ�׶α��, 1-64, 1byte
//    uint8_t SoftwareRequire;// 6.4.2 ��λ�׶��������, bool 0û��������� 1�������ü��������, 1byte 
//    uint8_t Mask;           // 6.4.3 ��λ�׶� bool 0 ������,1 ���� ��λ�׶�������λΪ���, 1byte 
//    uint8_t Prohibit;       // 6.4.4 ��λ�׶� bool 0 ����ֹ,1 ��ֹ ��λ�׶�������λΪ���, 1byte 
//}PhaseStageControl_TypeDef; // 6.4 ��λ�׶ο��Ʊ� 

//main 6
//typedef struct
//{
//    uint8_t TotalNumber;                    // 6.1 ʵ��������λ�׶���, 1-64 ,1byte 
//    PhaseStageConfig_TypeDef Config[64];    // 6.2 ��λ�׶����ñ� 
//    PhaseStageStatus_TypeDef Status[64];    // 6.3 ��λ�׶�״̬�� 
//    PhaseStageControl_TypeDef Control[64];  // 6.4 ��λ�׶ο��Ʊ� 
//}PhaseStageInfo_TypeDef;                    // 6 ��λ�׶���Ϣ

//��λ��ȫ��Ϣ****************************************************************************************************
//typedef struct
//{
//    uint8_t Number;         // 7.1.1 ��λ�� 
//    uint8_t Sequence[8];    // 7.1.2 ��ͻ���� ,byte8-byte1 bit63-bit0 ��Ӧ��λ64-��λ1,1��ʾ��λ�������λ��ͻ 
//}PhaseConflict_TypeDef;     // 7.1 ��λ��ͻ���ñ� 

//typedef struct
//{
//    uint8_t Number;          // 7.2.1 ��λ�� 
//    uint8_t TimeSequence[64];// 7.2.2 ��ͻ���� ,byte8-byte1 bit63-bit0 ��Ӧ��λ64-��λ1,1��ʾ��λ�������λ��ͻ 
//}PhaseGreenGapConfig_TypeDef;// 7.2 ��λ�̼�����ñ�

//typedef struct //main 7
//{
//    PhaseConflict_TypeDef Conflict[64];        // 7.1 ��λ��ͻ���ñ�
//    PhaseGreenGapConfig_TypeDef GreenGap[64];  // 7.2 ��λ�̼�����ñ�
//}PhaseSafeInfo_TypeDef;                        // 7 ��λ��ȫ��Ϣ

//��������***************************************************************************************************
//typedef struct 
//{
//    uint8_t Number;             // 8.2.1 �����źű�� 
//    uint8_t PhaseStage;         // 8.2.2 �����ź��������λ�׶α�� 
//    uint8_t PriorityLevel;      // 8.2.3 �����ź���������ȼ� ԽСԽ��, 0�����ȼ����
//    uint8_t Mask;               // 8.2.4 �����ź����α�־ bool 0������, 1���� 
//}UrgentPriorityConfig_TypeDef;          // 8.2 �������ñ�

//typedef struct 
//{
//    uint8_t Number;             // 8.3.1 �����źű�� 
//    uint8_t State;              // 8.3.2 �����ź�����״̬ bool 0û������, 1������ 
//    uint8_t Execution;          // 8.3.3 �����ź�ִ��״̬ bool 0û��ִ��, 1��ִ��
//}UrgentPriorityState_TypeDef;         // 8.3 ����״̬�� 

//typedef struct //main 8
//{
//    uint8_t PriorityNumber;                           // 8.1 ʵ�������� 1-64
//    UrgentPriorityConfig_TypeDef PriorityConfig[64];  // 8.2 �������ñ�
//    UrgentPriorityState_TypeDef PriorityState[64];    // 8.3 ����״̬�� 
//    
//    uint8_t UrgentNumber;                             // 8.4 ʵ�ʽ����� 1-64
//    UrgentPriorityConfig_TypeDef UrgentConfig[64];    // 8.5 �������ñ�
//    UrgentPriorityState_TypeDef UrgentState[64];      // 8.6 ����״̬�� 
//}UrgentPriority_TypeDef;                              // 8 ��������

//������Ϣ*************************************************************************************************
//typedef struct
//{
//    uint8_t Number;                 // 9.2.1 ������� 
//    uint8_t IntersectionNumber;     // 9.2.2 ��������·����� 1-8 �����źŻ����ƶ�·�� 
//    uint32_t Cycle;                 // 9.2.3 �������� ��λ�� 
//    uint8_t CoordinateStageNumber;  // 9.2.4 ������Э����λ�Ľ׶κ� 1-16
//    uint16_t PhaseDifference;       // 9.2.5 ������λ��ʱ�� 
//    uint8_t Sequence[16];           // 9.2.6 ��λ�׶�����,ÿ������ʾ��λ�׶α��, ��0ִ�е�16, ����16��0���
//    uint16_t Time[16];
//}PlanConfig_TypeDef;


//typedef struct //main 9
//{
//    uint8_t TotalNumber;                    // 6.1 ʵ�ʷ�����, 1-128 ,1byte 

//}PlanInfo_TypeDef;  // 9 ������Ϣ

#endif
