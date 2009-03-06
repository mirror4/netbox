#pragma once

class CBCriticalSection : public CRITICAL_SECTION
{
public:
	CBCriticalSection() : m_bInited(TRUE)
	{	::InitializeCriticalSection(this);}

	~CBCriticalSection() throw()
	{
		m_bInited = FALSE;
		::DeleteCriticalSection(this);
	}

	void Enter() throw()
	{	if(m_bInited)::EnterCriticalSection(this);}

	void Leave() throw()
	{	if(m_bInited)::LeaveCriticalSection(this);}

	BOOL IsInited(void)
	{	return m_bInited;}

private:
	BOOL m_bInited;
};

class CBLock
{
public:
	void Lock(void)
	{
		ASSERT(m_pcs != NULL);
		if(!m_bAcquired)
		{
			m_pcs->Enter();
			m_bAcquired = TRUE;
		}
	}

	void Unlock(void)
	{
		ASSERT(m_pcs != NULL);
		if(m_bAcquired)
		{
			m_pcs->Leave();
			m_bAcquired = FALSE;
		}
	}

	CBLock(CBCriticalSection* pcs, BOOL bInitialLock = TRUE) : m_bAcquired(FALSE)
	{
		m_pcs = pcs;
		if(bInitialLock)Lock();
	}

	~CBLock(void)
	{
		Unlock();
	}

private:
	BOOL m_bAcquired;
	CBCriticalSection* m_pcs;
};

class CBEvent : public CHandle
{
public:
	CBEvent(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE)
	{
		ASSERT(m_h == NULL);
		m_h = ::CreateEvent(NULL, bManualReset, bInitialState, NULL);

		if(m_h == NULL)
			AtlThrowLastWin32();
	}

	BOOL Pulse() throw()
	{
		ASSERT(m_h != NULL);
		return(::PulseEvent(m_h));
	}

	BOOL Reset() throw()
	{
		ASSERT(m_h != NULL);
		return(::ResetEvent(m_h));
	}

	BOOL Set() throw()
	{
		ASSERT(m_h != NULL);
		return(::SetEvent(m_h));
	}

	BOOL Wait(DWORD dwTimeout = INFINITE) throw()
	{
		ASSERT(m_h != NULL);
		DWORD dwRet = ::WaitForSingleObject(m_h, dwTimeout);

		return dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED;
	}
};

class CBSemaphore : public CHandle
{
public:
	CBSemaphore() throw()
	{}

	CBSemaphore(LONG nInitialCount, LONG nMaxCount)
	{
		Create(nInitialCount, nMaxCount);
	}

	BOOL Create(LONG nInitialCount, LONG nMaxCount) throw()
	{
		ASSERT(m_h == NULL);

		m_h = ::CreateSemaphore(NULL, nInitialCount, nMaxCount, NULL);
		return(m_h != NULL);
	}

	BOOL Release(LONG nReleaseCount = 1, LONG* pnOldCount = NULL) throw()
	{
		ASSERT(m_h != NULL);

		return(::ReleaseSemaphore(m_h, nReleaseCount, pnOldCount));
	}

	BOOL Wait(DWORD dwTimeout = INFINITE) throw()
	{
		ASSERT(m_h != NULL);

		DWORD dwRet = ::WaitForSingleObject(m_h, dwTimeout);

		return dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED;
	}
};

class CBThread;

class CBTLSBase
{
public:
	CBTLSBase(void);
	virtual void Init(CBThread* pThread);
	virtual void Exit(CBThread* pThread);

	void* GetPtr(CBThread* pThread = NULL) const throw();

protected:
	void Alloc(UINT nSize);

private:
	UINT m_nPos;
	CBTLSBase* m_pNext;

friend CBThread;
};

template <typename T>
class CBTls : public CBTLSBase
{
public:
	CBTls(void)
	{
		Alloc(sizeof(T));
	}

#pragma push_macro("new")
#undef new
	virtual void Init(CBThread* pThread)
	{
		T* p = (T*)GetPtr(pThread);

		::new(p) T;
	}
#pragma pop_macro("new")

	virtual void Exit(CBThread* pThread)
	{
		T* p = (T*)GetPtr(pThread);

		ASSERT(p != NULL);

		p->~T();
	}
};

template <typename T>
class CBTlsPtr
{
public:

	CBTlsPtr(CBTls<T> &tls) : p((T*)tls.GetPtr())
	{}

	operator T*() const throw()
	{
		return p;
	}

	bool operator!() const throw()
	{
		return (p == NULL);
	}

	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	T* operator->()
	{
		ASSERT(p!=NULL);
		return p;
	}

	T* p;
};

class CBThread : public CHandle
{
public:
	CBThread(void);
	~CBThread(void);

	static CBThread* GetThread(void);
	static CBThread** GetThreadID(void);

private:
	UINT m_nSize;
	CBAutoPtr<char> m_pTLS;

friend CBTLSBase;
};

class CBLockResource
{
public:
	CBLockResource(void) :
		m_ppThread(NULL),
		m_bLocked(0),
		m_dwLockCount(0)
	{}

	DWORD Lock(CBThread** ppThread = CBThread::GetThreadID())
	{
		if(m_ppThread != ppThread)
		{
			if(InterlockedExchange(&m_bLocked, 1))
				return 0;

			ASSERT(m_dwLockCount == 0);

			m_ppThread = ppThread;
		}

		m_dwLockCount ++;

		return m_dwLockCount;
	}

	void Unlock(CBThread** ppThread = NULL)
	{
		ASSERT(ppThread != NULL || m_ppThread == CBThread::GetThreadID());
		ASSERT(m_dwLockCount > 0);

		m_dwLockCount --;
		if(m_dwLockCount == 0)
		{
			m_ppThread = 0;
			InterlockedExchange(&m_bLocked, 0);
		}
	}

private:
	CBThread** m_ppThread;
	LONG m_bLocked;
	DWORD m_dwLockCount;
};

class CBQLock
{
public:
	BOOL Lock(void)
	{
		ASSERT(m_pql != NULL);
		if(!m_bAcquired)
		{
			if(m_pql->Lock())
				m_bAcquired = TRUE;
		}

		return m_bAcquired;
	}

	void Unlock(void)
	{
		ASSERT(m_pql != NULL);
		if(m_bAcquired)
		{
			m_pql->Unlock();
			m_bAcquired = FALSE;
		}
	}

	CBQLock(CBLockResource* pql) :
		m_bAcquired(FALSE),
		m_pql(pql)
	{}

	~CBQLock(void)
	{
		Unlock();
	}

private:
	BOOL m_bAcquired;
	CBLockResource* m_pql;
};

#define QLock(lk)	CBQLock l(&lk);if(!l.Lock())return HRESULT_FROM_WIN32(ERROR_BUSY);

