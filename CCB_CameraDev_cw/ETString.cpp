#include <windows.h>
#include "ETString.h"
CETString::CETString()
{

}
CETString::CETString(int Length,char ch):string(Length,ch)
{

}
CETString::CETString(const char *str):string(str)
{

}
CETString::CETString(const char *str,int length):string(str,length)
{

}
CETString::CETString(CETString &str)
{
   resize(str.size());
   memcpy((void *)c_str(),str.c_str(),str.size());
}
CETByteArray::CETByteArray()
{
	m_ptr=NULL;
	m_Size=0;
	SetSize(1023);
}
CETByteArray::CETByteArray(int Size)
{
	m_ptr=NULL;
	m_Size=0;
	SetSize(Size);
}

CETByteArray::~CETByteArray(void)
{
	if(m_ptr!=NULL)
	{
		free(m_ptr);
		m_ptr=NULL;
		m_Size=0;
	}
}
void CETByteArray::SetSize(int Size)
{
	char * ptr=(char *)malloc(Size+1);
	if(ptr!=NULL)
	{
		memset(ptr,0,Size+1);
		if(m_ptr!=NULL)
		{
			if(m_Size<Size)
			{
				memcpy(ptr,m_ptr,m_Size); 
			}
			else
			{
				memcpy(ptr,m_ptr,Size); 
			}
			free(m_ptr);
			m_ptr=NULL;
		}
		m_ptr= ptr;
		m_Size=Size;
	}
}
int CETByteArray::GetSize(void)
{
	return m_Size;
}
void CETByteArray::RemoveAll()
{
	if(m_ptr!=NULL)
	{
		free(m_ptr);
		m_ptr=NULL;
		m_Size=0;
	}
}
char CETByteArray::GetAt(int nIndex)
{
	return m_ptr[nIndex];
}
void CETByteArray::SetAt(int nIndex,char newElement)
{
	m_ptr[nIndex]=newElement;
}
int CETByteArray::SetData(char *pSrc,int Len,int Offset)
{
	if(Len>(m_Size-Offset)&&(m_Size-Offset)>0)
	{
		memcpy(m_ptr+Offset,pSrc,(m_Size-Offset));
		return (m_Size-Offset);
	}
	else
	{
		if((m_Size-Offset)>0)
		{
			memcpy(m_ptr+Offset,pSrc,Len);
			return Len;
		}
		else
		{
			return 0;
		}
	}
}
int CETByteArray::GetData(char *pDest,int Len,int Offset)
{
	if(Len>(m_Size-Offset)&&(m_Size-Offset)>0)
	{
		memcpy(pDest,m_ptr+Offset,(m_Size-Offset));
		return (m_Size-Offset);
	}
	else
	{
		if((m_Size-Offset)>0)
		{
			memcpy(pDest,m_ptr+Offset,Len);
			return Len;
		}
		else
		{
			return 0;
		}
	}
}
char * CETByteArray::GetPtr(void)
{
	return m_ptr;
}
CETByteArray& CETByteArray::operator =(CETByteArray &Other) 
{
	if(this!=&Other)
	{
		SetSize(Other.GetSize());
		memcpy(m_ptr,Other.GetPtr(),Other.GetSize());
	}
	return *this;
}
char& CETByteArray::operator [](int nIndex)
{
	return m_ptr[nIndex];
}
/////////////////////////////////////////////////////////////////////////////
CETStringArray::CETStringArray()
{

}
CETStringArray::~CETStringArray()
{
   RemoveAll();
}
int CETStringArray::GetSize()
{
	return size();
}
void CETStringArray::RemoveAt(int nIndex)
{
    delete (CETString *)at(nIndex);
	void *ptr=at(nIndex);
	for(Iterator=begin();Iterator!=end();Iterator++)
	{
        if(ptr==*Iterator) 
		{
			erase(Iterator);
			return;
		}
	}	
}
void CETStringArray::RemoveAll()
{
	int Count=size();
	for(int i=0;i<Count;i++)
	{
		delete (CETString *)at(i);
	}
	clear();
}
CETString& CETStringArray::GetAt(int nIndex)
{
   return *((CETString *)at(nIndex));
}
void CETStringArray::Add(char *pStr)
{
   CETString *pstring=new CETString(pStr);
   AddTailElement(pstring);
}
int CETStringArray::Find(CETString& str)
{
	int Index=0;
	for(Iterator=begin();Iterator!=end();Iterator++)
	{
		if((*(CETString *)(*Iterator)).compare(str)==0)
		{
           return Index;
		}
		Index++;
	}
	return -1;
}
CETString& CETStringArray::operator [](int nIndex)
{
	 return *((CETString *)at(nIndex));
}
//////////////////////////////////////////////////////////////
CETStringList::CETStringList()
{

}
CETStringList::~CETStringList()
{
    RemoveAll();
}
int CETStringList::GetSize()
{
	return size();
}
void CETStringList::RemoveAll()
{
	for(Iterator=begin();Iterator!=end();Iterator++)
	{
		delete (string *)(*Iterator);
	}
	clear();
}
void CETStringList::Add(char *pStr)
{
	CETString *pstring=new CETString(pStr);
	AddTailElement(pstring);
}
void * CETStringList::Find(CETString& str)
{
	for(Iterator=begin();Iterator!=end();Iterator++)
	{
		if((*(CETString *)(*Iterator)).compare(str)==0)
		{
			return (*Iterator);
		}
	}
	return NULL;
}
void CETStringList::Delete(void * ptr)
{
	 delete (CETString *)ptr;
    remove(ptr);
}