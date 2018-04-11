#include "ServiceVist.h"
#include <tchar.h>
#include <stdio.h>
CVisitSrv::CVisitSrv()
{
  m_InitalSyncVarFlag=FALSE;
}
CVisitSrv::~CVisitSrv()
{
	//ReleaseMutex(m_hMutex);
	CloseHandle(m_hMutex);
	CloseHandle(m_hSemaphoreout);
	CloseHandle(m_hSemaphorein);
	CloseHandle(m_hMapFile);
	if(m_pBuf!=NULL)
	{
		UnmapViewOfFile(m_pBuf);
		m_pBuf=NULL;
	}
}
void CVisitSrv::IntialSyncVar()
{
	if(m_InitalSyncVarFlag==FALSE)
	{
	DWORD ret;
	m_pBuf=NULL;
	m_hMutex=OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Global\\ctrl_mutex_entong"));
	if(m_hMutex==INVALID_HANDLE_VALUE)
	{
		ret=GetLastError();
		MessageBox(NULL,"OpenMutex fail","tip",MB_OK);
	}
	m_hSemaphoreout = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, _T("Global\\ctrl_sema_entong_out"));
	if(m_hSemaphoreout==INVALID_HANDLE_VALUE)
	{
		ret=GetLastError();
		MessageBox(NULL,"OpenSemaphore fail","tip",MB_OK);
	}
	m_hSemaphorein = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, _T("Global\\ctrl_sema_entong_in"));
	if(m_hSemaphorein==INVALID_HANDLE_VALUE)
	{
		ret=GetLastError();
		MessageBox(NULL,"OpenSemaphore fail","tip",MB_OK);
	}

	m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("Global\\ctrl_MemoryShare"));
	if(m_hMapFile==INVALID_HANDLE_VALUE)
	{
		ret=GetLastError();
		MessageBox(NULL,"OpenFileMapping fail","tip",MB_OK);
	}
	m_pBuf = (LPTSTR) MapViewOfFile(m_hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		sizeof(MemData));
	m_InitalSyncVarFlag=TRUE;
	}
}
void CVisitSrv::Service(MemData &memdata)
{
	DWORD ReadNum;
	DWORD WriteNum;
	char Buffer[1024]={0};
	IntialSyncVar();
	//WaitForSingleObject(m_hMutex,INFINITE);
	//ReleaseSemaphore(m_hSemaphorein,1,NULL);
	try
	{
		DWORD ret;
		if(WaitNamedPipe("\\\\.\\Pipe\\entong_in",INFINITE)==0)
		{
			MessageBox(NULL,"Connect pipe failed!","tip",MB_OK);
			return;
		}
		HANDLE  hPiple= CreateFile("\\\\.\\Pipe\\entong_in", GENERIC_READ | GENERIC_WRITE,0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  
		if (hPiple == INVALID_HANDLE_VALUE)  
		{  
			ret=GetLastError();
			if(ret!=ERROR_PIPE_BUSY)
			{
				sprintf(Buffer,"fail %d",ret);
				MessageBox(NULL,"Connect pipe failed!","tip",MB_OK);
				MessageBox(NULL,Buffer,"tip",MB_OK);
				return;
			}	
		} 
		//memset(m_pBuf,0,sizeof(MemData));
		//memcpy(m_pBuf,&memdata,sizeof(MemData));		
		WriteFile(hPiple,&memdata, sizeof(MemData), &WriteNum, NULL); 
		WaitForSingleObject(m_hSemaphoreout,1000);
		//memcpy(&memdata,m_pBuf,sizeof(MemData));
		ReadFile(hPiple, &memdata, sizeof(MemData), &ReadNum, NULL);
		CloseHandle(hPiple);
	}
	catch (...)
	{
	}

	//ReleaseMutex(m_hMutex);
}
int CVisitSrv::EnumDevices(DEVICE *pDevice)
{
	MemData memdata;
	memset(&memdata,0,sizeof(MemData));
	strncat(memdata.cmd,"EnumDevices",strlen("EnumDevices"));
	Service(memdata);
	if(strlen(memdata.Out)>0)
	{
	   strcpy(pDevice[0].szDeviceAddress,memdata.Out); 
	   strcpy(pDevice[0].szDeviceName,memdata.Out);
	   pDevice[0].bDeviceType = DEVICE_CONNECTED_TYPE;
	   return 1;
	}
	else
	{
		return 0;
	}
}
HANDLE CVisitSrv::OpenDevice(char *pszDevicePath)
{
	MemData memdata;
	memset(&memdata,0,sizeof(MemData));
	strncat(memdata.cmd,"OpenDevice",strlen("OpenDevice"));
	strncat(memdata.In,pszDevicePath,strlen(pszDevicePath));
	Service(memdata);
	return memdata.handle;
}
void CVisitSrv::CloseDevice(HANDLE handle)
{
	MemData memdata;
	memset(&memdata,0,sizeof(MemData));
	memdata.handle=handle;
	strncat(memdata.cmd,"CloseDevice",strlen("CloseDevice"));
	Service(memdata);
}
int CVisitSrv::SendAPDU(HANDLE handle,char *szCommand, char *szReply,int *pulSW)
{
	MemData memdata;
	memset(&memdata,0,sizeof(MemData));
	memdata.handle=handle;
	strncat(memdata.In,szCommand,strlen(szCommand));
	strncat(memdata.cmd,"SendAPDU",strlen("SendAPDU"));
	Service(memdata);
	memcpy(szReply,memdata.Out,strlen(memdata.Out));
	*pulSW=memdata.Extern;
	return 0;
}