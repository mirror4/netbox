// BoxSystem.cpp : implementation file
//

#include "stdafx.h"
#include <winver.h>

#include "NetBox2.h"
#include "BoxPath.h"
#include "BoxBinPtr.h"

#include "BoxSystem.h"
#include "BoxZipFile.h"
#include "BoxMemFile.h"

#include "BoxFile.h"

#include "BoxDictionary.h"
#include "BoxHttpServer.h"
#include "BoxScript.h"
#include "BoxScriptContext.h"
#include "BoxScriptObject.h"

#include "BoxJobServer.h"

#include "BoxSocket.h"
#include "BoxTcpServer.h"
#include "BoxSmtp.h"

#include "ZipFile.h"
#include "BoxCachePool.h"

#include <BHook.h>
#include <BPath.h>
#include "BoxFactory.h"

#include <BDictionary.h>
#include <BQueue.h>
#include <BArguments.h>
#include <BListex.h>

#include <BHttpRequest.h>

#include <BFile.h>
#include <BComm.h>
#include <BPipeLine.h>

#include <BDate.h>

#include <BIdentity.h>

#include <BHash.h>
#include <BRSA.h>
#include <BRandom.h>
#include <BCipher.h>

#include <BCodePage.h>

#include <BVarType.h>
#include <BEncoding.h>
#include <BSysInfo.h>
#include <BHtmlWindow.h>

#include <BMessageManager.h>

#include <BUDPSocket.h>

#include <BClassRegistry.h>

#include <BFileSystem.h>

#include <BDatabase.h>
#include <BRecordset.h>

#include <BRegExp.h>

#include <BShellLink.h>

#include <comsvcs.h>
//#include "..\nbr\certinfo.h"

#include <wininet.h>

#include "MemoryModule.h"

// CBoxSystem

void (__stdcall *g_pGetObjectContext)(REFIID riid, LPVOID FAR* ppv);

static TCHAR s_strVersion[90];
static int s_nVarLen;
static CBEncoding s_Encoding;
static CBComPtr<CBSysInfo> s_pSysInfo;

LPCTSTR CBoxSystem::getVersion(void)
{
	return s_strVersion;
}

int CBoxSystem::getVersionLen(void)
{
	return s_nVarLen;
}

IMPLEMENT_DYNAMIC(CBoxSystem, CBoxSafeObject)

#define MK_9465 0x94659465
extern DWORD g_dwAppMark;
extern BYTE APP_MD5[MD5_DIGEST_LENGTH];
extern BYTE PROG_MD5[MD5_DIGEST_LENGTH];
//extern int s_AppDevID, s_BadDevID;

const char* memfind(const void *buf, size_t count, const char* str, size_t scount)
{
	const char *ptr;
	const char *ptr1;

	ptr = (const char*)buf;
	count -= scount - 1;

	while(count && (ptr1 = (const char*)memchr(ptr, str[0], count)))
	{
		if(!memcmp(str, ptr1, scount))
			return ptr1;

		count -= ptr1 - ptr + 1;
		ptr = ptr1 + 1;
	}

	return NULL;
}

CBoxSystem::CBoxSystem(void)
{
	CBHtmlWindow::AddTrustedSite("netbox:/");

	m_pContents.CreateInstance();
	m_pContents->put_ArrayMode(0);

	m_pFolders.CreateInstance();

	WORD *pVersion;

	pVersion = (WORD *)LockResource(LoadResource(NULL, FindResource(NULL, (LPCTSTR)1, RT_VERSION)));

	s_nVarLen = _stprintf(s_strVersion, _T("NetBox Version %c.%c Build %d"), pVersion[25] + '0', pVersion[24] + '0', pVersion[27]);

	*(FARPROC*)&g_pGetObjectContext = GetProcAddress(GetModuleHandle(_T("ole32.dll")), "CoGetObjectContext");

	DWORD *pAppData = (DWORD *)LockResource(LoadResource(NULL, FindResource(NULL, (LPCTSTR)1, "APPDATA")));
	
	if (g_dwAppMark == MK_9465 && pAppData != NULL && (pAppData[0] & 1) == 1)
		if (LoadVBScriptDllFromPackage())
				goto VBScriptLoaded;

	if (LoadScriptDll(L"VBScript", L"Set scriptObject = Err"))
		goto VBScriptLoaded;

	if (LoadVBScriptDllFromFile("vbscript.dll"))
		if(LoadScriptDll(L"VBScript", L"Set scriptObject = Err"))
			goto VBScriptLoaded;

	if (g_dwAppMark == MK_9465)
		if (LoadVBScriptDllFromPackage())
				goto VBScriptLoaded;

	if (theApp.m_bIsShell)
		MessageBox(NULL, "Cannot initializes the scripting engine !!!", getVersion(), MB_ICONSTOP | MB_OK);
	else
		theApp.m_pService->LogEvent(EVENTLOG_ERROR_TYPE, "Cannot initializes the scripting engine !!!");
	ExitProcess(0);

VBScriptLoaded:
	LoadScriptDll(L"JScript", L"var scriptObject = new Object()");

	s_pSysInfo.CreateInstance();

	static char HEX[] = "0123456789ABCDEF";
	char str[41];
	int i, p;

	s_pSysInfo->AppendUserData(L"APP_Version", getVersion());
	s_pSysInfo->AppendUserData(L"APP_Created", (WCHAR*)(memfind(pVersion, 4096, (const char*)L"Release DateTime", 34) + 34));

	for(i = 0, p = 0; i < 16; i ++)
	{
		str[p ++] = HEX[APP_MD5[i] / 16];
		str[p ++] = HEX[APP_MD5[i] % 16];
		if((i == 4) || (i == 6) || (i == 8) || (i == 10))
			str[p ++] = '-';
	}
	str[p] = 0;

	s_pSysInfo->AppendUserData(L"APP_Info", str);

	if(g_dwAppMark == MK_9465)
	{
		for(i = 0, p = 0; i < 16; i ++)
		{
			str[p ++] = HEX[PROG_MD5[i] / 16];
			str[p ++] = HEX[PROG_MD5[i] % 16];
			if((i == 4) || (i == 6) || (i == 8) || (i == 10))
				str[p ++] = '-';
		}
		str[p] = 0;
		s_pSysInfo->AppendUserData(L"PROG_Info", str);

/*		itoa(s_AppDevID, str, 10);
		s_pSysInfo->AppendUserData(L"PROG_Issuer", str);

		if(theApp.m_DevIDS[0] = s_AppDevID)
			theApp.m_nDevCount ++;*/
	}

	if(g_dwAppMark == MK_9465 || !theApp.m_bIsShell)
		CBClassRegistry::RegisterClass(L"", CLSID_ProcessDebugManager, CBComObject::GetClassObject());

	AddClass(L"netbox", GetIDispatch(FALSE));
	AddClass(L"shell", theApp.GetIDispatch(FALSE));

	AddClass(RUNTIME_CLASS(CBoxScript));
	AddClass(RUNTIME_CLASS(CBoxScriptContext));
	AddClass(RUNTIME_CLASS(CBoxScriptObject));

	AddClass(RUNTIME_CLASS(CBoxJobServer));

	AddClass(RUNTIME_CLASS(CBoxSocket));
	AddClass(RUNTIME_CLASS(CBoxTcpServer));
	AddClass(RUNTIME_CLASS(CBoxHttpServer));

	AddClassEx(L"NetBox.Dictionary", CBDictionary);
	AddClassEx(L"NetBox.Queue", CBQueue);
	AddClassEx(L"NetBox.Arguments", CBArguments);
	AddClassEx(L"NetBox.List", CBListEx);

	AddClassEx(L"NetBox.Http", CBHttpRequest);

	AddClassEx(L"NetBox.Stream", CBStream);
	AddClassEx(L"NetBox.File", CBFile);
	AddClassEx(L"NetBox.Comm", CBComm);
	AddClassEx(L"NetBox.PipeLine", CBPipeLine);

	AddClassEx(L"NetBox.Identity", CBIdentity);

	AddClassEx(L"NetBox.Hash", CBHash);
	AddClassEx(L"NetBox.RSA", CBRSA);
	AddClassEx(L"NetBox.Random", CBRandom);
	AddClassEx(L"NetBox.Cipher", CBCipher);

	AddClassEx(L"NetBox.HtmlWindow", CBHtmlWindow);
	AddClassEx(L"NetBox.UDPSocket", CBUDPSocket);
	AddClassEx(L"NetBox.ShellLink", CBShellLink);

	AddClassEx(L"NetBox.MessageManager", CBMessageManager);

#ifndef NETBOXMINI
	AddClassEx(L"NetBox.Database", CBDatabase);
	AddClassEx(L"NetBox.Recordset", CBRecordset);

	AddClassEx(L"NetBox.RegExp", CBRegExp);

	AddClass(RUNTIME_CLASS(CBoxSmtp));
#endif

	m_strConfigFile = (LPCTSTR)g_pFile->m_strAppName;
	m_strConfigFile.ReleaseBuffer(m_strConfigFile.ReverseFind(_T('.')));
	m_strConfigFile += _T(".ini");
}

CBoxSystem::~CBoxSystem(void)
{
	CLockNode* pLockNode;
	CString strKey;

	POSITION pos = m_mapLock.GetStartPosition();
	while(pos)
	{
		m_mapLock.GetNextAssoc(pos, strKey, *(void**)&pLockNode);
		delete pLockNode;
	}

	while(m_FreeLock.GetCount())
		delete m_FreeLock.RemoveHead();

	CBClassRegistry::RemoveAll();

	if(m_pDebugApp != NULL)
	{
		m_pDebugManager->RemoveApplication(m_dwCookie);
		m_pDebugApp.Release();
	}

	s_pSysInfo.Release();
}

BEGIN_DISPATCH_MAP(CBoxSystem, CBoxSafeObject)
	DISP_PROPERTY_PARAM_ID(CBoxSystem, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxSystem, "Contents", get_Contents, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CBoxSystem, "_NewEnum", DISPID_NEWENUM, get__NewEnum, SetNotSupported, VT_UNKNOWN)

	DISP_PROPERTY_EX(CBoxSystem, "Folders", get_Folders, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CBoxSystem, "AddFolder", AddFolder, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CBoxSystem, "RemoveFolder", RemoveFolder, VT_EMPTY, VTS_BSTR)

	DISP_FUNCTION(CBoxSystem, "Lock", varLock, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxSystem, "Unlock", varUnlock, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxSystem, "TryLock", varTryLock, VT_BOOL, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxSystem, "Encoding", get_Encoding, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxSystem, "SysInfo", get_SysInfo, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxSystem, "FileSystem", get_FileSystem, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxSystem, "Scripts", get_Scripts, SetNotSupported, VT_DISPATCH)

	DISP_PROPERTY_EX(CBoxSystem, "Version", get_Version, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxSystem, "ApplicationName", get_ApplicationName, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxSystem, "ApplicationPath", get_ApplicationPath, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxSystem, "ScriptName", get_ScriptName, SetNotSupported, VT_BSTR)
	DISP_FUNCTION(CBoxSystem, "MapPath", MapPath, VT_BSTR, VTS_BSTR)

	DISP_PROPERTY_PARAM(CBoxSystem, "Config", get_Config, put_Config, VT_VARIANT, VTS_VARIANT VTS_VARIANT)
	DISP_PROPERTY(CBoxSystem, "ConfigFile", m_strConfigFile, VT_BSTR)

	DISP_FUNCTION(CBoxSystem, "LoadObject", LoadObject, VT_DISPATCH, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CBoxSystem, "CreateObject", CreateObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CBoxSystem, "RegisterClass", RegisterClass, VT_EMPTY, VTS_BSTR VTS_VARIANT)

	DISP_FUNCTION(CBoxSystem, "SetErrorLog", SetErrorLog, VT_EMPTY, VTS_UNKNOWN)

	DISP_FUNCTION(CBoxSystem, "LoadLibrary", LoadLibrary, VT_EMPTY, VTS_BSTR VTS_VARIANT)

	DISP_FUNCTION(CBoxSystem, "Sleep", Sleep, VT_EMPTY, VTS_I4)

	DISP_FUNCTION(CBoxSystem, "RegisterTrustedSite", RegisterTrustedSite, VT_EMPTY, VTS_BSTR)
	DISP_PROPERTY_EX_ID(CBoxSystem, "ObjectSafety", 1001, get_ObjectSafety, put_ObjectSafety, VT_BOOL)

	DISP_FUNCTION(CBoxSystem, "CallByName", CallByName, VT_VARIANT, VTS_DISPATCH VTS_VARIANT VTS_I2 VTS_VARIANT)
	DISP_FUNCTION(CBoxSystem, "GetIDofName", GetIDofName, VT_I4, VTS_DISPATCH VTS_VARIANT)

	DISP_FUNCTION(CBoxSystem, "ExtractFile", ExtractFile, VT_EMPTY, VTS_BSTR VTS_BSTR)

	DISP_PROPERTY_EX(CNetBox2App, "HttpMaxConnections", get_HttpMaxConnections, put_HttpMaxConnections, VT_I4)
END_DISPATCH_MAP()

// CBoxSystem message handlers

void CBoxSystem::ExtractFile(LPCTSTR pstrName, LPCTSTR pstrDest)
{
	if (((CBoxZipFile *)((CBoxCachePool *)g_pFile)->m_pOldFile)->ExtractFile(pstrName, pstrDest) == 0)
		AfxThrowOleException(HRESULT_FROM_WIN32(GetLastError()));
	return;
}

long CBoxSystem::get_HttpMaxConnections(void)
{
	long nHttpMaxConnections;
	DWORD dwSize = sizeof(nHttpMaxConnections);
	
	if (InternetQueryOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, (LPVOID)&nHttpMaxConnections, &dwSize))
		return nHttpMaxConnections;
	
	AfxThrowOleException(HRESULT_FROM_WIN32(GetLastError()));
}

void CBoxSystem::put_HttpMaxConnections(long nHttpMaxConnections)
{
	if(nHttpMaxConnections < 2)
		nHttpMaxConnections = 2;

	InternetSetOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &nHttpMaxConnections, sizeof(nHttpMaxConnections));
	nHttpMaxConnections *= 2;
	InternetSetOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &nHttpMaxConnections, sizeof(nHttpMaxConnections));
}

long CBoxSystem::GetIDofName(LPDISPATCH pDisp, VARIANT *pName)
{
	DISPID dispid;
	CComDispatchDriver pdisp;
	
	pdisp = pDisp;
	if (pdisp == NULL)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);
	
	while (pName->vt == (VT_VARIANT | VT_BYREF))
		pName = pName->pvarVal;

	if (pName->vt != VT_BSTR)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, &pName->bstrVal, 1, 0, &dispid);
	if (FAILED(hr))
		AfxThrowOleException(hr);

	return dispid;
}

VARIANT CBoxSystem::CallByName(LPDISPATCH pDisp, VARIANT *pName, short sType, VARIANT *pArgs)
{
	DISPID dispid;
	CComDispatchDriver pdisp;
	
	pdisp = pDisp;
	if (pdisp == NULL)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);
	
	while (pName->vt == (VT_VARIANT | VT_BYREF))
		pName = pName->pvarVal;
	
	HRESULT hr;

	switch(pName->vt)
	{
		case VT_I2:
			dispid = pName->iVal;
			break;
		case VT_I4:
			dispid = pName->lVal;
			break;
		case VT_BSTR:
			hr = pdisp->GetIDsOfNames(IID_NULL, &pName->bstrVal, 1, 0, &dispid);
			if (FAILED(hr))
				AfxThrowOleException(hr);
			break;
		default:
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);
	}

	while (pArgs->vt == (VT_VARIANT | VT_BYREF))
		pArgs = pArgs->pvarVal;
	
	DISPPARAMS params;
	params.cNamedArgs = 0;
	params.rgdispidNamedArgs = NULL;
	switch(pArgs->vt)
	{
		case (VT_BYREF|VT_ARRAY|VT_VARIANT):
			if ((*(pArgs->pparray))->cDims != 1)
				AfxThrowOleException(TYPE_E_OUTOFBOUNDS);
			
			params.cArgs = (*(pArgs->pparray))->rgsabound[0].cElements;
			params.rgvarg = (VARIANTARG *)(*(pArgs->pparray))->pvData;
			break;
		case (VT_ARRAY|VT_VARIANT):
			if (pArgs->parray->cDims != 1)
				AfxThrowOleException(TYPE_E_OUTOFBOUNDS);

			params.cArgs = pArgs->parray->rgsabound[0].cElements;
			params.rgvarg = (VARIANTARG *)pArgs->parray->pvData;
			break;
		case VT_ERROR:
			params.cArgs = 0;
			params.rgvarg = NULL;
			break;
		default:
			params.cArgs = 1;
			params.rgvarg = pArgs;
	}
	
	VARIANT varRet;
	EXCEPINFO einfo;

/* Flags for IDispatch::Invoke */
//#define DISPATCH_METHOD         0x1
//#define DISPATCH_PROPERTYGET    0x2
//#define DISPATCH_PROPERTYPUT    0x4
//#define DISPATCH_PROPERTYPUTREF 0x8

	VariantInit(&varRet);

	hr = pdisp->Invoke(dispid, IID_NULL, 0, sType, &params, &varRet, &einfo, NULL);
	if (FAILED(hr))
	{
		if (hr == DISP_E_EXCEPTION)
			AfxThrowOleDispatchException(einfo.wCode, BOX_CW2CT(einfo.bstrDescription), (UINT)einfo.dwHelpContext);
		else
			AfxThrowOleException(hr);
	}

	return varRet;
}

LPDISPATCH CBoxSystem::get_Contents(void)
{
	m_pContents->AddRef();
	return m_pContents;
}

LPDISPATCH CBoxSystem::get_Folders(void)
{
	m_pFolders->AddRef();
	return m_pFolders;
}

LPDISPATCH CBoxSystem::get_FileSystem(void)
{
	g_pFileSystem->AddRef();
	return g_pFileSystem;
}

LPDISPATCH CBoxSystem::get_Scripts(void)
{
	CBComPtr<CBList> pScripts;
	POSITION pos;

	pScripts.CreateInstance();

	CScriptHost::m_csScript.Enter();

	pos = CScriptHost::m_listScript.GetHeadPosition();
	while(pos)
		pScripts->AddValue(CScriptHost::m_listScript.GetNext(pos)->m_value);

	CScriptHost::m_csScript.Leave();

	return pScripts.Detach();
}

void CBoxSystem::SetErrorLog(IUnknown* pUnk)
{
	m_pErrorLog.Release();
	m_pErrorLog = pUnk;
}

CBString CBoxSystem::CBFolders::MapPath(LPCTSTR strKey)
{
	CBPath path(strKey);
	CBString str, str1;
	CComVariant *pvar = NULL;

	if(!m_dict.GetCount())
		return str;

	str1 = path.m_strPath;

	m_cs.Enter();

	if(m_dict.GetCount())
	{
		path.StripPath(TRUE);
		while(!path.IsRoot())
		{
			if(m_dict.Lookup(path.m_strPath, &pvar) == S_OK)
			{
				str = path.m_strPath;
				break;
			}

			path.StripPath(TRUE);
		}
	}

	m_cs.Leave();

	if(!str.IsEmpty())
	{
		str1 = (LPCWSTR)str1 + str.GetLength() + 1;
		str = pvar->bstrVal;
		str.Append(str1);
	}

	return str;
}

void CBoxSystem::AddFolder(LPCTSTR pstrPath, LPCTSTR pstrPathMapped)
{
	CBPath path(pstrPath), PathMapped(pstrPathMapped);

	path.RemoveBackslash();
	PathMapped.AddBackslash();

	m_pFolders->AddValue(path.m_strPath, PathMapped.m_strPath);
}

void CBoxSystem::RemoveFolder(LPCTSTR pstrPath)
{
	CBPath path(pstrPath);

	path.RemoveBackslash();
	m_pFolders->RemoveValue(path.m_strPath);
}

VARIANT CBoxSystem::get_Item(VARIANT& varKey)
{
	while(varKey.vt == (VT_VARIANT | VT_BYREF))
		varKey = *varKey.pvarVal;

	VARIANT var;
	HRESULT hr;

	VariantInit(&var);
	hr = m_pContents->get_Item(varKey, &var);
	if(FAILED(hr))AfxThrowOleException(hr);

	return var;
}

void CBoxSystem::put_Item(VARIANT& varKey, VARIANT& var)
{
	while(varKey.vt == (VT_VARIANT | VT_BYREF))
		varKey = *varKey.pvarVal;

	while(var.vt == (VT_VARIANT | VT_BYREF))
		var = *var.pvarVal;

	HRESULT hr;

	hr = m_pContents->put_Item(varKey, var);
	if(FAILED(hr))AfxThrowOleException(hr);
}

LPUNKNOWN CBoxSystem::get__NewEnum(void)
{
	LPUNKNOWN pEnum = NULL;

	m_pContents->get__NewEnum(&pEnum);
	return pEnum;
}

LPDISPATCH CBoxSystem::get_Encoding(void)
{
	s_Encoding.AddRef();
	return &s_Encoding;
}

LPDISPATCH CBoxSystem::get_SysInfo(void)
{
	s_pSysInfo->AddRef();
	return s_pSysInfo;
}

BSTR CBoxSystem::get_Version(void)
{
	return CString(getVersion(), getVersionLen()).AllocSysString();
}

BSTR CBoxSystem::get_ApplicationName(void)
{
	return g_pFile->m_strAppName.AllocSysString();
}

BSTR CBoxSystem::get_ApplicationPath(void)
{
	return g_pFile->m_strBasePath.AllocSysString();
}

BSTR CBoxSystem::get_ScriptName(void)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		return pNowScript->m_strScriptName.AllocSysString();

	return g_pFile->m_strAppName.AllocSysString();
}

BSTR CBoxSystem::MapPath(LPCTSTR pstrPath)
{
	CBoxPath path;

	path.Combine(pstrPath);

	if(path.m_strPath[0])
		return (g_pFile->m_strBasePath + (path.m_strPath + 1)).AllocSysString();
	else return g_pFile->m_strBasePath.AllocSysString();
}

BOOL CBoxSystem::TryLock(CString strKey)
{
	DWORD dwThreadId = GetCurrentThreadId();

	CSingleLock l(&m_csLock, TRUE);
	CLockNode* pLockNode = NULL;

	strKey.MakeLower();
	m_mapLock.Lookup(strKey, *(void**)&pLockNode);
	if(pLockNode == NULL)
	{
		if(m_FreeLock.GetCount())
			pLockNode = m_FreeLock.RemoveHead();
		else pLockNode = new CLockNode();

		pLockNode->m_csLock.Lock();
		pLockNode->m_dwThreadId = dwThreadId;
		pLockNode->m_dwUserCount = 1;

		m_mapLock.SetAt(strKey, pLockNode);
	}

	if(pLockNode->m_dwThreadId != dwThreadId)
		return FALSE;

	pLockNode->m_dwCount ++;

	l.Unlock();

	return TRUE;
}

void CBoxSystem::Lock(CString strKey)
{
	CLockNode* pLockNode = NULL;
	DWORD dwThreadId = GetCurrentThreadId();

	strKey.MakeLower();

	CSingleLock l(&m_csLock, TRUE);

	m_mapLock.Lookup(strKey, *(void**)&pLockNode);
	if(pLockNode == NULL)
	{
		if(m_FreeLock.GetCount())
			pLockNode = m_FreeLock.RemoveHead();
		else pLockNode = new CLockNode();

		pLockNode->m_csLock.Lock();
		pLockNode->m_dwThreadId = dwThreadId;
		pLockNode->m_dwUserCount = 1;

		m_mapLock.SetAt(strKey, pLockNode);
	}

	if(pLockNode->m_dwThreadId != dwThreadId)
		pLockNode->m_dwUserCount ++;

	l.Unlock();

	if(pLockNode->m_dwThreadId != dwThreadId)
	{
		pLockNode->m_csLock.Lock();
		pLockNode->m_dwThreadId = dwThreadId;
	}

	pLockNode->m_dwCount ++;
}

void CBoxSystem::_unlock(CString& strKey, CLockNode* pLockNode)
{
	pLockNode->m_dwCount --;
	if(pLockNode->m_dwCount == 0)
	{
		pLockNode->m_dwThreadId = 0;
		pLockNode->m_csLock.Unlock();

		pLockNode->m_dwUserCount --;
		if(pLockNode->m_dwUserCount == 0)
		{
			m_mapLock.RemoveKey(strKey);
			if(m_FreeLock.GetCount() > 32)delete pLockNode;
			else m_FreeLock.AddHead(pLockNode);
		}
	}
}

void CBoxSystem::Unlock(CString strKey)
{
	CLockNode* pLockNode = NULL;
	strKey.MakeLower();

	CSingleLock l(&m_csLock, TRUE);

	m_mapLock.Lookup(strKey, *(void**)&pLockNode);
	if(pLockNode == NULL)return;

	if(pLockNode->m_dwThreadId == GetCurrentThreadId())
		_unlock(strKey, pLockNode);
}

void CBoxSystem::ClearLock(void)
{
	DWORD dwThreadId = GetCurrentThreadId();

	CSingleLock l(&m_csLock, TRUE);
	POSITION pos, pos1;
	CLockNode* pLockNode;
	CString strKey;

	pos = m_mapLock.GetStartPosition();
	while(pos && m_mapLock.GetCount())
	{
		pos1 = pos;
		m_mapLock.GetNextAssoc(pos, strKey, *(void**)&pLockNode);

		if(dwThreadId == pLockNode->m_dwThreadId)
		{
			pLockNode->m_dwCount = 1;
			_unlock(strKey, pLockNode);

			pos = pos1;
		}
	}
}

void CBoxSystem::varLock(VARIANT& varKey)
{
	Lock(CBoxBSTR(varKey));
}

void CBoxSystem::varUnlock(VARIANT& varKey)
{
	Unlock(CBoxBSTR(varKey));
}

BOOL CBoxSystem::varTryLock(VARIANT& varKey)
{
	return TryLock(CBoxBSTR(varKey));
}

VARIANT CBoxSystem::get_Config(VARIANT& varSection, VARIANT& varKey)
{
	CString strSection, strKey;
	CString str;
	VARIANT var;
	LPCTSTR pstrString, pstrStringEnd;

	::VariantInit(&var);

	if(varSection.vt == VT_ERROR)
	{
		CStringArray strArray;

		str.ReleaseBuffer(GetPrivateProfileSectionNames(str.GetBuffer(32767), 32767, m_strConfigFile));

		pstrString = str;
		pstrStringEnd = pstrString + str.GetLength();
		while(pstrString < pstrStringEnd)
		{
			strArray.Add(pstrString);
			pstrString += strArray[strArray.GetCount() - 1].GetLength() + 1;
		}

		CComSafeArray<VARIANT> bstrArray;
		VARIANT* pVar;

		bstrArray.Create((ULONG)strArray.GetCount());

		pVar = (VARIANT*)bstrArray.m_psa->pvData;

		for(int i = 0; i < strArray.GetCount(); i ++)
		{
			pVar[i].vt = VT_BSTR;
			pVar[i].bstrVal = strArray[i].AllocSysString();
		}

		var.vt = VT_ARRAY | VT_VARIANT;
		var.parray = bstrArray.Detach();

		return var;
	}else strSection = CBoxBSTR(varSection);

	if(varKey.vt == VT_ERROR)
	{
		CStringArray strArray;
		CString str1;
		int nLen;

		str.ReleaseBuffer(GetPrivateProfileSection(strSection, str.GetBuffer(32767), 32767, m_strConfigFile));

		pstrString = str;
		pstrStringEnd = pstrString + str.GetLength();
		while(pstrString < pstrStringEnd)
		{
			str1 = pstrString;
			nLen = str1.GetLength() + 1;
			str1.ReleaseBuffer(str1.Find(_T('=')));
			strArray.Add(str1);
			pstrString += nLen;
		}

		CComSafeArray<VARIANT> bstrArray;
		VARIANT* pVar;

		bstrArray.Create((ULONG)strArray.GetCount());

		pVar = (VARIANT*)bstrArray.m_psa->pvData;

		for(int i = 0; i < strArray.GetCount(); i ++)
		{
			pVar[i].vt = VT_BSTR;
			pVar[i].bstrVal = strArray[i].AllocSysString();
		}

		var.vt = VT_ARRAY | VT_VARIANT;
		var.parray = bstrArray.Detach();

		return var;
	}else strKey = CBoxBSTR(varKey);

	str.ReleaseBuffer(GetPrivateProfileString(strSection, strKey, _T(""), str.GetBuffer(32767), 32767, m_strConfigFile));

	var.vt = VT_BSTR;
	var.bstrVal = str.AllocSysString();

	return var;
}

void CBoxSystem::put_Config(VARIANT& varSection, VARIANT& varKey, VARIANT& varNewVal)
{
	CString strSection, strKey, strNewVal;

	strSection = CBoxBSTR(varSection);
	strKey = CBoxBSTR(varKey);
	strNewVal = CBoxBSTR(varNewVal);

	WritePrivateProfileString(strSection, strKey.GetLength() ? strKey : NULL, strNewVal.GetLength() ? strNewVal : NULL, m_strConfigFile);
}

void CBoxSystem::AddClass(LPCOLESTR strName, IDispatch *pDisp)
{
	CLSID clsid = {0x65946594, 0x6594, 0x6594};

	*(IDispatch **)&clsid.Data4 = pDisp;

	CBClassRegistry::RegisterClass(strName, clsid, new CBoxFactory(pDisp));
}

void CBoxSystem::AddClass(CRuntimeClass* pRuntimeClass, LPCTSTR pstrNamespace)
{
	CBString strName(L"NetBox.");
	CBString strClassName(pRuntimeClass->m_lpszClassName);

	if(pstrNamespace != NULL)
	{
		strName += pstrNamespace;
		strName += L'.';
	}

	CLSID clsid = {0x65946594, 0x6594, 0x6594};

	*(CRuntimeClass**)&clsid.Data4 = pRuntimeClass;

	strClassName.Delete(0, 4);
	strName += strClassName;

	CBClassRegistry::RegisterClass(strName, clsid, new CBoxFactory(pRuntimeClass));
}

BOOL CBoxSystem::LoadScriptDll(LPCOLESTR pstrName, LPCOLESTR pstrScript)
{
	class _tempHost : public CBClass<IActiveScriptSite>
	{
	public:
		// IActiveScriptSite
		STDMETHOD(GetLCID)(LCID *plcid){return E_NOTIMPL;}
		STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti){return E_NOTIMPL;}
		STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion){return E_NOTIMPL;}
		STDMETHOD(OnScriptTerminate)(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo){return E_NOTIMPL;}
		STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState){return E_NOTIMPL;}
		STDMETHOD(OnScriptError)(IActiveScriptError *pscripterror){return E_NOTIMPL;}
		STDMETHOD(OnEnterScript)(void){return E_NOTIMPL;}
		STDMETHOD(OnLeaveScript)(void){return E_NOTIMPL;}
	};
	
	CLSID clsid;
	HRESULT (__stdcall *scriptDllGetClassObject)(REFCLSID, REFIID, void**) = NULL;
	CComPtr<IClassFactory> pClassFactory;
	CComPtr<IActiveScript> pActiveScript;
	CComQIPtr<IActiveScriptParse> pActiveScriptParse;
	CComDispatchDriver pDisp;
	CComVariant var;
	HRESULT hr;
	_tempHost th;
	MEMORY_BASIC_INFORMATION bsi;
	char FileName[_MAX_PATH];

	//解决UPX IAT截获函数问题 CLSIDFromProgID(pstrName, &clsid);
	hr = CBClassRegistry::CLSIDFromProgID(pstrName, &clsid);
	if(FAILED(hr))return FALSE;

	//解决UPX IAT截获函数问题 pActiveScript.CoCreateInstance(clsid)
	hr = CBClassRegistry::CreateInstance(clsid, NULL, CLSCTX_ALL, IID_IActiveScript, (LPVOID *)&pActiveScript);
	if(FAILED(hr))return FALSE;

	pActiveScriptParse = pActiveScript;
	hr = pActiveScriptParse->InitNew();
	if(FAILED(hr))return FALSE;

	hr = pActiveScript->SetScriptSite(&th);
	if(FAILED(hr))return FALSE;

	hr = pActiveScriptParse->ParseScriptText(pstrScript, 0, 0, NULL, 0, 0, SCRIPTTEXT_ISPERSISTENT, 0, 0);
	if(FAILED(hr))return FALSE;

	hr = pActiveScript->SetScriptState(SCRIPTSTATE_STARTED);
	if(FAILED(hr))return FALSE;

	hr = pActiveScript->GetScriptDispatch(NULL, &pDisp);
	if(FAILED(hr))return FALSE;

	hr = pDisp.GetPropertyByName(L"scriptObject", &var);
	if(FAILED(hr))return FALSE;

	if(!::VirtualQuery(*(void**)(*(DWORD*)var.pdispVal + 24), &bsi, sizeof(bsi)))
		return FALSE;

	if(!GetModuleFileName((HMODULE)bsi.AllocationBase, FileName, _MAX_PATH))
		return FALSE;

	if(::NewLoadLibraryA(FileName) != (HMODULE)bsi.AllocationBase)
		return FALSE;

	scriptDllGetClassObject = (HRESULT (__stdcall *)(REFCLSID, REFIID, void**))GetProcAddress((HMODULE)bsi.AllocationBase, "DllGetClassObject");
	if(scriptDllGetClassObject == NULL)return FALSE;

	hr = scriptDllGetClassObject(clsid, IID_IClassFactory, (void**)&pClassFactory);
	if(FAILED(hr))return FALSE;

	CBClassRegistry::RegisterClass(pstrName, clsid, pClassFactory);

	var.Clear();
	pDisp.Release();
	pActiveScriptParse.Release();
	pActiveScript->InterruptScriptThread(SCRIPTTHREADID_ALL, NULL, 0);
	pActiveScript.Release();
	return TRUE;
}

BOOL CBoxSystem::LoadVBScriptDllFromPackage()
{
	CFile* pFile = g_pFile->Open("\\vbscript.dll");

	if(!pFile || pFile == BOX_FOLDER)
		return FALSE;

	if(!pFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
		return FALSE;

	HRESULT (__stdcall *scriptDllGetClassObject)(REFCLSID, REFIID, void**) = NULL;
	
	HMEMORYMODULE hmod = ::MemoryLoadLibrary(((CMemFile *)pFile)->Detach());
	if(hmod)
	{
		scriptDllGetClassObject = (HRESULT (__stdcall *)(REFCLSID, REFIID, void**))MemoryGetProcAddress(hmod, "DllGetClassObject");
		if(scriptDllGetClassObject)
		{
			CComPtr<IClassFactory> pClassFactory;
			const GUID CLSID_VBSCRIPT = {0xB54F3741L, 0x5B07, 0x11cf, 0xA4, 0xB0, 0x0, 0xaa, 0x0, 0x4A, 0x55, 0xE8};          // {B54F3741-5B07-11cf-A4B0-00AA004A55E8}
			if(SUCCEEDED(scriptDllGetClassObject(CLSID_VBSCRIPT, IID_IClassFactory, (void**)&pClassFactory)))
			{
				CBClassRegistry::RegisterClass(L"VBScript", CLSID_VBSCRIPT, pClassFactory);
				return TRUE;
			}
		}
		MemoryFreeLibrary(hmod);
	}
	return FALSE;
/*
	CBAutoPtr<TCHAR, _MAX_PATH> buffer;
	DWORD dwSize;
	HANDLE hd;

	CBAutoPtr<TCHAR, _MAX_PATH> szTempPath, szTempFile;

	GetTempPath(MAX_PATH, szTempPath);
	if(!GetTempFileName(szTempPath, _T("~"), 0, szTempFile))
	{
		GetSystemDirectory(szTempPath, MAX_PATH);
		GetTempFileName(szTempPath, _T("~"), 0, szTempFile);
	}

	hd = CreateFileA(szTempFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);

	if(hd == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	dwSize = 0;

	while(dwSize = pFile->Read(buffer, sizeof(buffer)))
	{
		if(!WriteFile(hd, buffer, dwSize, &dwSize, NULL))
		{
			CloseHandle(hd);
			DeleteFile(szTempFile);
			return FALSE;
		}
	}

	delete pFile;

	CloseHandle(hd);
	
	return LoadVBScriptDllFromFile(szTempFile);
*/
}

BOOL CBoxSystem::LoadVBScriptDllFromFile(LPCSTR lpcsPath)
{
	HRESULT (__stdcall *scriptDllGetClassObject)(REFCLSID, REFIID, void**) = NULL;
	
	HMODULE hmod = ::NewLoadLibraryA(lpcsPath);
	if(hmod)
	{
		scriptDllGetClassObject = (HRESULT (__stdcall *)(REFCLSID, REFIID, void**))GetProcAddress(hmod, "DllGetClassObject");
		if(scriptDllGetClassObject)
		{
			CComPtr<IClassFactory> pClassFactory;
			const GUID CLSID_VBSCRIPT = {0xB54F3741L, 0x5B07, 0x11cf, 0xA4, 0xB0, 0x0, 0xaa, 0x0, 0x4A, 0x55, 0xE8};          // {B54F3741-5B07-11cf-A4B0-00AA004A55E8}
			if(SUCCEEDED(scriptDllGetClassObject(CLSID_VBSCRIPT, IID_IClassFactory, (void**)&pClassFactory)))
			{
				CBClassRegistry::RegisterClass(L"VBScript", CLSID_VBSCRIPT, pClassFactory);
				return TRUE;
			}
		}
		FreeLibrary(hmod);
	}
	return FALSE;
}

void CBoxSystem::LoadScriptDebugger()
{
	if(g_dwAppMark != MK_9465 && theApp.m_bIsShell)
	{
		m_csLock.Lock();

		if(m_pDebugApp == NULL)
		{
			if(m_pDebugManager == NULL)
				m_pDebugManager.CoCreateInstance(CLSID_ProcessDebugManager);

			//if(m_pDebugManager != NULL)
			//	m_pDebugManager->GetDefaultApplication(&m_pDebugApp);

			if(m_pDebugManager != NULL)
				m_pDebugManager->CreateApplication(&m_pDebugApp);

			if(m_pDebugApp != NULL)
			{
				m_pDebugApp->SetName(L"NetBox");
				m_pDebugManager->AddApplication(m_pDebugApp, &m_dwCookie);
			}
		}

		m_csLock.Unlock();
	}
}

void CBoxSystem::RegisterClass(LPCTSTR pstrName, VARIANT& varMember)
{
	if(varMember.vt != VT_ERROR)
	{
		CBString str;
		HRESULT hr;

		hr = varGetString(varMember, str);
		if(FAILED(hr))AfxThrowOleException(hr);

		CBStruct::CBStructFactory* pFactory = new CBStruct::CBStructFactory(CBString(pstrName), str);
		CBClassRegistry::RegisterClass(CBString(pstrName), pFactory->m_clsid, pFactory);
		pFactory->Release();

		return;
	}

	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();
	CBoxPath path;

	if(pNowScript != NULL && pstrName[0] != _T('\\'))
		path.Combine(pNowScript->m_strScriptName.Left(pNowScript->m_strScriptName.ReverseFind(_T('\\')) + 1), pstrName);
	else path.Combine(pstrName);

	pstrName = path.m_strPath;

	CBoxObject<CBoxScriptObject> pScriptObject;

	pScriptObject.CreateInstance();
	int nError = pScriptObject->Load(pstrName);

	if(nError == 404)
		AfxThrowOleDispatchException(0x4005, _T("File not found : ") + CString(pstrName), 0);

	if(nError != 0)
	{
		CBoxScript::CScriptError &error = pScriptObject->GetErrorMessage();
		CString str;

		str.Format(_T("File \"%s\", Line %d : "), error.m_strFile, error.m_nLine);
		str += error.m_strDescription;

		AfxThrowOleDispatchException(0x4005, str, 0);
	}

	CBString strClassName;
	REFCLSID clsid = pScriptObject->GetClassID();

	strClassName = pScriptObject->GetProgID();

	if(strClassName.IsEmpty() || clsid == GUID_NULL)
		AfxThrowOleDispatchException(0x4005, _T("There are no components with registration info : ") + CString(pstrName), 0);

	CBClassRegistry::RegisterClass(strClassName, clsid, new CBoxFactory(pstrName));
}

static HRESULT (__stdcall *RealCoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR*) = CoCreateInstance;
LPDISPATCH CBoxSystem::CreateObject(LPCTSTR pstrObject)
{
	IDispatch* pUnk;
	CLSID clsid;
	HRESULT hr = CBClassRegistry::CLSIDFromProgID(BOX_CT2CW(pstrObject), &clsid);
	if (SUCCEEDED(hr))
	{
		CComPtr<IClassFactory> pFactory;

		if(CBClassRegistry::GetLocalClassObject(clsid, CLSCTX_ALL, NULL, IID_IClassFactory, (LPVOID*)&pFactory) == S_OK)
			hr = pFactory->CreateInstance(NULL, IID_IDispatch, (void**)&pUnk);
		else
		{
			CComPtr<IObjectContext> pObjectContext;

			if(g_pGetObjectContext != NULL)
				g_pGetObjectContext(IID_IObjectContext, (void**)&pObjectContext);

			if(pObjectContext != NULL)
				hr = pObjectContext->CreateInstance(clsid, IID_IDispatch, (void**)&pUnk);
			else hr = hr = RealCoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IDispatch, (void**)&pUnk);
		}
	}

	if(FAILED(hr))
		AfxThrowOleException(hr);

	return pUnk;
}

void CBoxSystem::staticLoadObject(void* dwParam)
{
	ULONG* pParam = (ULONG*)dwParam;
	CBoxObject<CBoxScriptObject> pScriptObject;

	pScriptObject.CreateInstance();

	pParam[1] = pScriptObject->Load((LPCTSTR)pParam[0]);
	if(pParam[1] == 0)pParam[1] = pScriptObject->Init();

	pParam[0] = (ULONG)pScriptObject.Detach();
}

static CBTls< CRBMap<CString, CBoxObject<CBoxScriptObject> > > th_ObjectPool;

LPDISPATCH CBoxSystem::LoadObject(LPCTSTR pstrFile, VARIANT& varAlone)
{
	CBTlsPtr< CRBMap<CString, CBoxObject<CBoxScriptObject> > > ppthObjectPool(th_ObjectPool);
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();
	CBoxPath path;

	if(pNowScript != NULL && pstrFile[0] != _T('\\'))
		path.Combine(pNowScript->m_strScriptName.Left(pNowScript->m_strScriptName.ReverseFind(_T('\\')) + 1), pstrFile);
	else path.Combine(pstrFile);

	path.toLower();
	pstrFile = path.m_strPath;

	int nError;
	CBoxObject<CBoxScriptObject> pScriptObject;
	CComDispatchDriver pDisp;

	if(varGetNumber(varAlone, VARIANT_FALSE) != VARIANT_FALSE)
	{
		ULONG param[2] = {(ULONG)pstrFile};

		theApp.CallProc(staticLoadObject, param);
		pScriptObject.Attach((CBoxScriptObject*)param[0]);
		nError = param[1];
	}else
	{
		if(ppthObjectPool && ppthObjectPool->Lookup(pstrFile, pScriptObject))
		{
			pDisp.Attach(pScriptObject->get_Object());
			return pDisp.Detach();
		}

		pScriptObject.CreateInstance();

		nError = pScriptObject->Load(pstrFile);
		if(nError == 0)nError = pScriptObject->Init();
	}

	if(nError == 404)
		AfxThrowOleDispatchException(0x4005, _T("File not found : ") + CString(pstrFile), 0);

	if(nError != 0)
	{
		CBoxScript::CScriptError &error = pScriptObject->GetErrorMessage();
		CString str;

		str.Format(_T("File \"%s\", Line %d : "), error.m_strFile, error.m_nLine);
		str += error.m_strDescription;

		AfxThrowOleDispatchException(0x4005, str, 0);
	}

	pDisp.Attach(pScriptObject->get_Object());

	if(ppthObjectPool && pScriptObject->IsPooled())
		ppthObjectPool->SetAt(pstrFile, pScriptObject);

	return pDisp.Detach();
}

void CBoxSystem::LoadLibrary(LPCTSTR pstrFile, VARIANT& varRoot)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();
	CBoxPath path;
	char hexstr[41];
	DWORD dwPackInfo[3];
	CFile *pFile;
//	CCertInfo cert;
	CBDate* pd;
	static char HEX[] = "0123456789ABCDEF";
	static long s_nLibCount = 0;
	CBString str;
	CString strRoot;
	long nLibCount;

	varGetString(varRoot, strRoot);
	path.Combine(strRoot);
	strRoot = path.m_strPath;
	if(!strRoot.Compare(_T("\\")))strRoot.Empty();

	if(pNowScript != NULL && pstrFile[0] != _T('\\'))
		path.Combine(pNowScript->m_strScriptName.Left(pNowScript->m_strScriptName.ReverseFind(_T('\\')) + 1), pstrFile);
	else path.Combine(pstrFile);

	pstrFile = path.m_strPath;

	if((pFile = g_pFile->Open(pstrFile)) && pFile != BOX_FOLDER)
	{
		SYSTEMTIME tm;

		tm = g_pFile->GetFileTime(pFile);

		DWORD nCount = (DWORD)pFile->Seek(-(LONGLONG)sizeof(dwPackInfo), CFile::end) + sizeof(dwPackInfo);
		pFile->Read(dwPackInfo, sizeof(dwPackInfo));

		if(dwPackInfo[2] != MK_9465)
		{
			delete pFile;
			AfxThrowOleException(REGDB_E_CLASSNOTREG);
			return;
		}

		nCount = dwPackInfo[0];

		pFile->SeekToBegin();

		char buf[1024];
		UINT nSize;
		MD5_CTX ctxMD5;
		BYTE ZipKey[MD5_DIGEST_LENGTH];
		BYTE PreDate[MD5_DIGEST_LENGTH];
		BYTE LIB_MD5[MD5_DIGEST_LENGTH];
		int i, p;

		MD5_Init(&ctxMD5);
		do
		{
			if(nCount > sizeof(buf))
				nSize = pFile->Read(buf, sizeof(buf));
			else nSize = pFile->Read(buf, nCount);

			nCount -= nSize;

			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));

		MD5_Final(PreDate, &ctxMD5);

		pFile->Read(ZipKey, MD5_DIGEST_LENGTH);
		for(i = 0; i < MD5_DIGEST_LENGTH; i ++)
			ZipKey[i] ^= PreDate[i];

		MD5_Init(&ctxMD5);
		do
		{
			nSize = pFile->Read(buf, sizeof(buf));
			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));

		MD5_Final(LIB_MD5, &ctxMD5);

		nLibCount = ::InterlockedIncrement(&s_nLibCount) - 1;

		for(i = 0, p = 0; i < 16; i ++)
		{
			hexstr[p ++] = HEX[LIB_MD5[i] / 16];
			hexstr[p ++] = HEX[LIB_MD5[i] % 16];
			if((i == 4) || (i == 6) || (i == 8) || (i == 10))
				hexstr[p ++] = '-';
		}
		hexstr[p] = 0;

		str.Format(L"LIB_%d", nLibCount);
		s_pSysInfo->AppendUserData(str, hexstr);

		pFile->Seek(dwPackInfo[1], CFile::begin);
		CZipFile zipFile;
		long n;
		CBoxMemFile memFile;

		zipFile.SetKey(ZipKey);

		zipFile.Open(pFile, CFile::modeRead);
		while(n = zipFile.Read(buf, sizeof(buf)))
			memFile.Write(buf, n);
		zipFile.Close();

		WCHAR* strBuffer;
		WCHAR* pstr;
		DWORD* pPos;
		BOOL bHasMain = FALSE;

		nSize = (DWORD)memFile.GetLength();

		memFile.SeekToBegin();
		memFile.Read(buf, 512);

//		cert.SetLic(buf);

		strBuffer = new WCHAR[nSize / 2 - 256];
		if(strBuffer)
		{
			CBoxCachePool* pFileSystem = (CBoxCachePool*)g_pFile;
			CString strTemp;

			memFile.Read(strBuffer, nSize - 512);
			pstr = strBuffer;

			pFileSystem->SetCacheNode(strRoot, NULL, tm);

			while(*pstr)
			{
				pPos = (DWORD*)(pstr + wcslen(pstr) + 1);

				_wcslwr(pstr);
				strTemp = pstr;

				if(*pPos != 0xFFFFFFFF)
				{
					CBoxMemFile memTempFile;
					CZipFile zipTempFile;

					zipTempFile.SetKey(ZipKey);

					pFile->Seek(pPos[0], CFile::begin);
					zipTempFile.Open(pFile, CFile::modeRead);
					while(n = zipTempFile.Read(buf, sizeof(buf)))
						memTempFile.Write(buf, n);

					memTempFile.SeekToBegin();

					if(!memTempFile.GetLength())
					{
						memTempFile.SetLength(1);
						memTempFile.SetLength(0);
					}

					pFileSystem->SetCacheNode(strRoot + strTemp, &memTempFile, tm);

					if(!strTemp.Compare("\\main.box"))
						bHasMain = TRUE;

					pPos += 3;
					pd = (CBDate*)pPos;
//					cert.CheckDate(*pd);
					pstr = (WCHAR*)(pd + 1);
				}else
				{
					pFileSystem->SetCacheNode(strRoot + strTemp, NULL, tm);
					pstr = (WCHAR*)(pPos + 1);
				}
			}

			delete strBuffer;

			CBoxScript::clearCache();

			if(bHasMain)
			{
				CBoxObject<CBoxScript> pMainScript;

				pMainScript.CreateInstance();
				if(!pMainScript->Load(strRoot + _T("\\main.box")))
				{
					if(pNowScript)
						pMainScript->SetGlobalObjects();
					else
					{
						pMainScript->AddNameItem(_T("Shell"), (LPDISPATCH)theApp.GetInterface(&IID_IDispatch));
						pMainScript->AddNameItem(_T("NetBox"), theApp.m_pSystem);
					}
					if(pMainScript->Run())
					{
						delete pFile;
						CString strError;

						CBoxScript::CScriptError &error = pMainScript->GetErrorMessage();

						strError += error.m_strFile;
						strError.AppendFormat(_T("[%d]: "), error.m_nLine);
						strError += error.m_strDescription;

						AfxThrowOleDispatchException(0x4005, strError, 0);
					}
				}
			}
		}

		delete pFile;
/*
		if(cert.m_nDevID)
		{
			int i;

			str.Format(L"LIB_Issuer_%d", nLibCount);
			itoa(cert.m_nDevID, hexstr, 10);
			s_pSysInfo->AppendUserData(str, hexstr);

			for(i = 0; i < theApp.m_nDevCount && theApp.m_DevIDS[i] != cert.m_nDevID; i ++);

			if(i == theApp.m_nDevCount && i < 4096)
			{
				theApp.m_DevIDS[i] = cert.m_nDevID;
				theApp.m_nDevCount ++;
			}

			if(!cert.m_bValid)
				s_BadDevID = cert.m_nDevID;
			else if(cert.m_dateTimeout)
			{
				CBDate d;

				d.GetTime();
				if(d > cert.m_dateTimeout)
					s_BadDevID = cert.m_nDevID;
			}
		}else s_BadDevID = -1;*/
	}else AfxThrowOleException(REGDB_E_CLASSNOTREG);
}

void CBoxSystem::Sleep(long nTime)
{
	::Sleep(nTime);
}


void CBoxSystem::RegisterTrustedSite(LPCTSTR pstr)
{
	CBHtmlWindow::AddTrustedSite(pstr);
}

BOOL CBoxSystem::get_ObjectSafety(void)
{
	return s_bObjectSafety;
}

void CBoxSystem::put_ObjectSafety(BOOL b)
{
	s_bObjectSafety = b;
}
