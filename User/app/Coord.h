#ifndef COORD_H
#define COORD_H

#include "public.h"

typedef struct
{
    uint8_t OperationalMode;   //Э������ģʽ 0�Զ� 1-253�ֶ����� 254���ظ�Ӧ 255����
    uint8_t CorrectionMode;    //Э��У����ʽ 1���� 2פ���ȴ� 3ƽ������ 4������
    uint8_t MaximumMode;       //Э�����ʽ 1���� 2���1 3���2 4���Լ��
    uint8_t ForceMode;         //Э��ǿ�Ʒ�ʽ 1���� 2���� 3�̶�
    uint8_t Reserve[12];
}CoordType;   //Э����


extern CoordType   Coord;

void CoordInit(void);

#endif
