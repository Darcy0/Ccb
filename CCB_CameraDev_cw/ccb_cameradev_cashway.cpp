#include <stdlib.h>
#include <time.h>
#include "ccb_cameradev_cashway.h"
#include "Apdu.h"
#include "SAStatusLog.h"
#include "EloamCamera.h"

EloamCamera* eloamCamera;
HDRIVER gDriver=NULL;

extern "C" CCB_CAMERADEV_API char * GetCameraCode()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	char* cameraCode=eloamCamera->GetCameraCode();	
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return cameraCode;	
}

extern "C" CCB_CAMERADEV_API int OpenCamera()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	int iRet = -1;
	if(0 == EloamCamera::m_hDevState)
	{
		eloamCamera = new EloamCamera();
		LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͷ����..."));
		iRet =  eloamCamera->OpenCamera();
		if (iRet<0)
		{//����ͷ��ʧ��
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͷ��ʧ�ܡ�"));
			delete eloamCamera;
			eloamCamera = NULL;
		}
		else
		{//����ͷ�򿪳ɹ�
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͷ��ɹ���"));
		}
	}
	else 
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("����ͷ�Ѿ��򿪡�"),_T("End."));
		return 2;
	}
	DEVICE dev[10];
	T_S32 count=EnumDevices(dev);
	if (0==count)
	{
		LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����оƬ��ȡʧ��."));
	}
	else
	{
		gDriver=OpenDevice(dev[0].szDeviceAddress);
		if (NULL==gDriver)
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����оƬ��ʧ��."));
		}
		else
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����оƬ�򿪳ɹ�."));
		}
	}	
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int GetCameraRatio( int *w, int *h )
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	int ret =0;
	if(w==NULL || h == NULL)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%d\r\n"),__FUNCTION__,_T("���ڲ���ΪNULL"));
		return -1;
	}
	if(!eloamCamera)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%d\r\n"),__FUNCTION__,_T("�豸����δ��ʼ��"));
		return -1;
	}	
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	ret =  eloamCamera->GetCameraRatio(w,h);
	return ret;
}

extern "C" CCB_CAMERADEV_API int GetCameraFrame( BYTE* pBuf_Color,BYTE* pBuf_BW )
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	int ret = 0;
	if( pBuf_Color == NULL || pBuf_Color == NULL)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%d\r\n"),__FUNCTION__,_T("���ڲ���ΪNULL"));
		return -1;
	}
	if(!eloamCamera)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%d\r\n"),__FUNCTION__,_T("�豸����δ��ʼ��"));
		return -1;
	}
	ret =  eloamCamera->GetCameraFrame(pBuf_Color,pBuf_BW);
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return ret;
}

extern "C" CCB_CAMERADEV_API void CameraClose()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	if(1 == EloamCamera::m_hDevState)
	{
		if(eloamCamera)
		{
			eloamCamera->CameraClose();
			EloamCamera::m_hDevState = 0;
			delete eloamCamera;
			eloamCamera = NULL;
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("ִ���˹ر�����ͷ."));
		}	
	}
	else 
	{
		LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͷδ��."));
		return;
	}
	if (gDriver)
	{
		CloseDevice(gDriver);
		LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("ִ���˹رչ���оƬ����."));
	}	
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
}

extern "C" CCB_CAMERADEV_API void ExitCamera()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("�ýӿ�û��ʵ��"));
}

extern "C" CCB_CAMERADEV_API void GetCameraResolutions( char *eyed,CString &strRes )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("�ýӿ�û��ʵ��"));
}

extern "C" CCB_CAMERADEV_API void SetCameraResoltuions( char *eyed,int iWidth,int iHeight,CString fmt )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("�ýӿ�û��ʵ��"));
}

extern "C" CCB_CAMERADEV_API int GetCameraRatio_1( char *eyed,int *w, int *h )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("�ýӿ�û��ʵ��"));
	return 0;
}

extern "C" CCB_CAMERADEV_API void store_res( int w,int h )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("�ýӿ�û��ʵ��"));
}

extern "C" CCB_CAMERADEV_API int GetRandom( char* command, int clen,char **rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (sizeof(command)<3)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("����Ȳ�����"));
		return -1;
	}
	*rlen = command[2];
	//*rdata=new char(256);
	memset(*rdata,0,strlen(*rdata));
	int iRet=0;
	if(command[2]== 9)
	{
		time_t curtime;
		time(&curtime);
		curtime=curtime*1000;//���ɺ���
		memcpy(*rdata,(T_U8*)&curtime,sizeof(curtime));
		*rlen = 10;
	}
	else if(command[2]== 8)
	{
		iRet=APDU_GenRandomNumber(gDriver,8,(T_U8*)(*rdata),(T_U32 *)rlen);
	}
	else 
	{
		iRet=APDU_GenRandomNumber(gDriver,command[2],(T_U8*)(*rdata),(T_U32 *)rlen);
	}
	
	LogOut.StatusOut(Info,_T("(%s)  ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" /*CCB_CAMERADEV_API*/ int  __stdcall GetMessageEx( char* command, int clen,char **rdata, int *rlen )
{
	char *msg="13000101ELOAM";
	int msgLen=strlen(msg);
	//*rdata=(char*)malloc(msgLen+1);
	memset(*rdata,0,sizeof(*rdata));
	strcpy(*rdata,msg);	
	*rlen=strlen(*rdata);	
	return 0;
}

extern "C" CCB_CAMERADEV_API int WriteKey( char* command, int clen,char **rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (sizeof(command)<3)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("����Ȳ�����"));
		return -1;
	}
	if (NULL==command)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End:Command is NULL"));
		return -1;
	}
	T_U32 ulKeyFlags = command[0];
	T_U32 ulKeyID  = command[1];
	T_U32 ulKeyLen = command[2];
	T_U8 pbKey[256];//ID+KEY;
	T_S8 szMac[4];
	T_S32 nMacLen = sizeof(szMac);
	memcpy(pbKey,&command[3],ulKeyLen);
	int iRet=APDU_WriteKey(gDriver, ulKeyFlags,0,ulKeyID,pbKey,ulKeyLen);
	LogOut.StatusOut(Info,_T("(%s)  ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int InternalAuthenticate( char* command, int clen,char **rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (sizeof(command)<3)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("����Ȳ�����"));
		return -1;
	}
	T_U32 ulKeyID = command[1];
	T_U32 ulIADataLen = command[2];
	T_U8 pbIAData[256];
	//*rdata=new char(256);
	memset(*rdata,0,strlen(*rdata));
	memcpy(pbIAData,&command[3],ulIADataLen);
	int iRet=APDU_InternalAuthentication(gDriver,ulKeyID,pbIAData,ulIADataLen,(T_U8*)rdata,(T_U32*)rlen);
	LogOut.StatusOut(Info,_T("(%s)  ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int ExternalAuthenticate( char* command, int clen,char **rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (sizeof(command)<3)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("����Ȳ�����"));
		return -1;
	}
	T_U32 ulKeyID = command[1];
	T_U32 ulEAKeyLen = command[2];
	T_U8 pbEAKey[256];
	memcpy(pbEAKey,&command[3],ulEAKeyLen);
	int iRet=APDU_ExternalAuthentication(gDriver,ulKeyID, pbEAKey,ulEAKeyLen);
	LogOut.StatusOut(Info,_T("(%s)  ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int ImageSignature( char* command, int clen,char **rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (sizeof(command)<3)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("����Ȳ�����"));
		return -1;
	}
	T_U32 ulFileID = (command[1]<<8)|command[0];
	T_U32 ulInDataLen =command[2];
	T_U8 pbInData[512];
	memcpy(pbInData,&command[3],ulInDataLen);
	//*rdata=new char(256);
	memset(*rdata,0,strlen(*rdata));
	int iRet=0x00;
	if(command[0] == 5)
	{
		iRet=1;
	}
	else
	{
		iRet=APDU_RSAPubKeyOperate(gDriver,ulFileID,pbInData,ulInDataLen,(T_U8 *)rdata,(T_U32 *)rlen);
	}
	LogOut.StatusOut(Info,_T("(%s)  ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int ImageEncrypt( char* command, int clen,char **rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (sizeof(command)<3)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("����Ȳ�����"));
		return -1;
	}
	T_U32 ulKeyID = (command[1]<<8)|command[0];
	T_U32 ulIADataLen = command[1];
	T_U8 pbIAData[256];
	memcpy(pbIAData,&command[2],ulIADataLen);
	//*rdata=new char(256);
	memset(*rdata,0,strlen(*rdata));
	int iRet=APDU_SM2PriKeyOperate(gDriver,ulKeyID,pbIAData,ulIADataLen,(T_U8 *)rdata,(T_U32 *)rlen);
	LogOut.StatusOut(Info,_T("(%s)  ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}