/**
 * FLASH������ط�װ
 * 
 * @author ����
 */
 
#include "main.h"
#include "usr_flash.h"
#include <string.h> 

FLASH_Status FLASHStatus;
WM_Flag WMFlag;

/**
 * ���ֽڵ�Flashд�����
 */
bool Usr_Flash_Write(uint32_t Start_Address, uint8_t* Data, int32_t Data_Len)
{
	bool state = TRUE;
	uint16_t temp_F;
	uint32_t Address;
	Address = Start_Address;
	FLASH_Unlock();
//	Usr_Flash_Erase(Address);
//	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	FLASHStatus = FLASH_COMPLETE;
	while(Data_Len > 0)
	{
		temp_F = *((uint16_t*)Data);
    	FLASHStatus = FLASH_ProgramHalfWord(Address, temp_F);
		if(FLASHStatus != FLASH_COMPLETE) 
		{
			state = FALSE;
			break;
		}
    	Address = Address + 2;
		Data = Data + 2;
		Data_Len = Data_Len - 2;
	}
	FLASH_Lock();
	return state;
}

bool Usr_Flash_Erase(uint32_t Start_Address)
{
	bool state = TRUE;
	FLASHStatus = FLASH_ErasePage(Start_Address);
	if(FLASHStatus != FLASH_COMPLETE) {
		state = FALSE;
	}
	return state;
}

bool GetWMHeader(WM_Header * WMHeader)
{
	WMHeader = (WM_Header *) WM_DATA_ADDR_START;
	if(strcmp(IDSTRING_CONTENT, WMHeader->IDString) == 0) {
		return TRUE;
	}
	return FALSE;
}

void SetWMHeader(void)
{
	WM_Header WMHeader = {0};
	strcpy(WMHeader.IDString, IDSTRING_CONTENT);
	
	Usr_Flash_Write(WM_DATA_ADDR_START, (uint8_t *)&WMHeader.IDString, IDSTRING_MAX_LEN);
	Usr_Flash_Write(WM_DATA_ADDR_START + IDSTRING_MAX_LEN, (uint8_t *)&WMHeader.FirstID, WMDATA_ID_MAX_LEN);
}

bool GetWM(WM_Data *WMData, int32_t index)
{
	uint32_t Address;
	if(index > WMFlag.WM_Data_Num || index < 0) {
		return FALSE;
	}
	
	Address = WM_DATA_ADDR_START + WM_HEADER_LEN + WM_DATA_LEN * index;
	WMData->ID = *(uint32_t *) Address;
	WMData->WaterMark = (uint8_t *)(Address + WMDATA_ID_MAX_LEN);

	return TRUE;
}

bool WriteWM(WM_Data *WMData)
{
	if(WMFlag.WM_Data_Address + WM_DATA_LEN > WM_DATA_ADDR_END) {
		return FALSE;
		// EraseWM();
	}

	Usr_Flash_Write(WMFlag.WM_Data_Address, (uint8_t *)&WMData->ID, WMDATA_ID_MAX_LEN);
	Usr_Flash_Write(WMFlag.WM_Data_Address + WMDATA_ID_MAX_LEN, (uint8_t *)WMData->WaterMark, WM_MAX_LEN);
	
	// ����WMFlag��־
	WMFlag.WM_Data_Address += WM_DATA_LEN;
	++WMFlag.WM_Data_Num;
	return TRUE;
}

bool EraseWM() 
{
	uint32_t Address = WM_DATA_ADDR_START;
	
	FLASH_Unlock();
	while (Address < WM_DATA_ADDR_END)
	{
		if(FALSE == Usr_Flash_Erase(Address)) {
			FLASH_Lock();	
			return FALSE;
		}
		Address += FLASH_PAGE_SIZE;
	}
	FLASH_Lock();
	WMFlag.WM_Data_Address = WM_DATA_ADDR_START + WM_HEADER_LEN;
	WMFlag.WM_Data_Num = 0;
	
	// ��������Header
	SetWMHeader();
	return TRUE;
}

bool GetRecordHeader(WM_Header * WMHeader)
{
	WMHeader = (WM_Header *) WM_RECORD_ADDR_START;
	if(strcmp(IDSTRING_CONTENT, WMHeader->IDString) == 0) {
		return TRUE;
	}
	return FALSE;
}

void SetRecordHeader(void)
{
	WM_Header WMHeader = {0};
	strcpy(WMHeader.IDString, IDSTRING_CONTENT);
	WMHeader.FirstID = GetRecordLastID();
	
	Usr_Flash_Write(WM_RECORD_ADDR_START, (uint8_t *)&WMHeader.IDString, IDSTRING_MAX_LEN);
	Usr_Flash_Write(WM_RECORD_ADDR_START + IDSTRING_MAX_LEN, (uint8_t *)&WMHeader.FirstID, RECORD_ID_MAX_LEN);
}

bool GetRecord(WM_Record *WMRecord, int32_t index)
{
	uint32_t Address;
	if(index > WMFlag.WM_Record_Num || index < 0) {
		return FALSE;
	}
	
	Address = WM_RECORD_ADDR_START + WM_HEADER_LEN + WM_RECORD_LEN * index;
	WMRecord->ID = *(uint32_t *) Address;													 // ���õ�ַȡ����
	WMRecord->WMData_ID = *(uint16_t *) (Address + RECORD_ID_MAX_LEN);
	WMRecord->FrameNum = *(int16_t *)(Address + RECORD_ID_MAX_LEN + WMDATA_ID_MAX_LEN);

	return TRUE;
}

bool WriteRecord(WM_Record *WMRecord)
{
	uint32_t NextId = GetRecordLastID() + 1;
	
	if(WMFlag.WM_Record_Address + WM_RECORD_LEN > WM_RECORD_ADDR_END) {
		// FLASH����������
		return FALSE;
		// ��ǰ����ʽΪ����
		// EraseRecord();
	}
	
	// ����WmRecord��־
	WMRecord->ID = NextId;
	
	Usr_Flash_Write(WMFlag.WM_Record_Address, (uint8_t *)&WMRecord->ID, RECORD_ID_MAX_LEN);
	Usr_Flash_Write(WMFlag.WM_Record_Address + RECORD_ID_MAX_LEN, (uint8_t *)&WMRecord->WMData_ID, WMDATA_ID_MAX_LEN);
	Usr_Flash_Write(WMFlag.WM_Record_Address + RECORD_ID_MAX_LEN + WMDATA_ID_MAX_LEN, (uint8_t *)&WMRecord->FrameNum, FM_MAX_LEN);
	
	// ����WMFlag��־
	WMFlag.WM_Record_Address += WM_RECORD_LEN;
	++WMFlag.WM_Record_Num;
	++WMFlag.WM_Record_Last_ID;
	return TRUE;
}

bool EraseRecord() 
{
	uint32_t Address = WM_RECORD_ADDR_START;
	
	// ��������
	FLASH_Unlock();
	while (Address < WM_RECORD_ADDR_END)
	{
		if(FALSE == Usr_Flash_Erase(Address)) {
			FLASH_Lock();
			return FALSE;
		}
		Address += FLASH_PAGE_SIZE;
	}
	FLASH_Lock();
	
	// ��������
	WMFlag.WM_Record_Address = WM_RECORD_ADDR_START + WM_HEADER_LEN;
	WMFlag.WM_Record_Num = 0;
	
	// ���һ���µ�Header
	SetRecordHeader();
	
	return TRUE;
}

uint32_t GetRecordLastID(void)
{
	return WMFlag.WM_Record_Last_ID;
}

int32_t GetRecordIndexById(uint32_t RecordID)
{
	int i;
	WM_Record record;
	
	if(GetRecordLastID() < RecordID) {
		return -1;
	}
	
	for(i = 0; i < WMFlag.WM_Record_Num; ++i) {
		GetRecord(&record, i);
		if(record.ID >= RecordID) {
			return i;
		}
	}
	return -1;
}

void Usr_Flash_Init()
{
	uint32_t Address;
	uint16_t num;
	WM_Header WMHeader;
	WM_Record record;
	
	WMFlag.WM_Record_Last_ID = 0;
	
	// �ж�WM_Data���Ƿ����WM_Header
	if(!GetWMHeader(&WMHeader)) {
		EraseWM();
	}
	// ��ȡWM����
	Address = WM_DATA_ADDR_START + WM_HEADER_LEN;
	num = 0;
	while(*(uint32_t *)Address != 0xFFFFFFFF) {
		Address += WM_DATA_LEN;
		++num;
	}
	WMFlag.WM_Data_Address = Address;				   // ������Ҫд��ˮӡ���ݵĵ�ַ
	WMFlag.WM_Data_Num = num;						   // �Ѵ�ˮӡ���ݵĸ���
	
	// �ж�WM_Record���Ƿ����WM_Header
	if(!GetRecordHeader(&WMHeader)) {
		EraseRecord();
	}
	
	// ��ȡRecord����
	Address = WM_RECORD_ADDR_START + WM_HEADER_LEN;
	num = 0;
	while(*(uint32_t *)Address != 0xFFFFFFFF) {
		Address += WM_RECORD_LEN;
		++num;
	}
	WMFlag.WM_Record_Address = Address;				  // ������Ҫд��ƥ�������ݵĵ�ַ
	WMFlag.WM_Record_Num = num;						  // �Ѵ�ƥ�������ݵĸ���
	
	// ��ȡRecord_Last_ID
	if(num > 0) {
		GetRecord(&record, num - 1);
		WMFlag.WM_Record_Last_ID = record.ID;
	}
}
