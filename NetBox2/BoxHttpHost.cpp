// BoxHttpHost.cpp : implementation file
//

#include "stdafx.h"
#include "NetBox2.h"
#include "BoxHttpHost.h"
#include "BoxHttpSession.h"
#include "BoxHttpAccept.h"
#include "BoxHttpContext.h"
#include "BoxPath.h"
#include <BPath.h>

#include <openssl\rand.h>

// CBoxHttpHost

IMPLEMENT_DYNAMIC(CBoxHttpHost, CBoxContents)

CBoxHttpHost::CBoxHttpHost(void) :
	m_nSessionTimeout(20),
	m_bEnableScript(FALSE),
	m_bEnableDebug(FALSE),
	m_bEnableSession(TRUE),
	m_bEnableBrowse(FALSE),
	m_bGlobalApplication(FALSE),
	m_bAutoWebDAV(FALSE),
	m_bEnableWrite(FALSE),
	m_bIsStart(FALSE),
	m_nScriptTimeout(90)
{
	CBoxTimeCache::CCache time;
	CBoxObject<CBoxTimeCache::CTime> t;

	time.GetTime(t);
	m_nNowTime = t->m_time;

	m_pApplication.CreateInstance();
	m_strBrowseFooter = _T("\r\n<small>Host by <a href=\"http://www.9465.net\" target=\"_blank\">");
	m_strBrowseFooter.Append(CBoxSystem::getVersion(), CBoxSystem::getVersionLen());
	m_strBrowseFooter.Append(_T("</a></small>\r\n"), 14);
}


BOOL CBoxHttpHost::GetSession(CString& strSessionCookie, CBoxObject<CBoxHttpSession>& pSession, CBoxHttpContext *pContext)
{
	BYTE randstr[41];
	BOOL bSessionKey = FALSE;

	CSingleLock l(&m_cs, TRUE);

	if(!strSessionCookie.IsEmpty())
	{
		if(m_cacheSessions.Lookup(strSessionCookie, pSession))
		{
			pSession->m_tmLastTime = m_nNowTime;

			return TRUE;
		}
	}else
	{
		RAND_seed(&pSession, sizeof(pSession));
		RAND_seed(randstr, sizeof(randstr));

		do
		{
			RAND_bytes(randstr, 40);
			for(int i = 0; i < 40; i ++)
				randstr[i] = (randstr[i] % 26) + 'A';
			randstr[40] = 0;

			strSessionCookie = randstr;
		}while(m_cacheSessions.Exists(strSessionCookie));

		bSessionKey = TRUE;
	}

	pSession.CreateInstance();
	pSession->m_nTimeout = m_nSessionTimeout;
	pSession->m_pHost = this;
	pSession->m_tmLastTime = m_nNowTime;

	pSession->m_strKey = strSessionCookie;
	m_cacheSessions.Add(strSessionCookie, pSession);

	l.Unlock();

	CallEvent(L"OnSessionStart", pContext, pSession);

	return !bSessionKey;
}

void CBoxHttpHost::CallEvent(LPCWSTR strEvent, CBoxHttpContext *pContext, CBoxHttpSession *pSession, BOOL bAddHost)
{
	CBoxObject<CBoxScriptObject> pScriptObject;
	CBoxObject<CBoxHttpContext> pNullContext;
	CComDispatchDriver pDisp;

	pScriptObject.CreateInstance();
	if(pScriptObject->Load(m_strEventSink) != 0)
		return;

	if(pContext)pContext->Bind(NULL, pScriptObject);
	else
	{
		pNullContext.CreateInstance();
		pNullContext->m_strBasePath = m_strRoot;
		pScriptObject->AddNameItem(_T("Server"), pNullContext->GetIDispatch(FALSE));
		pScriptObject->AddNameItem(_T("Application"), m_pApplication);
	}
	if(pSession)pScriptObject->AddNameItem(_T("Session"), pSession->GetIDispatch(FALSE));
	if(bAddHost)pScriptObject->AddNameItem(_T("Host"), GetIDispatch(FALSE));

	if(pScriptObject->Init() != 0)
		return;

	pDisp.Attach(pScriptObject->get_Object());

	if(pDisp)
	{
		pDisp.Invoke0(strEvent);
		theApp.m_pSystem->ClearLock();
	}
}

extern __declspec(thread) CBoxHttpAccept* t_pHttpAccept;

void CBoxHttpHost::AbandonSession(CBoxHttpSession* pItem)
{
	CSingleLock l(&m_cs, TRUE);

	m_cacheSessions.Remove(pItem);

	if(t_pHttpAccept != NULL)
		if(m_strSessionDomain.IsEmpty())
			t_pHttpAccept->AddHeader(_T("Set-Cookie"), m_strHostKey + _T("=; path=/"));
		else t_pHttpAccept->AddHeader(_T("Set-Cookie"), m_strHostKey + _T("=; domain=") + m_strSessionDomain + _T("; path=/"));
}

void CBoxHttpHost::ResetSession(CBoxHttpSession* pItem)
{
	BYTE randstr[41];
	CString strSessionCookie;

	if(t_pHttpAccept != NULL)
	{
		CSingleLock l(&m_cs, TRUE);

		m_cacheSessions.Remove(pItem);

		CallEvent(L"OnSessionEnd", NULL, pItem);
		pItem->m_pContents->RemoveAll();

		RAND_seed(&pItem, sizeof(pItem));
		RAND_seed(randstr, sizeof(randstr));

		do
		{
			RAND_bytes(randstr, 40);
			for(int i = 0; i < 40; i ++)
				randstr[i] = (randstr[i] % 26) + 'A';
			randstr[40] = 0;

			strSessionCookie = randstr;
		}while(m_cacheSessions.Exists(strSessionCookie));

		pItem->m_strKey = strSessionCookie;
		m_cacheSessions.Add(strSessionCookie, pItem);

		l.Unlock();

		CallEvent(L"OnSessionStart", NULL, pItem);

		if(m_strSessionDomain.IsEmpty())
			t_pHttpAccept->AddHeader(_T("Set-Cookie"), m_strHostKey + _T('=') + strSessionCookie + _T("; path=/"));
		else t_pHttpAccept->AddHeader(_T("Set-Cookie"), m_strHostKey + _T('=') + strSessionCookie + _T("; domain=") + m_strSessionDomain + _T("; path=/"));
	}
}

void CBoxHttpHost::ClearSession(__int64 nNowTime)
{
	CSingleLock l(&m_cs, TRUE);
	CBoxHttpSession* pSession;

	m_nNowTime = nNowTime;
	nNowTime -= (__int64)600000000 * m_nSessionTimeout;

	while((pSession = m_cacheSessions.GetLast()) != NULL && pSession->m_tmLastTime < nNowTime)
		if(pSession->m_dwRef > 1)
		{
			pSession->m_tmLastTime = m_nNowTime;
			m_cacheSessions.Update(pSession);
		}else
		{
			pSession->ExternalAddRef();
			m_cacheSessions.Remove(pSession);
			CallEvent(L"OnSessionEnd", NULL, pSession);
			pSession->m_pContents->RemoveAll();
			pSession->ExternalRelease();
		}

	CBoxObject<CBoxHttpHost> pHost;
	CComVariant var;
	int i;
	int count = m_pContents->get_Count();

	for(i = 0; i < count; i ++)
	{
		var.Clear();
		m_pContents->GetValue(i, var);
		if(var.vt == VT_DISPATCH)
		{
			pHost = var.pdispVal;
			if(pHost != NULL)
				pHost->ClearSession(m_nNowTime);
		}
	}
}

void CBoxHttpHost::OnStart(void)
{
	if(m_bIsStart)return;
	m_bIsStart = TRUE;

	CSingleLock l(&m_cs, TRUE);

	CallEvent(L"OnApplicationStart", NULL, NULL, TRUE);

	CBoxObject<CBoxHttpHost> pHost;
	CComVariant var;
	int i;
	int count = m_pContents->get_Count();

	for(i = 0; i < count; i ++)
	{
		var.Clear();
		m_pContents->GetValue(i, var);
		if(var.vt == VT_DISPATCH)
		{
			pHost = var.pdispVal;
			if(pHost != NULL)
				pHost->OnStart();
		}
	}
}

void CBoxHttpHost::OnEnd(void)
{
	if(!m_bIsStart)return;
	m_bIsStart = FALSE;

	CSingleLock l(&m_cs, TRUE);

	CBoxObject<CBoxHttpHost> pHost;
	CComVariant var;
	int i;
	int count = m_pContents->get_Count();

	for(i = 0; i < count; i ++)
	{
		var.Clear();
		m_pContents->GetValue(i, var);
		if(var.vt == VT_DISPATCH)
		{
			pHost = var.pdispVal;
			if(pHost != NULL)
				pHost->OnEnd();
		}
	}

	CBoxHttpSession* pSession;

	while((pSession = m_cacheSessions.GetLast()) != NULL)
	{
		pSession->ExternalAddRef();
		m_cacheSessions.Remove(pSession);
		CallEvent(L"OnSessionEnd", NULL, pSession);
		pSession->m_pContents->RemoveAll();
		pSession->ExternalRelease();
	}

	CallEvent(L"OnApplicationEnd", NULL, NULL);
}

BEGIN_DISPATCH_MAP(CBoxHttpHost, CBoxContents)
	DISP_FUNCTION(CBoxHttpHost, "AddDefault", AddDefault, VT_EMPTY, VTS_BSTR)

	DISP_PROPERTY_PARAM(CBoxHttpHost, "Application", get_Application, put_Application, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY(CBoxHttpHost, "EnableScript", m_bEnableScript, VT_BOOL)
	DISP_PROPERTY(CBoxHttpHost, "Language", m_strLanguage, VT_BSTR)
	DISP_PROPERTY(CBoxHttpHost, "EnableDebug", m_bEnableDebug, VT_BOOL)
	DISP_PROPERTY_EX(CBoxHttpHost, "SessionTimeout", get_SessionTimeout, put_SessionTimeout, VT_I4)
	DISP_PROPERTY(CBoxHttpHost, "EnableSession", m_bEnableSession, VT_BOOL)
	DISP_PROPERTY(CBoxHttpHost, "GlobalApplication", m_bGlobalApplication, VT_BOOL)
	DISP_PROPERTY(CBoxHttpHost, "EnableBrowse", m_bEnableBrowse, VT_BOOL)
	DISP_PROPERTY(CBoxHttpHost, "AutoWebDAV", m_bAutoWebDAV, VT_BOOL)
	DISP_PROPERTY(CBoxHttpHost, "BrowseHeader", m_strBrowseHeader, VT_BSTR)
	DISP_PROPERTY(CBoxHttpHost, "BrowseFooter", m_strBrowseFooter, VT_BSTR)
	DISP_PROPERTY(CBoxHttpHost, "EnableWrite", m_bEnableWrite, VT_BOOL)
	DISP_PROPERTY(CBoxHttpHost, "ErrorMessage", m_strErrorMessage, VT_BSTR)
	DISP_PROPERTY_EX(CBoxHttpHost, "EventSink", get_EventSink, put_EventSink, VT_BSTR)
	DISP_PROPERTY(CBoxHttpHost, "SessionDomain", m_strSessionDomain, VT_BSTR)
	DISP_PROPERTY(CBoxHttpHost, "Root", m_strRoot, VT_BSTR)
	DISP_PROPERTY(CBoxHttpHost, "ScriptTimeout", m_nScriptTimeout, VT_I4)

	DISP_PROPERTY_EX(CBoxHttpHost, "ApplicationID", get_ApplicationID, put_ApplicationID, VT_BSTR)

	DISP_FUNCTION(CBoxHttpHost, "AddHeader", AddHeader, VT_EMPTY, VTS_BSTR VTS_BSTR)

	DISP_FUNCTION(CBoxHttpHost, "AddFolder", AddFolder, VT_DISPATCH, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CBoxHttpHost, "AttachFolder", AttachFolder, VT_EMPTY, VTS_BSTR VTS_DISPATCH)

	DISP_FUNCTION(CBoxHttpHost, "AddURLRewriter", AddURLRewriter, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CBoxHttpHost, "URLRewrite", URLRewriteTest, VT_BSTR, VTS_BSTR)
END_DISPATCH_MAP()

// CBoxHttpHost message handlers

void CBoxHttpHost::AddDefault(LPCTSTR pstrDefault)
{
	CString str(pstrDefault);
	str.MakeLower();

	m_astrDefault.Add(str);
}

void CBoxHttpHost::AddHeader(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	m_strAddHeader.Append(pstrName);
	m_strAddHeader.Append(_T(": "), 2);
	m_strAddHeader.Append(pstrValue);
	m_strAddHeader.Append(_T("\r\n"), 2);
}

long CBoxHttpHost::get_SessionTimeout(void)
{
	return m_nSessionTimeout;
}

void CBoxHttpHost::put_SessionTimeout(long nSessionTimeout)
{
	if(nSessionTimeout < 0)
		nSessionTimeout = 0;

	m_nSessionTimeout = nSessionTimeout;
}

VARIANT CBoxHttpHost::get_Application(VARIANT& varKey)
{
	if(varKey.vt == VT_ERROR)
	{
		VARIANT var;

		VariantInit(&var);

		var.vt = VT_DISPATCH;
		var.pdispVal = (LPDISPATCH)m_pApplication;
		m_pApplication->ExternalAddRef();

		return var;
	}else
		return m_pApplication->get_Item(varKey);
}

void CBoxHttpHost::put_Application(VARIANT& varKey, VARIANT& var)
{
	if(varKey.vt == VT_ERROR || var.vt == VT_ERROR)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	m_pApplication->put_Item(varKey, var);
}

BSTR CBoxHttpHost::get_EventSink(void)
{
	return m_strEventSink.AllocSysString();
}

void CBoxHttpHost::put_EventSink(LPCTSTR pstrName)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();
	CBoxPath path;

	if(pNowScript != NULL && pstrName[0] != _T('\\'))
		path.Combine(pNowScript->m_strScriptName.Left(pNowScript->m_strScriptName.ReverseFind(_T('\\')) + 1), pstrName);
	else path.Combine(pstrName);

	m_strEventSink = path.m_strPath;
}

BSTR CBoxHttpHost::get_ApplicationID(void)
{
	return m_strHostKey.AllocSysString();
}

void CBoxHttpHost::put_ApplicationID(LPCTSTR pstrID)
{
	m_strHostKey = m_pApplication->m_strHostKey = pstrID;
}

LPDISPATCH CBoxHttpHost::AddFolder(LPCTSTR pstrName, LPCTSTR pstrRoot)
{
	CBoxObject<CBoxHttpHost> pHost;
	CBPath path;

	pHost.CreateInstance();

	path = pstrRoot;
	path.RemoveBackslash();
	pHost->InitHost(CString(path));

	path = pstrName;
	path.RemoveBackslash();
	m_pContents->SetValue(CString(path), (LPDISPATCH)pHost);
	pHost->ExternalAddRef();
	return pHost;
}

void CBoxHttpHost::AttachFolder(LPCTSTR pstrName, LPDISPATCH pdispHost)
{
	CBoxObject<CBoxHttpHost> pHost;
	CString strName(pstrName);

	pHost = pdispHost;

	if(pHost == NULL || !pHost->IsKindOf(RUNTIME_CLASS(CBoxHttpHost)))
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	strName.Replace('/', '\\');

	m_pContents->SetValue(strName, (LPDISPATCH)pHost);
}

void CBoxHttpHost::AddURLRewriter(LPCTSTR pstrRE, LPCTSTR pstr)
{
	HRESULT hr;
	CAtlRegExp<CURLRECharTraits> *pre;

	pre = new CAtlRegExp<CURLRECharTraits>;
	hr = pre->Parse(pstrRE, false);
	if(FAILED(hr))
	{
		delete pre;
		AfxThrowOleException(hr);
	}
	m_areUrl.Add(pre);
	m_aUrl.Add(pstr);
}

BOOL CBoxHttpHost::URLRewrite(CString strURL, CString &strNewURL, CString &strNewQueryString)
{
	UINT i, c;
	int uNumGroups;
	CAtlREMatchContext<CURLRECharTraits> mc;
	LPCTSTR ptr, ptr1, ptrEnd;
	CString *pstrRewrite;

	c = m_areUrl.GetCount();
	for (i=0;i<c;i++)
		if (m_areUrl[i]->Match(strURL, &mc))
			break;

	if (i==c)
	{
		strNewURL = strURL;
		return false;
	}

	pstrRewrite = &strNewURL;
	uNumGroups = mc.m_uNumGroups;
	ptr = m_aUrl[i];
	ptrEnd = ptr + m_aUrl[i].GetLength();

	while(ptr < ptrEnd)
	{
		ptr1 = ptr;
		while(ptr1 < ptrEnd && *ptr1 != '$' && *ptr1 != '?')
			ptr1 ++;

		pstrRewrite->Append(ptr, ptr1 - ptr);
		ptr = ptr1 + 1;

		if (*ptr1 == '?')
		{
			pstrRewrite = &strNewQueryString;
			continue;
		}

		if(ptr < ptrEnd)
		{
			int ch = *ptr - '1';

			if(ch >= 0 && ch <= 9)
			{
				if (ch<uNumGroups)
				{
					const TCHAR* szStart = 0;
					const TCHAR* szEnd = 0;

					mc.GetMatch(ch, &szStart, &szEnd);
					pstrRewrite->Append(szStart, szEnd - szStart);
				}
				ptr ++;
			}
			else if(*ptr == '0')
			{
				if (0<uNumGroups)
				{
					const TCHAR* szStart = 0;
					const TCHAR* szEnd = 0;

					mc.GetMatch(0, &szStart, &szEnd);
					pstrRewrite->Append(szStart, szEnd - szStart);
				}
				ptr ++;
			}
			else if(*ptr == '$')
			{
				pstrRewrite->AppendChar('$');
				ptr ++;
			}
		}
	}

	return (pstrRewrite == &strNewQueryString);
}

BSTR CBoxHttpHost::URLRewriteTest(LPCTSTR pstr)
{
	CString strURL = pstr, strNewURL, strQueryString, strNewQueryString;

	int p = strURL.ReverseFind('?');
	if (p >= 0)
	{
		strQueryString = strURL.Mid(p+1);
		strURL = strURL.Left(p);
	}

	if (URLRewrite(strURL, strNewURL, strNewQueryString))
	{
		if (strQueryString.GetLength())
			strQueryString.AppendChar('&');
		strQueryString.Append(strNewQueryString);
	}

	if (strQueryString.GetLength())
	{
		strNewURL.AppendChar('?');
		strNewURL.Append(strQueryString);
	}

	return strNewURL.AllocSysString();
}