#include <windows.h>
#include "stdio.h"
#include "ETUtil.h"
#include "rc4.h"
HANDLE LogMutex= CreateMutex(NULL,FALSE,"ET_LogMutex");
void CUtil::WriteLog(char *pFileName,char * pMsg)
{
	SYSTEMTIME systime;
	FILE       *fp;
	if(pMsg==NULL)
	{
		return;
	}
	GetLocalTime(&systime);//GetSystemTime得到的是格林威治标准时间
	WaitForSingleObject(LogMutex,INFINITE);
	if((fp =fopen(pFileName, "a")) !=NULL)
		{		
			fprintf(fp,"[%d %d-%d %d:%d:%d %d] [pid=0x%x,tid=0x%x] %s\n",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds,GetCurrentProcessId(),GetCurrentThreadId(),pMsg);
			fclose(fp);
		}
    ReleaseMutex(LogMutex);
}
bool CUtil::WriteFormatLog(char *FileName,const char* pFormat, ...)
{
	char *pMemory=new char[MaxMallocMem+128];
	if(pMemory==NULL)
	{
		WriteLog(FileName,"WriteFormatLog() malloc memory fail");
		return false;
	}
	va_list VaList;
	memset(pMemory, 0, MaxMallocMem+128);
	va_start(VaList, pFormat);
	vsnprintf(pMemory, MaxMallocMem+128-1, pFormat, VaList); 
	WriteLog(FileName,pMemory);
	va_end(VaList);
	delete pMemory;
	pMemory =NULL;
	return true;
}
bool CUtil::WriteHexLog(char *FileName,char *pVarName,char *pBuffer,unsigned int Len)
{
	if(pVarName==NULL||pBuffer==NULL)
	{  
		WriteLog(FileName,"WriteHexLog() NULL parameter");
		return false;
	}
	char *pMemory=new char[MaxMallocMem+16];
	if(pMemory==NULL)
	{
		WriteLog(FileName,"WriteHexLog() malloc memory fail");
		return false;
	}
	unsigned int i;
	unsigned int Groups=Len/MaxMallocMem/2;
	for(i=0;i<Groups;i++)
	{
		memset(pMemory,0,MaxMallocMem+16);
		AscToHex(pMemory,(unsigned char *)pBuffer+MaxMallocMem/2*i,MaxMallocMem/2);
		WriteFormatLog(FileName,"%s[%d](%d) = %s ",pVarName,MaxMallocMem/2,i,pMemory);
	}
	memset(pMemory,0,MaxMallocMem+16);
	AscToHex(pMemory,(unsigned char *)pBuffer+MaxMallocMem/2*i,Len%(MaxMallocMem/2));
	if(i==0)
	{
		WriteFormatLog(FileName,"%s[%d] = %s ",pVarName,(Len%(MaxMallocMem/2)),pMemory);
	}
	else
	{
        WriteFormatLog(FileName,"%s[%d](%d) = %s ",pVarName,(Len%(MaxMallocMem/2)),i,pMemory);
	}
	delete pMemory;
	pMemory =NULL;
	return true;
}

bool CUtil::ExtractFilePath(char * str,char * result)
{
	if( strcmp(str,"") != 0 )
	{
		char * pos=strrchr(str,'\\');
		if( pos>0 )
		{
			memcpy(result,str,pos-str+1);
			return TRUE;
		}
	}

	strcpy(result,".\\");
	return TRUE;
}
int CUtil::SplitString(CETString &strIn, CETStringArray& strAryRe, LPCTSTR str)
{
	if (str ==""||strIn=="")
	{
		strAryRe.RemoveAll();
		return 0;
	}
	strAryRe.RemoveAll();
	int nStart = 0;
	int nEnd = 0;
	while ( (nEnd = strIn.find(str, nStart)) != -1 )
	{
		strAryRe.Add((char *)strIn.substr(nStart, nEnd-nStart).c_str());
		nStart = nEnd+strlen(str);
	}
	strAryRe.Add((char *)strIn.substr(nStart).c_str());
	return strAryRe.GetSize();
}
bool CUtil::ExtractFileName(char * str,char * result)
{
	char * pos=strrchr(str,'\\');
	memcpy(result,pos+1,strlen(pos));
	return TRUE;  
}
int CUtil::HexToAsc(unsigned char *pDst, char *pSrc, int nSrcLen)
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

int CUtil::AscToHex(char *pDst,unsigned char *pSrc,int SrcLen)
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
	{
		*pDst = '\0';
	}
	// 返回目标字符串长度
	return SrcLen * 2;
}
void CUtil::ET_RC4_EN(char *KeyStr,char *InStr,char *OutStr)
{
   RC4KEY Key;
   if(strlen(KeyStr)%2!=0||strlen(InStr)%2!=0)
   {
	   return;
   }
   unsigned char *pKey=(unsigned char *)new char[strlen(KeyStr)/2+16];
   unsigned char *pIn=(unsigned char *)new char[strlen(InStr)/2+16];
   HexToAsc(pKey,KeyStr,strlen(KeyStr));
   HexToAsc(pIn,InStr,strlen(InStr));
   rc4InitKey(&Key,pKey,strlen(KeyStr)/2);
   rc4Crypt(&Key,pIn,strlen(InStr)/2);
   AscToHex(OutStr,pIn,strlen(InStr)/2);
   delete pKey;
   delete pIn;
}
void CUtil::ET_RC4_DE(char *KeyStr,char *InStr,char *OutStr)
{
   ET_RC4_EN(KeyStr,InStr,OutStr);
}
bool CUtil::ReverseBytesString(unsigned char *pBytes, int stringLen)
{
	unsigned char byte;      
	int middle;      
	int i;             
	middle = stringLen/2;
	for(i=0;i<middle; i++)
	{
		byte = pBytes[i];
		pBytes[i] = pBytes[stringLen-i-1];
		pBytes[stringLen-i-1] = byte;
	}
	return true;
}
CNode::CNode()
{
    pPrev=NULL;
	pNext=NULL;
	pData=NULL;
}
CNode::~CNode()
{

}
CNodeList::CNodeList()
{
	pHeadNode=NULL;
}
CNodeList::CNodeList(CNode *pNode)
{
    pHeadNode=pNode;
}
CNodeList::~CNodeList()
{

}
bool CNodeList::AddNode(CNode *pNode)
{
   CNode *pTempNode=NULL;
   if(pHeadNode==NULL)
   {
      pHeadNode=pNode;
   }
   else
   {
	   pTempNode=pHeadNode;
	   while(pTempNode->pNext!=NULL)
	   {
          pTempNode=pTempNode->pNext;
	   }
	   pTempNode->pNext=pNode;
	   pNode->pPrev=pTempNode;
   }
   return true;
}
bool CNodeList::DeleteNode(void *pData)
{
   CNode *pTempNode=NULL;
   pTempNode=pHeadNode;
   while(pTempNode)
   {
      if(pTempNode->pData==pData)
	  {
		  if(pTempNode==pHeadNode)//head node
		  {
            pHeadNode=NULL;
		  }
		  else
		  {
		    if(pTempNode->pNext==NULL)//last node
		     {
               pTempNode->pPrev->pNext=NULL;
		      }
		    else
		     {    
			   pTempNode->pPrev->pNext=pTempNode->pNext;
			   pTempNode->pNext->pPrev=pTempNode->pPrev;
		      }
	      }
		  delete pTempNode;
		  pTempNode=NULL;
		  break;
	  }
      pTempNode=pTempNode->pNext;
   }
   return true;
}
CNode * CNodeList::FindNode(void *pData)
{
	CNode *pTempNode=NULL;
	pTempNode=pHeadNode;
	while(pTempNode)
	{
	   if(pTempNode->pData==pData)
		{
			return pTempNode;
	    }
	   pTempNode=pTempNode->pNext;
	}
   return NULL;
}
bool CNodeList::DeleteAllNode()
{   
   CNode *pTempNode=NULL;
   CNode *pDeleteNode=NULL;
   pTempNode=pHeadNode;
   while(pTempNode)
   {
      pDeleteNode=pTempNode;
      pTempNode=pTempNode->pNext;
	  delete pDeleteNode;
	  pDeleteNode=NULL;
   }
   return true;
}
CMemBuf::CMemBuf(void)
{
	m_pChar=NULL;
	m_Length=0;
	SetLength(1023);
}
CMemBuf::CMemBuf(unsigned int Size)
{
	m_pChar=NULL;
	m_Length=0;
	SetLength(Size);
}

CMemBuf::~CMemBuf(void)
{
	if(m_pChar!=NULL)
	{
		free(m_pChar);
		m_pChar=NULL;
		m_Length=0;
	}
}
CMemBuf& CMemBuf::operator =(CMemBuf &Other) 
{
   if(this!=&Other)
   {
       SetLength(Other.GetLength());
	   memcpy(m_pChar,Other.GetCharPtr(),Other.GetLength());
   }
   return *this;
}
bool CMemBuf::ReleaseBuf()
{
	if(m_pChar!=NULL)
	{
		free(m_pChar);
		m_pChar=NULL;
		m_Length=0;
	}
	return true;
}
bool CMemBuf::SetLength(unsigned int Size)
{
	char * pChar=(char *)malloc(Size+1);
	if(pChar!=NULL)
	{
		memset(pChar,0,Size+1);
		if(m_pChar!=NULL)
		{
			if(m_Length<Size)
			{
				memcpy(pChar,m_pChar,m_Length); 
			}
			else
			{
				memcpy(pChar,m_pChar,Size); 
			}
			free(m_pChar);
			m_pChar=NULL;
		}
		m_pChar= pChar;
		m_Length=Size;
		return true;
	}
	return false;
}

unsigned int CMemBuf::GetLength(void)
{
	return m_Length;
}
unsigned int CMemBuf::SetData(char *pSrc,unsigned int Len,unsigned int Offset)
{
    if(Len>(m_Length-Offset)&&(m_Length-Offset)>0)
	{
		memcpy(m_pChar+Offset,pSrc,(m_Length-Offset));
		return (m_Length-Offset);
	}
	else
	{
		if((m_Length-Offset)>0)
		{
		  memcpy(m_pChar+Offset,pSrc,Len);
		  return Len;
		}
		else
		{
			return 0;
		}
	}
}
unsigned int CMemBuf::GetData(char *pDes,unsigned int Len,unsigned int Offset)
{
	if(Len>(m_Length-Offset)&&(m_Length-Offset)>0)
	{
		memcpy(pDes,m_pChar+Offset,(m_Length-Offset));
		return (m_Length-Offset);
	}
	else
	{
		if((m_Length-Offset)>0)
		{
			memcpy(pDes,m_pChar+Offset,Len);
			return Len;
		}
		else
		{
			return 0;
		}
	}
}
int CMemBuf::MemReset(void)
{
	memset(m_pChar,0,m_Length);
	return 0;
}
int CMemBuf::MemSet(void * Src, unsigned int Num)
{
	if(Num>m_Length)
	{
		SetLength(Num);
	}
	memcpy(m_pChar,(char*)Src,Num);
	return 0;
}
char * CMemBuf::GetCharPtr(void)
{
	return m_pChar;
}
CETException::CETException(char *pErrorMsg)
{
   int MaxLen=GetLength();
   if(strlen(pErrorMsg)>=MaxLen)
   {
	   memcpy(m_pChar,pErrorMsg,MaxLen);
   }
   else
   {
      memcpy(m_pChar,pErrorMsg,strlen(pErrorMsg));
   }
   //vsnprintf(m_pChar,MaxLen,"%s",pErrorMsg);
}
CETException::~CETException()
{

}

