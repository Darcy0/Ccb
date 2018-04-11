#ifndef SERVICEVIST_H
#define SERVICEVIST_H
#pragma once
#include<windows.h>
#include "MiddleWareDataStruct.h"
typedef struct 
{
	char cmd[64];
	HANDLE handle;
	int InLength;
	char In[4096];
	int OutLength;
	char Out[4096];
	int Extern;
}MemData;
class CVisitSrv
{
public:
	CVisitSrv();
	~CVisitSrv();
	HANDLE m_hMutex; 
	HANDLE m_hSemaphoreout;
	HANDLE m_hSemaphorein;
	HANDLE m_hMapFile;
	BOOL m_InitalSyncVarFlag;
	void IntialSyncVar();
	void Service(MemData &memdata);
	int EnumDevices(DEVICE *pDevice);
	HANDLE OpenDevice(char *pszDevicePath);
	void CloseDevice(HANDLE handle);
	int SendAPDU(HANDLE handle,char *szCommand, char *szReply,int *pulSW);
	char *m_pBuf;
};
#endif