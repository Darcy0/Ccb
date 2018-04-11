// TSPCcid.cpp: implementation of the CTSPCcid class.
//
//////////////////////////////////////////////////////////////////////

#include "Ccid.h"
#include "PublicFunc.h"
#include "MiddleWareDataStruct.h"
#include "BotMgr.h"
#pragma comment(lib,"winscard.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCcid::CCcid()
{
	m_hContextHandle = NULL;
	//Init();
}

CCcid::~CCcid()
{
	DeInit();
}

bool CCcid::Init()
{
	if(m_hContextHandle==NULL)
	{
		unsigned long ulRet = SCardEstablishContext(SCARD_SCOPE_USER, NULL,NULL,&m_hContextHandle);
		if (ulRet != SCARD_S_SUCCESS) 
		{
			return false;	
		}
	}
	return true;
}

void CCcid::DeInit()
{
	if(m_hContextHandle!=NULL)
	{
	   SCardReleaseContext(m_hContextHandle);
	}
}
LONG CCcid::GetStatusChange(LPSCARD_READERSTATE rgReaderStates,int Count)
{
	WriteDSPLog("GetStatusChange");
	Init();
	return SCardGetStatusChange(m_hContextHandle,300,rgReaderStates,Count);
}
int CCcid::EnumDevices(DEVICE *pDevice)
{
	WriteDSPLog("Enum CCIDDevice");
    Init();
	unsigned long		ret = NO_ERROR;
	char				*pResponseBuffer = NULL;
	unsigned long		ResponseLength = 0;
	char				szReaderName[MAX_PATH] = {0}; 
	DEVICE				*pTempReaderSet = pDevice;
	int					nCount=0;
	int					nTagLen = 0;
	char				*pTempBuf = NULL;

	if( m_hContextHandle)
	{
		ret = SCardListReaders(m_hContextHandle, 0,NULL,&ResponseLength);
		if (ret != SCARD_S_SUCCESS && ret != SCARD_E_SERVICE_STOPPED && ret != ERROR_INVALID_HANDLE )
		{
			return nCount;
		}
		else
		{
			if(ret == SCARD_E_SERVICE_STOPPED || ret == ERROR_INVALID_HANDLE ) 
			{
				ret = SCardListReaders(m_hContextHandle, 0,NULL,&ResponseLength);
				if( ret != SCARD_S_SUCCESS)
				{
					return nCount;
				}
			}
			
			pResponseBuffer = new char[ResponseLength];
			if(NULL == pResponseBuffer)
			{
				return ERROR_NOT_ENOUGH_MEMORY;
			}
			ret = SCardListReaders(m_hContextHandle, 0, pResponseBuffer, 
								   &ResponseLength);
			if( ret != SCARD_S_SUCCESS)
			{
				delete []pResponseBuffer;
				return nCount;
			}
			else
			{
				//从返回结果中解析
				pTempBuf = pResponseBuffer;
				while(*pTempBuf != '\0')
				{
					unsigned long dwActiveProtocol = 0;
					SCARDHANDLE hCard=NULL;
					DWORD dwState;
					TCHAR szReaderName[255] = {0};
					DWORD dwCardNameLen = 255;
					BYTE bATR[32] = {0};
					DWORD dwAtrLen = 32;
					ret = SCardConnect(m_hContextHandle,
								pTempBuf,
								SCARD_SHARE_SHARED,
								SCARD_PROTOCOL_T1,
								&hCard,
								&dwActiveProtocol);
					if(ret == SCARD_S_SUCCESS)
					{
						ret = SCardStatus(hCard,szReaderName,&dwCardNameLen,&dwState,&dwActiveProtocol,bATR,&dwAtrLen);
						//根据ATR的值判断是否需要的KEY
						if(ret != SCARD_S_SUCCESS || bATR[9] != CCID_ID)
						{
							pTempBuf += strlen(pTempBuf) + 1;
							continue;
						}
						SCardDisconnect(hCard,SCARD_LEAVE_CARD);
						nCount++;
						strcpy(pTempReaderSet->szDeviceAddress,pTempBuf); 
						strcpy(pTempReaderSet->szDeviceName,pTempBuf);
						pTempReaderSet->bDeviceType = DEVICE_CONNECTED_TYPE;
						pTempReaderSet += 1;
					}
					pTempBuf += strlen(pTempBuf) + 1;
				}
			}			
		}
		delete []pResponseBuffer;	
	}
	return nCount;
}

HDRIVER CCcid::OpenDevice(char *pszDevicePath)
{
    Init();
	WriteDSPLog("Open CCIDDevice");
	unsigned long ret = NO_ERROR;
	SCARDHANDLE hCardHandle=NULL;
	unsigned long dwActiveProtocol = 0;
	PCCIDDRIVER pCCIDDriver = NULL;
	ret = SCardConnect(	m_hContextHandle,
						pszDevicePath,
						SCARD_SHARE_SHARED,
						SCARD_PROTOCOL_T1,
						&hCardHandle,
						&dwActiveProtocol);
	if(ret != SCARD_S_SUCCESS)
	{
		WriteDSPLog("Open CCIDDevice fail");
		return NULL;
	}
	pCCIDDriver = new CCIDDRIVER;
	pCCIDDriver->hHandle = hCardHandle;
	WriteDSPLog("Open CCIDDevice pCCIDDriver=%x",pCCIDDriver);
	return pCCIDDriver;
}
int CCcid::SendAPDU(HDRIVER hDriver,char *szCommand, char *szReply,  int *pulSW)
{
    Init();
	unsigned char cCmd[APDU_MAX_LEN]={0};
	unsigned char Rerundata[APDU_MAX_LEN]={0};
	char sReply[MAX_RESPONSE]={0};
	char cSW[2]={0};
	int ReturndataLen;
	szReply[0]=0;
	*pulSW=0;
	int szCMDLen = (int)strlen(szCommand);
	if((szCMDLen %2)!=0) return INVALID_CMD;
	if (szCMDLen<8) return INVALID_CMD;

	WriteDSPLog("CCcid::SendAPDU(handle=0x%x,szCommand=%s)",hDriver,szCommand);
	HexToAsc( cCmd, szCommand,szCMDLen);
	ReturndataLen =  SendData(hDriver,cCmd,szCMDLen/2,Rerundata,APDU_MAX_LEN);
	if(ReturndataLen<2)
	{
		WriteDSPLog("Return Data Length Error  ReturndataLen=%d",ReturndataLen);
		return -1;
	}
	
	AscToHex(sReply,Rerundata,ReturndataLen);
	strcpy(szReply,sReply);
	//如果是复位
	if(strncmp(szCommand,"00000000",8)==0)
	{
		*pulSW =0x00009000;
		return ReturndataLen;
	}
	szReply[2*ReturndataLen-4]=0;
	*pulSW =0;
	
	cSW[0] = Rerundata[ReturndataLen-1];
	cSW[1] = Rerundata[ReturndataLen-2];
	memcpy(pulSW,cSW,2);

	WriteDSPLog("CCcid::SendAPDU::szReply=%s,*pulSW=%x",szReply,*pulSW);
	return ReturndataLen-2;
}
int CCcid::SendData(HDRIVER hDriver,unsigned char *pData,unsigned long DataLen,unsigned char *pReturnData,int ReturnDataLen)
{
    Init();
	PCCIDDRIVER pCCIDDriver = (PCCIDDRIVER)hDriver;
	long ActiveProtocol=0;	
	unsigned char ReceiveData[MAX_RESPONSE] = {0};
	unsigned long    nResp;
	SCARD_IO_REQUEST IO_Request;
	long ret;
	
	nResp = MAX_RESPONSE;
	IO_Request.dwProtocol = ActiveProtocol;
	IO_Request.cbPciLength = (unsigned long)sizeof(SCARD_IO_REQUEST);
	
	ret = SCardTransmit(
		pCCIDDriver->hHandle,
		&IO_Request,
		pData,
		DataLen,
		0,
		ReceiveData,
		&nResp
		);
	
	if(ret != SCARD_S_SUCCESS)
	{
		WriteDSPLog("SCardTransmit ret=0x%08x",ret);
		return -1;
	}
	if(ReturnDataLen<nResp)
	{
      memcpy(pReturnData,ReceiveData,ReturnDataLen);
	}
	else
	{
      memcpy(pReturnData,ReceiveData,nResp);
	}
	return nResp;
}

void CCcid::CloseDevice(HDRIVER hDriver)
{
	Init();
	WriteDSPLog("Close CCIDDevice");
	PCCIDDRIVER pCCIDDriver = (PCCIDDRIVER)hDriver;
	SCardDisconnect(pCCIDDriver->hHandle,SCARD_LEAVE_CARD);
	pCCIDDriver->hHandle = 0;
	delete pCCIDDriver;
}