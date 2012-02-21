#include "StdAfx.h"
#include "ScriptHost.h"
#include "NetBox2.h"
#include "BoxSystem.h"
#include "BoxScriptContext.h"
#include <BClassRegistry.h>
#include <excpt.h>

static __declspec(thread) CScriptHost* t_pScriptHost = NULL;
static __declspec(thread) CScriptHost* t_pTopTransScriptHost = NULL;
CBCriticalSection CScriptHost::m_csScript;
CRBMap<CScriptHost*, CBComPtr<CBErrorTrace> > CScriptHost::m_listScript;

void CScriptHost::CheckTimeout(__int64 nNow)
{
	POSITION pos;

	m_csScript.Enter();

	pos = m_listScript.GetHeadPosition();
	while(pos)
	{
		CScriptHost* pScript = CScriptHost::m_listScript.GetNext(pos)->m_key;

		if((pScript->m_nTimeout > 0) && (nNow > pScript->m_nStart + (__int64)pScript->m_nTimeout * 10000000))
		{
			pScript->m_nTimeout = 0;
			pScript->Interrupt(_T("The maximum amount of time for a script to execute was exceeded."));
		}
	}

	m_csScript.Leave();
}

CScriptHost::CScriptHost(void) :
	m_bEnableDebug(FALSE),
	m_bStepDebug(FALSE),
	m_nErrorCode(0),
	m_dwRef(0),
	m_nErrorLine(0),
	m_scode(0),
	m_bRunning(TRUE),
	m_nTransaction(0),
	m_bCommit(TRUE),
	m_nTimeout(0)
{
	m_strErrorSource = _T("NetBox script runtime error");
	m_mapItem.CreateInstance();
	m_pErrorTrace.CreateInstance();
}

CScriptHost::~CScriptHost(void)
{
	m_csScript.Enter();
	m_listScript.RemoveKey(this);
	m_csScript.Leave();

	for(int i = 0; i < m_arrayBlock.GetCount(); i ++)
		delete m_arrayBlock[i];
}

BOOL CScriptHost::Create(LPCTSTR pstrScriptName, BOOL bEnable, BOOL bStep, int nTransaction)
{
	if(m_pActiveScript)return FALSE;

	m_bEnableDebug = bEnable;
	m_bStepDebug = bStep;
	m_nTransaction = nTransaction;
	if(m_bStepDebug)m_bEnableDebug = TRUE;

	if(m_bEnableDebug)
	{
		theApp.m_pSystem->LoadScriptDebugger();
		if(theApp.m_pSystem->m_pDebugApp != NULL)
		{
			theApp.m_pSystem->m_pDebugManager->CreateDebugDocumentHelper(NULL, &m_pDebugDoc);
			if(m_pDebugDoc != NULL)
			{
				m_pDebugDoc->Init(theApp.m_pSystem->m_pDebugApp, m_strFileName, m_strFileName, TEXT_DOC_ATTR_READONLY);
				m_pDebugDoc->Attach(NULL);
			}
		}
	}

	//解决UPX IAT截获函数问题 m_pActiveScript.CreateInstance(BOX_CT2CW(pstrScriptName));
	CLSID clsid;
	if (!(SUCCEEDED(CBClassRegistry::CLSIDFromProgID(BOX_CT2CW(pstrScriptName), &clsid)) && 
		SUCCEEDED(CBClassRegistry::CreateInstance(clsid, NULL, CLSCTX_ALL, IID_IActiveScript, (LPVOID *)&m_pActiveScript))))
	{
		m_strErrorMessage = _T("Unknown scripting language.");
		return FALSE;
	}

	m_pActiveScriptParse = m_pActiveScript;
	m_pActiveScriptParse->InitNew();

	return TRUE;
}

BOOL CScriptHost::CloneFrom(CScriptHost* pHost)
{
	if(!pHost->m_pActiveScript)
		return FALSE;

	m_bEnableDebug = pHost->m_bEnableDebug;
	m_bStepDebug = pHost->m_bStepDebug;
	m_nTransaction = pHost->m_nTransaction;
	m_nTimeout = pHost->m_nTimeout;

	pHost->m_pActiveScript->Clone(&m_pActiveScript);

	return m_pActiveScript != NULL;
}

void CScriptHost::SetScriptSite(void)
{
	m_pActiveScript->SetScriptSite(this);
}

void CScriptHost::AddNameItem(LPCTSTR pstrName, LPDISPATCH pUnk)
{
	if(m_pActiveScript != NULL)
	{
		CString strName(pstrName);

		m_pActiveScript->AddNamedItem(BOX_CT2CW(strName), SCRIPTITEM_ISVISIBLE);
		m_mapItem->SetValue(pstrName, pUnk);
	}
}

void CScriptHost::CloneParentNameItem()
{
	CScriptHost* pScriptHost = GetCurrentScript();

	if(m_pActiveScript != NULL)
		if(pScriptHost != NULL)
		{
			int i;
			CComVariant var;
			CString key;

			for(i = 0; i < pScriptHost->m_mapItem->get_Count(); i ++)
				if(pScriptHost->m_mapItem->GetAssoc(i, key, var) && (var.vt == VT_DISPATCH))
					AddNameItem(key, var.pdispVal);
		}else AddNameItem(_T("NetBox"), theApp.m_pSystem);
}

HRESULT CScriptHost::Load(LPCTSTR pstrScriptText)
{
	if(m_pActiveScriptParse != NULL)
		return m_pActiveScriptParse->ParseScriptText(BOX_CT2CW(pstrScriptText), 
			0, 0, NULL, 0, 0, SCRIPTTEXT_ISPERSISTENT, 0, 0);
	else return E_NOTIMPL;
}

long CScriptHost::Run(void)
{
	extern void (__stdcall *g_pGetObjectContext)(REFIID riid, LPVOID FAR* ppv);

	if(m_pActiveScript != NULL)
	{
		CBoxObject<CBoxTimeCache::CTime> pTime;

		m_time.GetTime(pTime);

		m_nStart = pTime->m_time;

		m_csScript.Enter();
		m_listScript.SetAt(this, m_pErrorTrace);
		m_csScript.Leave();

		CScriptHost* pOldScript = t_pScriptHost;
		CScriptHost* pOldTransScript = NULL;

		t_pScriptHost = this;

		m_pActiveScript->AddNamedItem(L"Trace", SCRIPTITEM_ISVISIBLE);

		if(m_nTransaction == 1 || m_nTransaction == 2)
		{
			CComPtr<IObjectContext> pObjectContext;

			if(g_pGetObjectContext != NULL)
				g_pGetObjectContext(IID_IObjectContext, (void**)&pObjectContext);
			if(pObjectContext == NULL)
				if(m_nTransaction == 1)
					m_nTransaction = 0;
				else m_nTransaction = 3;
		}

		if(m_nTransaction)
		{
			if(g_pGetObjectContext == NULL)
			{
				m_strErrorMessage = _T("Transaction not supported.");
				return -1;
			}

			CComVariant var;
			CComDispatchDriver pDisp;

			if(m_nTransaction == 3)
			{
				m_bCommit = TRUE;
				m_pTransactionContext.CoCreateInstance(CLSID_TransactionContext, NULL, CLSCTX_INPROC_SERVER);
				if(m_pTransactionContext == NULL)
				{
					m_strErrorMessage = _T("Transaction not supported.");
					return -1;
				}

				CComBSTR bstr(L"Netbox.ScriptContext");

				m_pTransactionContext->CreateInstance(bstr, &var);

				if(var.vt != VT_DISPATCH)
				{
					m_strErrorMessage = _T("Transaction not supported.");
					return -1;
				}

				pDisp = var.pdispVal;

				pOldTransScript = t_pTopTransScriptHost;
				t_pTopTransScriptHost = this;
			}else
			{
				CBoxObject<CBoxScriptContext> pScriptContext;

				pScriptContext.CreateInstance();
				pDisp = (IDispatch*)pScriptContext;
			}

			m_pActiveScript->GetScriptDispatch(NULL, &m_pScriptDisp);

			if(m_bStepDebug && m_pDebugDoc != NULL)
			{
				theApp.m_pSystem->m_pDebugApp->CauseBreak();
				var = SCRIPTSTATE_CONNECTED;
			}else
				var = SCRIPTSTATE_STARTED;

			pDisp.Invoke1((DISPID)9465, &var);

			if(m_pTransactionContext != NULL)
				t_pTopTransScriptHost = pOldTransScript;
		}else
		{
			m_pActiveScript->GetScriptDispatch(NULL, &m_pScriptDisp);
			if(m_bStepDebug && m_pDebugDoc != NULL)
			{
				theApp.m_pSystem->m_pDebugApp->CauseBreak();
				m_pActiveScript->SetScriptState(SCRIPTSTATE_CONNECTED);
			}else
				m_pActiveScript->SetScriptState(SCRIPTSTATE_STARTED);
		}

		t_pScriptHost = pOldScript;
	}

	return m_nErrorCode;
}

long CScriptHost::Stop(long nErrorCode)
{
	CComDispatchDriver pScriptDisp = m_pScriptDisp;

	if(pScriptDisp)
	{
		m_pScriptDisp.Release();

		if(m_nErrorCode == 0)m_nErrorCode = nErrorCode;

		CScriptHost* pOldScript = t_pScriptHost;

		if(pOldScript != this)
			t_pScriptHost = this;

		if(m_nErrorCode == 0)
			pScriptDisp.Invoke0(L"OnScriptEnd");

		if(m_nErrorCode != 0)m_bCommit = FALSE;

		if(m_pTransactionContext != NULL)
		{
			if(m_bCommit)
				if(FAILED(m_pTransactionContext->Commit()))
					m_bCommit = FALSE;

			if(m_bCommit)
				pScriptDisp.Invoke0(L"OnTransactionCommit");
			else
			{
				pScriptDisp.Invoke0(L"OnTransactionAbort");
				m_pTransactionContext->Abort();
			}
		}

		m_bRunning = FALSE;

		if(m_nErrorCode != 0)
			pScriptDisp.Invoke0(L"OnScriptError");

		if(pOldScript != this)
			t_pScriptHost = pOldScript;
	}

	m_bRunning = FALSE;

	if(m_pActiveScript != NULL)
		m_pActiveScript->InterruptScriptThread(SCRIPTTHREADID_ALL, NULL, 0);

	return m_nErrorCode;
}

long CScriptHost::CallEvent(LPCTSTR pstrFun)
{
	CScriptHost* pOldScript = t_pScriptHost;

	t_pScriptHost = this;

	CStringW strW(pstrFun);
	if(m_pScriptDisp)
		m_pScriptDisp.Invoke0(strW);

	t_pScriptHost = pOldScript;

	if(m_bRunning)return m_nErrorCode;
	return -1;
}

long CScriptHost::CallCommand(long CtrlCode)
{
	CScriptHost* pOldScript = t_pScriptHost;
	CComVariant v(CtrlCode);

	t_pScriptHost = this;

	if(m_pScriptDisp)
		m_pScriptDisp.Invoke1(L"OnServiceCommand", &v);

	t_pScriptHost = pOldScript;

	if(m_bRunning)return m_nErrorCode;
	return -1;
}

void CScriptHost::Close(void)
{
	if(m_pActiveScript != NULL)
	{
		Stop(m_nErrorCode);
		m_pActiveScript->Close();
		m_pActiveScript.Release();
		m_pActiveScriptParse.Release();
		m_pTransactionContext.Release();

		if(m_pDebugDoc != NULL)
		{
			m_pDebugDoc->Detach();
			m_pDebugDoc.Release();
		}

		m_bRunning = TRUE;
		m_nErrorCode = 0;
		m_nTransaction = 0;
		m_nTimeout = 0;
		m_bCommit = TRUE;
		m_strErrorSource = _T("NetBox script runtime error");
		m_strErrorMessage.Empty();
		m_strScriptName.Empty();

		m_mapItem->RemoveAll();
	}
}

CScriptHost* CScriptHost::GetCurrentScript()
{
	return t_pScriptHost;
}

CScriptHost* CScriptHost::GetTransScript()
{
	return t_pTopTransScriptHost;
}

// IUnknown
STDMETHODIMP_(ULONG) CScriptHost::AddRef()
{
	return InterlockedIncrement(&m_dwRef);
}

STDMETHODIMP_(ULONG) CScriptHost::Release()
{
	if (m_dwRef == 0)
		return 0;

	LONG lResult = InterlockedDecrement(&m_dwRef);
	if (lResult == 0)
		delete this;

	return lResult;
}

STDMETHODIMP CScriptHost::QueryInterface(REFIID iid, LPVOID far* ppvObj)     
{
	*ppvObj = NULL;

	if (IsEqualIID(iid, IID_IUnknown))
		*ppvObj = (IUnknown*)this;
	else if (IsEqualIID(iid, IID_IActiveScriptSite))
		*ppvObj = (IActiveScriptSite*)this;
	else if (IsEqualIID(iid, __uuidof(IActiveScriptSiteDebug)))
		*ppvObj = (IActiveScriptSiteDebug*)this;

	if(*ppvObj)
	{
		AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}

// IActiveScriptSite
HRESULT STDMETHODCALLTYPE CScriptHost::GetLCID(LCID *plcid)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti)
{
   	if(ppiunkItem != NULL)*ppiunkItem = NULL;
	if(ppti != NULL)*ppti = NULL;

	if(dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		if(!_wcsicmp(pstrName, L"Trace"))
			return m_pErrorTrace.QueryInterface(ppiunkItem);

		CComVariant var;

		*ppiunkItem = NULL;
		m_mapItem->GetValue(BOX_CW2CT(pstrName), var);
		if(var.vt == VT_DISPATCH)
			*ppiunkItem = var.pdispVal;

		if(*ppiunkItem != NULL)
			(*ppiunkItem)->AddRef();
	}else return E_FAIL;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CScriptHost::GetDocVersionString(BSTR *pbstrVersion)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::OnScriptTerminate(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::OnStateChange(SCRIPTSTATE ssScriptState)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::OnScriptError(IActiveScriptError *pscripterror)
{
	if(m_bRunning && (m_nErrorCode == 0 || t_pScriptHost != this))
	{
		DWORD dwCookie;
		LONG nErrorChar;
		EXCEPINFO e;
		int i;

		pscripterror->GetSourcePosition(&dwCookie, &m_nErrorLine, &nErrorChar);
		pscripterror->GetExceptionInfo(&e);

		for(i = 0; i < m_arrayLine.GetCount(); i += 3)
			if(m_arrayLine[i] > m_nErrorLine)
				break;

		i -= 3;
		if(i >= 0)
		{
			m_nErrorLine -= m_arrayLine[i];
			m_nErrorLine += m_arrayLine[i + 2];
			m_strErrorFile = m_arrayFile[m_arrayLine[i + 1]];
		}

		if((e.scode & 0xffff0000) == ((FACILITY_WIN32 << 16) | 0x80000000) && e.bstrDescription == NULL)
		{
			if(IS_WINNT)
			{
				LPWSTR pMsgBuf = NULL;
				if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, e.scode & 0xffff, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
				{
					m_strErrorMessage = pMsgBuf;
					LocalFree(pMsgBuf);
				}
			}else
			{
				LPSTR pMsgBuf = NULL;
				if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, e.scode & 0xffff, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
				{
					m_strErrorMessage = pMsgBuf;
					LocalFree(pMsgBuf);
				}
			}
		}else if(e.bstrDescription != NULL)
			m_strErrorMessage = e.bstrDescription;

		m_nErrorCode = -1;

		if(e.bstrSource != NULL)
			m_strErrorSource = e.bstrSource;

		m_scode = e.scode;

		if(e.bstrDescription != NULL)
			::SysFreeString(e.bstrDescription);
		if(e.bstrHelpFile != NULL)
			::SysFreeString(e.bstrHelpFile);
		if(e.bstrSource != NULL)
			::SysFreeString(e.bstrSource);

		CBString strMsg(m_strErrorFile);

		strMsg.AppendFormat(L"(%d): ", m_nErrorLine);
		strMsg += m_strErrorMessage;

		m_pErrorTrace->Write((LPCWSTR)strMsg, strMsg.GetLength() * 2);

		if(t_pScriptHost && t_pScriptHost != this)
			t_pScriptHost->m_pErrorTrace->Write((LPCWSTR)strMsg, strMsg.GetLength() * 2);

		if(theApp.m_pSystem->m_pErrorLog)
		{
			CBStringA astrMsg;
			SYSTEMTIME st;

			GetSystemTime(&st);

			astrMsg.Format("[%02d-%02d %02d:%02d:%02d]", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			astrMsg += strMsg;

			astrMsg.Append("\r\n", 2);

			theApp.m_pSystem->m_pErrorLog->Write((LPCSTR)astrMsg, astrMsg.GetLength(), 0);
		}

	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CScriptHost::OnEnterScript(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::OnLeaveScript(void)
{
	return E_NOTIMPL;
}

// IActiveScriptSiteDebug
HRESULT STDMETHODCALLTYPE CScriptHost::GetDocumentContextFromPosition(DWORD dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext **ppsc)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::GetApplication(IDebugApplication **ppda)
{
	if(m_bEnableDebug && m_pDebugDoc != NULL)
	{
		theApp.m_pSystem->m_pDebugApp.CopyTo(ppda);
		return S_OK;
	}else return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CScriptHost::GetRootApplicationNode(IDebugApplicationNode **ppdanRoot)
{
	if (!ppdanRoot)
		return E_INVALIDARG;

	if (m_pDebugDoc)
		return m_pDebugDoc->GetDebugApplicationNode(ppdanRoot);
	return E_NOTIMPL;
}

static CCriticalSection s_csDebug;

HRESULT STDMETHODCALLTYPE CScriptHost::OnScriptErrorDebug(IActiveScriptErrorDebug *pErrorDebug, BOOL *pfEnterDebugger, BOOL *pfCallOnScriptErrorWhenContinuing)
{
	CString str;

	OnScriptError(pErrorDebug);

	DWORD dwCookie;
	LONG nErrorChar;
	ULONG nErrorLine;
	EXCEPINFO e;
	int i;
	CString strErrorFile(m_strScriptName);

	pErrorDebug->GetSourcePosition(&dwCookie, &nErrorLine, &nErrorChar);
	pErrorDebug->GetExceptionInfo(&e);

	for(i = 0; i < m_arrayLine.GetCount(); i += 3)
		if(m_arrayLine[i] > nErrorLine)
			break;

	i -= 3;
	if(i >= 0)
	{
		nErrorLine -= m_arrayLine[i];
		nErrorLine += m_arrayLine[i + 2];
		strErrorFile = m_arrayFile[m_arrayLine[i + 1]];
	}

	if(e.bstrSource)
		str = e.bstrSource;
	if(e.scode != 0)
		str.AppendFormat(_T("\n\nError Number: %08X\nProgram: "), e.scode);
	else str += _T("\n\nProgram: ");
	str += strErrorFile;
	str.AppendFormat(_T("\nLine: %d\nDescription: "), nErrorLine);

	if((e.scode & 0xffff0000) == ((FACILITY_WIN32 << 16) | 0x80000000) && e.bstrDescription == NULL)
	{
		if(IS_WINNT)
		{
			LPWSTR pMsgBuf = NULL;
			if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, e.scode & 0xffff, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
			{
				str += pMsgBuf;
				LocalFree(pMsgBuf);
			}
		}else
		{
			LPSTR pMsgBuf = NULL;
			if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, e.scode & 0xffff, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
			{
				str += pMsgBuf;
				LocalFree(pMsgBuf);
			}
		}
	}else str += e.bstrDescription;

	if(e.bstrDescription != NULL)
		::SysFreeString(e.bstrDescription);
	if(e.bstrHelpFile != NULL)
		::SysFreeString(e.bstrHelpFile);
	if(e.bstrSource != NULL)
		::SysFreeString(e.bstrSource);

	str += _T("\n\nDo you wish to Debug?");

	CSingleLock l(&s_csDebug, TRUE);

	if(MessageBox(::GetForegroundWindow(), str, CBoxSystem::getVersion(), MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND) == IDYES)
	{
		l.Unlock();

		if(pfEnterDebugger)
			*pfEnterDebugger = TRUE;
		if(pfCallOnScriptErrorWhenContinuing)
			*pfCallOnScriptErrorWhenContinuing = FALSE;
	}

	return S_OK;
}

