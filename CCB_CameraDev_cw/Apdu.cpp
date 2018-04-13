#include "stdio.h"
#include "stdlib.h"
#include "Apdu.h"
#include "DSPProvider.h"
#include "PublicFunc.h"

T_S32 EnumDevices(DEVICE *pDevice)
{
	return DSP_EnumDevices(pDevice);
}

T_S32 EnumAllDevices(DEVICE *pDevice)
{
	return DSP_EnumDevices(pDevice);
}

HDRIVER OpenDevice(T_S8 * pszDeviceStr)
{
	return DSP_OpenDevice(pszDeviceStr);
}

void CloseDevice(HDRIVER hDriver)
{
	DSP_CloseDevice(hDriver);
}

HANDLE APDU_BeginTransaction(IN HDRIVER hToken,IN ULONG ulWaitTime)
{
	HANDLE hMutex=NULL;
	char szMutex[MAX_PATH+40] = {0};

	BotReader *pBotRdr = (BotReader *)hToken;
	if(NULL == pBotRdr)
		return NULL;

	if(strlen(pBotRdr->szDevpath)==0)
	{
		SetLastError(ERROR_INVALID_NAME);
		goto ret;
	}

	strcpy(szMutex,"ETAPDU_74CF8916721B4e6cBA1FA98CD6A79626_");
	strcat(szMutex,pBotRdr->szDevpath+4);

	hMutex=CreateMutex(NULL,FALSE,szMutex);
	if(hMutex)
	{
		int nErr=GetLastError();
		if(WaitForSingleObject(hMutex,ulWaitTime)==WAIT_TIMEOUT)
		{
			CloseHandle(hMutex);
			hMutex=NULL;
			goto ret;
		}
	}
	else
	{

	}
ret:
	return hMutex;
}

APDU_RT EndTransaction(IN HANDLE hMutex)
{
	if(hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
	return RT_OK;
}

T_S32 SendAPDU(IN HDRIVER hDriver,IN T_S8* szCommand, OUT T_S8* szReply, OUT T_U16* pulSW)
{
	T_S32 ret = 0;
	T_S32 sw = 0;

	HANDLE hMutex = APDU_BeginTransaction(hDriver); if (!hMutex) return RT_UNKNOW_ERROR;

	ret = DSP_XSendAPDU(hDriver,(char*)szCommand,(char*)szReply,&sw);
	*pulSW = (T_U16)sw;

	EndTransaction(hMutex);

	return ret;
}

APDU_RT APDU_ReloadPIN(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8 *pbReloadPIN,
					IN T_S32 ulReloadPINLen,IN T_U8 *pbNewPIN,IN T_S8 ulNewPINLen)
{
	T_S32 nType = 0;
	APDU_RT  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szBuf[512] = {0};
	T_S8 TempPin[64]={0};
    T_S8 szMac[512] = {0};
	T_S32 nMacLen = sizeof(szMac);
	T_U8 psKey[8] = {0};
	T_S8 szDesRet[512] = {0};
	T_S32 nDesRet = sizeof(szDesRet);
	
	if ( (hToken == NULL || hToken == PARA_INVALID_HTOKEN) )
	{
      return RT_INVALID_PARAMETER;
	}
    if(!(ptPINType == SO_PIN || ptPINType == USER_PIN))
	{
      return RT_INVALID_PARAMETER;
	}
	if(pbReloadPIN == NULL || pbNewPIN == NULL|| ulReloadPINLen != 16)
	{
		return RT_INVALID_PARAMETER;
	}

	PinToMac(pbNewPIN,ulNewPINLen,szMac,&nMacLen);
	AscToHex(TempPin, (T_U8*)szMac,nMacLen);
	TempPin[16] = 0;
	
	AscToHex(szBuf, pbReloadPIN,16);
 
	DES(TempPin,szBuf,TRUE,szDesRet,&nDesRet);

	memset(szBuf, 0, sizeof(szBuf));
	memcpy(szBuf,pbReloadPIN,16);	
	for(T_S32 i=0;i<8;i++)
	{
		psKey[i] = szBuf[i] ^ szBuf[i+8];
	}
	
	memset(szBuf,0,sizeof(szBuf));
	AscToHex(szBuf, psKey,8);
	
	nMacLen = sizeof(szMac);
	MAC("0000000000000000",szDesRet,szBuf,szMac,&nMacLen);
	szMac[8] = 0;
		
	if (ptPINType == USER_PIN)
		nType = 0;
	else if (ptPINType == SO_PIN)
		nType = 1;
	else
		return RT_INVALID_PARAMETER;
	
	sprintf(szCommand,"845E00%02X0C%s%s",nType,szDesRet,szMac);
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);
	if ( nSW != RT_OK ) 
	{
		nSW = RT_UNKNOW_ERROR;
	}

	EndTransaction(hMutex);
	return nSW;

}

APDU_RT APDU_VerifyPIN_SM4(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8* pbPIN,IN T_U32 ulPinLen)
{
	APDU_RT  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};

	APDU_RT rv = RT_OK;
	T_S8 str[256]={0};

	T_U8 szMac[512]={0};
	T_S32 nMacLen = sizeof(szMac);

	if(ulPinLen > MAX_PIN_LENGTH || ulPinLen < MIN_PIN_LENGTH || NULL == pbPIN)
	{
		return RT_INVALID_PARAMETER;
	}
	if(ptPINType != USER_PIN && ptPINType != SO_PIN)
	{
		return RT_INVALID_PARAMETER;
	}

	PinToMac_SM4(pbPIN,ulPinLen,(T_S8*)szMac,&nMacLen);
	AscToHex(str, szMac,nMacLen);
	str[32] = 0;

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do 
	{	
		rv = APDU_RemandEncyptPIN_SM4(hToken,str);
		if (rv != RT_OK) 
		{
			break;
		}	

		switch (ptPINType)
		{
		case SO_PIN:
			str[16] = 0;
			sprintf(szCommand,"0420000108%s",str);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			rv = nSW;
			break;
		case USER_PIN:
			str[16] = 0;
			sprintf(szCommand,"0420000008%s",str);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			rv = nSW;
			break;
		default:
			rv = RT_INVALID_PARAMETER;
			break;
		}
	}while(0);

	EndTransaction(hMutex);
	return rv;
}
APDU_RT APDU_RemandEncyptPIN_SM4(IN HDRIVER hToken,IN OUT T_S8* pPin)
{
	T_U16  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szBuf[512] = {0};
	T_S8 szDes[512] = {0};
	T_S32 nDes = sizeof(szDes);

	if (!pPin) 
	{
		return RT_INVALID_PARAMETER;
	}
	strcpy(szBuf,pPin);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,"0084000008",szReply,&nSW);
		if (nSW!=RT_OK) 
		{
			break;
		}
		SM4_Verify(szReply, szBuf, TRUE, szDes, &nDes, 0x00);
		strcpy(pPin,szDes);
	}while(0);
	EndTransaction(hMutex);
	return nSW;
}

APDU_RT APDU_VerifyPIN(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8* pbPIN,IN T_U32 ulPinLen)
{
	APDU_RT  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};

	APDU_RT rv = RT_OK;
	T_S8 str[256]={0};

	T_U8 szMac[512]={0};
	T_S32 nMacLen = sizeof(szMac);
	
	if(ulPinLen > MAX_PIN_LENGTH || ulPinLen < MIN_PIN_LENGTH || NULL == pbPIN)
	{
		return RT_INVALID_PARAMETER;
	}
	if(ptPINType != USER_PIN && ptPINType != SO_PIN)
	{
		return RT_INVALID_PARAMETER;
	}

	PinToMac(pbPIN,ulPinLen,(T_S8*)szMac,&nMacLen);
	AscToHex(str, szMac,nMacLen);
	str[16] = 0;	
	 
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		rv = APDU_RemandEncyptPIN(hToken,str);
		if (rv != RT_OK) 
		{
			break;
		}	

		switch (ptPINType)
		{
		case SO_PIN:
			str[16] = 0;
			sprintf(szCommand,"0420000108%s",str);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			rv = nSW;
			break;
		case USER_PIN:
			str[16] = 0;
			sprintf(szCommand,"0420000008%s",str);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			rv = nSW;
			break;
		default:
			rv = RT_INVALID_PARAMETER;
			break;
		}
	}while(0);
	EndTransaction(hMutex);

	return rv;
}


APDU_RT APDU_ChangePIN_SM4(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8 *pbOldPIN,
					   IN T_U32 ulOldPinlen,IN T_U8 *pbNewPIN,IN T_U32 ulNewPinLen)
{
	T_U16  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szMac[40];
	T_S32 nMac = sizeof(szMac);
	T_S8 szNewMac[40];
	T_S32 nNewMac = sizeof(szNewMac);
	T_S8 szChiperPin[40];
	T_S32 nChiperPin = sizeof(szChiperPin);
	T_S8 szChiperNewPin[40];
	T_S32 nChiperNewPin = sizeof(szChiperNewPin);
	T_S8 szBufPIN[128] = {0};

	if( pbOldPIN==NULL || pbNewPIN == NULL ||
		ulNewPinLen < MIN_PIN_LENGTH || ulNewPinLen > MAX_PIN_LENGTH ||
		ulOldPinlen < MIN_PIN_LENGTH || ulOldPinlen > MAX_PIN_LENGTH )
	{
		return RT_INVALID_PARAMETER;
	}

	if(!PinToMac_SM4(pbOldPIN,ulOldPinlen,szMac,&nMac))
		return RT_INVALID_PARAMETER;

	AscToHex(szBufPIN,(unsigned char*)szMac,nMac);
	szBufPIN[16] = 0;
	strcpy(szMac,szBufPIN);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,"0084000008",szReply,&nSW);
		if (nSW != RT_OK)
		{
			nSW = RT_INVALID_PARAMETER;
			break;
		}
		SM4_Verify(szReply,szMac,TRUE,szChiperPin,&nChiperPin, 0x00);

		if(!PinToMac_SM4(pbNewPIN,ulNewPinLen,szNewMac,&nNewMac))
		{
			nSW = RT_INVALID_PARAMETER;
			break;
		}

		memset(szBufPIN,0,sizeof(szBufPIN));
		AscToHex(szBufPIN,(unsigned char*)szNewMac,nNewMac);
		szBufPIN[16] = 0;
		strcpy(szNewMac,szBufPIN);

		SM4_Verify(szNewMac,szMac,TRUE,szChiperNewPin,&nChiperNewPin, 0xFF);

		T_S32 nType = 0;

		if (ptPINType == USER_PIN)
			nType = 0;
		else if (ptPINType == SO_PIN)
			nType = 1;
		else
		{
			nSW = RT_INVALID_PARAMETER;	  
			break;
		}

		//szChiperPin[16] = 0;
		//szChiperNewPin[16] = 0;
		sprintf(szCommand,"845E01%02X%02x%s%s",
			nType,strlen(szChiperPin)/2 + strlen(szChiperNewPin)/2,szChiperPin,szChiperNewPin);

		SendAPDU(hToken,szCommand,szReply,&nSW);
	}while(0);

	EndTransaction(hMutex);

	return nSW;
}



APDU_RT APDU_ChangePIN(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8 *pbOldPIN,
					IN T_U32 ulOldPinlen,IN T_U8 *pbNewPIN,IN T_U32 ulNewPinLen)
{
	T_U16  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szMac[40];
	T_S32 nMac = sizeof(szMac);
	T_S8 szNewMac[40];
	T_S32 nNewMac = sizeof(szNewMac);
	T_S8 szChiperPin[40];
	T_S32 nChiperPin = sizeof(szChiperPin);
	T_S8 szChiperNewPin[40];
	T_S32 nChiperNewPin = sizeof(szChiperNewPin);
	T_S8 szBufPIN[128] = {0};
	
	if( pbOldPIN==NULL || pbNewPIN == NULL ||
		ulNewPinLen < MIN_PIN_LENGTH || ulNewPinLen > MAX_PIN_LENGTH ||
		ulOldPinlen < MIN_PIN_LENGTH || ulOldPinlen > MAX_PIN_LENGTH )
	{
		return RT_INVALID_PARAMETER;
	}

	if(!PinToMac(pbOldPIN,ulOldPinlen,szMac,&nMac))
		return RT_INVALID_PARAMETER;
	
	AscToHex(szBufPIN,(unsigned char*)szMac,nMac);
	szBufPIN[16] = 0;
	strcpy(szMac,szBufPIN);
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do 
	{
		SendAPDU(hToken,"0084000008",szReply,&nSW);
		if (nSW != RT_OK)
		{
			nSW = RT_INVALID_PARAMETER;
			break;
		}
		
		DES(szReply,szMac,TRUE,szChiperPin,&nChiperPin);
		
		if(!PinToMac(pbNewPIN,ulNewPinLen,szNewMac,&nNewMac))
		{
			nSW =  RT_INVALID_PARAMETER;
			break;
		}
		
		memset(szBufPIN,0,sizeof(szBufPIN));
		AscToHex(szBufPIN,(unsigned char*)szNewMac,nNewMac);
		szBufPIN[16] = 0;
		strcpy(szNewMac,szBufPIN);
		
		DES(szNewMac,szMac,TRUE,szChiperNewPin,&nChiperNewPin);
		
		T_S32 nType = 0;
		
		if (ptPINType == USER_PIN)
			nType = 0;
		else if (ptPINType == SO_PIN)
			nType = 1;
		else
		{
			nSW = RT_INVALID_PARAMETER;	  
			break;
		}
		
		sprintf(szCommand,"845E01%02X%02x%s%s",
			nType,strlen(szChiperPin)/2 + strlen(szChiperNewPin)/2,szChiperPin,szChiperNewPin);
		
		SendAPDU(hToken,szCommand,szReply,&nSW);
	}while(0);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_UnblockPIN(IN HDRIVER hToken,IN T_U8 *pbSOPIN,IN T_U32 ulSOPinlen,IN T_U8 *pbUserPIN,IN T_U32 ulUserPINLen)
{
	APDU_RT rv = RT_OK;
	T_U16  nSW = 0;
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szBuf[512] = {0};
	T_S8 szDesRet[512] = {0};
	T_S32 nDesRet = sizeof(szDesRet);
	T_S8 TempPin[64]={0};
	T_S8 szMac[512] = {0};
	T_S32 nMacLen = sizeof(szMac);
	T_U8 Random[24]={0};
	T_U32 RandomLen=sizeof(Random);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		rv= APDU_VerifyPIN( hToken,SO_PIN,pbSOPIN,ulSOPinlen);
		if(rv !=0x9000)
		{
			nSW = rv;
			break;
		}
		PinToMac(pbUserPIN,ulUserPINLen,szMac,&nMacLen);
		AscToHex(TempPin, (T_U8*)szMac,nMacLen);
		char Apdu[64]="8424000014";
		szBuf[0]='0';
		szBuf[1]='8';
		memcpy(szBuf+2,TempPin,16);
		DES(szBuf,"45545F556E626C6F636B5F557365725F",TRUE,szDesRet,&nDesRet);
		memcpy(Apdu+10,szDesRet,32);
		rv= APDU_GenRandomNumber(hToken,8,Random,&RandomLen);
		if(rv !=0x9000)
		{
			nSW = rv;
			break;
		}
		memset(szDesRet,0,sizeof(szDesRet));
		nDesRet=sizeof(szDesRet);
		AscToHex(szDesRet,Random,RandomLen);
		memcpy(Random,szDesRet,8);
		memset(szDesRet,0,sizeof(szDesRet));
		MAC((char *)Random,Apdu,"45545F556E626C6F636B5F557365725F",szDesRet,&nDesRet);
		memcpy(Apdu+42,szDesRet,8);
		SendAPDU(hToken,Apdu,szReply,&nSW);
	}while(0);
	
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_RemandEncyptPIN(IN HDRIVER hToken,IN OUT T_S8* pPin)
{
	T_U16  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szBuf[512] = {0};
	T_S8 szDes[512] = {0};
	T_S32 nDes = sizeof(szDes);
	
	if (!pPin) 
	{
		return RT_INVALID_PARAMETER;
	}
	strcpy(szBuf,pPin);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,"0084000008",szReply,&nSW);
		if (nSW!=RT_OK) 
		{
			break;
		}
		DES(szReply,szBuf,TRUE,szDes,&nDes);
		strcpy(pPin,szDes);
	}while(0);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_CreateFile(IN HDRIVER hToken,IN FILE_CLASS fcFileType,IN T_U32 ulFileID,
					IN T_U32 ulFileSize,IN T_U8 bCreateOrReadOrSignRight,IN T_U8 bDeleteOrWriteOrAddRight,
					IN CREATE_FILE_FLAGS cffFlags, IN T_U8 *pbFileName,IN T_U32 ulFileNamelen)
{	
	T_U16  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szName[34] = {0};
	
	if ( (hToken == NULL || hToken == PARA_INVALID_HTOKEN) 
		|| !(fcFileType == FILE_CLASS_EF_PRIVATEKEY ||fcFileType == FILE_CLASS_EF_COMMON 
		|| fcFileType==FILE_CLASS_EF_CYC_REC || fcFileType == FILE_CLASS_EF_FIXED_LEN_REC 
		||fcFileType == FILE_CLASS_EF_NO_FIXED_LEN_REC || fcFileType == FILE_CLASS_DF 
		|| fcFileType == FILE_CLASS_MF || fcFileType == FILE_CLASS_KEY)
		|| ulFileID>0xFFFF || ulFileID==0x0000
		|| bCreateOrReadOrSignRight>0xFF || bCreateOrReadOrSignRight<0x00
		|| bDeleteOrWriteOrAddRight>0xFF || bDeleteOrWriteOrAddRight<0x00
		)
	{
		return RT_INVALID_PARAMETER;
	}

	if((FILE_CLASS_MF == fcFileType || FILE_CLASS_DF == fcFileType)
		&& (pbFileName == NULL || ulFileNamelen < 5 || ulFileNamelen > 16))
	{
		return RT_INVALID_PARAMETER;
	}

	if(FILE_CLASS_MF == fcFileType || FILE_CLASS_DF == fcFileType)
	{
		AscToHex(szName, pbFileName, ulFileNamelen);
	}
	
	switch(fcFileType)
	{
	case FILE_CLASS_MF:
		sprintf(szCommand,"80E00000%02X383F00%04X%02X%02X%02XFFFF00%s",
			11 + ulFileNamelen, ulFileSize, bCreateOrReadOrSignRight, bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID, szName);
		break;
	case FILE_CLASS_DF:
		sprintf(szCommand,"80E00000%02X38%04X%04X%02X%02X%02X%02X%02X01%s",
			11 + ulFileNamelen, ulFileID, ulFileSize, bCreateOrReadOrSignRight, bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID, cffFlags.bUsePurwrite, cffFlags.bUsePurview, szName);
		break;
		
	case FILE_CLASS_KEY:
		sprintf(szCommand,"80E000000B210000%04XFF%02X%02X%02X%02X00",
			ulFileSize, bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID,cffFlags.bSafeAttribute,cffFlags.bInitState);
		break;	
		
	case FILE_CLASS_EF_COMMON:
		sprintf(szCommand,"80E000000B01%04X%04X%02X%02X%02X%02X%02X00",
			ulFileID,ulFileSize,bCreateOrReadOrSignRight,bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID,cffFlags.bSafeAttribute,cffFlags.bInitState);
		break;
		
	case FILE_CLASS_EF_PRIVATEKEY:
		sprintf(szCommand,"80E000000B22%04X%04X%02X00%02X%02X%02X00",
			ulFileID, ulFileSize, bCreateOrReadOrSignRight,
			cffFlags.bShortFileID, cffFlags.bSafeAttribute, cffFlags.bInitState);
		break;	
		
	case FILE_CLASS_EF_FIXED_LEN_REC:
		sprintf(szCommand,"80E000000B02%04X%02X%02X%02X%02X%02X%02X%02X00",
			ulFileID, cffFlags.bRecordCount, (ulFileSize & 0XFF),bCreateOrReadOrSignRight,bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID,cffFlags.bSafeAttribute,cffFlags.bInitState);
		break;

	case FILE_CLASS_EF_NO_FIXED_LEN_REC:
		sprintf(szCommand,"80E000000B04%04X%04X%02X%02X%02X%02X%02X00",
			ulFileID,ulFileSize,bCreateOrReadOrSignRight,bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID,cffFlags.bSafeAttribute,cffFlags.bInitState);
		break;	
	case FILE_CLASS_EF_CYC_REC:
		sprintf(szCommand,"80E000000B06%04X%02X%02X%02X%02X%02X%02X%02X00",
			ulFileID, cffFlags.bRecordCount, (ulFileSize & 0XFF),bCreateOrReadOrSignRight,bDeleteOrWriteOrAddRight,
			cffFlags.bShortFileID,cffFlags.bSafeAttribute,cffFlags.bInitState);
		break;
	default:
		return RT_INVALID_PARAMETER;
		break;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU( hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_DeleteFile(IN HDRIVER hToken,IN T_U32 ulFileID)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| ulFileID>0xFFFF || ulFileID==0x0000)
	{
		return RT_INVALID_PARAMETER;
	}

	sprintf(szCommand,"80E4000102%04X",ulFileID);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);
	return nSW;	
}

APDU_RT APDU_DeleteFileEx(IN HDRIVER hToken,IN FILE_CLASS fcFileClass,IN T_U32 ulFileID)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN ) 
		|| !(fcFileClass==FILE_CLASS_EF_PRIVATEKEY ||fcFileClass==FILE_CLASS_EF_COMMON || fcFileClass==FILE_CLASS_EF_CYC_REC\
			|| fcFileClass==FILE_CLASS_EF_FIXED_LEN_REC ||fcFileClass==FILE_CLASS_EF_NO_FIXED_LEN_REC
			|| fcFileClass==FILE_CLASS_DF || fcFileClass==FILE_CLASS_MF )
		|| ulFileID>0xFFFF || ulFileID==0x0000)
	{
		return RT_INVALID_PARAMETER;
	}

	switch(fcFileClass)
	{
	case FILE_CLASS_DF:
	case FILE_CLASS_MF:
		sprintf(szCommand,"80E4000002%04X",ulFileID);
		break;
	case FILE_CLASS_EF_COMMON:
	case FILE_CLASS_EF_PRIVATEKEY:
	case FILE_CLASS_EF_FIXED_LEN_REC:
	case FILE_CLASS_EF_NO_FIXED_LEN_REC:
	case FILE_CLASS_EF_CYC_REC:
		
		sprintf(szCommand,"80E4000102%04X",ulFileID);
	    break;
	default:
		return RT_INVALID_PARAMETER;
	    break;
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);
	return nSW;
}

APDU_RT APDU_EraseDF(IN HDRIVER hToken)
{	
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	
	if (hToken==NULL || hToken==PARA_INVALID_HTOKEN ) 
	{
		return RT_INVALID_PARAMETER;
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,(T_S8*)"80E4000000",szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_ReadBinary(IN HDRIVER hToken,IN T_U32 ulOffset,IN T_U32 ulSize,
						   OUT T_U8 *pbOutData,IN OUT T_U32* pulOutDataLen)
{
	T_U32 i;
	FILE_ATTR fiFileInfo;
	APDU_RT rv = RT_OK;
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulOutDataLen == NULL)
	{
		return RT_INVALID_PARAMETER;
	}

	if (ulSize > *pulOutDataLen)
		return RT_BUFFER_SMALL;
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
		if (rv != RT_OK) 
		{
			nSW = rv;
			break;
		}

		if (pbOutData == NULL)
		{
			*pulOutDataLen = fiFileInfo.wFileSpareSize;
			nSW = RT_OK;
			break;
		}

		if ((ulOffset + ulSize) > fiFileInfo.wFileSpareSize)
		{
			nSW = RT_RANGE_OVERFLOW;
			break;
		}

		*pulOutDataLen = 0;
		for(i = 0; i < ulSize / TRANSFER_BUFFER_LEN; i++)
		{
			sprintf( szCommand, "00B0%04X%02X", ulOffset + i*TRANSFER_BUFFER_LEN, TRANSFER_BUFFER_LEN);
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				goto ret;
			}
			HexToAsc(pbOutData+i*TRANSFER_BUFFER_LEN, szReply, TRANSFER_BUFFER_LEN*2);
			*pulOutDataLen += TRANSFER_BUFFER_LEN;
		}
		
		if ( ulSize % TRANSFER_BUFFER_LEN )
		{
			sprintf( szCommand, "00B0%04X%02X",ulOffset + i * TRANSFER_BUFFER_LEN, 
					ulSize%TRANSFER_BUFFER_LEN );
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				break;
			}
			HexToAsc(pbOutData + i * TRANSFER_BUFFER_LEN, szReply, (ulSize%TRANSFER_BUFFER_LEN) * 2 );
			*pulOutDataLen += (ulSize%TRANSFER_BUFFER_LEN);	
		}
	}while(0);

ret:
	EndTransaction(hMutex);

	return nSW;
}

// 2008.6.20 zhanxin hong
APDU_RT APDU_ReadBinaryEx(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U32 ulOffset,IN T_U32 ulSize,
						   OUT T_U8 *pbOutData,IN OUT T_U32* pulOutDataLen)
{
	T_U32 i;
	APDU_RT rv = RT_OK;
	FILE_ATTR fiFileInfo;	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulOutDataLen==NULL)
	{
		return RT_INVALID_PARAMETER;
	}
	
	if (ulSize>*pulOutDataLen)
		RT_BUFFER_SMALL;

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		if((rv=APDU_SelectFile(hToken,ulFileID))!=RT_OK)
		{
			break;
		}
		
		rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
		if (rv!=RT_OK) 
		{
			break;
		}
		
		if (pbOutData==NULL)
		{
			*pulOutDataLen = fiFileInfo.wFileSpareSize;
			break;
		}
		
		if ((ulOffset+ulSize)>fiFileInfo.wFileSpareSize)
		{
			rv = RT_RANGE_OVERFLOW;
			break;
		}
		
		*pulOutDataLen = 0;
		for( i=0; i < ulSize / TRANSFER_BUFFER_LEN; i++ )
		{
			sprintf( szCommand, "00B0%04X%02X", ulOffset + i*TRANSFER_BUFFER_LEN, TRANSFER_BUFFER_LEN);
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				rv = nSW;
				goto ret;
			}
			HexToAsc( pbOutData+i*TRANSFER_BUFFER_LEN, szReply, TRANSFER_BUFFER_LEN*2 );
			*pulOutDataLen += TRANSFER_BUFFER_LEN;
		}
		
		if ( ulSize % TRANSFER_BUFFER_LEN )
		{
			sprintf( szCommand, "00B0%04X%02X",ulOffset + i * TRANSFER_BUFFER_LEN, ulSize%TRANSFER_BUFFER_LEN );
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				rv = nSW;
				break;
			}
			HexToAsc( pbOutData + i * TRANSFER_BUFFER_LEN, szReply, (ulSize%TRANSFER_BUFFER_LEN) * 2 );
			*pulOutDataLen += (ulSize%TRANSFER_BUFFER_LEN);	
		}
	}while(0);

ret:
	EndTransaction(hMutex);
	return rv;
}

APDU_RT APDU_WriteBinary(IN HDRIVER hToken,IN T_U32 ulOffset,IN T_U32 ulSize,
						IN T_U8 *pbData,IN OUT T_U32 *pulLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	FILE_ATTR fiFileInfo;
	APDU_RT rv = RT_OK;
	T_S8 szBuf[1024] = {0};
	T_U32 i = 0;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulLen==NULL)
	{
		return RT_INVALID_PARAMETER;
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
		if (rv != RT_OK) 
			break;
		
		if (pbData==NULL)
		{
			*pulLen = fiFileInfo.wFileSpareSize;
			break;
		}

		if ( ulSize > *pulLen ) 
		{
			rv = RT_RANGE_OVERFLOW;
			break;
		}

		if ((ulOffset+ulSize)>fiFileInfo.wFileSpareSize)
		{
			rv = RT_OFFSET_RANGE_OVERFLOW;
			break;
		}

		*pulLen = 0;
		for( i=0; i < ulSize / TRANSFER_BUFFER_LEN; i++ )
		{
			AscToHex(szBuf, pbData+i*TRANSFER_BUFFER_LEN, TRANSFER_BUFFER_LEN);
			sprintf( szCommand, "00D6%04X%02X%s", ulOffset + i*TRANSFER_BUFFER_LEN, TRANSFER_BUFFER_LEN ,szBuf);
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				rv = nSW;
				goto ret;
			}
			*pulLen += TRANSFER_BUFFER_LEN;
		}
		
		if ( ulSize % TRANSFER_BUFFER_LEN )
		{
			AscToHex(szBuf, pbData+i*TRANSFER_BUFFER_LEN, ulSize%TRANSFER_BUFFER_LEN );
			sprintf( szCommand, "00D6%04X%02X%s", ulOffset + i*TRANSFER_BUFFER_LEN, ulSize%TRANSFER_BUFFER_LEN ,szBuf);
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				rv = nSW;
				break;
			}
			*pulLen += (ulSize%TRANSFER_BUFFER_LEN);	
		}
	}while(0);

ret:
	EndTransaction(hMutex);
	return rv;
}


APDU_RT APDU_WriteBinaryEx(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U32 ulOffset,
						IN T_U32 ulSize,IN T_U8 *pbData,IN OUT T_U32 *pulLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	
	APDU_RT  rv = RT_OK;
	FILE_ATTR fiFileInfo;
	T_S8 szBuf[1024] = {0};
	T_U32 i = 0;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulLen==NULL)
	{
		return RT_INVALID_PARAMETER;
	}


	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		if(( rv = APDU_SelectFile(hToken,ulFileID))!=RT_OK)
		{
			break;
		}
		
		rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
		if (rv!=RT_OK) 
		{
			break;
		}
		
		if (pbData==NULL)
		{
			*pulLen = fiFileInfo.wFileSpareSize;
			break;
		}
		
		if ( ulSize > *pulLen ) 
		{
			rv = RT_RANGE_OVERFLOW;
			break;
		}
		
		if ((ulOffset+ulSize)>fiFileInfo.wFileSpareSize)
		{
			rv = RT_OFFSET_RANGE_OVERFLOW;
			break;
		}

		*pulLen = 0;
		for( i=0; i < ulSize / TRANSFER_BUFFER_LEN; i++ )
		{
			AscToHex(szBuf, pbData+i*TRANSFER_BUFFER_LEN, TRANSFER_BUFFER_LEN);
			sprintf( szCommand, "00D6%04X%02X%s", ulOffset + i*TRANSFER_BUFFER_LEN, TRANSFER_BUFFER_LEN ,szBuf);
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK ) 
			{
				rv = nSW;//RETURN(nSW);
				goto ret;
			}
			*pulLen += TRANSFER_BUFFER_LEN;
		}
		
		if ( ulSize % TRANSFER_BUFFER_LEN )
		{
			AscToHex(szBuf, pbData+i*TRANSFER_BUFFER_LEN, ulSize%TRANSFER_BUFFER_LEN );
			sprintf( szCommand, "00D6%04X%02X%s", ulOffset + i*TRANSFER_BUFFER_LEN, ulSize%TRANSFER_BUFFER_LEN ,szBuf);
			SendAPDU( hToken, szCommand, szReply, &nSW );
			if( nSW != RT_OK )
			{
				rv = nSW;//RETURN(nSW);
				break;
			}
			*pulLen += (ulSize%TRANSFER_BUFFER_LEN);	
		}
	}while(0);

ret:
	EndTransaction(hMutex);
	return rv;
}


APDU_RT APDU_ReadRecord(IN HDRIVER hToken,IN T_U8 bRecordID,IN T_U8 bCtrlFlags,
					IN T_U32 ulSize,OUT T_U8 *pbOutData,IN OUT T_U32* pulOutDataLen)
{		
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	APDU_RT rv = RT_OK;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulOutDataLen==NULL
		|| ulSize> 0xFF)
	{
		return RT_INVALID_PARAMETER;
	}
	
	if (ulSize>*pulOutDataLen)
		return RT_BUFFER_SMALL;
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		if (pbOutData==NULL)
		{
			FILE_ATTR fiFileInfo;
			rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
			if (rv!=RT_OK)
			{
				break;
			}
			*pulOutDataLen = fiFileInfo.wFileSpareSize & 0xFF;
			break;
		}

		*pulOutDataLen = 0;	
		sprintf( szCommand, "00B2%02X%02X%02X",bRecordID,bCtrlFlags,ulSize);
		SendAPDU( hToken, szCommand, szReply, &nSW );
		if( nSW != RT_OK ) 
		{
			rv = nSW;
			break;
		}
		HexToAsc(pbOutData, szReply, strlen(szReply) );
		*pulOutDataLen = strlen(szReply)/2;	
	}while(0);

	EndTransaction(hMutex);

	return rv;
}

APDU_RT APDU_UpdateRecord(IN HDRIVER hToken,IN T_U8 bRecordID,IN T_U8 bCtrlFlags,
						IN T_U32 ulSize,IN T_U8 *pbData,IN OUT T_U32* pulDataLen)
{	
	T_S8 szBuf[1024] = {0};
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	APDU_RT rv = RT_OK;

	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulDataLen==NULL || ulSize> 0xFF)
	{
		return RT_INVALID_PARAMETER;
	}
	
	if (ulSize>*pulDataLen)
		return RT_BUFFER_SMALL;
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		if (pbData==NULL)
		{
			FILE_ATTR fiFileInfo;
			rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
			if (rv!=RT_OK) break;
			*pulDataLen = fiFileInfo.wFileSpareSize & 0xFF;
			break;
		}

		AscToHex(szBuf,pbData,ulSize);
		sprintf( szCommand, "00DC%02X%02X%02X%s",bRecordID,bCtrlFlags,ulSize,szBuf);
		SendAPDU( hToken, szCommand, szReply, &nSW );
		rv = nSW;

	}while(0);

	
	EndTransaction(hMutex);
	return rv;
}

APDU_RT APDU_AppendRecord(IN HDRIVER hToken,IN T_U8 bCtrlFlags,IN T_U32 ulSize,
						OUT T_U8 *pbData,IN OUT T_U32* pulDataLen)
{	
	T_S8 szBuf[1024] = {0};
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	APDU_RT rv = RT_OK;

	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| pulDataLen==NULL
		|| ulSize> 0xFF)
	{
		return RT_INVALID_PARAMETER;
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		if (pbData==NULL)
		{
			FILE_ATTR fiFileInfo;
			rv = APDU_GetFileInfo(hToken,FILE_CLASS_EF_COMMON,&fiFileInfo);
			if (rv!=RT_OK) break;
			*pulDataLen = fiFileInfo.wFileSpareSize & 0xFF;
			break;
		}

		if (ulSize>*pulDataLen)
		{
			rv = RT_BUFFER_SMALL;
			break;
		}

		AscToHex(szBuf,pbData,ulSize);
		sprintf( szCommand, "00E200%02X%02X%s",bCtrlFlags,ulSize,szBuf);
		SendAPDU( hToken, szCommand, szReply, &nSW );
		rv = nSW;
	}while(0);

	EndTransaction(hMutex);

	return rv;
}

APDU_RT APDU_SelectFile(IN HDRIVER hToken,IN T_U32 ulFileID)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| ulFileID>0xFFFF || ulFileID==0x0000)
	{
		return RT_INVALID_PARAMETER;
	}
		
	sprintf(szCommand,"00A4000002%04X",ulFileID);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_SelectApp(IN HDRIVER hToken,IN T_U32 ulFileID,OUT T_U8 *pbReply,IN OUT T_U32 *pulReplyLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	T_U32 ulDataLen;
	
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| ulFileID>0xFFFF || ulFileID==0x0000)
	{
		return RT_INVALID_PARAMETER;
	}
	
	sprintf(szCommand,"00A4000002%04X",ulFileID);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,szCommand,szReply,&nSW);
		
		if ((nSW&0xFF00)!=0x6100)
		{
			nSW = RT_FAILED;
			break;
		}

		ulDataLen = nSW&0xFF;

		nSW = APDU_GetResponse(hToken,ulDataLen,pbReply,pulReplyLen);
	}while(0);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_GetFileInfo(IN HDRIVER hToken,IN FILE_CLASS fcFileClass,OUT PFILE_ATTR fiFileInfo)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;
	T_S8 szBuf[128] = {0};
	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN || fiFileInfo == NULL) 
		|| !(fcFileClass==FILE_CLASS_EF_PRIVATEKEY ||fcFileClass==FILE_CLASS_EF_COMMON || fcFileClass==FILE_CLASS_EF_CYC_REC\
		|| fcFileClass==FILE_CLASS_EF_FIXED_LEN_REC ||fcFileClass==FILE_CLASS_EF_NO_FIXED_LEN_REC
		|| fcFileClass==FILE_CLASS_DF || fcFileClass==FILE_CLASS_MF ) )
	{
		return RT_INVALID_PARAMETER;
	}

	switch(fcFileClass)
	{
	case FILE_CLASS_DF:
	case FILE_CLASS_MF:
		sprintf(szCommand,"801C00000A");
		break;
	case FILE_CLASS_EF_COMMON:
	case FILE_CLASS_EF_PRIVATEKEY:
	case FILE_CLASS_EF_FIXED_LEN_REC:
	case FILE_CLASS_EF_NO_FIXED_LEN_REC:
	case FILE_CLASS_EF_CYC_REC:	
		sprintf(szCommand,"801C01000A");
	    break;
	default:
	    break;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	if (nSW!=RT_OK) return nSW;

	//wTotalSpareSize
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply ,4);
	(*fiFileInfo).wTotalSpareSize = strtol(szBuf,NULL,16);

	//wFileSpareSize
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+4,4);
	(*fiFileInfo).wFileSpareSize = strtol(szBuf,NULL,16);

	//wFileRemainSpareSize
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+8,4);
	(*fiFileInfo).wFileRemainSpareSize = strtol(szBuf,NULL,16);

	//bFileType
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+12,2);
	(*fiFileInfo).bFileType = (T_U8)strtol(szBuf,NULL,16);

	//bCreateOrReadRight
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+14,2);
	(*fiFileInfo).bCreateOrReadRight = (T_U8)strtol(szBuf,NULL,16);

	//bDeleteOrWriteRight
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+16,2);
	(*fiFileInfo).bDeleteOrWriteRight = (T_U8)strtol(szBuf,NULL,16);

	//bAppOrSafe
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+18,2);
	(*fiFileInfo).bAppOrSafe = (T_U8)strtol(szBuf,NULL,16);

	return nSW;
}

APDU_RT APDU_SetDeviceInfo(IN HDRIVER hToken,IN DEVICE_INFO diDevInfo)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;

	T_S8 szSerialNumber[128] = {0};
	T_S8 szPrefix[128] = {0};
	T_S8 szDisplayContext[128] = {0};
	T_U32 ulDataLen;
	
	if ( hToken==NULL || hToken==PARA_INVALID_HTOKEN ) 
	{
		return RT_INVALID_PARAMETER;
	}

	memset(szCommand, 0, COMMAND_MAX_LEN);

	AscToHex(szSerialNumber,(T_U8*)&diDevInfo.sn[0],
		diDevInfo.sn[0]+1);
	AscToHex(szPrefix,(T_U8*)&diDevInfo.preFix[0],
		diDevInfo.preFix[0]+1);
	AscToHex(szDisplayContext,(T_U8*)&diDevInfo.displayContext[0],
		diDevInfo.displayContext[0]+1);

	ulDataLen = 6 + 6 + 6 + 4 + (diDevInfo.sn[0]+2) + (diDevInfo.preFix[0]+2) + (diDevInfo.displayContext[0]+2);
	
	sprintf(szCommand,"80160000%02X0104%04X%04X0204%08X0304%08X0402%04X05%s06%s07%s",
		ulDataLen,diDevInfo.ulVenderID,diDevInfo.ulProductID,diDevInfo.cfgWord, diDevInfo.ulProduceDate, diDevInfo.timeOut, szSerialNumber, \
		szPrefix, szDisplayContext);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_GetDeviceInfo(IN HDRIVER hToken,OUT DEVICE_INFO *pdiDevInfo)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;

	T_S8 szBuf[128] = {0};
	T_U32 ulOffSet = 0;
	
	if ( hToken==NULL || hToken==PARA_INVALID_HTOKEN 
		|| pdiDevInfo==NULL || pdiDevInfo==PARA_INVALID_HTOKEN) 
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,"801A0000FF",szReply,&nSW);
		if (nSW!=RT_OK) 
		{
			T_U32 ulSWLen = nSW&0xFF00;
			if (ulSWLen != 0x6c00)
			{
				goto ret;
			}
			else
			{
				sprintf(szCommand,"801A0000%02X",nSW&0xFF);
				SendAPDU(hToken,szCommand,szReply,&nSW);
				if (nSW != RT_OK) 
				{
					goto ret;
				}
			}
		}
	}while(0);
	EndTransaction(hMutex);

	
	//VID
	if (memcmp(szReply,"0104",2)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 4;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet ,4);
	(*pdiDevInfo).ulVenderID = strtol(szBuf,NULL,16);
	ulOffSet += 4;	

	//PID
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,4);
	(*pdiDevInfo).ulProductID = strtol(szBuf,NULL,16);
	ulOffSet += 4;	

	//CFGWORD
	if (memcmp(szReply+ulOffSet,"0204",2)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 4;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,8);
	(*pdiDevInfo).cfgWord = strtol(szBuf,NULL,16);
	ulOffSet += 8;

	//ulProduceDate
	if (memcmp(szReply+ulOffSet,"0304",4)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 4;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,8);
	(*pdiDevInfo).ulProduceDate = strtol(szBuf,NULL,16);
	ulOffSet += 8;

	//TIMEOUT
	if (memcmp(szReply+ulOffSet,"0402",4)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 4;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,4);
	(*pdiDevInfo).timeOut = strtol(szBuf,NULL,16);
	ulOffSet += 4;
	
	//sbSerialNumber
	if (memcmp(szReply+ulOffSet,"05",2)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 2;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,2);
	T_S32 ulLen = strtol(szBuf,NULL,16);
	ulOffSet += 2;
	(*pdiDevInfo).sn[0] = (T_U8)ulLen;
	HexToAsc((T_U8*)&((*pdiDevInfo).sn[1]),szReply+ulOffSet,ulLen*2);
	(*pdiDevInfo).sn[ulLen + 1] = '\0';
	ulOffSet += ulLen*2;

	//PREFIX 
	if (memcmp(szReply+ulOffSet,"06",2)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 2;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,2);
	ulLen = strtol(szBuf,NULL,16);
	ulOffSet += 2;
	(*pdiDevInfo).preFix[0] = (T_U8)ulLen;
	HexToAsc((T_U8*)&((*pdiDevInfo).preFix[1]),szReply+ulOffSet,ulLen*2);
	(*pdiDevInfo).preFix[ulLen + 1] = '\0';
	ulOffSet += ulLen*2;

	//TEXT
	if (memcmp(szReply+ulOffSet,"07",2)!=0) return RT_UNKNOW_ERROR;
	ulOffSet += 2;
	memset(szBuf,0,sizeof(szBuf));
	strcpy(szBuf,"0x");
	memcpy(&szBuf[2],szReply+ulOffSet,2);
	ulLen = strtol(szBuf,NULL,16);
	ulOffSet += 2;
	(*pdiDevInfo).displayContext[0] = (T_U8)ulLen;
	HexToAsc((T_U8*)&((*pdiDevInfo).displayContext[1]),szReply+ulOffSet,ulLen*2);
	(*pdiDevInfo).displayContext[ulLen + 1] = '\0';
	ulOffSet += ulLen*2;
	
	return nSW;
ret:
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_RSAGenRsaKeyPair(IN HDRIVER hToken,IN RSA_BITS rbBits,
							IN T_U32 ulPrvKeyID, IN T_U32 ulPubKeyID)
{	
	APDU_RT rv = RT_UNKNOW_ERROR;
	T_U32 ulPriKeyFileSize = 0;
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_S8 szBuf[256] = {0};
	T_S32 len = 0;
	T_U32 ulOffSet = 0;
	T_U16  nSW=0;
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| !(rbBits==RSA_1024_BIT|| rbBits==RSA_2048_BIT)
		|| ulPrvKeyID<=0 || ulPrvKeyID>0xFFFF
		|| ulPubKeyID<=0 || ulPubKeyID>0xFFFF)
	{
		return RT_INVALID_PARAMETER;
	}

	if(rbBits == RSA_1024_BIT)
	{
		sprintf(szCommand,"8046000007010080%04X%04X",ulPubKeyID,ulPrvKeyID);
	}
	else if(rbBits == RSA_2048_BIT)
	{
		sprintf(szCommand,"8046000007010100%04X%04X",ulPubKeyID,ulPrvKeyID);
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	
	return nSW;
}

APDU_RT APDU_SM2Gensm2KeyPair(IN HDRIVER hToken,
							  IN T_U32 ulPrvKeyID, IN T_U32 ulPubKeyID)
{	
	APDU_RT rv = RT_UNKNOW_ERROR;
	T_U32 ulPriKeyFileSize = 0;
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_S8 szBuf[256] = {0};
	T_S32 len = 0;
	T_U32 ulOffSet = 0;
	T_U16  nSW=0;

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulPrvKeyID<=0 || ulPrvKeyID>0xFFFF
		|| ulPubKeyID<=0 || ulPubKeyID>0xFFFF)
	{
		return RT_INVALID_PARAMETER;
	}

	sprintf(szCommand,"8046000007020020%04X%04X",ulPubKeyID,ulPrvKeyID);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_RSAPriKeyOperate(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
						IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szData[REPLY_MAX_LEN]={0};
	T_U8 szReply[REPLY_MAX_LEN]={0};
	T_U32 szReplyLen = sizeof(szReply);
	T_U32 tmpLen = 0;
	T_U16  nSW=0;

	T_S8 szBuf[1024] = {0};
	T_U32 ulBufLen = sizeof(szBuf);
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulFileID<=0 || ulFileID>0xFFFF
		|| pulOutDataLen == NULL )
	{
		return RT_INVALID_PARAMETER;
	}

	if(pbInData == NULL && ulInDataLen == 0)
	{
		sprintf(szCommand,"80C2%04X00",ulFileID);
	}
	else
	{
		if(!(ulInDataLen==0x0100 || ulInDataLen==0x80))
		{
			return RT_INVALID_PARAMETER;
		}
		
		AscToHex(szBuf,pbInData,ulInDataLen);
		if (ulInDataLen==0x0100)
		{
			sprintf(szCommand,"80C2%04X00%04X%s",ulFileID,strlen(szBuf)/2,szBuf);
		}
		else
		{
			sprintf(szCommand,"80C2%04X%02X%s",ulFileID,strlen(szBuf)/2,szBuf);
		}
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,szCommand,szData,&nSW);

		//2048 RSA sign return 0x6100
		memset(pbOutData, 0, *pulOutDataLen);
		if ((nSW&0xFF00) == 0x6100)
		{
			if (nSW == 0x6100)
			{
				nSW = APDU_GetResponse(hToken,0x80,szReply,&szReplyLen);
				memcpy(pbOutData, szReply, szReplyLen);
				tmpLen = szReplyLen;
			}
			
			nSW = APDU_GetResponse(hToken,nSW&0xFF,szReply,&szReplyLen);
			memcpy(pbOutData + tmpLen, szReply, szReplyLen);
			tmpLen += szReplyLen;
		}

		*pulOutDataLen = tmpLen;
	}while(0);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_SM2PriKeyOperate(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
							  IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szData[REPLY_MAX_LEN]={0};
	T_U8 szReply[REPLY_MAX_LEN]={0};
	T_U32 szReplyLen = sizeof(szReply);
	T_U32 tmpLen = 0;
	T_U16  nSW=0;

	T_S8 szBuf[1024] = {0};
	T_U32 ulBufLen = sizeof(szBuf);

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulFileID<=0 || ulFileID>0xFFFF
		|| pulOutDataLen == NULL )
	{
		return RT_INVALID_PARAMETER;
	}

	if(pbInData == NULL && ulInDataLen == 0)
	{
		sprintf(szCommand,"80C8010002%04X",ulFileID);
	}
	else
	{
		/*if(!(ulInDataLen==0x0020))
		{
			return RT_INVALID_PARAMETER;
		}*/
		ulInDataLen += 2;
		if (ulInDataLen < 99 || ulInDataLen > 255)
		{
			return RT_INVALID_PARAMETER;
		}
		AscToHex(szBuf,pbInData,ulInDataLen);
		sprintf(szCommand,"80C4%04X%02X%s",ulFileID,szBuf);
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szData,&nSW);

	memset(pbOutData, 0, *pulOutDataLen);
	if ((nSW&0xFF00) == 0x6100)
	{
		nSW = APDU_GetResponse(hToken,nSW&0xFF,szReply,&szReplyLen);
		memcpy(pbOutData, szReply, szReplyLen);
		tmpLen = szReplyLen;
	}

	*pulOutDataLen = tmpLen;
	
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_SM2PubKeyEncrypt(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
							  IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_U8 szReply[REPLY_MAX_LEN]={0};
	T_S8 szData[REPLY_MAX_LEN]={0};
	T_U32 tmpLen = 0;
	T_U32 szReplyLen = sizeof(szReply);
	T_U16  nSW=0;

	T_S8 szBuf[1024] = {0};
	T_U32 ulBufLen = sizeof(szBuf);

	ulInDataLen += 2;
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulFileID<=0 || ulFileID>0xFFFF
		|| pbInData == NULL || pbOutData == NULL
		|| ulInDataLen < 3 || ulInDataLen > 158
		|| pulOutDataLen == NULL )
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(szBuf,pbInData,ulInDataLen);
	sprintf(szCommand,"80C00100%02X%04X%s",ulInDataLen,ulFileID,szBuf);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szData,&nSW);

	memset(pbOutData, 0, *pulOutDataLen);
	if ((nSW&0xFF00) == 0x6100)
	{
		nSW = APDU_GetResponse(hToken,nSW&0xFF,szReply,&szReplyLen);
		memcpy(pbOutData, szReply, szReplyLen);
		tmpLen = szReplyLen;
	}

	*pulOutDataLen = tmpLen;

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_SM2PubKeyVerify(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInSignData,IN T_U8 *pbInSM3HashData,
							  IN T_U32 ulInDataLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_U8 szReply[REPLY_MAX_LEN]={0};
	T_S8 szData[REPLY_MAX_LEN]={0};
	T_U32 tmpLen = 0;
	T_U32 szReplyLen = sizeof(szReply);
	T_U16  nSW=0;

	T_S8 szBufSign[1024] = {0};
	T_S8 szBufHash[1024] = {0};

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulFileID<=0 || ulFileID>0xFFFF
		|| !(ulInDataLen == 32)
		|| pbInSignData == NULL || pbInSignData == NULL
		|| pbInSM3HashData == NULL || pbInSM3HashData == NULL )
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(szBufSign,pbInSignData,64);
	AscToHex(szBufHash,pbInSM3HashData,32);

	sprintf(szCommand,"80CA010062%04X%s%s",ulFileID,szBufSign,szBufHash);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szData,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_RSAPubKeyOperate(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
						IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_U8 szReply[REPLY_MAX_LEN]={0};
	T_S8 szData[REPLY_MAX_LEN]={0};
	T_U32 tmpLen = 0;
	T_U32 szReplyLen = sizeof(szReply);
	T_U16  nSW=0;
	
	T_S8 szBuf[1024] = {0};
	T_U32 ulBufLen = sizeof(szBuf);
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulFileID<=0 || ulFileID>0xFFFF
		|| !(ulInDataLen == 0x100 || ulInDataLen == 0x80)
		|| pbInData == NULL || pbOutData == NULL
		|| pulOutDataLen == NULL )
	{
		return RT_INVALID_PARAMETER;
	}
	
	if(!(ulInDataLen==0x0100 || ulInDataLen==0x80))
	{
		return RT_INVALID_PARAMETER;
	}
	AscToHex(szBuf,pbInData,ulInDataLen);
	if (ulInDataLen==0x0100)
	{
		sprintf(szCommand,"80C6%04X00%04X%s",ulFileID,strlen(szBuf)/2,szBuf);
	}
	else
	{
		sprintf(szCommand,"80C6%04X%02X%s",ulFileID,strlen(szBuf)/2,szBuf);
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szData,&nSW);

	memset(pbOutData, 0, *pulOutDataLen);
	if ((nSW&0xFF00) == 0x6100)
	{
		if (nSW == 0x6100)
		{
			//2048 RSA sign return 0x6100
			nSW = APDU_GetResponse(hToken,0x80,szReply,&szReplyLen);
			memcpy(pbOutData, szReply, szReplyLen);
			tmpLen = szReplyLen;
		}

		nSW = APDU_GetResponse(hToken,nSW&0xFF,szReply,&szReplyLen);
		memcpy(pbOutData + tmpLen, szReply, szReplyLen);
		tmpLen += szReplyLen;
	}

	*pulOutDataLen = tmpLen;

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_GetResponse(IN HDRIVER hToken,IN T_U32 ulSize,OUT T_U8 *pbOutData,
						IN OUT T_U32 *pulOutDataLen)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16  nSW=0;
	T_S32 szReplyLen = 0;
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| pulOutDataLen == NULL )
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		if (pbOutData==NULL)
		{
			SendAPDU(hToken,"00C00000FF",szReply,&nSW);
			if ((nSW&0xFF00)!=0x6C00) 
				break;
			*pulOutDataLen = nSW&0xFF;
			nSW = RT_OK;
			break;
		}

		if (ulSize==0)
		{
			nSW = RT_INVALID_PARAMETER;
			break;
		}
		

		if (ulSize>*pulOutDataLen)
		{
			nSW = RT_BUFFER_SMALL;
			break;
		}


		sprintf(szCommand,"00C00000%02X",ulSize);
		SendAPDU(hToken,szCommand,szReply,&nSW);
		if (!(nSW == RT_OK || (nSW&0xFF00) == 0x6100))
		{
			break;
		}

		HexToAsc(pbOutData, szReply, strlen(szReply));
		*pulOutDataLen = strlen(szReply) / 2;
	}while(0);

	EndTransaction(hMutex);

	return nSW;
	
}

APDU_RT APDU_SymmetricKeyIDOperate(IN HDRIVER hToken,IN ENCRYPT_TYPE etType,IN T_U32 ulKeyID,
								IN T_U8 *pbInData,IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,
								IN OUT T_U32 *pulOutDataLen)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16  nSW=0;
	
	T_S8 szBuf[1024] = {0};
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| !(etType==ET_ENCRYPT || etType==ET_DECRYPT)
		|| ulKeyID>0xFF
		|| pbInData == NULL || ulInDataLen <= 0 && ulInDataLen > 0xFF
		|| pulOutDataLen == NULL )
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(szBuf,pbInData,ulInDataLen);
	
	switch(etType)
	{
	case ET_ENCRYPT:
		DebugPrint(1,"**ET_ENCRYPT**");
		sprintf(szCommand,"0088%02X%02X%02X%s",0x00,ulKeyID,ulInDataLen,szBuf);
		break;
	case ET_DECRYPT:
		DebugPrint(1,"**ET_DECRYPT**");
		sprintf(szCommand,"0088%02X%02X%02X%s",0x01,ulKeyID,ulInDataLen,szBuf);
		break;
	default:
		return RT_UNKNOW_ERROR;
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		SendAPDU(hToken,szCommand,szReply,&nSW);
		if ((nSW&0xFF00)!=0x6100) break;
		
		nSW = APDU_GetResponse(hToken,nSW&0xFF,pbOutData,pulOutDataLen);
	}while(0);

	EndTransaction(hMutex);

	return nSW;
}

#if 0
APDU_RT APDU_WriteKey(IN HDRIVER hToken, IN T_U32 ulKeyFlags,T_BOOL endFlag, IN T_U32 ulKeyID,
					IN T_U8* pbKey, IN T_U32 ulKeyLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16  nSW=0;
	
	T_S8 szBuf[1024] = {0};
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulKeyID<=0 && ulKeyID>0xFF
		|| pbKey == NULL 
		/*|| !(ulKeyLen==8 || ulKeyLen==16) */)
	{
		return RT_INVALID_PARAMETER;
	}

	if(ulKeyFlags == 0x01)
	{
		ulKeyFlags = (endFlag == TRUE) ? 0x81 : 0x01;
	}

	AscToHex(szBuf,pbKey,ulKeyLen);
	
	sprintf(szCommand,"80D4%02X%02X%02X%s",ulKeyFlags,ulKeyID,ulKeyLen,szBuf);


	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}
#endif
APDU_RT APDU_WriteKey(IN HDRIVER hToken, IN T_U32 ulKeyFlags,T_BOOL endFlag, IN T_U32 ulKeyID,
					IN T_U8* pbKey, IN T_U32 ulKeyLen)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16  nSW=0;
	
	T_S8 szBuf[1024] = {0};
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| ulKeyID<=0 && ulKeyID>0xFF
		|| pbKey == NULL 
		/*|| !(ulKeyLen==8 || ulKeyLen==16) */)
	{
		return RT_INVALID_PARAMETER;
	}


	AscToHex(szBuf,pbKey,ulKeyLen);
	
	sprintf(szCommand,"80D4%02X%02X%02X%s",ulKeyFlags,ulKeyID,ulKeyLen,szBuf);


	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}
APDU_RT APDU_GenRandomNumber(IN HDRIVER hToken,IN T_U32 ulSize,OUT T_U8 *pbOutData,
							IN OUT T_U32 *pulOutDataLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16  nSW=RT_UNKNOW_ERROR;	

	T_U32 ulRandomLen = 0;
	T_U32 i=0;
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| pbOutData == NULL 
		|| pulOutDataLen == NULL)
	{
		return RT_INVALID_PARAMETER;
	}

	if (ulSize>*pulOutDataLen)
		return RT_BUFFER_SMALL;

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		for (i=0;i<ulSize/RANDOM_BLOCK_LEN;i++)
		{
			sprintf(szCommand,"00840000%02X",RANDOM_BLOCK_LEN);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			if (nSW!=RT_OK) 
				goto ret;
			HexToAsc(pbOutData+(i*RANDOM_BLOCK_LEN), szReply,strlen(szReply));
			ulRandomLen += strlen(szReply)/2;
		}
		if (ulSize%RANDOM_BLOCK_LEN)
		{
			sprintf(szCommand,"00840000%02X",ulSize%RANDOM_BLOCK_LEN);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			if (nSW!=RT_OK)
				goto ret;
			HexToAsc(pbOutData+(i*RANDOM_BLOCK_LEN), szReply,strlen(szReply));
			ulRandomLen += strlen(szReply)/2;
		}

		*pulOutDataLen = ulRandomLen;
	}while(0);	

ret:
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_ExternalAuthentication(IN HDRIVER hToken,IN T_U32 ulKeyID,IN T_U8* pbEAKey,
								IN T_U32 ulEAKeyLen)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16  nSW=RT_UNKNOW_ERROR;	

	T_S8 szEAKey[128] = {0};
	T_S8 szBuf[128]={0};
	T_S32 ulBufLen = 16;
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| pbEAKey == NULL || ulEAKeyLen != 16 || ulKeyID > 0xFF)
	{
		return RT_INVALID_PARAMETER;
	}
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	//SendAPDU(hToken,"0084000008",szReply,&nSW);
	
	//AscToHex(szEAKey,pbEAKey,ulEAKeyLen);
	//DES(szReply,szEAKey,TRUE,szBuf,&ulBufLen);

	//sprintf(szCommand,"008200%02X08%s",ulKeyID,szBuf);
	sprintf(szCommand,"008200%02X%02X%s",ulKeyID,ulEAKeyLen,pbEAKey);
	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_InternalAuthentication(IN HDRIVER hToken,IN T_U32 ulKeyID,IN T_U8* pbIAData,
							IN T_U32 ulIADataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen)
{
	return APDU_SymmetricKeyIDOperate(hToken,ET_ENCRYPT,ulKeyID,pbOutData,ulIADataLen,pbOutData,pulOutDataLen);
}

APDU_RT APDU_VerifyTransportKey(IN HDRIVER hToken,IN T_U8* pbKey,IN T_U32 ulKeyLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16 nSW=RT_UNKNOW_ERROR;	
	T_S8 szKey[1024] = {0};
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| pbKey == NULL
		|| ulKeyLen!=8)
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(szKey,pbKey,ulKeyLen);
	
	sprintf(szCommand,"80060000%02X%s",ulKeyLen,szKey);
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);
	
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_SetupTransportKey(IN HDRIVER hToken,IN T_U8* pbKey,IN T_U32 ulKeyLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16 nSW=RT_UNKNOW_ERROR;	
	T_S8 szKey[1024] = {0};
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| pbKey == NULL
		|| ulKeyLen!=8)
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(szKey,pbKey,ulKeyLen);
	
	sprintf(szCommand,"80060001%02X%s",ulKeyLen,szKey);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,szCommand,szReply,&nSW);
	
	EndTransaction(hMutex);

	return nSW;
}

#if 0
APDU_RT APDU_GenDigest(IN HDRIVER hToken,IN DIGEST_TYPE eDigType,
							IN T_U8 *pbInData, IN T_U32 ulInDataLen,
							OUT T_U8 *pbOutData, OUT T_U32 *pulOutDataLen)
#endif
APDU_RT APDU_GenDigest(IN HDRIVER hToken,IN DIGEST_TYPE eDigType,
							IN T_U8 *pbInData, IN T_U32 ulInDataLen)
{	
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_S8 szHex[DIGEST_BLOCK_LEN * 2 + 1] = {0};
	T_U16  nSW=RT_UNKNOW_ERROR;	

	T_U32 ulRandomLen = 0;
	T_U32 ulOffset = 0;
	T_U16 hashType = 0;
	T_U16 type = 0;
	T_U16 typeDataBuf = 0;
	T_BOOL isOneBuf = FALSE;
	T_U32 i=0;
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
#if 0
		|| pbOutData == NULL || pulOutDataLen == NULL
#endif
		|| pbInData == NULL)
	{
		return RT_INVALID_PARAMETER;
	}

	if(eDigType >= DIGEST_END)
	{
		return RT_INVALID_PARAMETER;
	}

	if(eDigType == DIGEST_SHA1_TYPE)
	{
		hashType = 1;
	}
	else if(eDigType == DIGEST_SHA256_TYPE)
	{
		hashType = 2;
	}
	else if (eDigType == DIGEST_SM3_TYPE)
	{
		hashType = 3;
	}

	if((ulInDataLen / DIGEST_BLOCK_LEN == 1 && ulInDataLen % DIGEST_BLOCK_LEN == 0)
		|| ulInDataLen / DIGEST_BLOCK_LEN == 0)
	{
		isOneBuf = TRUE;
	}

	type = hashType;

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		for(i = 0; i < ulInDataLen/DIGEST_BLOCK_LEN; i++)
		{
			if(isOneBuf == TRUE)
			{
				typeDataBuf = 0;//only one block
			}
			else if(i == 0)
			{
				typeDataBuf = 1;//the first block
			}
			else if(ulInDataLen%DIGEST_BLOCK_LEN == 0 && i == (ulInDataLen/DIGEST_BLOCK_LEN - 1))
			{
				type = (hashType << 4) | hashType;
				typeDataBuf = 3;//the last block
			}
			else
			{
				type = (hashType << 4) | hashType;
				typeDataBuf = 2;//middle block
			}
			memset(szHex, 0, DIGEST_BLOCK_LEN * 2 + 1);
			AscToHex(szHex, pbInData + ulOffset, DIGEST_BLOCK_LEN);
			sprintf(szCommand,"80CC%02X%02X%02X%s", type, typeDataBuf, DIGEST_BLOCK_LEN, szHex);
			
			SendAPDU(hToken,szCommand,szReply,&nSW);
			if(isOneBuf == FALSE)
			{
				if (nSW!=RT_OK) goto ret;
			}
			ulOffset += DIGEST_BLOCK_LEN;
		}
		if (ulInDataLen % DIGEST_BLOCK_LEN)
		{
			type = (hashType << 4) | hashType;
			typeDataBuf = (isOneBuf == TRUE) ? 0x00 : 0x03;//must be the last block
			memset(szHex, 0, DIGEST_BLOCK_LEN * 2 + 1);
			AscToHex(szHex, pbInData + ulOffset, ulInDataLen % DIGEST_BLOCK_LEN);
			sprintf(szCommand,"80CC%02X%02X%02X%s", type, typeDataBuf, 
						ulInDataLen % DIGEST_BLOCK_LEN, szHex);
			SendAPDU(hToken,szCommand,szReply,&nSW);
			//if ((nSW&0xFF00)!=0x6100) return nSW;
		}
	}while(0);


ret:
	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_FinishInit(IN HDRIVER hToken)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16 nSW=RT_UNKNOW_ERROR;	
	
	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN)
	{
		return RT_INVALID_PARAMETER;
	}
	
	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	sprintf(szCommand,"8008000000");
	SendAPDU(hToken,szCommand,szReply,&nSW);
	
	EndTransaction(hMutex);

	return nSW;
}
APDU_RT APDU_GetLeftUserPinTryCount(IN HDRIVER hToken)
{	
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16 nSW=RT_UNKNOW_ERROR;	

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN)
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	sprintf(szCommand,"0420000000");
	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}
APDU_RT APDU_ResetBootloader(IN HDRIVER hToken, IN T_U8* pbTransPIN, IN T_U32 ulTransPINLen)
{
	APDU_RT  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};

	APDU_RT rv = RT_OK;
	T_S8 str[256]={0};
	
	if(ulTransPINLen > MAX_PIN_LENGTH || ulTransPINLen < MIN_PIN_LENGTH || NULL == pbTransPIN)
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(str, pbTransPIN, ulTransPINLen);
	

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		rv = APDU_RemandEncyptPIN(hToken,str);
		if (rv != RT_OK) 
		{
			break;
		}	

		str[16] = 0;
		sprintf(szCommand,"8000000108%s",str);
		SendAPDU(hToken,szCommand,szReply,&nSW);
		rv = nSW;
	}while(0);

	EndTransaction(hMutex);

	return rv;

}

//reset smartcard
APDU_RT APDU_ResetSmartCard(IN HDRIVER hToken, OUT T_U8 *pbOutData, IN OUT T_U32 *pulOutDataLen)
{	
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_S8 szTemp[16] = {0};
	T_U16  nSW=RT_UNKNOW_ERROR;	
	T_S32 len = 0;

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN
		|| pbOutData == NULL 
		|| pulOutDataLen == NULL)
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,"8000000000",szReply,&nSW);

	EndTransaction(hMutex);

	sprintf(szTemp, "%04X", nSW);
	strcat(szReply, szTemp);
	len = strlen(szReply);
	if (len > *pulOutDataLen)
	{
		return RT_BUFFER_SMALL;
	}
	HexToAsc(pbOutData, szReply, len);

	*pulOutDataLen = len / 2;

	return nSW;
}

APDU_RT APDU_ChangePrivateKeyState(IN HDRIVER hToken,IN T_U32 ulFileID)
{
	T_S8 szCommand[COMMAND_MAX_LEN]={0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};
	T_U16	nSW = 0;

	if ( (hToken==NULL || hToken==PARA_INVALID_HTOKEN) 
		|| ulFileID>0xFFFF || ulFileID==0x0000)
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	sprintf(szCommand,"8046%04X00",ulFileID);
	SendAPDU(hToken,szCommand,szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;

}

//production test
#if 1//def PRODUCTION_TEST

APDU_RT APDU_RestoreWhiteCard(IN HDRIVER hToken)
{	
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16 nSW=RT_UNKNOW_ERROR;	

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN)
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,"803884200100",szReply,&nSW);

	EndTransaction(hMutex);


	return nSW;
}

APDU_RT APDU_DeleteMF(IN HDRIVER hToken)
{	
	T_S8 szReply[REPLY_MAX_LEN]={0};
	T_U16 nSW=RT_UNKNOW_ERROR;	

	if(hToken==NULL || hToken==PARA_INVALID_HTOKEN)
	{
		return RT_INVALID_PARAMETER;
	}

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	SendAPDU(hToken,"800E000000",szReply,&nSW);

	EndTransaction(hMutex);

	return nSW;
}

APDU_RT APDU_GetSuperRight(IN HDRIVER hToken, IN T_U8* pbSuperPIN, IN T_U32 ulSuperPINLen)
{
	APDU_RT  nSW = 0;
	T_S8 szCommand[COMMAND_MAX_LEN] = {0};
	T_S8 szReply[REPLY_MAX_LEN] = {0};

	APDU_RT rv = RT_OK;
	T_S8 str[256]={0};

	if(ulSuperPINLen > MAX_PIN_LENGTH || ulSuperPINLen < MIN_PIN_LENGTH || NULL == pbSuperPIN)
	{
		return RT_INVALID_PARAMETER;
	}

	AscToHex(str, pbSuperPIN, ulSuperPINLen);

	HANDLE hMutex = APDU_BeginTransaction(hToken); if (!hMutex) return RT_UNKNOW_ERROR;

	do
	{
		rv = APDU_RemandEncyptPIN(hToken,str);
		if (rv != RT_OK) 
		{
			break;
		}	

		str[16] = 0;
		sprintf(szCommand,"8034000008%s",str);
		SendAPDU(hToken,szCommand,szReply,&nSW);
		rv = nSW;
	}while(0);
	
	EndTransaction(hMutex);

	return rv;

}
#endif

#if 1//defCCB_CAMERADEV_API  
 int GetRandom(char* command, int clen,char **rdata, int *rlen)
 {
	 HDRIVER hToken;
	 *rlen = command[2];
	 if(clen == 0)
	 {
		 return 0x6a86;
	 }
	 return (APDU_GenRandomNumber(hToken,command[2],(T_U8*)*rdata,(T_U32 *)rlen));
 }

 int WriteKey(char* command, int clen,char **rdata, int *rlen)
 {
	  HDRIVER hToken;
	  T_U32 ulKeyFlags = command[0];
	  T_U32 ulKeyID  = command[1];
	  T_U32 ulKeyLen = command[2];
	  T_U8 pbKey[256];//ID+KEY;
	  T_S8 szMac[4];
	  T_S32 nMacLen = sizeof(szMac);
	  if(clen == 0)
	 {
		 return 0x6a86;
	 }
	  memcpy(pbKey,&command[3],ulKeyLen);
	  memcpy(*rdata,pbKey,0x20);
	  *rlen = 0x20;
	  //PinToMac_SM4((unsigned char *)command,(ulKeyLen+3),szMac,&nMacLen);
	  //memcpy(&pbKey[ulKeyLen+3],szMac,ulKeyLen);
	  //ulKeyLen+=4;
	  return (APDU_WriteKey(hToken, ulKeyFlags,0,ulKeyID,pbKey,ulKeyLen));
 }

 int InternalAuthenticate(char* command, int clen,char **rdata, int *rlen)
 {
	 HDRIVER hToken;
	 T_U32 ulKeyID = command[1];
	 T_U32 ulIADataLen = command[2];
	 T_U8 pbIAData[256];
	 if(clen == 0)
	 {
		 return 0x6a86;
	 }
	 memcpy(pbIAData,&command[3],ulIADataLen);
	
	 return ( APDU_InternalAuthentication(hToken,ulKeyID,pbIAData,ulIADataLen,(T_U8*)*rdata,(T_U32*)rlen));
 }

 int ExternalAuthenticate(char* command, int clen,char **rdata, int *rlen)
 {
	 HDRIVER hToken;
	 T_U16 SW;
	 T_U32 ulKeyID = command[1];
	 T_U32 ulEAKeyLen = command[2];
	 T_U8 pbEAKey[256];
	 memcpy(pbEAKey,&command[3],ulEAKeyLen);
	   if(clen == 0)
	 {
		 return 0x6a86;
	 }
	   SW = APDU_ExternalAuthentication(hToken,ulKeyID, pbEAKey,ulEAKeyLen);
	  *rlen = 2;
     (*rdata)[0] = SW&0xff;
     (*rdata)[1] = (SW>>8)&0xff;
	 return SW;
 }

 int ImageSignature(char* command, int clen,char *rdata, int *rlen)
 {
	  HDRIVER hToken;
	  T_U32 ulFileID = (command[1]<<8)|command[0];
	  T_U32 ulInDataLen =command[2];
	  T_U8 pbInData[512];
	  if(clen == 0)
	 {
		 return 0x6a86;
	 }
	  memcpy(pbInData,&command[3],ulInDataLen);
	  if(command[0] == 5)
	  {
		  //todo
		  return 1;
	  }
	 // else if(command[1] == 6)
	  //{
		  return (APDU_RSAPubKeyOperate(hToken,ulFileID,pbInData,ulInDataLen,(T_U8 *)*rdata,(T_U32 *)rlen));
	 // }
	  //else if(command[1] == 7)
	  //{
	      //return (APDU_SM2PubKeyEncrypt(hToken,ulFileID,pbInData,ulInDataLen,(T_U8 *)rdata,(T_U32 *)rlen));
	 // }
	 // else
	  //{
		//  return 0xff;
	  //}

	 //
 }

 int ImageEncrypt(char* command, int clen,char *rdata, int *rlen)
 {
	 HDRIVER hToken;
	 T_U32 ulKeyID = (command[1]<<8)|command[0];
	 T_U32 ulIADataLen = command[1];
	 T_U8 pbIAData[256];
	  if(clen == 0)
	 {
		 return 0x6a86;
	 }
	 memcpy(pbIAData,&command[2],ulIADataLen);
	 
	 return (APDU_SM2PriKeyOperate(hToken,ulKeyID,pbIAData,ulIADataLen,(T_U8 *)*rdata,(T_U32 *)rlen));
 }





#endif



