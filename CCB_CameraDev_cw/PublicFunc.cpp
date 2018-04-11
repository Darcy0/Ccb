// Util.cpp: implementation of the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "stdio.h"
#include "io.h"
#include "stdarg.h"
#include "ETUtil.h"
#include "H_Des.h"
#include "Sm4.h"
#define MAX_DBG_MSG_LEN (6 * 1024)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void DebugPrint(int nTabNum,char *format, ...)
{
	SYSTEMTIME systime;
	if(_access("C:\\ET_Log",0)==0)
	{
		FILE *fp;
		va_list args;
		
		if((fp =fopen("C:\\ET_Log\\apdu.log", "a")) !=NULL)
		{		
			va_start(args, format);
			GetLocalTime(&systime);//GetSystemTime得到的是格林威治标准时间
			fprintf(fp,"[%d %d-%d %d:%d:%d] [pid=%ld,tid=%ld] ",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond,GetCurrentProcessId(),GetCurrentThreadId());
			vfprintf(fp, format, args);
			fprintf(fp,"\n");
			fclose(fp);
			va_end(args);
		}
	}
#ifdef _DEBUG	
	{	
		char pbuffer[MAX_DBG_MSG_LEN];
		va_list ap;
		va_start(ap, format);
		
        _vsnprintf(pbuffer, sizeof(pbuffer), format, ap); 
		for(int i=0;i<nTabNum;i++)
			OutputDebugString("\t");
        OutputDebugString(pbuffer);    // 输出到 VC++ 的 debug 窗口
		OutputDebugString("\n");
		va_end(ap);
	}
#endif
};

int HexToAsc(unsigned char *pDst, char *pSrc, int nSrcLen)
{
	for(int i=0; i<nSrcLen; i+=2)
	{
		// 输出高4位
		if(*pSrc>='0' && *pSrc<='9')
		{
			*pDst = (*pSrc - '0') << 4;
		}
		else if(*pSrc>='A' && *pSrc<='F')
		{
			*pDst = (*pSrc - 'A' + 10) << 4;
		}
		else
		{
			*pDst = (*pSrc - 'a' + 10) << 4;
		}
		
		pSrc++;
		
		// 输出低4位
		if(*pSrc>='0' && *pSrc<='9')
		{
			*pDst |= *pSrc - '0';
		}
		else if(*pSrc>='A' && *pSrc<='F')
		{
			*pDst |= *pSrc - 'A' + 10;
		}
		else
		{
			*pDst |= *pSrc - 'a' + 10;
		}
		
		pSrc++;
		pDst++;
	}
	// 返回目标数据长度
	return nSrcLen / 2;
}

int AscToHex(char *pDst,unsigned char *pSrc,int SrcLen)
{	
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf的字符查找表
	
	for(int i=0; i<SrcLen; i++)
	{
		*pDst++ = tab[*((BYTE*)pSrc) >> 4];		// 输出低4位
		*pDst++ = tab[*((BYTE*)pSrc) & 0x0f];	// 输出高4位
		pSrc++;
	}
	
	// 输出字符串加个结束符
	if(pDst)
		*pDst = '\0';
	
	// 返回目标字符串长度
	return SrcLen * 2;
}
bool ExtractFilePath(char * str,char * result)
{
	if( strcmp(str,"") != 0 )
	{
		char * pos=strrchr(str,'\\');
		if( pos>0 )
		{
			memcpy(result,str,pos-str+1);
			return true;
		}
	}

	strcpy(result,".\\");
	return true;
}
bool ExtractFileName(char * str,char * result)
{
	char * pos=strrchr(str,'\\');
	memcpy(result,pos+1,strlen(pos));
	return true;  
}
int TrimSpace(char* pInData,int InDataLen,char* pOutData,int *OutDataLen)
{
	if(NULL == pInData || NULL == pOutData)
		return 0;
	if(InDataLen<=0)
		return 0;
	char* tmpCommand = new char[InDataLen+1];
	memset(tmpCommand,0,InDataLen+1);
	int i = 0;
	
	//去掉空格
   	while (*pInData)
	{
		if (*pInData == ' ')
		{
			pInData++;
			continue;
		}
		tmpCommand[i] = *pInData++;
		i++;
	}
	
	if(*OutDataLen < i)
		goto end;
	*OutDataLen = i;
	strcpy(pOutData,tmpCommand);
	
end:
	delete[] tmpCommand;
	// 返回目标字符串长度
	return i;
}

char* MAC(char * IN szInitValue, char * IN szData, char * IN szKey,char* szResult, int* pulOutLen)
{
	return Mac(szInitValue,szData,szKey,szResult,pulOutLen);
}

unsigned long PinToMac(unsigned char* pszPin,unsigned long ulPinLen,char *pszMac, int* pulMacLen)
{
	char* lpMac = NULL;
	char szMac[512] = {0};
	char szPin[1024] = {0};
	AscToHex(szPin,pszPin,ulPinLen);
	if(ulPinLen == 0)
	{
		*pulMacLen = 0;
		return false;
	}
	lpMac = MAC("0000000000000000",szPin,"0000000000000000",pszMac,pulMacLen);
	//memcpy(psMac,lpMac,8);
	if (pulMacLen) 
		*pulMacLen = 16;
	return true;
}

bool MAC_SM4(char * IN psInitValue, char * IN psData, char * IN psKey,char* pResult, int* psOutLen);
unsigned long PinToMac_SM4(unsigned char* pszPin,unsigned long ulPinLen,char *pszMac, int* pulMacLen)
{
	char* lpMac = NULL;
	char szMac[512] = {0};
	char szPin[1024] = {0};
	AscToHex(szPin,pszPin,ulPinLen);
	if(ulPinLen == 0)
	{
		*pulMacLen = 0;
		return false;
	}
	MAC_SM4("00000000000000000000000000000000",szPin,"00000000000000000000000000000000",pszMac,pulMacLen);
	//memcpy(psMac,lpMac,8);
	if (pulMacLen) 
		*pulMacLen = 16;
	return true;
}


char* DES(char * IN pszData, char * IN pszKey, bool IN bEncrypt,char * pszOutResult, int* pulOutLen)
{
	return Des( pszData, pszKey, bEncrypt,pszOutResult,(int *)pulOutLen);
}

bool MAC_SM4(char * IN psInitValue, char * IN psData, char * IN psKey,char* pResult, int* psOutLen)
{
	int iInitValLen= 0, iMsgLen= 0, iKeyLen= 0, i, j;
	unsigned char bXOR[16], bMsg[1024+ 16], bKey[16], bMAC[4* 2+ 1];
	char *pMAC= NULL;
	sm4_context ctx;

	try
	{
		// Parameter Checking
		iInitValLen= strlen(psInitValue);
		iKeyLen= strlen(psKey);
		iMsgLen= strlen(psData);

		if(!iInitValLen || !iMsgLen || !iKeyLen) return false;

		// Initial
		memset(bXOR, 0, 16);
		if(!HexToAsc(bXOR, (char *)psInitValue, iInitValLen)) return false;

		memset(bMsg, 0, 1024 + 16);
		if(!HexToAsc(bMsg, (char *)psData, iMsgLen)) return false;

		memset(bKey, 0, 16);
		if(!HexToAsc(bKey, (char *)psKey, iKeyLen)) return false;

		// Filling
		iMsgLen= iMsgLen/ 2; iKeyLen= iKeyLen/ 2;
		bMsg[iMsgLen++]= 0x80;

		if(iMsgLen% 16)
		{		
			for(; iMsgLen% 16 && iMsgLen< 1024 + 16; ) bMsg[iMsgLen++]= 0x00;
		}

		// Calculate
		for(i= 0; i< iMsgLen/ 16; i++)
		{
			for(j= 0; j< 16; j++) bXOR[j]= bXOR[j]^ bMsg[i * 16 + j];

			sm4_setkey_enc(&ctx,bKey);
			sm4_crypt_ecb(&ctx,0,16,bXOR,bXOR);
		}

		// Result,只需要四个字节MAC
		memset(bMAC, 0, 4*2 + 1);
		if(!AscToHex((char *)bMAC, bXOR, 4)) return false;
		pMAC= (char *)bMAC;
		if(NULL != pResult)
		{
			if(*psOutLen >= (int)strlen((char *)bMAC))
			{
				strcpy(pResult,(char *)bMAC);
				*psOutLen = (int)strlen((char *)bMAC);
			}
			else
				*psOutLen = (int)strlen((char *)bMAC);
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}



bool SM4_Verify(char * IN psData, char * IN psKey, bool IN bEncrypt,char * psOutResult, int* psOutLen, unsigned char FillChar)
{
	int iMsgLen= 0, iKeyLen= 0, i;
	unsigned char bMsg[1024+ 8], bKey[16], bSM4[(1024+ 16)* 2+ 1];
	char *psResult= NULL;
	sm4_context ctx;

	try
	{
		// Parameter Checking
		iMsgLen= strlen(psData);
		iKeyLen= strlen(psKey);

		if(!iMsgLen || !iKeyLen) return false;

		// Initial
		memset(bMsg, 0, sizeof(bMsg));
		if(!HexToAsc(bMsg, (char *)psData, iMsgLen)) return false;

		memset(bKey, 0, sizeof(bKey));
		if(!HexToAsc(bKey, (char *)psKey, iKeyLen)) return false; 

		// Filling
		iMsgLen= iMsgLen/ 2; iKeyLen= iKeyLen/ 2;
		if(iMsgLen% 16)
		{
			//验证密码不能填80
			//bMsg[iMsgLen++]= 0x80;
			for(; iMsgLen% 16 && iMsgLen< 1024+ 16; ) bMsg[iMsgLen++]= FillChar;
		}

		// Calculate
		for(i= 0; i< iMsgLen/ 16; i++)
		{
			if (bEncrypt)
				sm4_setkey_enc(&ctx,bKey);
			else
				sm4_setkey_dec(&ctx,bKey);

			sm4_crypt_ecb(&ctx,0,16,&bMsg[i* 16],&bMsg[i* 16]);
		}

		// Result
		memset(bSM4, 0, sizeof(bSM4));
		if(!AscToHex((char *)bSM4, bMsg, iMsgLen)) return false;
		psResult= (char *)bSM4;
		int len = strlen((char *)bSM4);
		if(NULL != psOutResult)
		{
			if(*psOutLen >= len)
			{
				strcpy(psOutResult,(char *)bSM4);
				*psOutLen = len;
			}
			else
			{
				*psOutLen = len;
			}
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}


bool SM4(char * IN psData, char * IN psKey, bool IN bEncrypt,char * psOutResult, int* psOutLen)
{
	int iMsgLen= 0, iKeyLen= 0, i;
	unsigned char bMsg[1024+ 8], bKey[16], bSM4[(1024+ 16)* 2+ 1];
	char *psResult= NULL;
	sm4_context ctx;

	try
	{
		// Parameter Checking
		iMsgLen= strlen(psData);
		iKeyLen= strlen(psKey);

		if(!iMsgLen || !iKeyLen) return false;

		// Initial
		memset(bMsg, 0, sizeof(bMsg));
		if(!HexToAsc(bMsg, (char *)psData, iMsgLen)) return false;

		memset(bKey, 0, sizeof(bKey));
		if(!HexToAsc(bKey, (char *)psKey, iKeyLen)) return false; 

		// Filling
		iMsgLen= iMsgLen/ 2; iKeyLen= iKeyLen/ 2;
		if(iMsgLen% 16)
		{
			bMsg[iMsgLen++]= 0x80;
			for(; iMsgLen% 16 && iMsgLen< 1024+ 16; ) bMsg[iMsgLen++]= 0x00;
		}

		// Calculate
		for(i= 0; i< iMsgLen/ 16; i++)
		{
			if (bEncrypt)
				sm4_setkey_enc(&ctx,bKey);
			else
				sm4_setkey_dec(&ctx,bKey);

			sm4_crypt_ecb(&ctx,0,16,&bMsg[i* 16],&bMsg[i* 16]);
		}

		// Result
		memset(bSM4, 0, sizeof(bSM4));
		if(!AscToHex((char *)bSM4, bMsg, iMsgLen)) return false;
		psResult= (char *)bSM4;
		int len = strlen((char *)bSM4);
		if(NULL != psOutResult)
		{
			if(*psOutLen >= len)
			{
				strcpy(psOutResult,(char *)bSM4);
				*psOutLen = len;
			}
			else
			{
				*psOutLen = len;
			}
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}



void WriteLog(int tab,char *fmt,...)
{
	if(_access("C:\\MiddleWareLog",0)==0)
	{
		FILE *fp;
		va_list args;
		tab++;
		
		if((fp =fopen("c:\\MiddleWareLog\\log.txt", "a")) !=NULL)
		{		
			va_start(args,fmt);
			fprintf(fp,"[pid=%ld,tid=%ld]  ",GetCurrentProcessId(),GetCurrentThreadId());
			for(int i=0;i<tab;i++)
				fprintf(fp,"\t");
			vfprintf(fp, fmt, args);
			fprintf(fp, "\n");
			fclose(fp);
			
			va_end(args);
		}
	}

#ifdef _DEGUG_
	char pbuffer[1024*10] = {0};
	va_list ap;
	va_start(ap, fmt);

	for(int i=0;i<tab;i++)
		OutputDebugString("\t");	
	_vsnprintf(pbuffer, sizeof(pbuffer), fmt, ap);    
	OutputDebugString(pbuffer);    // 输出到 VC++ 的 debug 窗口
	OutputDebugString("\n");
	
	va_end(ap);
#endif
}

void WriteLogHex(char* DataName,unsigned char* Data,int nLen)
{
	char szBuf[1024*10] = {0};
	AscToHex(szBuf,Data,nLen);
	WriteLog(2,"{%s}0x%x:%s",DataName,strlen(szBuf)/2,szBuf);
}


BOOL ReadCurRegString(char* szSubKey, char* szValueName, char* szValue)
{

	HKEY m_hKey;
	char szPath[512] = { 0 };
	DWORD dwtype = REG_SZ;
	DWORD dwLen = 256; 
	LONG nFlag = RegOpenKeyExA(HKEY_CURRENT_USER,  szSubKey, 0 , KEY_READ, &m_hKey);

	if (nFlag == ERROR_SUCCESS)
	{
		nFlag = RegQueryValueExA (m_hKey, szValueName,NULL, &dwtype, (LPBYTE)szValue, &dwLen);
		if(nFlag != ERROR_SUCCESS)
		{
			RegCloseKey(m_hKey);
			return FALSE;
		}
		RegCloseKey(m_hKey);
		return TRUE;
	}
	return FALSE;

}

BOOL WriteCurRegString(char* szSubKey, char *szValuName, char* szValue)
{

	if (!szSubKey || !szValue)
	{
		return FALSE;
	}

	// 根键、子键名称、和到子键的句柄
	HKEY hRoot = HKEY_CURRENT_USER;
	HKEY hKey;// 打开指定子键
	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;
	// 如果不存在不创建
	LONG lRet =  RegCreateKeyExA(
						hRoot,
						szSubKey,
						0,
						NULL,
						REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
						NULL,
						&hKey,
						&dwDisposition
						);
	if(lRet != ERROR_SUCCESS)
		return FALSE;

	// 得到当前执行文件的文件名（包含路径）
	lRet = RegSetValueExA(
					hKey,
					szValuName,
					0,
					REG_SZ,
					(BYTE*)szValue,
					strlen(szValue));
	if(lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	// 关闭子键句柄
	RegCloseKey(hKey);
	return TRUE;
}