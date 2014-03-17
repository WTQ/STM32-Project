/**
 * USART����DSP��ARM����ˮӡ������ط�װ
 *
 * @author ����
 * @�޸�   �ö���
 */

#ifndef USR_DSP_H
#define USR_DSP_H

#include "main.h"

// ������󳤶�
#define MESSAGE_MAX_LEN	30

// ��������
#define M_CMD_UNDEFNIE	0x00
#define M_CMD_START		0xAA
#define M_CMD_STOP		0x55
#define M_CMD_WMDATA	0x03
// �ȴ���������
#define M_CMD_START_HANDLE	0xbb

// DSP״̬
#define DSP_UNDEFINE	0x00
#define DSP_RUNNING		0x01
#define DSP_STOPPING	0x02
#define DSP_START_HANDS	0x03


// ˮӡ�ж����鳤�ȣ�֡���� ����ʱ��ֱ��ʣ�������2�ı�����ʱ��ֱ��� WM_GROUP_NUM/25 �룩
#define WM_GROUP_NUM	(2*7)

// ˮӡë������������ ��֤ˮӡ������
#define FRAME_FLAG		(5*WM_GROUP_NUM)

// ˮӡԤ�о��ķ�ֵ	����Ԥ�о������ȣ�Խ�ߣ����龯��Խ�ߣ�©����Խ�ͣ�����ֵ70��
#define WM_AND_MIN		70

// ˮӡ�����о�����ֵ �����о������ȣ�Խ�ߣ����龯��Խ�ͣ�©����Խ�ߣ�����ֵ65��
#define WM_FINAL_MIN	65

// ˮӡƥ���ʱ��������λ��ms
// #define DSP_TMR_INTERVAL	100

// ����֡���ĳ���
#define DSP_FRAME_MUL		WM_GROUP_NUM

// ˮӡ�����״̬
typedef enum {
	TRANS_OK = 0,
	TRANS_ING,
	TRANS_ERR,
	TRANS_IDLE
} Trans_State;

// ˮӡ�����״̬
typedef enum {
	HANDLE_OK = 0,
	HANDLE_READY,
	HANDLE_ERR,
	HANDLE_IDLE
} Handle_State;

// ˮӡ������Ϣ�ṹ��
typedef struct {
	int32_t TransState;
	int32_t HandleState;
	int8_t	Command;
	int8_t	cDataLen;
	int8_t	rDataLen;
	uint8_t Data[MESSAGE_MAX_LEN];
	uint32_t sTime;
} Message_Data;

// ˮӡ���еĽṹ��
typedef struct 
{
	uint8_t		Data[MESSAGE_MAX_LEN];	// �յ�������
	uint8_t		WM_Data[MESSAGE_MAX_LEN];	// ��ƥ�������
	int			WM_ID;		//��֡������ͬ
	int			Right_Num;	//�������֡��ȷλ�����ܺͣ�����֡������ͬ
}WM_Array;

void DSP_Init(void);
void DSP_Start(void);
void DSP_Stop(void);
void DSP_ReseveMsg(void);

// �жϵ���
void Handle_Water(void);
// �������
void DSP_Water_Handle(__IO uint32_t localtime);
// �������
void Push_Data(WM_Array);
void PreHandle(void);
void ZhongArr(uint8_t *, uint8_t *);

// �ҳ��ַ�����1�ĸ���
int8_t Find_One(uint8_t n);
int8_t Find_Str(uint8_t *str, int8_t num);

extern Message_Data MessageData;
extern uint8_t DSPState;
#endif
