#pragma once

class CBoxFactory : public IClassFactory
{
public:
	CBoxFactory(CRuntimeClass* pClass) :
		m_dwRef(0),
		m_pClass(pClass),
		m_pDisp(NULL)
	{}

	CBoxFactory(CString strScriptName) :
		m_dwRef(0),
		m_pClass(NULL),
		m_pDisp(NULL),
		m_strScriptName(strScriptName)
	{}

	CBoxFactory(IDispatch *pDisp) :
		m_dwRef(0),
		m_pClass(NULL),
		m_pDisp(pDisp)
	{}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_dwRef);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		if (m_dwRef == 0)
			return 0;

		LONG lResult = InterlockedDecrement(&m_dwRef);
		if (lResult == 0)
			delete this;

		return lResult;
	}

	STDMETHODIMP QueryInterface(REFIID iid, LPVOID far* ppvObj)     
	{
		*ppvObj = NULL;

		if (IsEqualIID(iid, IID_IUnknown))
			*ppvObj = (IUnknown*)this;
		else if (IsEqualIID(iid, IID_IClassFactory))
			*ppvObj = (IClassFactory*)this;

		if(*ppvObj)
		{
			AddRef();
			return S_OK;
		}

		return E_NOTIMPL;
	}

	STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj);
	STDMETHOD(LockServer)(BOOL fLock)
	{
		return S_OK;
	}

private:
	long m_dwRef;

	CRuntimeClass* m_pClass;
	CString m_strScriptName;
	IDispatch *m_pDisp;
};

class CBoxStruct : public IDispatch,
					public IObjectSafety
{
public:
	CBoxStruct(CBoxFactory* pFactory) :
		m_dwRef(0),
		m_pFactory(pFactory)
	{}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_dwRef);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		if (m_dwRef == 0)
			return 0;

		LONG lResult = InterlockedDecrement(&m_dwRef);
		if (lResult == 0)
			delete this;

		return lResult;
	}

	STDMETHODIMP QueryInterface(REFIID iid, LPVOID far* ppvObj)     
	{
		*ppvObj = NULL;

		if (IsEqualIID(iid, IID_IUnknown))
			*ppvObj = (IDispatch*)this;
		else if (IsEqualIID(iid, IID_IDispatch))
			*ppvObj = (IDispatch*)this;
		else if (IsEqualIID(iid, IID_IObjectSafety))
			*ppvObj = (IObjectSafety*)this;

		if(*ppvObj)
		{
			AddRef();
			return S_OK;
		}

		return E_NOTIMPL;
	}

	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions);
	STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);

private:
	long m_dwRef;
	CComPtr<CBoxFactory> m_pFactory;
	CAutoVectorPtr<BYTE> m_pStruct;
};

