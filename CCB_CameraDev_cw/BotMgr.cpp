#include "BotMgr.h"
#include "PublicFunc.h"
#include "MiddleWareDataStruct.h"
#include <string.h>
#include <io.h> 
#include <time.h>
#include "ETUtil.h"
int GetLastSeparatorPosition( TCHAR *strBuffer, TCHAR *strSeparator )
{
	TCHAR strDirectory[MAX_PATH] = {0};
	strcpy(strDirectory,strBuffer);
	char *pdest;
	int  result = 0;

	do 
	{
		pdest = strstr( strDirectory, strSeparator );
		if(pdest == '\0')
		{
			break;
		}
		result += (int)(pdest - strDirectory + 1);
		strcpy(strDirectory,pdest + strlen(strSeparator));
	} while(1);

	return result;
}
BOOL EnableDebugPrivilege(BOOL fEnable)  
{  
	BOOL fOk = FALSE;    // Assume function fails  
	HANDLE hToken;  

	// Try to open this process's access token  
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,&hToken))  
	{    
		WriteDSPLog("OpenProcessToken success");
		TOKEN_PRIVILEGES tp;  
		tp.PrivilegeCount = 1;  
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);  
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;  
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);  
		DWORD ret=GetLastError();
		fOk = (ret == ERROR_SUCCESS); 
		if(fOk==TRUE)
		{
		   WriteDSPLog("up privilege success");
		}
		else
		{
		   char msg[64]={0};
		   sprintf(msg,"up privilege fail %d",ret);
		   WriteDSPLog(msg);
		   WriteDSPLog("up privilege fail");
		}
		CloseHandle(hToken);  
	} 
	else
	{
	   WriteDSPLog("OpenProcessToken fail");
	}
	return(fOk);  
}
CBotMgr::CBotMgr(void)
{
	vecReader.clear();
    EnableDebugPrivilege(FALSE);
}

CBotMgr::~CBotMgr(void)
{
	vecReader.clear();
}


BOOL CBotMgr::SCardGetList()
{
	DWORD                     dwReturn;
	LPGUID                    lpDevGuid;

    vecReader.clear();
	lpDevGuid = (LPGUID)&GUID_DEVINTERFACE_USB_DEVICE;
	dwReturn = GetUKList(lpDevGuid);
	lpDevGuid = (LPGUID)&GUID_DEVINTERFACE_CDROM_DEVICE;
	dwReturn = GetUKList(lpDevGuid);
	if(vecReader.size()>0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CBotMgr::SCardTransmit(HANDLE CardHandle,LPCTSTR strApdu, LPSTR strResp, UINT *nSW)
{

	DWORD            dwRet = NO_ERROR;
	char             SendData[MAX_APDU_LEN],ReceiveData[MAX_RESPONSE];
	ULONG            nCmdLen,nResp;

	ZeroMemory(ReceiveData,sizeof(ReceiveData));
	ZeroMemory(SendData,sizeof(SendData));


	*nSW             = 0x0;
	strResp[0]        = 0;

	//对Command进行组织数据
	//目前的发送格式为T('C')L(两字节,Lh,Ll)V(APDU)
	SendData[0] = 0x0A;
	nCmdLen = strlen(strApdu) / 2;

	//判断命令是否太长
	if(nCmdLen > sizeof(SendData))
	{
		nCmdLen = sizeof(SendData);
	}

	SendData[1] = nCmdLen >> 8;
	SendData[2] = nCmdLen & 0xFF;

	HexToAsc((unsigned char *)(SendData+3),(char *)strApdu,nCmdLen*2);
    CMemBuf SendHex(2048);
	AscToHex((char *)SendHex.GetCharPtr(),(unsigned char *)SendData,nCmdLen + 3);
	//WriteLog("【SendHex】=%s",SendHex.GetCharPtr());
	//
#if 1	
	dwRet = WriteData_UDisk(nCmdLen + 3,(unsigned char*)SendData,CardHandle);
	if(dwRet)
	{
		WriteDSPLog("send WriteData_UDisk");
		#if 1

		dwRet = WriteData_UDisk(nCmdLen + 3,(unsigned char*)SendData,CardHandle);
		if(dwRet)
		{
			WriteDSPLog("resend WriteData_UDisk");
			dwRet = WriteData_UDisk(nCmdLen + 3,(unsigned char*)SendData,CardHandle);
			if(dwRet)
			{
				return FALSE;
			}
			//return FALSE;
		}
#endif
		 return FALSE;
	}	
	dwRet = ReadData_UDisk((unsigned char*)ReceiveData,CardHandle,&nResp);
	if(dwRet)
	{
		WriteDSPLog("read ReadData_UDisk");
#if 1
		
		dwRet = ReadData_UDisk((unsigned char*)ReceiveData,CardHandle,&nResp);
		if(dwRet)
		{
			WriteDSPLog("reread ReadData_UDisk");
			dwRet = ReadData_UDisk((unsigned char*)ReceiveData,CardHandle,&nResp);
			if(dwRet)
			{
				return FALSE;
			}
			//return FALSE;
		}
#endif
		return FALSE;
	}
#endif
#if 0
	unsigned long retLen=nCmdLen + 3;

	DWORD ret=CommunicateWithDevice(CardHandle,WRITEDEVICE,(unsigned char*)SendData,&retLen);
	if(ret==0)
	{
		retLen=MAX_RESPONSE;
		ret=CommunicateWithDevice(CardHandle,READDEVICE,(unsigned char*)ReceiveData,&retLen);
		if(ret!=0)
		{
			WriteDSPLog("CommunicateWithDevice READDEVICE fail 0x%x",ret);
			return FALSE;
		}
	}
	else
	{
		WriteDSPLog("CommunicateWithDevice WRITEDEVICE fail 0x%x",ret);
		return FALSE;
	}
	nResp=retLen;
#endif
	CMemBuf RecvHex(2048);	
	AscToHex((char *)RecvHex.GetCharPtr(),(unsigned char *)ReceiveData,nResp);
	//WriteLog("【RecvHex】=%s",RecvHex.GetCharPtr());

	if(strcmp(strApdu,"8000000000") == 0)
	{
		//复位命令,复位应答没有SW,所有数据均是ATR
		*nSW = 0x9000;
		AscToHex((char *)strResp, (unsigned char *)(ReceiveData + 3), nResp - 3);
	}
	else
	{
		if ( nResp >= 5 ) 
		{
			*nSW = ((unsigned char)(ReceiveData[nResp - 2]) * 256) 
				+ (unsigned char)ReceiveData[nResp - 1];
			AscToHex((char *)strResp, (unsigned char *)(ReceiveData + 3), nResp - 5);
		}
	}
	return TRUE;
}

BOOL CBotMgr::SCardReset(HANDLE CardHandle,LPSTR strResp)
{
	INT nSW;
	if( 0 == (SendAPDU(CardHandle,"8000000000",strResp, &nSW)))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CBotMgr::SCardClose(HANDLE handle)
{
	BOOL bRet;
	BotReader *pBotRdr = (BotReader *)handle;
	bRet = CloseHandle(pBotRdr->hToken);
	pBotRdr->hToken = NULL;
	return bRet;
}

HANDLE CBotMgr::SCardOpen(char *pDeviceStr)
{
	BotReader stuReader;
	HANDLE	CardHandle, hToken; 
	WriteDSPLog("generic ");
	hToken = CreateFile( pDeviceStr, 
		                      GENERIC_READ|GENERIC_WRITE,
		                      FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
		                      CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(hToken == INVALID_HANDLE_VALUE)
	{
		char TipMsg[128]={0};
		sprintf(TipMsg,"CBotMgr::SCardOpen fail lasterror=%d",GetLastError());
		MessageBox(NULL,TipMsg,"Tip",MB_OK);
		return NULL;
	}

	CardHandle = (HANDLE)SetTokenHandle(pDeviceStr,hToken);

	return CardHandle;
}

BotReader *CBotMgr::SetTokenHandle(char *pDeviceStr, HANDLE hToken)
{
	for(int i=0;i<vecReader.size();i++)
	{
		if(strcmp(pDeviceStr,vecReader[i].szDevpath) == 0)
		{
			vecReader[i].hToken = hToken;
			return &vecReader[i];
		}
	}
	return NULL;
}


BOOL CBotMgr::IsWin98(VOID)
{
	OSVERSIONINFO   osV;   
	BOOL b=FALSE;

	osV.dwOSVersionInfoSize   =   sizeof(OSVERSIONINFO);   
	GetVersionEx   (&osV);   
	if ( osV.dwMajorVersion == 4 && osV.dwMinorVersion > 0 )
	{
		b=TRUE;
	}
	return b;
}

BOOL CBotMgr::CheckETString(char *pdevpath)
{
	char        *pstr, *pbustring, *pbt;
	char        chdp[MAX_PATH]={0};

	// make a copy of device path.
	strcpy(chdp, pdevpath);

	// make lowercase
	pstr = chdp;
	while ( *pstr !=0)
	{
		_tolower(*pstr ++);
	}

	pstr = strstr(chdp, "cis");

	if(!pstr)
	{
		return FALSE;
	}

	pbustring = strchr(pstr, '#');
	if(!pbustring)
	{
		return FALSE;
	}

	pbt = strchr(pbustring, '&');
	if(!pbt)
	{
		return FALSE;
	}

	return TRUE;
}

DWORD   CBotMgr::GetUKList(LPGUID &lpDevGuid)
{
	BOOL                      bNoMoreItems;
	int                       iMemberindex;
	DWORD                     dwReturn,dwDevIntfsDetaDataSize;
	SP_DEVICE_INTERFACE_DATA  SpDevIntfsData;
	DWORD                     dwRequiredSize;
	HANDLE                    hHandle = NULL;
	HDEVINFO                  hDevInfoSet = NULL;
	DWORD                     dwRet = 0;
	char                      *pcVendor, *pcProductType, *pcProcuctVer;
	pcVendor = pcProductType = pcProcuctVer = NULL;

	hDevInfoSet = SetupDiGetClassDevs(lpDevGuid,
		                              NULL,
		                              NULL,
		                              DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	
	if(hDevInfoSet == INVALID_SCHANDLE_VALUE)
	{
		return GetLastError();
	}
	
	//Find valid UDisk Device
	for(iMemberindex = 0,bNoMoreItems = FALSE,dwReturn = ERROR_SUCCESS;
	    !bNoMoreItems && dwReturn == ERROR_SUCCESS;iMemberindex++)
	{
		//Enum Device Interface
		SpDevIntfsData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		if(!SetupDiEnumDeviceInterfaces(hDevInfoSet,
			                            NULL,
			                            lpDevGuid,
			                            iMemberindex,
			                            &SpDevIntfsData))
		{
			dwReturn = GetLastError();
			if(dwReturn == ERROR_NO_MORE_ITEMS) 
			{
				bNoMoreItems = TRUE;
			}
			continue;
		}
		
		//Get the interface information of device
		
		if(!SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
			                                &SpDevIntfsData,
			                                NULL,
			                                0,
			                                &dwRequiredSize,
			                                NULL))
		{
			if(ERROR_INSUFFICIENT_BUFFER != GetLastError())
			{
				dwReturn = GetLastError();
				continue;
			}
		}
		
		//Malloc Required Size Buffer
		dwDevIntfsDetaDataSize = dwRequiredSize;
		
		pSpDevIntfsDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new BYTE[dwDevIntfsDetaDataSize];
		
		if(!pSpDevIntfsDetData)
		{
			dwReturn = GetLastError();
			continue;
		}
		
		//Get Device Interface detail
		pSpDevIntfsDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		dwRequiredSize = 0;
		if( !SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
			                                 &SpDevIntfsData,
											 pSpDevIntfsDetData,
			                                 dwDevIntfsDetaDataSize,
											 &dwRequiredSize,
											 NULL))
		{
			dwReturn = GetLastError();
			
			if(pSpDevIntfsDetData)
			{
				delete[] pSpDevIntfsDetData;
				pSpDevIntfsDetData = NULL;
			}
			continue;
		}
		
		char szDevicePath[MAX_PATH] = {0};
		if(CheckETString(pSpDevIntfsDetData->DevicePath) == 1)
		{
			BotReader pTmpReader;
			int n = vecReader.size();
		    strcpy(pTmpReader.szDevpath,pSpDevIntfsDetData->DevicePath);
			sprintf_s(pTmpReader.szReaderName,"cis Ultra TrustKey %d",n);
			pTmpReader.hToken = NULL;
			vecReader.push_back(pTmpReader);
		}
		delete[] pSpDevIntfsDetData;
		pSpDevIntfsDetData=NULL;
	}
	
	//Destory Device Infomation Set(List)
	if( !SetupDiDestroyDeviceInfoList(hDevInfoSet))
	{
		vecReader.clear();
		return GetLastError();
	}
	return NO_ERROR;
}
DWORD CBotMgr::WriteData_UDisk(DWORD dwNum,unsigned char *chBuf,HANDLE hHandle)
{
	CBW                 cbw;
	unsigned long       ret_len;

	// 初始化 cbw
	memset((unsigned char *)&cbw, 0, sizeof(CBW));

	cbw.dwCBWXferLength     = dwNum;
	cbw.bCBWFlags           = 0;
	cbw.cdb[0]              = 0xF0;
	cbw.cdb[1]              = 0x45;
	cbw.cdb[2]              = 0x54;
	cbw.cdb[3]              = 0xA5;

	return SendCmd_UDisk(&cbw, chBuf, &ret_len,hHandle);	
	//return SendCmd_UDiskEx(&cbw, chBuf, &ret_len,hHandle);	
}
///////////////////////////////////////////////////////////////////////////////
//                                                           ReadData_UDisk()       
// 从设备读取数据
//
// chBuf 读到的内容, 返回读到的数据长度
// 
DWORD CBotMgr::ReadData_UDisk(unsigned char *chBuf,HANDLE hHandle,unsigned long *ret_len)
{
	CBW             cbw;
	memset((unsigned char *)&cbw, 0, sizeof(CBW));

	cbw.dwCBWXferLength     = 512;
	cbw.bCBWFlags           = 1;
	cbw.cdb[0]              = 0xF0;
	cbw.cdb[1]              = 0x45;
	cbw.cdb[2]              = 0x54;
	cbw.cdb[3]              = 0x5A;

	return SendCmd_UDisk(&cbw, chBuf, ret_len,hHandle);
	//return SendCmd_UDiskEx(&cbw, chBuf, ret_len,hHandle);
}
DWORD CBotMgr::CommunicateWithDevice(HANDLE hDevice,
									 BOOL ReadWriteFlag,
									 unsigned char *pData,
									 unsigned long *pRetLen)
{
	DWORD lastError=0;
	uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;

	ZeroMemory(&sptdwb, sizeof(uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

	sptdwb.SPTD.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	sptdwb.SPTD.PathId = 0;
	sptdwb.SPTD.TargetId = 1;
	sptdwb.SPTD.Lun = 0;
	sptdwb.SPTD.CdbLength = 8;
	sptdwb.SPTD.SenseInfoLength = 24;
	
	sptdwb.SPTD.DataTransferLength = *pRetLen;
	sptdwb.SPTD.TimeOutValue = 40;
	sptdwb.SPTD.DataBuffer = pData;
	sptdwb.SPTD.SenseInfoOffset = offsetof(uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseInfo);

	if(ReadWriteFlag==READDEVICE)
	{
		sptdwb.SPTD.DataIn = SCSI_IOCTL_DATA_IN;
		WriteDSPLog("CommunicateWithDevice READDEVICE *pRetLen=%d",*pRetLen);
		sptdwb.SPTD.Cdb[0] = 0xF0;
		sptdwb.SPTD.Cdb[1] = 0x45;
		sptdwb.SPTD.Cdb[2] = 0x54;
		sptdwb.SPTD.Cdb[3] = 0x5A;
	}
	else if(ReadWriteFlag==WRITEDEVICE)
	{
		sptdwb.SPTD.DataIn = SCSI_IOCTL_DATA_OUT;
		sptdwb.SPTD.Cdb[0] = 0xF0;
		sptdwb.SPTD.Cdb[1] = 0x45;
		sptdwb.SPTD.Cdb[2] = 0x54;
		sptdwb.SPTD.Cdb[3] = 0xA5;	
	}

	if(!DeviceIoControl(hDevice,
		                IOCTL_SCSI_PASS_THROUGH_DIRECT,
		                &sptdwb,
		                sizeof(uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
		                &sptdwb,
		                sizeof(uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
		                pRetLen,
		                NULL ))
	{
       lastError = GetLastError();
	   WriteDSPLog("[Error] CommunicateWithDevice 0x%x",lastError);
	   CMemBuf Buffer;
	   sprintf(Buffer.GetCharPtr(),"ThreadId:0x%x CommunicateWithDevice: DeviceIoControl() error:0x%x",GetCurrentThreadId(),lastError);
	   MessageBox(NULL,Buffer.GetCharPtr(),"Tip",MB_OK);
	}
	else
	{
		if(ReadWriteFlag==READDEVICE)
		{
           *pRetLen=sptdwb.SPTD.DataTransferLength;
		   if(sptdwb.SPTD.DataBuffer)
		   {
			   memcpy(pData,sptdwb.SPTD.DataBuffer,*pRetLen);
		   }
		}
	}

	return lastError; //if success,nonzero
}



DWORD CBotMgr::WaitUKMutex(LPSTR pMutexName,DWORD dwTime,HANDLE* phMutex)
{
	DWORD dwRet,dwSysVer;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	dwSysVer=GetVersion();
	if (!(dwSysVer & 0x80000000))	// win2K,XP,2003
	{
		InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);		
		sa.lpSecurityDescriptor = &sd;		
	}

	if (strlen(pMutexName) < 4)
	{
		return -7;
	}

	char strMutexName[MAX_PATH] = {0};
	sprintf(strMutexName,"ETBotDev_%s",pMutexName + 4);
	*phMutex=CreateMutex(&sa,FALSE,strMutexName);
	if (*phMutex==NULL)
		return GetLastError();

	dwRet=WaitForSingleObject(*phMutex,dwTime);
	if(dwRet==WAIT_FAILED)
	{
		CloseHandle(*phMutex);
		*phMutex=NULL;
		return GetLastError();
	}
	else if(dwRet==WAIT_TIMEOUT)  
	{
		return 0x80000010; // RT_MUTEX_ERROR
	}
	else
		return NO_ERROR;
}
void  CBotMgr::ReleaseUKMutex(HANDLE hMutex)
{
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
}


DWORD CBotMgr::SendCmd_UDisk(CBW *pcbw,
							 unsigned char *p_data,
							 unsigned long *retdata_len,
							 HANDLE hHandle)
{
	int  i; 
	CMemBuf Buffer;
	DWORD  dwReturn = NO_ERROR,dwBytesIoReturn;
	uSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER uSptdWb;

	ZeroMemory(&uSptdWb, sizeof(SPTDWB));
	uSptdWb.SPTD.Length= sizeof(SCSI_PASS_THROUGH_DIRECT);
	uSptdWb.SPTD.TargetId= 1;
	uSptdWb.SPTD.CdbLength= SCSI_CDB_LEN;
	uSptdWb.SPTD.SenseInfoLength= 16;
	if(pcbw->bCBWFlags)
	{
		uSptdWb.SPTD.DataIn= SCSI_IOCTL_DATA_IN;
	}
	else
	{
		uSptdWb.SPTD.DataIn= SCSI_IOCTL_DATA_OUT;
	}
	uSptdWb.SPTD.DataTransferLength= pcbw->dwCBWXferLength;
	uSptdWb.SPTD.TimeOutValue= 40;
	uSptdWb.SPTD.DataBuffer=p_data;
	uSptdWb.SPTD.SenseInfoOffset= offsetof(SPTDWB, ucSenseInfo);
	for(i= 0; i< 16; i++)
	{
		uSptdWb.SPTD.Cdb[i]= pcbw->cdb[i];
	}

	// IO Control
	clock_t start, finish;
	start = clock();
	if(!DeviceIoControl(hHandle, 
		                IOCTL_SCSI_PASS_THROUGH_DIRECT, 
		                &uSptdWb, 
						sizeof(SPTDWB),
		                &uSptdWb, 
						sizeof(SPTDWB),
		                &dwBytesIoReturn, 
						(LPOVERLAPPED)NULL))
	{
		dwReturn = GetLastError();
		sprintf(Buffer.GetCharPtr(),"ThreadId:0x%x SendCmd_UDisk: DeviceIoControl() error:0x%x",GetCurrentThreadId(),dwReturn);
		//MessageBox(NULL,Buffer.GetCharPtr(),"Tip",MB_OK);
		WriteDSPLog("SendCmd_UDisk uuuu DeviceIoControl fail hHandle =0x%x LastError=0x%x",hHandle,dwReturn);
		
		return dwReturn;
	}
	finish = clock();
	//WriteLog("cpu clock =%d",finish - start);

	if(pcbw->bCBWFlags)
	{
		*retdata_len=uSptdWb.SPTD.DataTransferLength;
		if(uSptdWb.SPTD.DataBuffer)
		{
			memcpy(p_data,uSptdWb.SPTD.DataBuffer,*retdata_len);
		}
	}
	else
	{
		*retdata_len=0;
	}

	return dwReturn;
}
DWORD CBotMgr::SendCmd_UDiskEx(CBW *pcbw,
							   unsigned char *p_data,
							   unsigned long *retdata_len,
							   HANDLE hHandle)
{
	int  i; 
	CMemBuf Buffer;
	DWORD  dwReturn = NO_ERROR,dwBytesIoReturn;
	uSCSI_PASS_THROUGH_WITH_BUFFER uSptWb;

	ZeroMemory(&uSptWb, sizeof(SPTWB));
	uSptWb.SPTD.Length= sizeof(SCSI_PASS_THROUGH);
	uSptWb.SPTD.TargetId= 1;
	uSptWb.SPTD.CdbLength= SCSI_CDB_LEN;
	uSptWb.SPTD.SenseInfoLength= 16;
	if(pcbw->bCBWFlags)
	{
		uSptWb.SPTD.DataIn= SCSI_IOCTL_DATA_IN;
	}
	else
	{
		uSptWb.SPTD.DataIn= SCSI_IOCTL_DATA_OUT;
	}
	uSptWb.SPTD.DataTransferLength= pcbw->dwCBWXferLength;
	uSptWb.SPTD.TimeOutValue= 40;
	uSptWb.SPTD.DataBufferOffset = offsetof(uSCSI_PASS_THROUGH_WITH_BUFFER, DataBuf);
	uSptWb.SPTD.SenseInfoOffset = offsetof(uSCSI_PASS_THROUGH_WITH_BUFFER, SenseBuf);
	for(i= 0; i< 16; i++)
	{
		uSptWb.SPTD.Cdb[i]= pcbw->cdb[i];
	}
	memcpy(uSptWb.DataBuf,p_data,pcbw->dwCBWXferLength);
	DWORD length = offsetof(uSCSI_PASS_THROUGH_WITH_BUFFER, DataBuf) + uSptWb.SPTD.DataTransferLength;
	// IO Control
	clock_t start, finish;
	start = clock();
	if(!DeviceIoControl(hHandle, 
		                IOCTL_SCSI_PASS_THROUGH, 
		                &uSptWb, 
		                sizeof(uSCSI_PASS_THROUGH_WITH_BUFFER),
		                &uSptWb, 
		                sizeof(uSCSI_PASS_THROUGH_WITH_BUFFER),
		                &dwBytesIoReturn, 
		                (LPOVERLAPPED)NULL))
	{
		dwReturn = GetLastError();
		WriteDSPLog("SendCmd_UDiskEx DeviceIoControl fail hHandle =0x%x LastError=0x%x",hHandle,dwReturn);
		sprintf(Buffer.GetCharPtr(),"ThreadId:0x%x SendCmd_UDiskEx: DeviceIoControl() error:0x%x",GetCurrentThreadId(),dwReturn);
		MessageBox(NULL,Buffer.GetCharPtr(),"Tip",MB_OK);
		return dwReturn;
	}
	finish = clock();
	//WriteLog("Ex cpu clock =%d",finish - start);
	if(pcbw->bCBWFlags)
	{
		*retdata_len=uSptWb.SPTD.DataTransferLength;
		if(uSptWb.SPTD.DataBufferOffset)
		{
			memcpy(p_data,uSptWb.DataBuf,*retdata_len);
		}
	}
	else
	{
		*retdata_len=0;
	}

	return dwReturn;
}
int CBotMgr::EnumDevices(DEVICE *pDevice)
{
	WriteDSPLog("Enum BotDevice");
	SCardGetList();
    for(int i=0;i<vecReader.size();i++)
	{
		strcpy(pDevice[i].szDeviceAddress,vecReader[i].szDevpath); 
		strcpy(pDevice[i].szDeviceName,vecReader[i].szReaderName);
		pDevice[i].bDeviceType = DEVICE_CONNECTED_TYPE;
	}
    return vecReader.size();
}
HANDLE CBotMgr::OpenDevice(char *pszDevicePath)
{
	WriteDSPLog("+++++++Open BotDevice %s",pszDevicePath);
	HANDLE handle=SCardOpen(pszDevicePath);
	WriteDSPLog("Open BotDevice handle= 0x%x",handle);
	return handle;
}
void CBotMgr::CloseDevice(HANDLE handle)
{
    SCardClose(handle);
	WriteDSPLog("-------Close BotDevice handle= 0x%x",handle);
}

int CBotMgr::SendAPDU(HANDLE handle,char *szCommand, char *szReply,  int *pulSW)
{
	WriteDSPLog("SendAPDU(handle=0x%x,szCommand=%s)",handle,szCommand);

	HANDLE hMutex;
	DWORD  dwRet = NO_ERROR;

	if(NULL == handle || INVALID_HANDLE_VALUE == handle)
		return ERROR_INVALID_HANDLE;
	if(NULL == szCommand || NULL == szReply || NULL == pulSW)
		return ERROR_INVALID_PARAMETER;

	BotReader *pBotRdr = (BotReader *)handle;

	if(NULL == pBotRdr)
		return ERROR_INVALID_HANDLE;

	if(dwRet = WaitUKMutex(pBotRdr->szDevpath,MUTEX_WAIT_TIME,&hMutex))
	{
		return FALSE;
	}

	
	SCardTransmit(pBotRdr->hToken,szCommand,szReply, (UINT *)pulSW);
	
	
	
	ReleaseUKMutex(hMutex);

	WriteDSPLog("szReply=%s,*pulSW=%x",szReply,*pulSW);
	return 0;
}



#define LOG_FILE     "c:\\ET_Log\\Dsp.log"
#define LOG_DIR	     "c:\\ET_Log"
void WriteDSPLog(const char* pFormat, ...)
{
	int rv = _access(LOG_DIR,0);
	if(rv == 0)//目录存在
	{
		char *pMemory=new char[MaxMallocMem+128];
		if(pMemory==NULL)
		{
			CUtil::WriteLog(LOG_FILE,"DEBUG_LOG() malloc memory fail");
			return ;
		}
		va_list VaList;
		memset(pMemory, 0, MaxMallocMem+128);
		va_start(VaList, pFormat);
		vsnprintf(pMemory, MaxMallocMem+128-1, pFormat, VaList); 
		CUtil::WriteLog(LOG_FILE,pMemory);
		va_end(VaList);
		delete pMemory;
		pMemory =NULL;
	}
}
