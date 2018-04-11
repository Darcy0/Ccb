#ifndef ETUTIL_H
#define ETUTIL_H
#include <list>
#include "ETString.h"
using namespace std;
#define MaxMallocMem 4096
class CUtil
{
public:
	  static bool ExtractFilePath(char * str,char * result);
	  static bool ExtractFileName(char * str,char * result);
	  static int  SplitString(CETString &strIn, CETStringArray& strAryRe, LPCTSTR str);
	  static int  AscToHex(char *pDst,unsigned char *pSrc,int SrcLen);
	  static int  HexToAsc(unsigned char *pDst, char *pSrc, int nSrcLen);
	  static void ET_RC4_EN(char *KeyStr,char *InStr,char *OutStr);
	  static void ET_RC4_DE(char *KeyStr,char *InStr,char *OutStr);
	  static bool ReverseBytesString(unsigned char *pBytes, int stringLen);
	  static void WriteLog(char *pFileName,char * pMsg);
	  static bool WriteFormatLog(char *FileName,const char* pFormat, ...);
	  static bool WriteHexLog(char *FileName,char *pVarName,char *pBuffer,unsigned int Len);
};
class CNode
{
public: 
	   CNode();
	   ~CNode();
	   CNode * pPrev;
	   CNode * pNext;
	   void  * pData;
};
class CNodeList
{
public:
      CNodeList();
	  CNodeList(CNode *pNode);
	  ~CNodeList();
	  CNode *pHeadNode; 
	  bool AddNode(CNode *pNode);
	  bool DeleteNode(void *pData);
	  CNode * FindNode(void *pData);
	  bool DeleteAllNode();
};
class CMemBuf
{
public:
	CMemBuf(void);
	CMemBuf(unsigned int Size);
	~CMemBuf(void);
	CMemBuf& operator =(CMemBuf &Other);
	bool SetLength(unsigned int Size);
	bool ReleaseBuf();
	unsigned int GetLength(void);
	unsigned int SetData(char *pSrc,unsigned int Len,unsigned int Offset=0);
	unsigned int GetData(char *pDes,unsigned int Len,unsigned int Offset=0);
protected:
	char *m_pChar;
	unsigned int m_Length;
public:
	int MemReset(void);
	int MemSet(void * Src, unsigned int Size);
	char * GetCharPtr(void);
};
class CETException: public CMemBuf
{
public:
	CETException(char *pErrorMsg);
	~CETException();
};
typedef DWORD (__stdcall *FWorkProc)(LPVOID lpThreadParameter);
struct ThreadSt
{
	HANDLE  Handle;
	FWorkProc pWorkProc;
	void *pParam;
	int Status;
	bool TerminateFlag;
};

#endif