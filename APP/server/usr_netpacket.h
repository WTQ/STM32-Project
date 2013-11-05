/**
 * ����ͨ�����ݰ�����
 *
 * @author ����
 */
 
#ifndef USR_NETPACKET_H
#define USR_NETPACKET_H

// �ֽڶ���ķ�ʽ
#pragma pack(push, 1)

typedef enum {
	PT_CMDEND = 0,
	PT_TEST,
	PT_TEXT,
	PT_START,
	PT_STOP,
	PT_WMRECORD,
	PT_WMDATA,
} PACKET_TYPE;

// ��ͷ
typedef struct _NetPacketHead {
	uint16_t version;
	uint32_t ePType;
	uint16_t nLen;
} NetPacketHead;

// ����
typedef struct _NetPacket {
	NetPacketHead netPacketHead;
	char *packetBody;
} NetPacket;

#pragma pack(pop)

#endif
