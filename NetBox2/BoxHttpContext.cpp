// BoxHttpContext.cpp : implementation file
//

#include "stdafx.h"
#include "BoxHttpContext.h"
#include "BoxString.h"
#include "NetBox2.h"
#include "BoxHttpScript.h"
#include "BoxPath.h"
#include "BoxFile.h"
#include <BEncoding.h>

// CBoxHttpContext

IMPLEMENT_DYNAMIC(CBoxHttpContext, CBoxSafeObject)

void CBoxHttpContext::Bind(CBoxHttpAccept* pAccept, CBoxScript* pScript)
{
	if(pAccept)
	{
		m_pAccept = pAccept;
		m_pResponse->Bind(pAccept, pScript);
		m_pRequest->Bind(pAccept);

		pScript->AddNameItem(_T("Server"), GetIDispatch(FALSE));
	}

	pScript->AddNameItem(_T("Response"), m_pResponse);
	pScript->AddNameItem(_T("Request"), m_pRequest);
	pScript->AddNameItem(_T("NetBox"), theApp.m_pSystem);
	pScript->AddNameItem(_T("Application"), m_pAccept->GetApplication());
}

BEGIN_DISPATCH_MAP(CBoxHttpContext, CBoxSafeObject)
	DISP_FUNCTION(CBoxHttpContext, "CreateObject", CreateObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpContext, "HtmlEncode", HtmlEncode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpContext, "UrlEncode", UrlEncode, VT_BSTR, VTS_BSTR)

	DISP_PROPERTY_EX(CBoxHttpContext, "ScriptTimeout", get_ScriptTimeout, put_ScriptTimeout, VT_I4)

	DISP_FUNCTION(CBoxHttpContext, "Execute", Execute, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpContext, "GetLastError", GetLastErrorDISP, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CBoxHttpContext, "MapPath", MapPath, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpContext, "Transfer", Transfer, VT_EMPTY, VTS_BSTR)
END_DISPATCH_MAP()


// CBoxHttpContext message handlers

void CBoxHttpContext::put_ScriptTimeout(long tm)
{

}

long CBoxHttpContext::get_ScriptTimeout(void)
{
	return -1;
}

LPDISPATCH CBoxHttpContext::CreateObject(LPCTSTR pstrClass)
{
	return theApp.m_pSystem->CreateObject(pstrClass);
}

void CBoxHttpContext::Execute(LPCTSTR pstrPath)
{
	if(!pstrPath)return;
	if(m_pAccept == NULL)return;

	CBoxPath path;
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL && pstrPath[0] != _T('\\'))
		path.Combine(pNowScript->m_strScriptName.Left(pNowScript->m_strScriptName.ReverseFind(_T('\\')) + 1), pstrPath);
	else path.Combine(pstrPath);

	pstrPath = path.m_strPath;

	CBoxObject<CBoxScript> pScript;
	size_t len = _tcslen(pstrPath);

	if(len > 4)
	{
		CString str = pstrPath + _tcslen(pstrPath) - 4;

		if(IsBoxFile(str))
			pScript.CreateInstance();
		else if(IsAspFile(str))
		{
			pScript.CreateInstance(RUNTIME_CLASS(CBoxHttpScript));
			((CBoxHttpScript*)(CBoxScript*)pScript)->SetBasePath(m_pAccept->GetBasePath());
		}
	}

	if(pScript != NULL)
	{
		if(pScript->Load(pstrPath) != 0)
			AfxThrowOleDispatchException(0x4005, "The call to Server.Execute failed while loading the page.", 0);

		CBoxScript* pOldScript = m_pResponse->m_pScript;
		m_pResponse->m_pScript = pScript;

		pScript->SetGlobalObjects();

		pScript->Run();

		m_pResponse->m_pScript = pOldScript;
	}else
	{
		CFile *pFile = g_pFile->Open(pstrPath);

		if(pFile)
		{
			char buffer[1024];
			UINT n;

			while(n = pFile->Read(buffer, sizeof(buffer)))
				m_pAccept->WriteToClient(buffer, n);
			delete pFile;
		}else AfxThrowOleDispatchException(0x4005, "The call to Server.Execute failed while loading the page.", 0);
	}
}

LPDISPATCH CBoxHttpContext::GetLastErrorDISP(void)
{
	if(m_pAccept == NULL)return NULL;
	return m_pResponse->m_pScript->GetLastErrorDISP();
}

BSTR CBoxHttpContext::MapPath(LPCTSTR pstrPath)
{
	if(!pstrPath)return NULL;

	CBoxPath path;
	CString str;

	if(m_pAccept == NULL)
	{
		str = g_pFile->m_strBasePath + m_strBasePath;
		path.Combine(str, pstrPath);
	}else if(pstrPath[0] == '\\' || pstrPath[0] == '/')
	{
		str = g_pFile->m_strBasePath + m_pAccept->GetBasePath();
		path.Combine(str, pstrPath);
	}else
	{
		str = g_pFile->m_strBasePath + m_pAccept->GetMapedPath();
		path.Combine(str.Left(str.ReverseFind(_T('\\')) + 1), pstrPath);
	}

	if(path.m_strPath[0])
		return CString(path.m_strPath + 1).AllocSysString();
	return ::SysAllocString(L"");
}

void CBoxHttpContext::Transfer(LPCTSTR pstrPath)
{
	if(!pstrPath)return;

	Execute(pstrPath);
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		pNowScript->Stop(0);
}

BSTR CBoxHttpContext::HtmlEncode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::HtmlEncode(strText);
	return strText.AllocSysString();
}

BSTR CBoxHttpContext::UrlEncode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	strText = CBEncoding::UrlEncode(pstrText, _tcslen(pstrText));

	return strText.AllocSysString();
}
