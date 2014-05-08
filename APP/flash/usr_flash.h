/**
 * FLASH操作相关封装
 * 
 * @author 王特
 * 修改 矫东航
 */

#ifndef USR_FLASH_H
#define USR_FLASH_H

#include <stdint.h>
#include <stm32f10x.h>

// stm32F107VC的页大小为2KB
#define FLASH_PAGE_SIZE			((uint16_t)0x800)

// Header的IDString的长度
#define IDSTRING_MAX_LEN		10
// Header的IDString的内容
#define IDSTRING_CONTENT		"ID String"
// Header的FirstID的长度，暂未用
#define HEADER_FIRSTID_LEN		4
// 水印长度
#define WM_MAX_LEN				12
// 水印标号长度
#define WMDATA_ID_MAX_LEN		2
// 记录标号长度
#define RECORD_ID_MAX_LEN		4
// 帧数长度
#define FM_MAX_LEN				4
// WM_Header长度
#define WM_HEADER_LEN			(IDSTRING_MAX_LEN + 4)
// WM_Data长度
#define WM_DATA_LEN				(WMDATA_ID_MAX_LEN + WM_MAX_LEN)
// WM_Record长度
#define WM_RECORD_LEN			(RECORD_ID_MAX_LEN + WM_MAX_LEN + FM_MAX_LEN)

// 水印数据结构体地址, 10*2K
#define WM_DATA_ADDR_START		((uint32_t)0x08036000)
#define WM_DATA_ADDR_END		((uint32_t)0x0803AFFF)
#define WM_DATA_ADDR_LEN		(WM_DATA_ADDR_END - WM_DATA_ADDR_START + 1)

// 匹配结果数据结构体地址, 10*2K
#define WM_RECORD_ADDR_START	((uint32_t)0x0803B000)
#define WM_RECORD_ADDR_END		((uint32_t)0x0803FFFF)
#define WM_RECORD_ADDR_LEN		(WM_RECORD_ADDR_END - WM_RECORD_ADDR_START + 1)

typedef struct _WM_Header {
	char IDString[IDSTRING_MAX_LEN];
	uint32_t FirstID;
} WM_Header;

// 水印数据结构体
typedef struct _WM_Data {
	uint16_t ID;
	uint8_t* WaterMark;
} WM_Data;

// 匹配结果数据结构体
typedef struct _WM_Record {
	uint32_t ID;
	uint32_t FrameNum;
	uint8_t *WMData;
} WM_Record;

// 保存位置的结构体
typedef struct _WM_Flag {
	uint32_t WM_Data_Address;
	int32_t WM_Data_Num;
	uint32_t WM_Record_Address;
	int32_t WM_Record_Num;
	uint32_t WM_Record_Last_ID;
//	int32_t WM_BUG;
} WM_Flag;

// FLASH 部分的封装
bool Usr_Flash_Write(uint32_t Start_Address, uint8_t* Data, int32_t Data_Len);
bool Usr_Flash_Erase(uint32_t Start_Address);

// WM_Data相关的操作封装
bool GetWMHeader(WM_Header * WMHeader);
void SetWMHeader(void);
bool GetWM(WM_Data *WMData, int8_t index);
bool WriteWM(WM_Data *WMData);
bool EraseWM(void);

// WM_Record相关操作封转
bool GetRecordHeader(WM_Header * WMHeader);
void SetRecordHeader(void);
bool GetRecord(WM_Record *WMRecord, int8_t index);
bool WriteRecord(WM_Record *WMRecord);
bool EraseRecord(void);
uint32_t GetRecordLastID(void);
int32_t GetRecordIndexById(uint32_t RecordID);

// 初始化
void Usr_Flash_Init(void);

// 全局变量WMFlag
extern WM_Flag WMFlag;

#endif
