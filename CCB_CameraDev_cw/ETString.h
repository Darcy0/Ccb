#ifndef ETSTRING_H
#define ETSTRING_H
#include "StlEx.h"
#include <string>
using namespace std;
class CETString:public string
{
public:
	CETString();
	CETString(int Length,char ch);
	CETString(const char *str);
	CETString(const char *str,int length);
	CETString(CETString &str);
};
class CETByteArray
{
public:
	CETByteArray();
	CETByteArray(int Size);
	~CETByteArray(void);

	int GetSize(void);
	void SetSize(int Size);	
	void RemoveAll();
    char GetAt(int nIndex);
	void SetAt(int nIndex,char newElement);
	int SetData(char *pSrc,int Len,int Offset=0);
	int GetData(char *pDes,int Len,int Offset=0);
	char * GetPtr(void);
	CETByteArray& operator =(CETByteArray &Other);
	char& operator [](int nIndex); 
protected:
	char *m_ptr;
	int m_Size;
};
class CETStringArray:public CVoidVector
{
public:
    CETStringArray();
	~CETStringArray();
	int GetSize(void);
	void RemoveAt(int nIndex);
	void RemoveAll();
	CETString& GetAt(int nIndex);
	int Find(CETString&);
	void Add(char *pStr);
	CETString& operator [](int nIndex);
};
class CETStringList:public CVoidList
{
public:
	CETStringList();
	~CETStringList();
	int GetSize(void);
	void RemoveAll();
	void* Find(CETString&);
	void Add(char *pStr);
	void Delete(void * ptr);
};
#endif