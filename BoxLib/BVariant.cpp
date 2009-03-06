#include "StdAfx.h"
#include "BVariant.h"
#include "BVarType.h"
#include "BMStream.h"
#include "BFile.h"
#include "BClassRegistry.h"

CBCriticalSection CBClassRegistry::m_cs;
CRBMap<CBString, CBComPtr<IClassFactory> > CBClassRegistry::m_mapID2Factory;

HRESULT __stdcall CBClassRegistry::GetLocalClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv)
{
	LPOLESTR ppsz = NULL;
	CBComPtr<IClassFactory> pFactory;
	HRESULT hr;

	hr = ::StringFromCLSID(rclsid, &ppsz);
	if(FAILED(hr))return hr;

	m_cs.Enter();
	m_mapID2Factory.Lookup(ppsz, pFactory);
	m_cs.Leave();

	CoTaskMemFree(ppsz);

	if(pFactory != NULL)
		return pFactory->QueryInterface(riid, ppv);

	return CO_E_CLASSSTRING;
}

BOOL CBClassRegistry::IsLocalObject(REFCLSID rclsid)
{
	LPOLESTR ppsz = NULL;
	CBComPtr<IClassFactory> pFactory;
	HRESULT hr;

	hr = ::StringFromCLSID(rclsid, &ppsz);
	if(FAILED(hr))FALSE;

	m_cs.Enter();
	m_mapID2Factory.Lookup(ppsz, pFactory);
	m_cs.Leave();

	CoTaskMemFree(ppsz);

	return pFactory != NULL;
}

static HRESULT (__stdcall *RealCoGetClassObject)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID FAR*) = CoGetClassObject;
HRESULT __stdcall CBClassRegistry::GetClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hr;

	hr = GetLocalClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);
	if(SUCCEEDED(hr))return hr;

	return RealCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);
}

HRESULT __stdcall CBClassRegistry::CreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	CComPtr<IClassFactory> pFactory;
	HRESULT hr;

	hr = GetClassObject(rclsid, dwClsContext, NULL, IID_IClassFactory, (LPVOID*)&pFactory);
	if(FAILED(hr))return hr;

	return pFactory->CreateInstance(pUnkOuter, riid, ppv);
}

HRESULT __stdcall CBClassRegistry::CreateLocalInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	CComPtr<IClassFactory> pFactory;
	HRESULT hr;

	hr = GetLocalClassObject(rclsid, dwClsContext, NULL, IID_IClassFactory, (LPVOID*)&pFactory);
	if(FAILED(hr))return hr;

	return pFactory->CreateInstance(pUnkOuter, riid, ppv);
}

static __declspec(thread) CAtlArray< CBComPtr<IPersistStream> >* th_pObjects;
static __declspec(thread) short th_nMode;

#define MODE_OBJ		1
#define MODE_EXTOBJ		2
#define MODE_MULTOBJ	4

void SetPersistMode(short mode)
{
	th_nMode = mode;
}

HRESULT WriteObjectToStream(IUnknown* pUnk, IStream* pStream, BOOL bWriteClassID)
{
	CBAutoPtr< CAtlArray< CBComPtr<IPersistStream> > > pObjects;

	if (pUnk != NULL)
	{
		CBComPtr<IPersistStream> spStream;

		HRESULT hr = pUnk->QueryInterface(IID_IPersistStream, (void**)&spStream);
		if (FAILED(hr))
		{
			hr = pUnk->QueryInterface(IID_IPersistStreamInit, (void**)&spStream);
			if (FAILED(hr))return hr;
		}

		CLSID clsid;

		hr = spStream->GetClassID(&clsid);
		if(FAILED(hr))return hr;

		if(th_pObjects != NULL)
		{
			if(!(th_nMode & MODE_OBJ))
				return E_ACCESSDENIED;

			if(!(th_nMode & MODE_EXTOBJ) && !CBClassRegistry::IsLocalObject(clsid))
				return E_ACCESSDENIED;

			size_t i;

			for(i = 0; i < th_pObjects->GetCount(); i ++)
				if((*th_pObjects)[i] == spStream)
				{
					if(!(th_nMode & MODE_MULTOBJ) || !bWriteClassID)
						return CBComObject::SetErrorInfo(L"Duplicate object.");

					hr = WriteClassStm(pStream, IID_IPersistStream);
					if(FAILED(hr))return hr;

					return pStream->Write(&i, sizeof(size_t), NULL);
				}
		}

		if(bWriteClassID)
		{
			if(!(th_nMode & MODE_OBJ))
				return E_ACCESSDENIED;

			if(!(th_nMode & MODE_EXTOBJ) && !CBClassRegistry::IsLocalObject(clsid))
				return E_ACCESSDENIED;

			hr = WriteClassStm(pStream, clsid);
			if(FAILED(hr))return hr;
		}

		if(th_pObjects == NULL)
		{
			if(!pObjects.Allocate(1))
				return E_OUTOFMEMORY;

			th_pObjects = pObjects;
		}

		th_pObjects->Add(spStream);

		hr = spStream->Save(pStream, TRUE);

		if(pObjects != NULL)
			th_pObjects = NULL;

		return hr;
	}

	return WriteClassStm(pStream, CLSID_NULL);
}

HRESULT LoadObjectFromStream(IStream* pStream, REFIID riid, void ** ppvObj)
{
	CBAutoPtr< CAtlArray< CBComPtr<IPersistStream> > > pObjects;
	HRESULT hr;

	if(*ppvObj == NULL)
	{
		CLSID clsid;

		hr = ReadClassStm(pStream, &clsid);
		if(FAILED(hr))return hr;

		if(clsid == CLSID_NULL)
		{
			*ppvObj = NULL;
			return S_OK;
		}

		if(!(th_nMode & MODE_OBJ))
			return E_ACCESSDENIED;

		if(clsid == IID_IPersistStream)
		{
			if(!(th_nMode & MODE_MULTOBJ))
				return CBComObject::SetErrorInfo(L"Duplicate object.");

			size_t i;

			hr = ReadStream(pStream, &i, sizeof(size_t));
			if(FAILED(hr))return hr;

			if(th_pObjects == NULL || i >= th_pObjects->GetCount())
				return DISP_E_UNKNOWNINTERFACE;

			return (*th_pObjects)[i]->QueryInterface(riid, ppvObj);
		}

		if(th_nMode & MODE_EXTOBJ)
			hr = CBClassRegistry::CreateInstance(clsid, NULL, CLSCTX_ALL, riid, ppvObj);
		else
			hr = CBClassRegistry::CreateLocalInstance(clsid, NULL, CLSCTX_ALL, riid, ppvObj);

		if(FAILED(hr))return hr;
	}

	CBComPtr<IPersistStream> spStream;

	hr = ((IUnknown*)*ppvObj)->QueryInterface(IID_IPersistStreamInit, (void**)&spStream);
	if (FAILED(hr))
	{
		hr = ((IUnknown*)*ppvObj)->QueryInterface(IID_IPersistStream, (void**)&spStream);
		if (FAILED(hr))return hr;
	}else
		((IPersistStreamInit*)spStream.p)->InitNew();

	if(th_pObjects == NULL)
	{
		if(!pObjects.Allocate(1))
			return E_OUTOFMEMORY;

		th_pObjects = pObjects;
	}

	th_pObjects->Add(spStream);

	hr = spStream->Load(pStream);

	if(pObjects != NULL)
		th_pObjects = NULL;

	return hr;
}

HRESULT WriteObjectToVariant(IDispatch* pObject, VARIANT* pvar)
{
	CBTempStream mStream;
	HRESULT hr;

	SetPersistMode(7);
	hr = WriteObjectToStream(pObject, &mStream);
	if(FAILED(hr))return hr;

	return mStream.GetVariant(pvar);
}
