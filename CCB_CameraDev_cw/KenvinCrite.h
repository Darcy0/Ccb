#ifndef __KENVINCRITE_H__
#define __KENVINCRITE_H__



class CKevinCritSec	//�����ٽ���
{
public:	//�ٽ�����������
	CKevinCritSec() 
	{
		InitializeCriticalSection(&m_CritSec);	//��ʼ���ٽ����
	};
	~CKevinCritSec() 
	{
		DeleteCriticalSection(&m_CritSec);	//ɾ���ٽ����
	};
	void Lock() 
	{
		EnterCriticalSection(&m_CritSec);	//�����ٽ���
	};
	void Unlock() 
	{
		LeaveCriticalSection(&m_CritSec);	// �뿪�ٽ���
	};
protected:
	CRITICAL_SECTION m_CritSec;
};


class CKevinAutoLock
{
public:
	CKevinAutoLock(CKevinCritSec * plock)	//��������Զ���������һ���ٽ����������
	{
		m_pLock = plock;
		m_pLock->Lock();
	};
	~CKevinAutoLock()	//�������٣��Զ�����
	{
		m_pLock->Unlock();
	};
protected:
	CKevinCritSec * m_pLock;	//����һ���ٽ�����������
};


class CKevinMutexLock
{
public:
	CKevinMutexLock(HANDLE pMutex)	
	{
		PrihMutex = pMutex;
		bGetMutex = false;
		if (WaitForSingleObject(PrihMutex, 3000) == WAIT_OBJECT_0)
		{
			bGetMutex = true;
		}
	};
	~CKevinMutexLock()	
	{
		if (bGetMutex)
		{
			ReleaseMutex(PrihMutex);
		}
	};


	bool GetLockStatus() 
	{
		return bGetMutex;
	};


protected:
	HANDLE PrihMutex;
	bool   bGetMutex;
};

#endif