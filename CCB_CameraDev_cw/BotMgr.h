#ifndef BOT_MGR_H
#define BOT_MGR_H
#include <windows.h>
#include <DBT.H>			// User For DEV_BROADCAST_DEVICEINTERFACE
#include <setupapi.h>		// User	For SetupDixx()
#include "devioctl.h"		// User For DeviceIoCntl()
#include "ntddscsi.h"		// User For SCSI- 2
#include "MiddleWareDataStruct.h"
#include "Ccid.h"
#include <vector>
#include <iterator>
#include "ETUtil.h"
using namespace std;
#pragma comment (lib, "setupapi.lib")

#define			MAX_RESPONSE		1024
#define			MAX_APDU_LEN		1024

#define			INVALID_SCHANDLE_VALUE  NULL

// Const
const GUID GUID_DEVINTERFACE_USB_DEVICE= {0x53F56307L, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_USB_DEVICE_WIN98 = { 0x509FE614, 0x1292, 0x4BA0, { 0xAA, 0x51, 0xf5, 0xf2, 0xf1, 0x17, 0xdd, 0x92 } };
const GUID GUID_DEVINTERFACE_CDROM_DEVICE= {0x53F56308L, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const int SCSI_CDB_LEN= 16;
const int SCSI_DATA_LEN= 0x10000;

const int SCSI_PASS_THROUGH_SENSE_LEN= 32;
const int SCSI_PASS_THROUGH_DATABUFFER_LEN= 512;


//------------------------------------------------------------------------------
// command descriptor block
typedef unsigned char CDB[16];
//------------------------------------------------------------------------------
// command block wrapper
typedef struct _CBW
{
	unsigned int dwCBWSignature;   	// dwCBWSignature: the constant 0x55 0x53 0x42 0x43(LSB)
	unsigned int dwCBWtag;         		// dwCBWtag:
	unsigned int dwCBWXferLength;  	// dwCBWXferLength:number of bytes to transfer
	unsigned char bCBWFlags;          	// bmCBWFlags:
	//   Bit 7: direction - the device shall ignore this bit if the
	//   dCBWDataTransferLength field is zero, otherwise:
	//   0 = Data-Out from host to the device,
	//   1 = Data-In from the device to the host.
	//   Bit 6: obsolete. The host shall set this bit to zero.
	//   Bits 5..0: reserved - the host shall set these bits to zero.
	unsigned char bCBWlun;		       // bmCBWlun:
	unsigned char bCBWCBLength;       // bCBWLength: 0x01..0x10
	CDB cdb;               				 // CBWCB: the command descriptor block
} CBW;


// SCSI_PASS_THROUGH_DIRECT
typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER
{
	SCSI_PASS_THROUGH_DIRECT SPTD;

	ULONG ulFiller;				// realign buffer to double word boundary
	UCHAR ucSenseInfo[SCSI_PASS_THROUGH_SENSE_LEN];

}uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *uPSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;

typedef uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER	SPTDWB;		// 自定义类型 3
typedef uPSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER	PSPTDWB;	// 自定义类型 4

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFER
{
	SCSI_PASS_THROUGH SPTD;

	ULONG ulFiller;				// realign buffer to double word boundary
	UCHAR             SenseBuf[32];
	UCHAR             DataBuf[8192];

}uSCSI_PASS_THROUGH_WITH_BUFFER, *uPSCSI_PASS_THROUGH_WITH_BUFFER;

typedef uSCSI_PASS_THROUGH_WITH_BUFFER	SPTWB;		// 自定义类型 3
typedef uPSCSI_PASS_THROUGH_WITH_BUFFER	PSPTWB;	// 自定义类型 4

typedef HANDLE BOTHANDLE;
void WriteDSPLog(const char* pFormat, ...);

#define READDEVICE 0
#define WRITEDEVICE 1
class CBotMgr
{
public:
	CBotMgr(void);
	~CBotMgr(void);
    
public:
	vector<BotReader> vecReader;
	//void WriteLog(const char* pFormat, ...);
	DWORD CommunicateWithDevice(HANDLE hDevice,
		BOOL ReadWriteFlag,
		unsigned char *pData,
		unsigned long *pRetLen);
	BOOL SCardGetList();
	BOOL SCardTransmit(HANDLE CardHandle,LPCTSTR strApdu, LPSTR strResp, UINT *nSW);
	BOOL SCardReset(HANDLE CardHandle,LPSTR strResp);
	BOOL SCardClose(HANDLE CardHandle);
	HANDLE SCardOpen(char *pDeviceStr);

	int EnumDevices(DEVICE *pDevice);
	HANDLE OpenDevice(char *pszDevicePath);
	void CloseDevice(HANDLE hDriver);	
	int SendAPDU(HANDLE hHandle,char *szCommand, char *szReply,  int *pulSW);
	
private:
	SP_DEVICE_INTERFACE_DETAIL_DATA *pSpDevIntfsDetData;
	BotReader *SetTokenHandle(char *pDeviceStr, HANDLE hToken);
	DWORD WaitUKMutex(LPSTR pMutexName,DWORD dwTime,HANDLE* phMutex);
	void  ReleaseUKMutex(HANDLE hMutex);


protected:
	BOOL	IsWin98(VOID);	
	BOOL	CheckETString(char *pdevpath);
	DWORD	GetUKList(LPGUID &lpDevGuid);
	DWORD WriteData_UDisk(DWORD dwNum,unsigned char *chBuf,HANDLE hHandle);
	DWORD ReadData_UDisk(unsigned char *chBuf,HANDLE hHandle,unsigned long *ret_len);
	DWORD SendCmd_UDisk(CBW *pcbw,unsigned char *p_data,unsigned long *retdata_len,HANDLE hHandle);
	DWORD SendCmd_UDiskEx(CBW *pcbw,unsigned char *p_data,unsigned long *retdata_len,HANDLE hHandle);
};
#endif