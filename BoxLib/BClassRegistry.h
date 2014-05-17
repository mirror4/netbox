#pragma once

#include "BThread.h"

class CBClassRegistry
{
public:
	static HRESULT __stdcall GetLocalClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv);
	static HRESULT __stdcall GetClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv);
	static HRESULT __stdcall CreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);
	static HRESULT __stdcall CreateLocalInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);
	static HRESULT __stdcall CLSIDFromProgID(LPCOLESTR lpszProgID, LPCLSID lpclsid);
	static BOOL IsLocalObject(REFCLSID rclsid);
	static HRESULT RegSvr32(LPCWSTR pstrName, int nAction);

	static HRESULT RegisterClass(LPCWSTR lpszProgID, REFCLSID rclsid, IClassFactory* pFactory);
	static void RemoveAll();

private:
	static CRBMap<CBString, CBComPtr<IClassFactory> > m_mapID2Factory;
	static CBCriticalSection m_cs;
};

