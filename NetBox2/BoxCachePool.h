#pragma once
#include "BoxFile.h"
#include "BoxCacheNode.h"
#include "BoxLRUCache.h"

class CBoxCachePool : public CBoxFile
{
public:
	CBoxCachePool(CBoxFile* pOldFile);
	~CBoxCachePool(void);

	void RemoveAll(void)
	{
		CSingleLock l(&m_cs, TRUE);

		m_cache.RemoveAll();
	}

public:
	virtual CFile* Open(LPCTSTR pstrName);
	virtual SYSTEMTIME GetFileTime(CFile* pFile);
	virtual void GetFileTimeString(CFile* pFile, CString& strTime);

	void SetCacheNode(LPCTSTR pstrName, CMemFile* pFile, SYSTEMTIME tm);

	static void CheckOpenFile(LPCTSTR pstrFile, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwFlagsAndAttributes, HANDLE& hd);

	CBoxFile* m_pOldFile;

private:

	CCriticalSection m_cs;
	CBoxLRUCache<CBoxCacheNode> m_cache;
};
