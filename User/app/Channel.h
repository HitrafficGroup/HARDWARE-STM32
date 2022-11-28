#ifndef CHANNEL_H
#define CHANNEL_H
#include <stdint.h>


enum LampDrive_Type
{
    LD_BLACK        = 0, 
    LD_RED          = 0x0001, 
    LD_YELLOW       = 0x0002, 
    LD_REDYELLOW    = 0x0003, 
    LD_GREEN        = 0x0004, 
    LD_ALL          = 0x0007,
};

//��������CHANNEL_CONTROL_TYPE
typedef enum 
{
    CCT_OTHER = 1,      //����
    CCT_VEHICLE,        //����
    CCT_PEDESTRIAN,     //����
    CCT_OVERLAP,        //����
    CCT_FLASH,
    CCT_GREEN,
    CCT_RED,
}CHANNEL_CONTROL_TYPE;  //ͨ����������

//����ģʽCHANNEL_FLASH_MODE
#define CFM_Yellow      ((uint8_t)0x02)     //����
#define CFM_Red         ((uint8_t)0x04)     //����
#define CFM_Alternate   ((uint8_t)0x08)     //������

//�Զ�ģʽCHANNEL_DIM_MODE
#define CDM_Green       ((uint8_t)0x01)     //�̵�
#define CDM_Yellow      ((uint8_t)0x02)     //�Ƶ�
#define CDM_Red         ((uint8_t)0x04)     //���
#define CDM_Alternate   ((uint8_t)0x08)     //����

//ͨ�����Ʒ�λCHANNEL_POSITION_MODE
#define POS_Other       ((uint8_t)0x00)     //����
#define POS_East        ((uint8_t)0x01)     //��
#define POS_South       ((uint8_t)0x02)     //��
#define POS_West        ((uint8_t)0x03)     //��
#define POS_North       ((uint8_t)0x04)     //��
#define POS_NorthEast   ((uint8_t)0x05)     //����
#define POS_SouthEast   ((uint8_t)0x06)     //����
#define POS_SouthWest   ((uint8_t)0x07)     //����
#define POS_NorthWest   ((uint8_t)0x08)     //����

//ͨ�����Ʒ���CHANNEL_DIRECTION_MODE
#define DIR_Other       ((uint8_t)0x00)     //����
#define DIR_Left        ((uint8_t)0x01)     //��ת
#define DIR_Straight    ((uint8_t)0x02)     //ֱ��
#define DIR_Right       ((uint8_t)0x03)     //��ת
#define DIR_Pedestrian  ((uint8_t)0x04)     //����
#define DIR_Turn        ((uint8_t)0x05)     //��ͷ
#define DIR_Bicycle     ((uint8_t)0x06)     //���г�
//Left Straight Right Turn Pedestrian Bicycle


//�ֿط�λ
#define MANUAL_POS_Other       ((uint8_t)0x00)     //����
#define MANUAL_POS_East        ((uint8_t)0x01)     //��
#define MANUAL_POS_South       ((uint8_t)0x02)     //��
#define MANUAL_POS_West        ((uint8_t)0x04)     //��
#define MANUAL_POS_North       ((uint8_t)0x08)     //��
#define MANUAL_POS_NorthEast   ((uint8_t)0x10)     //����
#define MANUAL_POS_SouthEast   ((uint8_t)0x20)     //����
#define MANUAL_POS_SouthWest   ((uint8_t)0x40)     //����
#define MANUAL_POS_NorthWest   ((uint8_t)0x80)     //����

//�ֿط���
#define MANUAL_DIR_Other       ((uint8_t)0x00)     //����
#define MANUAL_DIR_Left        ((uint8_t)0x01)     //��ת
#define MANUAL_DIR_Straight    ((uint8_t)0x02)     //ֱ��
#define MANUAL_DIR_Right       ((uint8_t)0x04)     //��ת
#define MANUAL_DIR_Pedestrian  ((uint8_t)0x08)     //����
#define MANUAL_DIR_Turn        ((uint8_t)0x10)     //��ͷ
#define MANUAL_DIR_Bicycle     ((uint8_t)0x20)     //���г�


typedef struct
{
    uint8_t Num;            //ͨ����
    uint8_t ControlSource;  //����Դ
    uint8_t ControlType;    //��������
    uint8_t Flash;          //����ģʽ
    uint8_t Dim;            //�Զ�ģʽ
    uint8_t Position;       //
    uint8_t Direction;      //
    uint8_t CountdownID;    //�������Ƕ���һ�ŵ���ʱ���ñ�
}ChannelType; //ͨ�����ݶ���

typedef struct
{
    uint8_t Maximum;//���ͨ���������32
    ChannelType Channel[32];
    uint8_t Reserve[15];
}ChannelTable;   //1+8*32+15 = 272 = 0x0110


/********************************************************/
typedef struct//ÿbit����һ��ͨ���Ķ�Ӧ��ɫ״̬
{
    uint32_t     Reds;
    uint32_t     Yellows;
    uint32_t     Greens;
    uint32_t     Flash;     //1����0����
}ChannelStatusType;
/********************************************************/
typedef struct//ÿbit����һ��ͨ���Ķ�Ӧ��ɫ״̬
{
    uint32_t     Reds;
    uint32_t     Yellows;
    uint32_t     Greens;
}ChannelReadStatusType;

//�Զ���ͨ��״̬������
typedef enum 
{
    BLACK = 0,
    GREEN,
    YELLOW,
    RED,
    RED_YELLOW,         //���ͬ��
    RED_YELLOW_FLASH,   //��ƽ�����
    GREEN_FLASH,
    YELLOW_FLASH,
    RED_FLASH,
}ChannelStateMode;
/********************************************************/


extern ChannelTable             ChannelTab;         //ͨ����
extern ChannelStatusType        ChannelStatus;      //ͨ��״̬��
extern ChannelReadStatusType    ChannelReadStatus;


void ChannelInit(void);
void AutoFlashMode(void);
void AutoAllRedMode(void);
void AutoLampOffMode(void);

uint8_t isVehPhase(uint8_t phaseNum);
uint8_t isPedPhase(uint8_t phaseNum);
uint32_t GetAppointChannel(uint8_t Pos, uint8_t Dir);

void LampControl(uint8_t tick10msCount);

#endif
