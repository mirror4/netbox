#pragma once

#include "ScriptHost.h"

// CBoxScriptContext

class CBoxScriptContext : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxScriptContext)

public:
	afx_msg void Start(long sc);
	afx_msg void SetAbort(void);
	afx_msg void SetCommit(void);

	DECLARE_DISPATCH_MAP()

	CComPtr<CScriptHost> m_pTranspHost;
};


