
class CKevinCritSec	//操作临界区
{
public:	//临界区操作函数
	CKevinCritSec() 
	{
		InitializeCriticalSection(&m_CritSec);	//初始化临界对象
	};
	~CKevinCritSec() 
	{
		DeleteCriticalSection(&m_CritSec);	//删除临界对象
	};
	void Lock() 
	{
		EnterCriticalSection(&m_CritSec);	//进入临界区
	};
	void Unlock() 
	{
		LeaveCriticalSection(&m_CritSec);	// 离开临界区
	};
protected:
	CRITICAL_SECTION m_CritSec;
};


class CKevinAutoLock
{
public:
	CKevinAutoLock(CKevinCritSec * plock)	//定义对象，自定上锁，有一个临界区对象参数
	{
		m_pLock = plock;
		m_pLock->Lock();
	};
	~CKevinAutoLock()	//对象销毁，自动解锁
	{
		m_pLock->Unlock();
	};
protected:
	CKevinCritSec * m_pLock;	//包含一个临界区操作对象
};