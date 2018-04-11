// TSPCcid.h: interface for the CTSPCcid class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_TSPCCID_H__E3977907_A087_4FC2_AA48_90F9F3200E25__INCLUDED_)
#define AFX_TSPCCID_H__E3977907_A087_4FC2_AA48_90F9F3200E25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "TSPDefine.h"
//#include "TSPBase.h"
#include <winscard.h>
#include "MiddleWareDataStruct.h"
//#include "Util.h"

#define INVALID_DEVICE           -1
#define INVALID_CMD              -2
#define CCID_ID					0x54
#define APDU_MAX_LEN			1024
#define MAX_RESPONSE			2048
typedef void* HDRIVER;

typedef struct _CCIDDRIVER
{
	UINT_PTR hHandle;//真正的设备句柄
	
}CCIDDRIVER,*PCCIDDRIVER;

class CCcid
{
public:
	CCcid();
	virtual ~CCcid();
public:
	bool Init();
	void DeInit();
	int EnumDevices(DEVICE *pDevice);
	LONG GetStatusChange(LPSCARD_READERSTATE rgReaderStates,int Count);
	HDRIVER OpenDevice(char *pszDevicePath);
	void CloseDevice(HDRIVER hDriver);	
	int SendAPDU(HDRIVER hDriver,char *szCommand, char *szReply,  int *pulSW);

private:
	SCARDCONTEXT m_hContextHandle;
	int SendData(HDRIVER hDriver,unsigned char *pData,unsigned long DataLen,unsigned char *pReturnData,int ReturnDataLen);

};

#endif // !defined(AFX_TSPCCID_H__E3977907_A087_4FC2_AA48_90F9F3200E25__INCLUDED_)
