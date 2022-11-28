/*
*********************************************************************************************************
*	                                  
*	ģ������ : CAN�������
*	�ļ����� : can_network.c
*	��    �� : V1.0
*
*********************************************************************************************************
*/
#include "bsp.h"


/* ����ȫ�ֱ��� */
CanTxMsg CanTxMsgStruct;			/* ���ڷ��� */
CanRxMsg CanRxMsgStruct;			/* ���ڽ��� */


/*
*********************************************************************************************************
*	�� �� ��: SendCanMsg
*	����˵��: ����һ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SendCanMsg(uint8_t *p, uint8_t length)
{
	uint8_t n;
	CanTxMsgStruct.DLC = length;
	for(n = 0;n < length; n++)
	{
		CanTxMsgStruct.Data[n] = *p;
		p++;
	}
	CAN_Transmit(CAN1, &CanTxMsgStruct);
}

/*
*********************************************************************************************************
*	�� �� ��: can_Init
*	����˵��: ����CANӲ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void can_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;	
    
    //id    [31:24]     [23:16]                 [15:8]      [7:0]
    //mask  [31:24]     [23:16]                 [15:8]      [7:0]
    //map   stid[10:3]  stid[2:0]exid[17:13]    exid[12:5]  exid[4:0] IDE RTR 0
    
    uint16_t std_id = 0x10;
    uint32_t ext_id = 0;
    uint32_t mask = 0;
    
	/* PA11��PA12��������ΪAFIOģʽ, �л���CAN���� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* ʹ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* ����CAN�źŽ�������: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	/* GPIO����Ϊ��������ģʽ */
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* ����CAN�źŷ�������: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		/* ����Ϊ����������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* ����GPIO����ٶ� */
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* ԭ����CAN���ź�USB��������ͬ�Ŀ��ߣ����ŵ���ӳ�书�ܽ�CAN�����л���PB8��PB9 */
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE);	/* ʹ��CAN1����ӳ�� */
	
	/* ʹ��CAN����ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	CAN_DeInit(CAN1);											/* ��λCAN�Ĵ��� */
	CAN_StructInit(&CAN_InitStructure);		/* ���CAN�����ṹ���ԱΪȱʡֵ */
	
	/*
		TTCM = time triggered communication mode
		ABOM = automatic bus-off management 
		AWUM = automatic wake-up mode
		NART = no automatic retransmission
		RFLM = receive FIFO locked mode 
		TXFP = transmit FIFO priority		
	*/
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* ��ֹʱ�䴥��ģʽ��������ʱ���), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* ��ֹ�Զ����߹رչ��� */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* ��ֹ�Զ�����ģʽ */
	CAN_InitStructure.CAN_NART = DISABLE;			/* ��ֹ�ٲö�ʧ��������Զ��ش����� */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* ��ֹ����FIFO����ģʽ */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* ��ֹ����FIFO���ȼ� */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/* ����CANΪ��������ģʽ */
	
	/* 
		CAN ������ = RCC_APB1Periph_CAN / Prescaler / (SJW + BS1 + BS2);
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		�����У�����CAN������Ϊ500Kbps		
		CAN ������ = 360000000 / 6 / (1 + 6 + 5) / = 500kHz		
	*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 9;
	CAN_Init(CAN1, &CAN_InitStructure);
	
	/* ����CAN�˲���0 */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;		/* �˲�����ţ�0-13����14���˲��� */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/* �˲���ģʽ������ID����ģʽ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/* 32λ�˲� */
    //��׼֡
	CAN_FilterInitStructure.CAN_FilterIdHigh = std_id<<5;               /* �����ID�ĸ�16bit */
	CAN_FilterInitStructure.CAN_FilterIdLow = 0 | CAN_ID_STD;			/* �����ID�ĵ�16bit */
    //��չ֡
	//CAN_FilterInitStructure.CAN_FilterIdHigh = ((ext_id<<3)>>16)&0xffff;/* �����ID�ĸ�16bit */
	//CAN_FilterInitStructure.CAN_FilterIdLow = (ext_id<<3) | CAN_ID_EXT; /* �����ID�ĵ�16bit */
    
    //���μĴ���������
	//�����˼·���Ƚ���׼CAN ID����չCAN ID��Ӧ��IDֵ������ȡ����Ϊʲô��
    //�����Ϊ���ҳ�����CAN ID����Щλ����ͬ�ģ�����ͬ��λ��˵����Ҫ���ģ�
    //��Ҫ���ĵ�λ��Ӧ��������λӦ������Ϊ1�������Ҫȡ��һ�¡��������������3λ��
    
    //����Ϊʲô����18λ����Ϊ��ISO11898�п��Կ�����
    //��׼CAN IDռID18~ID28��Ϊ����CAN_FilterIdHigh���룬Ӧ����2λ��
    //����Ϊ������չCAN��Ӧ����Ӧ��������16λ����ˣ��ܹ�Ӧ����2+16��18λ��
    //Ҳ��������һ����ʽ����⣺ֱ�ӿ�Mapping�����ݣ�����STDID���EXID[0]ƫ����18λ,�������18λ.
    
	mask =(std_id<<18);
	mask ^=ext_id;//�������ı�׼CAN����չCAN����ȡ��
	mask =~mask;
	mask <<=3;//����������3λ
	mask |=0x02; //ֻ��������֡��������Զ��֡
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(mask>>16)&0xffff; //�������μĴ������ֽ�
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=mask&0xffff;   //�������μĴ������ֽ�
    
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;		/* �˲�����FIFO 0 */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				/* ʹ���˲��� */
	CAN_FilterInit(&CAN_FilterInitStructure);
    /* ��䷢�Ͳ����� Ҳ������ÿ�η��͵�ʱ���� */
	CanTxMsgStruct.StdId = 0x29;
	CanTxMsgStruct.ExtId = 0x00;
	CanTxMsgStruct.RTR = CAN_RTR_DATA;	//����֡-Զ��֡
	CanTxMsgStruct.IDE = CAN_ID_STD;	//CAN_ID_EXD;		//��׼֡-��չ֡
}

/*
*********************************************************************************************************
*	�� �� ��: can_NVIC_Config
*	����˵��: ����CAN�ж�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
void can_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* CAN FIFO0 ��Ϣ�����ж�ʹ�� */ 
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: can_ISR
*	����˵��: CAN�жϷ������. ��������� stm32f10x_it.c�б�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void can_ISR(void)
{
	CAN_Receive(CAN1, CAN_FIFO0, &CanRxMsgStruct);
    bsp_LedToggle(LED_COM);
	if ((CanRxMsgStruct.StdId == 0x10) && (CanRxMsgStruct.IDE == CAN_ID_STD))// && (CanRxMsgStruct.DLC == 3)
	{
        printf_fifo_hex(CanRxMsgStruct.Data, CanRxMsgStruct.DLC);
        if((Manual_start == CanRxMsgStruct.Data[0] || Manual_auto == CanRxMsgStruct.Data[0]) && CanRxMsgStruct.DLC == 6)
        {
			CanTxMsgStruct.DLC = 6;
            memcpy(CanTxMsgStruct.Data, CanRxMsgStruct.Data, 6);
            CAN_Transmit(CAN1, &CanTxMsgStruct);
            //SendCanMsg(CanTxMsgStruct.Data, 6);
            
            if(Manual_start == CanRxMsgStruct.Data[0])//��ʼ�ֶ�
            {
                #if DEBUG
                printf("RemoteGuard\r\n");
                #endif
                if(Manual_appoint == CanRxMsgStruct.Data[1])
                {
                    ManualCtrl.Pos = CanRxMsgStruct.Data[2];
                    ManualCtrl.Dir = CanRxMsgStruct.Data[3];
                    ManualCtrl.AutoTime = CanRxMsgStruct.Data[4]|(CanRxMsgStruct.Data[5]<<8);
                    if(ManualCtrl.Pos == MANUAL_POS_Other || ManualCtrl.Dir == MANUAL_DIR_Other)
                    {
                        if(OP.WorkMode != ManualAppoint)
                        {
                            if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                            OP.WorkMode = ManualAppoint;
                            ManualCtrl.StartFlag = 1;
                            ManualCtrl.RemoteCtrlFlag = 1;
                            ManualCtrl.EnforceFlag = 0;
                            ManualCtrl.OrderFlag = 0;
                            ManualCtrl.ExitFlag = 0;
                            //ManualCtrl.Time = 0;
                        }
                        #if DEBUG
                        printf("ManualCtrlT_resident \r\n");
                        #endif
                    }
                    else 
                    {
                        if(OP.WorkMode != ManualAppoint)
                        {
                            if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                            OP.WorkMode = ManualAppoint;
                            ManualCtrl.StartFlag = 1;
                        }
                        
                        ManualCtrl.RemoteCtrlFlag = 1;
                        ManualCtrl.EnforceFlag = 1;
                        ManualCtrl.OrderFlag = 1;
                        ManualCtrl.ExitFlag = 0;
                        //ManualCtrl.Time = 0;
                        ManualCtrl.ChannelOnsNext = GetAppointChannel(ManualCtrl.Pos, ManualCtrl.Dir);
                        
                        #if DEBUG
                        printf("ManualCtrlT_Appoint\r\n");
                        printf("ChannelOnsNext = %04x\r\n", ManualCtrl.ChannelOnsNext);
                        printf("Pos = %02x\r\n", ManualCtrl.Pos);
                        printf("Dir = %02x\r\n", ManualCtrl.Dir);
                        #endif
                    }
                }
                else
                {
                    ManualCtrl.RemoteCtrlFlag = 1;
                    ManualCtrl.AutoTime = CanRxMsgStruct.Data[4]|(CanRxMsgStruct.Data[5]<<8);
                    ManualCtrl.Time = ManualCtrl.AutoTime;
                    if(OP.WorkMode < Flashing) OP.WorkModeBK = OP.WorkMode;
                    if(Manual_yellowflash == CanRxMsgStruct.Data[1])
                    {
                        OP.WorkMode = ManualFlashing;
                    }
                    else if(Manual_allred == CanRxMsgStruct.Data[1])
                    {
                        OP.WorkMode = ManualAllRead;
                    }
                    else if(Manual_lampoff == CanRxMsgStruct.Data[1])
                    {
                        OP.WorkMode = ManualLampOff;
                    }
                    else if(Manual_nextphase == CanRxMsgStruct.Data[1])
                    {
                        OP.WorkMode = ManualStep;
                        ManualCtrl.NextStepFlag = 1;
                    }
                }
                OP.WorkMode_Reason = WMR_RemoteGuard;
            }
            else if(Manual_auto == CanRxMsgStruct.Data[0])
            {
                if(OP.WorkMode == ManualAppoint)
                {
                    ManualCtrl.ExitFlag = 1;
                }
                else if(OP.WorkMode >= SPECIAL_MODE)
                {
                    ManualCtrl.LocalCtrlFlag = 0;
                    ManualCtrl.RemoteCtrlFlag = 0;
                    OP.WorkMode = OP.WorkModeBK;
                    OP.WorkMode_Reason = WMR_Normal;
                }
                #if DEBUG
                printf("ManualCtrlT_auto\r\n");
                #endif
            }
        }
	}
}
