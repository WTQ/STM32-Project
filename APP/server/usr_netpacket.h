/**
 * 网络通信数据包定义
 *
 * @author 王特
 */
 
#ifndef USR_NETPACKET_H
#define USR_NETPACKET_H

// 字节对齐的方式
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

// 包头
typedef struct _NetPacketHead {
	uint16_t version;
	uint32_t ePType;
	uint16_t nLen;
} NetPacketHead;

// 包体
typedef struct _NetPacket {
	NetPacketHead netPacketHead;
	char *packetBody;
} NetPacket;

#pragma pack(pop)

#endif
