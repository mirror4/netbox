#include "StdAfx.h"
#include "BHook.h"
#include "BPath.h"
#include "BCollection.h"
#include "BClassRegistry.h"

HRESULT BCLSIDFromProgID(LPCOLESTR lpszProgID, LPCLSID lpclsid)
{
	return CBClassRegistry::CLSIDFromProgID(lpszProgID, lpclsid);
}

HRESULT BCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	return CBClassRegistry::CreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

static CRBMap<CBString, CLSID> m_mapProg2ID;

static HRESULT (__stdcall *RealCLSIDFromProgID)(LPCOLESTR, LPCLSID) = CLSIDFromProgID;
HRESULT __stdcall CBClassRegistry::CLSIDFromProgID(LPCOLESTR lpszProgID, LPCLSID lpclsid)
{
	if(*lpszProgID)
	{
		BOOL bFind;
		CBString str(lpszProgID);

		str.MakeLower();

		m_cs.Enter();
		bFind = m_mapProg2ID.Lookup(str, *lpclsid);
		m_cs.Leave();

		if(bFind)return S_OK;
	}

	HRESULT hr = RealCLSIDFromProgID(lpszProgID, lpclsid);
	if(FAILED(hr))
		hr = CLSIDFromString((LPOLESTR)lpszProgID, lpclsid);

	return hr;
}
static CBHook m_CLSIDFromProgID(CLSIDFromProgID, CBClassRegistry::CLSIDFromProgID);

static HRESULT __stdcall NewCLSIDFromProgIDEx(LPCOLESTR lpszProgID, LPCLSID lpclsid)
{
	return CBClassRegistry::CLSIDFromProgID(lpszProgID, lpclsid);
}
static CBHook m_CLSIDFromProgIDEx(_T("OLE32.DLL"), _T("CLSIDFromProgIDEx"), NewCLSIDFromProgIDEx);
static CBHook m_CoGetClassObject(CoGetClassObject, CBClassRegistry::GetClassObject);
static CBHook m_CoCreateInstance(CoCreateInstance, CBClassRegistry::CreateInstance);

HRESULT CBClassRegistry::RegisterClass(LPCWSTR lpszProgID, REFCLSID rclsid, IClassFactory* pFactory)
{
	LPOLESTR ppsz = NULL;
	HRESULT hr;

	hr = ::StringFromCLSID(rclsid, &ppsz);
	if(FAILED(hr))return hr;

	m_cs.Enter();

	if(lpszProgID)
	{
		CBString str(lpszProgID);

		str.MakeLower();

		//ASSERT(!m_mapProg2ID.Lookup(str));
		m_mapProg2ID.SetAt(str, rclsid);
	}

	//ASSERT(!m_mapID2Factory.Lookup(ppsz));
	m_mapID2Factory.SetAt(ppsz, pFactory);

	m_cs.Leave();

	CoTaskMemFree(ppsz);

	return S_OK;
}

extern "C" HANDLE __stdcall NewCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
extern "C" HANDLE __stdcall NewCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

HRESULT CBClassRegistry::RegSvr32(LPCWSTR pstrName, int nAction)
{
	HMODULE hMod;

	if(IS_NT)
	{
		hMod = ::LoadLibraryExW(pstrName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if(!hMod)
		{
			CloseHandle(NewCreateFileW(pstrName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0));
			hMod = ::LoadLibraryExW(pstrName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		}

		if(!hMod)return AtlHresultFromLastError();
	}else
	{
		CBStringA strName(pstrName);

		hMod = ::LoadLibraryExA(strName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if(!hMod)
		{
			CloseHandle(NewCreateFileA(strName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0));
			hMod = ::LoadLibraryExA(strName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		}

		if(!hMod)return AtlHresultFromLastError();
	}

	CBHook::DoHook();

	FARPROC proc = ::GetProcAddress(hMod, nAction ? "DllRegisterServer" : "DllUnregisterServer");
	if(!proc)return AtlHresultFromLastError();

	proc();

	FreeLibrary(hMod);

	return S_OK;
}

void CBClassRegistry::RemoveAll()
{
	m_cs.Enter();
	m_mapProg2ID.RemoveAll();
	m_mapID2Factory.RemoveAll();
	m_cs.Leave();
}

