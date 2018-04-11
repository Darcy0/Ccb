// Util.h: interface for the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLICFUNCL_H__C642814C_961E_4854_9DCF_026B4222FF74__INCLUDED_)
#define AFX_PUBLICFUNC_H__C642814C_961E_4854_9DCF_026B4222FF74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MUTEX_WAIT_TIME (40000)
#define TAB_LEVEL_MAIN		0x0
#define TAB_LEVEL_DEVMGR	0x1
#define TAB_LEVEL_APDU	0x1
#define TAB_LEVEL_CCID		0x2
void DebugPrint(int nTabNum,char *format, ...);//打印日志
int AscToHex(char *Dest,unsigned char *Src,int SrcLen);
int HexToAsc(unsigned char *Dest,char *Src,int SrcLen);
int TrimSpace(char* pInData,int InDataLen,char* pOutData,int *OutDataLen);
char* MAC(char* IN szInitValue, char* IN szData, char* IN szKey,char* szResult, int* pulOutLen);
unsigned long PinToMac(unsigned char* pszPin,unsigned long ulPinLen,char *pszMac,int* pulMacLen);
unsigned long PinToMac_SM4(unsigned char* pszPin,unsigned long ulPinLen,char *pszMac,int* pulMacLen);
char* DES(char* IN pszData, char* IN pszKey, bool IN bEncrypt,char* pszOutResult,int* pulOutLen);
void WriteLog(int tab,char *fmt,...);
void WriteLogHex(char* DataName,unsigned char* Data,int nLen);
bool ExtractFilePath(char * str,char * result);
bool ExtractFileName(char * str,char * result);

bool MAC_SM4(char* IN szInitValue, char* IN szData, char* IN szKey,char* szResult, int* pulOutLen);
bool SM4(char* IN pszData, char* IN pszKey, bool IN bEncrypt,char* pszOutResult,int* pulOutLen);
bool SM4_Verify(char * IN psData, char * IN psKey, bool IN bEncrypt,char * psOutResult, int* psOutLen, unsigned char FillChar);

BOOL ReadCurRegString(char* szSubKey, char* szValueName, char* szValue);		//读注册表信息
BOOL WriteCurRegString(char* szSubKey, char *szValuName, char* szValue);		//写注册表信息


#endif // !defined(AFX_UTIL_H__C642814C_961E_4854_9DCF_026B4222FF74__INCLUDED_)
