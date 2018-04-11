#include "BotMgr.h"
#include "DSPProvider.h"
#include "MiddleWareDataStruct.h"
#include "PublicFunc.h"
#include "ServiceVist.h"
//#include "KenvinCrite.h"

class CProvide
{
public:
	CProvide(void);
};
CProvide::CProvide()
{

}



int DeviceClass=2;
CProvide Provide;
CBotMgr BotMgr;
CCcid Ccid;
CVisitSrv visitSrv;

//数据的读取只能同时一个人存在
//HANDLE m_MutexSolt;

void GetModulePath(char *Path,int Len)
{
	char Buf[1024]={0};
	HMODULE hModule=NULL;
	hModule=GetModuleHandle("etpkcs11.dll");
	if(hModule==NULL)
	{
		return;
	}
	GetModuleFileName(hModule, Buf, 1024);
	memset(Path,0,Len);
	ExtractFilePath(Buf,Path);
}
void SetDeviceType()
{
	char FileName[1024]={0};
	char Buf[1024]={0};
    GetModulePath(Buf,sizeof(Buf));
	if(strlen(Buf)==0)
	{
      return;
	}
	sprintf(FileName,"%sEntong.ini",Buf);
	memset(Buf,0,sizeof(Buf));
	::GetPrivateProfileString("DEVICE_TYPE","TYPE","BOT",Buf,sizeof(Buf),FileName);
	if(strcmp(Buf,"CCID")==0)
	{
      DeviceClass=0;
	  WriteDSPLog("DEVICE_TYPE=CCID");
	}
	else
	{
	   WriteDSPLog("DEVICE_TYPE=BOT");
       DeviceClass=2;
	}
	//WriteDSPLog("DEVICE_TYPE=visitservice");
	//DeviceClass=1;
}
LONG GetStatusChange(LPSCARD_READERSTATE rgReaderStates,int Count)
{
	return Ccid.GetStatusChange(rgReaderStates,Count);
}
int DSP_EnumDevices(DEVICE *pDevice)
{   
	SetDeviceType();
	if(DeviceClass==0)
	{
      return Ccid.EnumDevices(pDevice);
	}
	else if(DeviceClass==1)
	{
		return visitSrv.EnumDevices(pDevice);
	}
	else
	{
	  return BotMgr.EnumDevices(pDevice);
	}
}
HANDLE DSP_OpenDevice(char * pszDeviceStr)
{
/*	m_MutexSolt = CreateMutex(NULL, false ,"MutexDsp");  
	if (!m_MutexSolt)  
	{
		DWORD ret =  GetLastError();
		WriteDSPLog("创建互斥失败！pszDeviceStr = %s ret =%04x ",pszDeviceStr, ret);
		return NULL;
	}

	CKevinMutexLock  MutexLock(m_MutexSolt);
	if (!MutexLock.GetLockStatus())
	{
		DWORD ret =  GetLastError();
		WriteDSPLog("获取互斥体失败！pszDeviceStr = %s  ret =%04x ",pszDeviceStr, ret);
		return NULL;
	}
*/

	SetDeviceType();
	if(DeviceClass==0)
	{
    return Ccid.OpenDevice(pszDeviceStr);
	}
	else if(DeviceClass==1)
	{
		return visitSrv.OpenDevice(pszDeviceStr);
	}
	else
	{
	return BotMgr.OpenDevice(pszDeviceStr);
	}
}
void DSP_CloseDevice(HANDLE handle)
{
	if(DeviceClass==0)
	{
      Ccid.CloseDevice(handle);
	}
	else if(DeviceClass==1)
	{
	  visitSrv.CloseDevice(handle);
	}
	else
	{
		BotMgr.CloseDevice(handle);
	}
	//释放互斥
	//CloseHandle(m_MutexSolt);
}
int DSP_XSendAPDU(HANDLE handle,char *szCommand, char *szReply,  int *pulSW)
{
/*
	CKevinMutexLock  MutexLock(m_MutexSolt);
	if (!MutexLock.GetLockStatus())
	{
		WriteDSPLog("DSP_XSendAPDU::szCommand = %s 获取互斥体失败！",szCommand);
		return NULL;
	}
*/

	if(DeviceClass==0)
	{
     return Ccid.SendAPDU(handle,szCommand,szReply,pulSW);
	}
	else if(DeviceClass==1)
	{
		return visitSrv.SendAPDU(handle,szCommand,szReply,pulSW);
	}
	else
	{
	return BotMgr.SendAPDU(handle,szCommand,szReply,pulSW);
	}
}
