// BoxScriptContext.cpp : implementation file
//

#include "stdafx.h"
#include "BoxScriptContext.h"

// CBoxScriptContext

IMPLEMENT_DYNCREATE(CBoxScriptContext, CWnd)

BEGIN_DISPATCH_MAP(CBoxScriptContext, CWnd)
	DISP_FUNCTION_ID(CBoxScriptContext, "", 9465, Start, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CBoxScriptContext, "SetAbort", SetAbort, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxScriptContext, "SetCommit", SetCommit, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// CBoxScriptContext message handlers

void CBoxScriptContext::Start(long sc)
{
	CComPtr<CScriptHost> pHost;

	pHost = CScriptHost::GetCurrentScript();
	m_pTranspHost = CScriptHost::GetTransScript();
	if(pHost != NULL)
	{
		pHost->AddNameItem(_T("ObjectContext"), GetIDispatch(FALSE));
		pHost->SetScriptState((SCRIPTSTATE)sc);
	}
}

void CBoxScriptContext::SetAbort(void)
{
	if(m_pTranspHost != NULL)
		m_pTranspHost->SetCommit(FALSE);
}

void CBoxScriptContext::SetCommit(void)
{
	if(m_pTranspHost != NULL)
		m_pTranspHost->SetCommit();
}

