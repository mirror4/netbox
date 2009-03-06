
#pragma once

#include "BoxLRUCache.h"
#include "BoxHttpHost.h"
#include <BDictionary.h>

class CBoxHttpContext;
class CBoxHttpHost;

// CBoxHttpSession command target

class CBoxHttpSession : public CBoxSafeObject,
						public CBoxLRUCacheNode
{
	DECLARE_DYNAMIC(CBoxHttpSession)
public:
	CBoxHttpSession()
	{
		m_pContents.CreateInstance();
		m_pContents->put_ArrayMode(0);
	}

public:
	__int64 m_tmLastTime;
	long m_nTimeout;
	CBoxHttpHost *m_pHost;
	CString m_strKey;

public:
	afx_msg VARIANT get_Item(VARIANT& varKey);
	afx_msg void put_Item(VARIANT& varKey, VARIANT& var);
	afx_msg LPDISPATCH get_Contents(void);

	afx_msg LPUNKNOWN get__NewEnum(void);

	afx_msg long get_SessionID(void);
	afx_msg BSTR get_SessionKey(void);
	afx_msg DATE get_LastAccessTime(void);
	afx_msg LPDISPATCH get_StaticObjects(void);

	afx_msg long get_Timeout(void);
	afx_msg void put_Timeout(long tm);

	afx_msg void Reset(void);
	afx_msg void Abandon(void);

	DECLARE_DISPATCH_MAP()

public:
	CBComPtr<CBDictionary> m_pContents;
};
