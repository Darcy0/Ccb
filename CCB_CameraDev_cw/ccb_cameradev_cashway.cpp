#include "ccb_cameradev_cashway.h"
#include "Apdu.h"
#include "SAStatusLog.h"
#include "EloamCamera.h"

EloamCamera* eloamCamera;

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
	int ret =0;
	eloamCamera = new EloamCamera();
	ret =  eloamCamera->OpenCamera();
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return ret;
}

extern "C" CCB_CAMERADEV_API int GetCameraRatio( int *w, int *h )
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	int ret =0;
	if(w==NULL || h == NULL)
	{
		LogOut.StatusOut(Info,_T("(%s) 说明:%d\r\n"),__FUNCTION__,_T("存在参数为NULL"));
		return -1;
	}
	if(!eloamCamera)
	{
		LogOut.StatusOut(Info,_T("(%s) 说明:%d\r\n"),__FUNCTION__,_T("设备对象未初始化"));
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
		LogOut.StatusOut(Info,_T("(%s) 说明:%d\r\n"),__FUNCTION__,_T("存在参数为NULL"));
		return -1;
	}
	if(!eloamCamera)
	{
		LogOut.StatusOut(Info,_T("(%s) 说明:%d\r\n"),__FUNCTION__,_T("设备对象未初始化"));
		return -1;
	}
	ret =  eloamCamera->GetCameraFrame(pBuf_Color,pBuf_BW);
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return ret;
}

extern "C" CCB_CAMERADEV_API void CameraClose()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	if(!eloamCamera)
	{
		return;
	}
	eloamCamera->CameraClose();
	delete eloamCamera;
	eloamCamera = NULL;
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
}

extern "C" CCB_CAMERADEV_API void ExitCamera()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("该接口没有实现"));
}

extern "C" CCB_CAMERADEV_API void GetCameraResolutions( char *eyed,CString &strRes )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("该接口没有实现"));
}

extern "C" CCB_CAMERADEV_API void SetCameraResoltuions( char *eyed,int iWidth,int iHeight,CString fmt )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("该接口没有实现"));
}

extern "C" CCB_CAMERADEV_API int GetCameraRatio_1( char *eyed,int *w, int *h )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("该接口没有实现"));
	return 0;
}

extern "C" CCB_CAMERADEV_API void store_res( int w,int h )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("该接口没有实现"));
}

extern "C" CCB_CAMERADEV_API int GetRandom( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	HDRIVER hToken;
	int iRet=APDU_GenRandomNumber(hToken,clen,(T_U8*)rdata,(T_U32 *)rlen);
	LogOut.StatusOut(Info,_T("(%s)  返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C"CCB_CAMERADEV_API int GetMessageEx( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("该接口没有实现"));
	return 0;
}

extern "C" CCB_CAMERADEV_API int WriteKey( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==command)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End:Command is NULL"));
		return -1;
	}
	HDRIVER hToken;
	T_U32 ulKeyFlags = command[0];
	T_BOOL endFlag = command[1]&0x01;
	T_U32 ulKeyID  = command[2];
	T_U32 ulKeyLen = command[3];
	T_U8 pbKey[256];
	memcpy(pbKey,&command[4],ulKeyLen);
	int iRet=APDU_WriteKey(hToken, ulKeyFlags,endFlag,ulKeyID,pbKey,ulKeyLen);
	LogOut.StatusOut(Info,_T("(%s)  返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int InternalAuthenticate( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==command)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End:Command is NULL"));
		return -1;
	}
	HDRIVER hToken;
	T_U32 ulKeyID = command[0];
	T_U32 ulIADataLen = command[1];
	T_U8 pbIAData[256];
	memcpy(pbIAData,&command[2],ulIADataLen);
	int iRet=APDU_InternalAuthentication(hToken,ulKeyID,pbIAData,ulIADataLen,(T_U8*)rdata,(T_U32*)rlen);
	LogOut.StatusOut(Info,_T("(%s)  返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int ExternalAuthenticate( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==command)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End:Command is NULL"));
		return -1;
	}
	HDRIVER hToken;
	T_U32 ulKeyID = command[0];
	T_U32 ulEAKeyLen = command[1];
	T_U8 pbEAKey[256];
	memcpy(pbEAKey,&command[2],ulEAKeyLen);
	int iRet=APDU_ExternalAuthentication(hToken,ulKeyID, pbEAKey,ulEAKeyLen);
	LogOut.StatusOut(Info,_T("(%s)  返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int ImageSignature( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==command)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End:Command is NULL"));
		return -1;
	}
	HDRIVER hToken;
	T_U32 ulFileID = (command[1]<<8)|command[0];
	T_U32 ulInDataLen =command[2];
	T_U8 pbInData[512];
	memcpy(pbInData,&command[3],ulInDataLen);
	int iRet=0xff;
	if(command[0] == 5)
	{
		//todo
		return 1;
	}
	else if(command[0] == 6)
	{
		iRet=APDU_RSAPubKeyOperate(hToken,ulFileID,pbInData,ulInDataLen,(T_U8 *)rdata,(T_U32 *)rlen);
	}
	else if(command[0] == 7)
	{
		iRet=APDU_RSAPubKeyOperate(hToken,ulFileID,pbInData,ulInDataLen,(T_U8 *)rdata,(T_U32 *)rlen);
	}
	LogOut.StatusOut(Info,_T("(%s)  返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}

extern "C" CCB_CAMERADEV_API int ImageEncrypt( char* command, int clen,char *rdata, int *rlen )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==command)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End:Command is NULL"));
		return -1;
	}
	HDRIVER hToken;
	T_U32 ulKeyID = command[0];
	T_U32 ulIADataLen = command[1];
	T_U8 pbIAData[256];
	memcpy(pbIAData,&command[2],ulIADataLen);
	int iRet=APDU_SymmetricKeyIDOperate(hToken,ET_ENCRYPT,ulKeyID,pbIAData,ulIADataLen,(T_U8 *)rdata,(T_U32 *)rlen);
	LogOut.StatusOut(Info,_T("(%s)  返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
	return iRet;
}
