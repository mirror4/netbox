#pragma once

#include "BoxObject.h"

// CBoxTimeCache

class CBoxTimeCache : public CWinThread
{
public:
	class CTime : public CBoxBaseObject
	{
	public:
		__int64 m_time;
		CString m_strTime;
	};

	class CCache
	{
	public:
		CCache(void) : m_pBoxTimeCache(NULL)
		{
			CBoxTimeCache::GetNewTimeCache(m_pBoxTimeCache);
		}

		void GetTime(CBoxObject<CBoxTimeCache::CTime>& pTime)
		{
			m_pBoxTimeCache->GetTime(pTime);
		}

	private:
		CBoxObject<CBoxTimeCache> m_pBoxTimeCache;
	};

	CBoxTimeCache();           // protected constructor used by dynamic creation

public:
	virtual BOOL InitInstance();
	virtual void OnFinalRelease();

public:
	static void GetNewTimeCache(CBoxObject<CBoxTimeCache>& pBoxTimeCache);
	void GetTime(CBoxObject<CTime>& pTime)
	{
		CSingleLock l(&m_cs, TRUE);

		pTime = m_pTime;
	}

private:
	void GetNewTime(void);

	CCriticalSection m_cs;
	CBoxObject<CTime> m_pTime;
};
