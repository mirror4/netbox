#include "StdAfx.h"
#include "BThread.h"
#include "BMStream.h"

BOOL _is_NT()
{
	static unsigned int s_osver = 0;

	if(s_osver == 0)
		_get_osver(&s_osver);

	return ((s_osver & 0x8000) == 0);
}

__declspec(thread) EXCEPINFO* th_pExcepInfo = NULL;
static CBCriticalSection s_csComObject;
static CLSID CLSID_FreeThreadedMarshaler;
static DWORD s_dwMarshalSize;
static BOOL s_bMarshalInited;
static BYTE s_bufMarshalData[1024];

HRESULT CBComObject::SetErrorInfo(LPCWSTR lpszDescription)
{
	if(th_pExcepInfo != NULL)
	{
		if(th_pExcepInfo->bstrDescription)
		{
			SysFreeString(th_pExcepInfo->bstrDescription);
			th_pExcepInfo->bstrDescription = NULL;
		}
		if(th_pExcepInfo->bstrHelpFile)
		{
			SysFreeString(th_pExcepInfo->bstrHelpFile);
			th_pExcepInfo->bstrHelpFile = NULL;
		}
		if(th_pExcepInfo->bstrSource)
		{
			SysFreeString(th_pExcepInfo->bstrSource);
			th_pExcepInfo->bstrSource = NULL;
		}

		th_pExcepInfo->wCode = 0x4005;
		if(lpszDescription != NULL)
			th_pExcepInfo->bstrDescription = SysAllocString(lpszDescription);
	}

	return DISP_E_EXCEPTION;
}

CBComObject::~CBComObject(void)
{
	if(m_stubThis)
	{
		s_csComObject.Enter();
		m_stubThis->Clear();
		m_stubThis->Release();
		s_csComObject.Leave();
	}
}

STDMETHODIMP CBComObject::QueryClass(REFIID iid, void ** ppvObject) throw()
{
	if(ppvObject == NULL)return E_POINTER;

	if(iid == __uuidof(CBComObject))
	{
		*ppvObject = (CBComObject*)this;
		InternalAddRef();
		return S_OK;
	}else if(iid == __uuidof(IObjectStub))
	{
		s_csComObject.Enter();

		if(m_stubThis == NULL)
			m_stubThis = new _ObjectStub(this);

		m_stubThis->AddRef();
		*ppvObject = m_stubThis;

		s_csComObject.Leave();

		return S_OK;
	}else if((iid == IID_IMarshal) && GetMarshalMode())
	{
		*ppvObject = (IMarshal*)this;
		InternalAddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}

BOOL s_bObjectSafety = false;
// IObjectSafety
STDMETHODIMP CBComObject::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
{
	if (!s_bObjectSafety)
		return E_NOINTERFACE;
	
	if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
		return E_POINTER;

	*pdwSupportedOptions = 3;
	*pdwEnabledOptions   = 1;

	return S_OK;
}

STDMETHODIMP CBComObject::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	if (!s_bObjectSafety)
		return E_NOINTERFACE;

	return S_OK;
}

static void InitMarshalPool(void)
{
	if(!s_bMarshalInited)
	{
		s_csComObject.Enter();

		if(!s_bMarshalInited)
		{
			CBClass<IUnknown> obj;
			CBComPtr<IUnknown> pUnk;
			CBComPtr<IMarshal> pMarshal;
			CBTempStream mStream;

			CoCreateFreeThreadedMarshaler(NULL, &pUnk);
			pMarshal = pUnk;

			if(SUCCEEDED(pMarshal->GetUnmarshalClass(IID_IUnknown, &obj, MSHCTX_INPROC, NULL, MSHLFLAGS_NORMAL, &CLSID_FreeThreadedMarshaler)) &&
                SUCCEEDED(pMarshal->MarshalInterface(&mStream, IID_IUnknown, &obj, MSHCTX_INPROC, NULL, MSHLFLAGS_NORMAL)))
			{
				s_dwMarshalSize = (DWORD)mStream.GetLength();
				mStream.SeekToBegin();
				mStream.Read(s_bufMarshalData, s_dwMarshalSize);
				s_bMarshalInited = TRUE;
			}
		}

		s_csComObject.Leave();
	}
}

BOOL CBComObject::IsFreeThreaded(IUnknown *pUnk)
{
	if(pUnk == NULL)return TRUE;

	InitMarshalPool();

	CBComPtr<IMarshal> pMarshal(pUnk);
	CLSID clsid;

	if(pMarshal == NULL)return FALSE;
	if(FAILED(pMarshal->GetUnmarshalClass(IID_IUnknown, pUnk, MSHCTX_INPROC, NULL, MSHLFLAGS_NORMAL, &clsid)))
		return FALSE;

	return clsid == CLSID_FreeThreadedMarshaler;
}

// IMarshal
STDMETHODIMP CBComObject::GetUnmarshalClass(REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, CLSID *pCid)
{
	if(dwDestContext == MSHCTX_INPROC)
	{
		InitMarshalPool();

		*pCid = CLSID_FreeThreadedMarshaler;
		return S_OK;
	}

	CBComPtr<IMarshal> pMsh;
	HRESULT hr = CoGetStandardMarshal(riid, (LPUNKNOWN)pv, dwDestContext, pvDestContext, mshlflags, &pMsh);
	if (SUCCEEDED(hr))
		hr = pMsh->GetUnmarshalClass(riid, pv, dwDestContext, pvDestContext, mshlflags, pCid);

	return hr;
}

STDMETHODIMP CBComObject::GetMarshalSizeMax(REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, DWORD *pSize)
{
	if(dwDestContext == MSHCTX_INPROC)
	{
		InitMarshalPool();

		*pSize = s_dwMarshalSize;
		return S_OK;
	}

	CBComPtr<IMarshal> pMsh;
	HRESULT hr = CoGetStandardMarshal(riid, (LPUNKNOWN)pv, dwDestContext, pvDestContext, mshlflags, &pMsh);
	if (SUCCEEDED(hr))
		hr = pMsh->GetMarshalSizeMax(riid, pv, dwDestContext, pvDestContext, mshlflags, pSize);

	return hr;
}

STDMETHODIMP CBComObject::MarshalInterface(IStream *pStm, REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags)
{
	if(dwDestContext == MSHCTX_INPROC)
	{
		InitMarshalPool();

		HRESULT hr;

		hr = pStm->Write(s_bufMarshalData, sizeof(void*), NULL);
		if(FAILED(hr))return hr;

		hr = pStm->Write((void*)&pv, sizeof(pv), NULL);
		if(FAILED(hr))return hr;

		if(s_dwMarshalSize > sizeof(pv) * 2)
		{
			hr = pStm->Write(s_bufMarshalData + sizeof(pv) * 2, s_dwMarshalSize - sizeof(pv) * 2, NULL);
			if(FAILED(hr))return hr;
		}

		AddRef();
		return S_OK;
	}

	CBComPtr<IMarshal> pMsh;
	HRESULT hr = CoGetStandardMarshal(riid, (LPUNKNOWN)pv, dwDestContext, pvDestContext, mshlflags, &pMsh);
	if (SUCCEEDED(hr))
		hr = pMsh->MarshalInterface(pStm, riid, pv, dwDestContext, pvDestContext, mshlflags);

	return hr;
}

STDMETHODIMP CBComObject::UnmarshalInterface(IStream *pStm, REFIID riid, void **ppv)
{
	return E_UNEXPECTED;
}

STDMETHODIMP CBComObject::ReleaseMarshalData(IStream *pStm)
{
	return E_UNEXPECTED;
}

STDMETHODIMP CBComObject::DisconnectObject(DWORD dwReserved)
{
	return E_UNEXPECTED;
}

STDMETHODIMP CBComObject::_ObjectStub::QueryInterface(REFIID iid, void ** ppvObject) throw()
{
	if(ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;
	if (iid == IID_IUnknown || iid == __uuidof(IObjectStub))
		*ppvObject = (void*)(IObjectStub*)this;
	else
	{
		HRESULT hr = E_NOINTERFACE;

		if(m_pObject)
		{
			s_csComObject.Enter();
			if(m_pObject && m_pObject->AddRef())
			{
				hr = m_pObject->QueryInterface(iid, ppvObject);
				m_pObject->Release();
			}
			s_csComObject.Leave();
		}

		return hr;
	}
	AddRef();

	return S_OK;
}

//-----------------------  TypeInfo

BSTR _s_bstrBlank = 2 + (BSTR)L"\0\0";
VARIANT s_varError = {VT_ERROR};

void CBTypeInfo::Init(REFIID iid, LPCWSTR pstrName, METHOD_ENTRY* pEntry, UINT nCount, PFNINVOKE pInvoke)
{
	m_pstrName = pstrName;
	m_pEntry = pEntry;
	m_nCount = nCount;
	m_pInvoke = pInvoke;

	for(UINT i = 0; i < nCount; i ++)
		m_pEntry[i].ulNameHash = LHashValOfName(LOCALE_SYSTEM_DEFAULT, pEntry[i].szName);

	ZeroMemory(&m_TypeAttr, sizeof(m_TypeAttr));

	m_TypeAttr.guid = iid;
	m_TypeAttr.memidConstructor = MEMBERID_NIL;
	m_TypeAttr.memidDestructor = MEMBERID_NIL;
	m_TypeAttr.cbSizeInstance = 4;
	m_TypeAttr.typekind = TKIND_DISPATCH;
	m_TypeAttr.cFuncs = 256;
	m_TypeAttr.cImplTypes = 1;
	m_TypeAttr.cbSizeVft = 28;
	m_TypeAttr.cbAlignment = 4;
	m_TypeAttr.wTypeFlags = 4160;
}

STDMETHODIMP CBTypeInfo::GetTypeAttr(TYPEATTR **ppTypeAttr)
{
	*ppTypeAttr = &m_TypeAttr;

	return S_OK;
}

STDMETHODIMP CBTypeInfo::GetTypeComp(ITypeComp **ppTComp)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetFuncDesc(UINT index, FUNCDESC **ppFuncDesc)
{
	if(index < m_nCount)
	{
		*ppFuncDesc = &m_pEntry[index].Function;
		return S_OK;
	}

	if(m_pTypeInfo)
		return m_pTypeInfo->GetFuncDesc(index - m_nCount, ppFuncDesc);

	return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP CBTypeInfo::GetVarDesc(UINT index, VARDESC **ppVarDesc)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetNames(MEMBERID memid, BSTR *rgBstrNames, UINT cMaxNames, UINT *pcNames)
{
	if(rgBstrNames == NULL)
		return E_POINTER;

	if(cMaxNames < 1)
		return E_INVALIDARG;

	for(UINT i = 0; i < m_nCount; i ++)
		if(memid == m_pEntry[i].Function.memid)
		{
			if(pcNames)*pcNames = 1;
			*rgBstrNames = ::SysAllocString(m_pEntry[i].szName);
			return S_OK;
		}

	if(m_pTypeInfo)
		return m_pTypeInfo->GetNames(memid, rgBstrNames, cMaxNames, pcNames);

	return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP CBTypeInfo::GetRefTypeOfImplType(UINT index, HREFTYPE *pRefType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetImplTypeFlags(UINT index, INT *pImplTypeFlags)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetIDsOfNames(LPOLESTR *rgszNames, UINT cNames, MEMBERID *pMemId)
{
	HRESULT hr = S_OK;
	UINT i, j;

	for(i = 0; i < cNames; i ++)
	{
		ULONG ulHashName = LHashValOfName(LOCALE_SYSTEM_DEFAULT, rgszNames[i]);

		for(j = 0; j < m_nCount; j ++)
			if(ulHashName == m_pEntry[j].ulNameHash && !_wcsicmp(rgszNames[i], m_pEntry[j].szName))
			{
				pMemId[i] = m_pEntry[j].Function.memid;
				break;
			}

		if(j == m_nCount)
		{
			if(m_pTypeInfo)
				hr = m_pTypeInfo->GetIDsOfNames(&rgszNames[i], cNames - i, &pMemId[i]);
			else hr = DISP_E_UNKNOWNNAME;
		}
	}

	return hr;
}

STDMETHODIMP CBTypeInfo::Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, 
			VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	HRESULT hr;

	EXCEPINFO* pOldExcepInfo = th_pExcepInfo;

	if(pExcepInfo != NULL)
	{
		ZeroMemory(pExcepInfo, sizeof(EXCEPINFO));
		th_pExcepInfo = pExcepInfo;
	}

	hr = m_pInvoke(pvInstance, memid, wFlags, pDispParams, pVarResult);

	if(th_pExcepInfo != NULL)
	{
		if(hr != DISP_E_EXCEPTION)
		{
			if(th_pExcepInfo->bstrDescription)
			{
				SysFreeString(th_pExcepInfo->bstrDescription);
				th_pExcepInfo->bstrDescription = NULL;
			}

			if(th_pExcepInfo->bstrHelpFile)
			{
				SysFreeString(th_pExcepInfo->bstrHelpFile);
				th_pExcepInfo->bstrHelpFile = NULL;
			}

			if(th_pExcepInfo->bstrSource)
			{
				SysFreeString(th_pExcepInfo->bstrSource);
				th_pExcepInfo->bstrSource = NULL;
			}
		}

		if(FAILED(hr))
		{
			if((hr & 0xffff0000) == ((FACILITY_WIN32 << 16) | 0x80000000))
			{
				if(IS_NT)
				{
					LPWSTR pMsgBuf = NULL;
					if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, hr & 0xffff, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
					{
						th_pExcepInfo->bstrDescription = ::SysAllocString(pMsgBuf);
						LocalFree(pMsgBuf);
					}
				}else
				{
					LPSTR pMsgBuf = NULL;
					if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, hr & 0xffff, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
					{
						th_pExcepInfo->bstrDescription = ::SysAllocString(CBString(pMsgBuf));
						LocalFree(pMsgBuf);
					}
				}

				th_pExcepInfo->scode = hr;
				hr = DISP_E_EXCEPTION;
			}

			if(hr == DISP_E_EXCEPTION && th_pExcepInfo->bstrSource == NULL)
				th_pExcepInfo->bstrSource = ::SysAllocString(m_pstrName);
		}
	}

	th_pExcepInfo = pOldExcepInfo;

	if(hr == DISP_E_MEMBERNOTFOUND && m_pTypeInfo)
	{
		IUnknown* pUnk;

		hr = ((IUnknown*)pvInstance)->QueryInterface(m_pTypeInfo->m_TypeAttr.guid, (void**)&pUnk);
		if(FAILED(hr))return hr;

		hr = m_pTypeInfo->Invoke(pUnk, memid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

		pUnk->Release();

		return hr;
	}

	return hr;
}



STDMETHODIMP CBTypeInfo::GetDocumentation(MEMBERID memid, BSTR *pBstrName, BSTR *pBstrDocString,
        DWORD *pdwHelpContext, BSTR *pBstrHelpFile)
{
	if(pBstrName)*pBstrName = ::SysAllocString(m_pstrName);
	return S_OK;
}

STDMETHODIMP CBTypeInfo::GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR *pBstrDllName,
		BSTR *pBstrName, WORD *pwOrdinal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID *ppv)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::CreateInstance(IUnknown *pUnkOuter, REFIID riid, PVOID *ppvObj)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetMops(MEMBERID memid, BSTR *pBstrMops)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTypeInfo::GetContainingTypeLib(ITypeLib **ppTLib, UINT *pIndex)
{
	return E_NOTIMPL;
}

STDMETHODIMP_(void) CBTypeInfo::ReleaseTypeAttr(TYPEATTR *pTypeAttr)
{
}

STDMETHODIMP_(void) CBTypeInfo::ReleaseFuncDesc(FUNCDESC *pFuncDesc)
{
}

STDMETHODIMP_(void) CBTypeInfo::ReleaseVarDesc(VARDESC *pVarDesc)
{
}
