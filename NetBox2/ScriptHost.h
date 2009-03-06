#pragma once

#include "BoxTimeCache.h"
#include "BoxObject.h"
#include "BoxDictionary.h"
#include <BoxLib.h>
#include <BErrorTrace.h>

#include <mtx.h>

#include <activscp.h>
#include "activdbg.h"

#pragma warning(disable : 4584)
class CScriptHost :
	public IUnknown,
	public IActiveScriptSite,
	public IActiveScriptSiteDebug
{
public:
	CScriptHost(void);
	~CScriptHost(void);
	BOOL Create(LPCTSTR pstrScriptName, BOOL bEnable, BOOL bStep, int nTransaction);
	BOOL CloneFrom(CScriptHost* pHost);
	void SetScriptSite(void);
	void AddNameItem(LPCTSTR pstrName, LPDISPATCH pUnk);
	void CloneParentNameItem();
	HRESULT Load(LPCTSTR pstrScriptText);
	long Run(void);
	long Stop(long nErrorCode);
	void Close(void);
	long CallEvent(LPCTSTR pstrFun);
	long CallCommand(long CtrlCode);

	LPDISPATCH GetScriptDispatch(void)
	{
		if(m_pActiveScript != NULL && m_pScriptDisp == NULL)
			m_pActiveScript->GetScriptDispatch(NULL, &m_pScriptDisp);
		return m_pScriptDisp;
	}

	void SetFileName(LPCTSTR pstrName)
	{
		m_pErrorTrace->SetName(CBString(pstrName));
		m_strFileName = pstrName;
	}

	BOOL IsClosed(void)
	{
		return (m_pScriptDisp == NULL);
	}

	void Interrupt(LPCTSTR strError = NULL)
	{
		CComPtr<IActiveScript> pActiveScript = m_pActiveScript;

		if(pActiveScript != NULL)
		{
			if(strError)m_strErrorMessage = strError;
			pActiveScript->InterruptScriptThread(SCRIPTTHREADID_ALL, NULL, SCRIPTINTERRUPT_RAISEEXCEPTION);
		}
	}

	void SetScriptState(SCRIPTSTATE sc)
	{
		m_pActiveScript->SetScriptState(sc);
	}

	void SetCommit(BOOL bCommit = TRUE)
	{
		m_bCommit = bCommit;
	}

	static CScriptHost* GetCurrentScript();
	static CScriptHost* GetTransScript();

	CString m_strScriptName;

	CString m_strErrorSource;
	CString m_strErrorFile;
	ULONG m_nErrorLine;
	CString m_strErrorMessage;
	SCODE m_scode;

public:
	// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

public:
	// IActiveScriptSite
    STDMETHOD(GetLCID)(LCID *plcid);
    STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti);
    STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion);
    STDMETHOD(OnScriptTerminate)(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo);
    STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState);
    STDMETHOD(OnScriptError)(IActiveScriptError *pscripterror);
    STDMETHOD(OnEnterScript)(void);
    STDMETHOD(OnLeaveScript)(void);

public:
	// IActiveScriptSiteDebug
	STDMETHOD(GetDocumentContextFromPosition)(DWORD dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext **ppsc);
	STDMETHOD(GetApplication)(IDebugApplication **ppda);
	STDMETHOD(GetRootApplicationNode)(IDebugApplicationNode **ppdanRoot);
	STDMETHOD(OnScriptErrorDebug)(IActiveScriptErrorDebug *pErrorDebug, BOOL *pfEnterDebugger, BOOL *pfCallOnScriptErrorWhenContinuing);

public:
	CPtrArray m_arrayBlock;
	CStringArray m_arrayFile;
	CWordArray m_arrayLine;
	int m_arrayExtData[16];

public:
	static CBCriticalSection m_csScript;
	static CRBMap<CScriptHost*, CBComPtr<CBErrorTrace> > m_listScript;
	static void CheckTimeout(__int64 nNow);

public:
	int m_nTimeout;

private:
	long m_dwRef;
	CComPtr<ITransactionContext> m_pTransactionContext;
	CBComPtr<CBErrorTrace> m_pErrorTrace;
	int m_nTransaction;
	CBComPtr<IActiveScript> m_pActiveScript;
	CComQIPtr<IActiveScriptParse> m_pActiveScriptParse;
	CComPtr<IDebugDocumentHelper> m_pDebugDoc;
	CComDispatchDriver m_pScriptDisp;
	CBoxObject<CBoxDictionary> m_mapItem;
	CStringW m_strFileName;
	BOOL m_bEnableDebug;
	BOOL m_bCommit;
	BOOL m_bStepDebug;
	BOOL m_bRunning;
	long m_nErrorCode;
	__int64 m_nStart;

	CBoxTimeCache::CCache m_time;
};

#pragma warning(default : 4584)
