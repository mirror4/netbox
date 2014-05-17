#pragma once

#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <objsafe.h>

#include "blib.h"

#ifndef ASSERT
#define ASSERT(expr) _ASSERTE(expr)
#endif

BOOL _is_NT();

#ifndef IS_NT
#define IS_NT		_is_NT()
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#ifndef hAppInstance
#define	hAppInstance	((HINSTANCE)0x400000)
#endif

typedef CStringT< wchar_t, StrTraitATL< wchar_t, ChTraitsCRT< wchar_t > > > CBString;
typedef CStringT< char, StrTraitATL< char, ChTraitsCRT< char > > > CBStringA;

BOOL inline iscrlf(int ch)
{
	return ch == '\r' || ch == '\n';
}

#define INTERFACE_BEGIN_EX(base_class) \
public: \
	STDMETHOD_(ULONG, AddRef)() throw() \
	{	return InternalAddRef();} \
	STDMETHOD_(ULONG, Release)() throw() \
	{	return InternalRelease();} \
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw() \
	{	if(ppvObject == NULL)return E_POINTER; \
		if(InternalQueryInterface(iid, ppvObject) == S_OK)return S_OK; \
		return base_class::QueryInterface(iid, ppvObject);} \
	template<class Q> HRESULT QueryInterface(Q** pp) \
	{	return QueryInterface(__uuidof(Q), (void **)pp);} \
private: \
	 HRESULT inline InternalQueryInterface(REFIID iid, void ** ppvObject) \
	 {

#define INTERFACE_BEGIN		INTERFACE_BEGIN_EX(CBComObject)

#define INTERFACE_END return E_NOTIMPL;}

#define DECLARE_CLASS_EX(class_name, base_class) \
public: \
	static inline IClassFactory* GetClassObject(void) \
	{	static CBFactory<class_name> s_Factory_##class_name;return &s_Factory_##class_name;} \
	STDMETHOD(QueryClass)(REFIID iid, void ** ppvObject) throw() \
	{	if(ppvObject == NULL)return E_POINTER; \
		if(iid == __uuidof(class_name)){*ppvObject = (void*)this;InternalAddRef();return S_OK;} \
		return base_class::QueryClass(iid, ppvObject);}

#define DECLARE_CLASS(class_name) DECLARE_CLASS_EX(class_name, CBComObject)

#define INTERFACE_DEF_IID(iface_id, iface)		if(iid == iface_id){*ppvObject = (void*)(iface*)this;InternalAddRef();return S_OK;}
#define INTERFACE_DEF(iface)					INTERFACE_DEF_IID(__uuidof(iface), iface)
#define INTERFACE_DEF_AGGREGATE(iface_id, punk)	if(iid == iface_id)return punk->QueryInterface(iface_id, ppvObject);
#define INTERFACE_DEF_EX(iface_id, pfun)	if(iid == iface_id)return pfun(iface_id, ppvObject);

#define STREAM_INTERFACE \
	INTERFACE_DEF(IStream) \
	INTERFACE_DEF(ISequentialStream)

#define PERSIST_INTERFACE \
	INTERFACE_DEF(IPersist) \
	INTERFACE_DEF_IID(IID_IPersistStream, IPersistStreamInit) \
	INTERFACE_DEF(IPersistStreamInit)

MIDL_INTERFACE("94659465-0049-4F62-6A65-637453747562")
IObjectStub : public IUnknown
{};

extern "C" ULONG __stdcall InterlockedAddRef(long* pdwRef);
extern "C" ULONG __stdcall InterlockedRelease(long* pdwRef);

template <class T>
class CBClass : public T
{
public:
	CBClass(void) : m_dwRef(1)
	{}

	virtual ~CBClass(void)
	{}

	static HRESULT GetErrorResult(void)
	{
		return AtlHresultFromLastError();
	}

public:
	// IUnknown
	STDMETHOD_(ULONG, AddRef)() throw()
	{
		return InternalAddRef();
	}

	STDMETHOD_(ULONG, Release)() throw()
	{
		return InternalRelease();
	}

	STDMETHOD(QueryClass)(REFIID iid, void ** ppvObject) throw()
	{
		return E_NOTIMPL;
	}

	template<class Q>
	HRESULT QueryInterface(Q** pp)
	{
		return QueryInterface(__uuidof(Q), (void **)pp);
	}

	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw()
	{
		if(ppvObject == NULL)
			return E_POINTER;

		*ppvObject = NULL;
		if (iid == IID_IUnknown || iid == __uuidof(T))
			*ppvObject = (void*)(T*)this;
		else return QueryClass(iid, ppvObject);
		AddRef();

		return S_OK;
	}

	ULONG InternalAddRef(void)
	{
		return InterlockedAddRef(&m_dwRef);
	}

	ULONG InternalRelease(void)
	{
		long dwResult = InterlockedRelease(&m_dwRef);
		if (dwResult == 0)
			delete this;

		return dwResult;
	}

private:
	long m_dwRef;
};

typedef CBClass<IUnknown> CBObject;

template <class T>
class CBFactory : public CBClass<IClassFactory>
{
public:
	// IClassFactory
	STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
	{
		if(ppvObj == NULL)
			return E_POINTER;

		if(pUnkOuter != NULL)
			return CLASS_E_NOAGGREGATION;

		T* p;

		p = new T;
		if(p == NULL)
			return E_OUTOFMEMORY;

		HRESULT hr = p->QueryInterface(riid, ppvObj);
		p->InternalRelease();

		return hr;
	}

	STDMETHOD(LockServer)(BOOL fLock)
	{
		return S_OK;
	}
};

#define APARTMENT_MODE	virtual BOOL GetMarshalMode(void){return FALSE;}

class __declspec(uuid("94650000-0000-0000-0000-000000000000"))
CBComObject : public CBClass<IObjectSafety>,
			  public IMarshal
{
	INTERFACE_BEGIN_EX(CBClass<IObjectSafety>)
	INTERFACE_END

public:
	CBComObject(void) : m_stubThis(NULL)
	{}

	virtual ~CBComObject(void);

	virtual BOOL GetMarshalMode(void)
	{	return TRUE;}

	static inline IClassFactory* GetClassObject(void)
	{
		static CBFactory<CBComObject> s_Factory_CBComObject;
		return &s_Factory_CBComObject;
	}

	STDMETHOD(QueryClass)(REFIID iid, void ** ppvObject) throw();

public:
	// IObjectSafety
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions);
	STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);

	// IMarshal
    STDMETHOD(GetUnmarshalClass)(REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, CLSID *pCid);
	STDMETHOD(GetMarshalSizeMax)(REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, DWORD *pSize);
	STDMETHOD(MarshalInterface)(IStream *pStm, REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags);
	STDMETHOD(UnmarshalInterface)(IStream *pStm, REFIID riid, void **ppv);
	STDMETHOD(ReleaseMarshalData)(IStream *pStm);
	STDMETHOD(DisconnectObject)(DWORD dwReserved);


public:
	void* GetTypeInfo(void)
	{
		return NULL;
	}

	static HRESULT SetErrorInfo(LPCWSTR lpszDescription);
	static HRESULT SetErrorInfo(LPCSTR lpszDescription)
	{
		return SetErrorInfo(CBString(lpszDescription));
	}

public:
	static BOOL IsFreeThreaded(IUnknown *pUnk);

private:
	class _ObjectStub : public CBClass<IObjectStub>
	{
	public:
		_ObjectStub(CBComObject* pObject) : m_pObject(pObject)
		{}

		void Clear(void)
		{
			m_pObject = NULL;
		}

		STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw();

	private:
		CBComObject *m_pObject;
	};

	_ObjectStub *m_stubThis;
};

extern HRESULT BCLSIDFromProgID(LPCOLESTR lpszProgID, LPCLSID lpclsid);
extern HRESULT BCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);

template <class T>
class CBComPtr
{
public:
	CBComPtr(int nNull = NULL) throw() : p(NULL)
	{
		ASSERT(nNull == 0);
		(void)nNull;
	}

	CBComPtr(T* lp) throw() : p(NULL)
	{
		operator=(lp);
	}

	template <typename Q>
	CBComPtr(Q* lp) throw() : p(NULL)
	{
		operator=(lp);
	}

	CBComPtr(const CBComPtr<T>& lp) throw() : p(NULL)
	{
		operator=(lp);
	}

	template <typename Q>
	CBComPtr(const CComPtrBase<Q>& lp) throw() : p(NULL)
	{
		operator=(lp);
	}

	template <typename Q>
	CBComPtr(const CBComPtr<Q>& lp) throw() : p(NULL)
	{
		operator=(lp);
	}

	~CBComPtr() throw()
	{
		Release();
	}

	HRESULT CreateInstance() throw()
	{
		Attach(new T);

		return S_OK;
	}

	HRESULT CreateInstance(IClassFactory* pClassFactory) throw()
	{
		if(pClassFactory != NULL)
		{
			T *p1 = NULL;
			HRESULT hr = pClassFactory->CreateInstance(NULL, __uuidof(T), (void**)&p1);
			if(FAILED(hr))return hr;

			Attach(p1);

			return S_OK;
		}

		return E_POINTER;
	}

	HRESULT CreateInstance(REFCLSID rclsid) throw()
	{
		T *p1;

		HRESULT hr = BCreateInstance(rclsid, NULL, CLSCTX_ALL, __uuidof(T), (void**)&p1);
		if(FAILED(hr))return hr;

		Attach(p1);

		return S_OK;
	}

	HRESULT CreateInstance(LPCOLESTR szProgID) throw()
	{
		CLSID clsid;
		HRESULT hr = BCLSIDFromProgID(szProgID, &clsid);

		if (SUCCEEDED(hr))
			return CreateInstance(clsid);

		return hr;
	}

	HRESULT GetObject(REFCLSID rclsid) throw()
	{
		IUnknown *p1;

		HRESULT hr = GetActiveObject(rclsid, NULL, (void**)&p1);
		if(FAILED(hr))return hr;

		Attach(p1);

		return S_OK;
	}

	HRESULT GetObject(LPCOLESTR szProgID) throw()
	{
		CLSID clsid;
		HRESULT hr = BCLSIDFromProgID(szProgID, &clsid);

		if (SUCCEEDED(hr))
			return GetObject(clsid);

		return hr;
	}

	template <class Q>
	HRESULT QueryInterface(Q** pp) const throw()
	{
		return QueryInterface(__uuidof(Q), (void**)pp);
	}

	HRESULT QueryInterface(REFIID iid, void ** ppvObject) const throw()
	{
		if(p == NULL)return E_NOTIMPL;
		return p->QueryInterface(iid, (void**)ppvObject);
	}

	BOOL IsFreeThreaded(void)
	{
		return CBComObject::IsFreeThreaded(p);
	}

	T* operator=(int nNull) throw()
	{
		ASSERT(nNull == 0);
		(void)nNull;
		Release();
		return NULL;
	}

	T* operator=(T* lp) throw()
	{
		if(lp != NULL)
			lp->AddRef();

		return Attach(lp);
	}

	template <typename Q>
	T* operator=(Q* lp) throw()
	{
		T *p1 = NULL;
		if(lp != NULL)
		{
			HRESULT hr = lp->QueryInterface(__uuidof(T), (void**)&p1);
			if(FAILED(hr))return NULL;
		}

		return Attach(p1);
	}

	template <typename Q>
	T* operator=(const CComPtrBase<Q>& lp) throw()
	{
		return operator=((IUnknown*)lp);
	}

	T* operator=(const CBComPtr<T>& lp) throw()
	{
		return operator=(lp.p);
	}

	template <typename Q>
	T* operator=(const CBComPtr<Q>& lp) throw()
	{
		return operator=(lp.p);
	}

	operator T*() const throw()
	{
		return p;
	}

	T& operator*() const throw()
	{
		return *p;
	}

	T** operator&() throw()
	{
		return &p;
	}

	bool operator!() const throw()
	{
		return (p == NULL);
	}

	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	T* operator->()
	{
		ASSERT(p!=NULL);
		return p;
	}

	void Release() throw()
	{
		T* pTemp = Detach();
		if(pTemp)pTemp->Release();
	}

	T* Attach(T* p2) throw()
	{
		T* p1 = (T*)InterlockedExchangePointer((void**)&p, p2);
		if(p1)p1->Release();

		return p2;
	}

	template <typename Q>
	T* Attach(Q* lp) throw()
	{
		T *p1 = NULL;
		if(lp != NULL)
		{
			HRESULT hr = lp->QueryInterface(__uuidof(T), (void**)&p1);
			if(FAILED(hr))return NULL;
			p1->Release();
		}

		return Attach(p1);
	}

	T* Detach() throw()
	{
		return (T*)InterlockedExchangePointer((void**)&p, NULL);
	}

// IDispatch specific stuff
	HRESULT GetPropertyByName(LPCOLESTR lpsz, VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar);
		DISPID dwDispID;
		HRESULT hr = GetIDOfName(lpsz, &dwDispID);
		if (SUCCEEDED(hr))
			hr = GetProperty(dwDispID, pVar);
		return hr;
	}

	HRESULT GetProperty(DISPID dwDispID, VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar != NULL);
		if (pVar == NULL)
			return E_POINTER;
		
		if(p == NULL)
			return E_INVALIDARG;
			
		DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
		return p->Invoke(dwDispID, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
				&dispparamsNoArgs, pVar, NULL, NULL);
	}

	HRESULT PutPropertyByName(LPCOLESTR lpsz, VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar);
		DISPID dwDispID;
		HRESULT hr = GetIDOfName(lpsz, &dwDispID);
		if (SUCCEEDED(hr))
			hr = PutProperty(dwDispID, pVar);
		return hr;
	}

	HRESULT PutProperty(DISPID dwDispID, VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar != NULL);
		if (pVar == NULL)
			return E_POINTER;

		if(p == NULL)
			return E_INVALIDARG;

		DISPPARAMS dispparams = {NULL, NULL, 1, 1};
		dispparams.rgvarg = pVar;
		DISPID dispidPut = DISPID_PROPERTYPUT;
		dispparams.rgdispidNamedArgs = &dispidPut;

		if (pVar->vt == VT_UNKNOWN || pVar->vt == VT_DISPATCH || 
			(pVar->vt & VT_ARRAY) || (pVar->vt & VT_BYREF))
		{
			HRESULT hr = p->Invoke(dwDispID, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
				&dispparams, NULL, NULL, NULL);
			if (SUCCEEDED(hr))
				return hr;
		}
		return p->Invoke(dwDispID, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
				&dispparams, NULL, NULL, NULL);
	}

	HRESULT GetIDOfName(LPCOLESTR lpsz, DISPID* pdispid) throw()
	{
		return p->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&lpsz), 1, LOCALE_USER_DEFAULT, pdispid);
	}

	// Invoke a method by DISPID with no parameters
	HRESULT Invoke0(DISPID dispid, VARIANT* pvarRet = NULL) throw()
	{
		DISPPARAMS dispparams = { NULL, NULL, 0, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}

	// Invoke a method by name with no parameters
	HRESULT Invoke0(LPCOLESTR lpszName, VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = Invoke0(dispid, pvarRet);
		return hr;
	}

	// Invoke a method by DISPID with a single parameter
	HRESULT Invoke1(DISPID dispid, VARIANT* pvarParam1, VARIANT* pvarRet = NULL) throw()
	{
		DISPPARAMS dispparams = { pvarParam1, NULL, 1, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}

	// Invoke a method by name with a single parameter
	HRESULT Invoke1(LPCOLESTR lpszName, VARIANT* pvarParam1, VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = Invoke1(dispid, pvarParam1, pvarRet);
		return hr;
	}

	// Invoke a method by DISPID with two parameters
	HRESULT Invoke2(DISPID dispid, VARIANT* pvarParam1, VARIANT* pvarParam2, VARIANT* pvarRet = NULL) throw()
	{
		if(pvarParam1 == NULL || pvarParam2 == NULL)
			return E_INVALIDARG;

		CComVariant varArgs[2] = { *pvarParam2, *pvarParam1 };
		DISPPARAMS dispparams = { &varArgs[0], NULL, 2, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}

	// Invoke a method by name with two parameters
	HRESULT Invoke2(LPCOLESTR lpszName, VARIANT* pvarParam1, VARIANT* pvarParam2, VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = Invoke2(dispid, pvarParam1, pvarParam2, pvarRet);
		return hr;
	}

	// Invoke a method by DISPID with N parameters
	HRESULT InvokeN(DISPID dispid, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL) throw()
	{
		DISPPARAMS dispparams = { pvarParams, NULL, nParams, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}

	// Invoke a method by name with Nparameters
	HRESULT InvokeN(LPCOLESTR lpszName, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = InvokeN(dispid, pvarParams, nParams, pvarRet);
		return hr;
	}

	T* p;
};

typedef CBComPtr<IDispatch> CBDispatchPtr;

template< typename T, size_t size = 0 >
class CBAutoPtr
{
public:
	CBAutoPtr() throw() :
		m_p( NULL )
	{
		if( size )Allocate(size);
	}

	~CBAutoPtr() throw()
	{
		Free();
	}

	operator T*() const throw()
	{
		return( m_p );
	}

	CBAutoPtr< T >& operator=( CBAutoPtr< T >& p ) throw()
	{
		Free();
		Attach( p.Detach() );  // Transfer ownership

		return( *this );
	}

	// Allocate the vector
	bool Allocate( size_t nElements ) throw()
	{
		Free();
		ATLTRY( m_p = new T[nElements] );
		if( m_p == NULL )
		{
			return( false );
		}

		return( true );
	}
	// Attach to an existing pointer (takes ownership)
	void Attach( T* p ) throw()
	{
		Free();
		m_p = p;
	}
	// Detach the pointer (releases ownership)
	T* Detach() throw()
	{
		T* p;

		p = m_p;
		m_p = NULL;

		return( p );
	}
	// Delete the vector pointed to, and set the pointer to NULL
	void Free() throw()
	{
		if(m_p)
		{
			delete[] m_p;
			m_p = NULL;
		}
	}

public:
	T* m_p;
};

typedef HRESULT (*PFNINVOKE)(PVOID, MEMBERID, WORD, DISPPARAMS *, VARIANT *);

class CBTypeInfo : public CBClass<ITypeInfo>
{
public:
	struct METHOD_ENTRY
	{
		LPCWSTR		szName;
		FUNCDESC	Function;
		ULONG		ulNameHash;
	};

public:
	CBTypeInfo()
	{}

	CBTypeInfo(REFIID iid, LPCWSTR pstrName, METHOD_ENTRY* pEntry, UINT nCount, PFNINVOKE pInvoke = NULL)
	{
		Init(iid, pstrName, pEntry, nCount, pInvoke);
	}

	void Init(REFIID iid, LPCWSTR pstrName, METHOD_ENTRY* pEntry, UINT nCount, PFNINVOKE pInvoke = NULL);

	HRESULT GetClassID(CLSID *pClassID)
	{
		*pClassID = m_TypeAttr.guid;
		return S_OK;
	}

public:
	// ITypeInfo
	STDMETHOD(GetTypeAttr)(TYPEATTR **ppTypeAttr);
	STDMETHOD(GetTypeComp)(ITypeComp **ppTComp);
	STDMETHOD(GetFuncDesc)(UINT index, FUNCDESC **ppFuncDesc);
	STDMETHOD(GetVarDesc)(UINT index, VARDESC **ppVarDesc);
	STDMETHOD(GetNames)(MEMBERID memid, BSTR *rgBstrNames, UINT cMaxNames, UINT *pcNames);
	STDMETHOD(GetRefTypeOfImplType)(UINT index, HREFTYPE *pRefType);
	STDMETHOD(GetImplTypeFlags)(UINT index, INT *pImplTypeFlags);
	STDMETHOD(GetIDsOfNames)(LPOLESTR *rgszNames, UINT cNames, MEMBERID *pMemId);
	STDMETHOD(Invoke)(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, 
			VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	STDMETHOD(GetDocumentation)(MEMBERID memid, BSTR *pBstrName, BSTR *pBstrDocString,
        DWORD *pdwHelpContext, BSTR *pBstrHelpFile);
	STDMETHOD(GetDllEntry)(MEMBERID memid, INVOKEKIND invKind, BSTR *pBstrDllName,
		BSTR *pBstrName, WORD *pwOrdinal);
	STDMETHOD(GetRefTypeInfo)(HREFTYPE hRefType, ITypeInfo **ppTInfo);
	STDMETHOD(AddressOfMember)(MEMBERID memid, INVOKEKIND invKind, PVOID *ppv);
	STDMETHOD(CreateInstance)(IUnknown *pUnkOuter, REFIID riid, PVOID *ppvObj);
	STDMETHOD(GetMops)(MEMBERID memid, BSTR *pBstrMops);
	STDMETHOD(GetContainingTypeLib)(ITypeLib **ppTLib, UINT *pIndex);
	STDMETHOD_(void, ReleaseTypeAttr)(TYPEATTR *pTypeAttr);
	STDMETHOD_(void, ReleaseFuncDesc)(FUNCDESC *pFuncDesc);
	STDMETHOD_(void, ReleaseVarDesc)(VARDESC *pVarDesc);

private:
	METHOD_ENTRY* m_pEntry;
	UINT m_nCount;
	LPCWSTR m_pstrName;
	TYPEATTR m_TypeAttr;
	PFNINVOKE m_pInvoke;

public:
	CBTypeInfo* m_pTypeInfo;
};

MIDL_INTERFACE("94659465-0000-0049-4469-737053747562")
IDispatchStub : public IDispatch
{};

class CBDispatchStub : public CBComObject,
					public IDispatchStub
{
	INTERFACE_BEGIN
		INTERFACE_DEF(IDispatchStub)
		INTERFACE_DEF_IID(IID_IDispatch, IDispatchStub)
	INTERFACE_END

public:

	template <class T>
	CBDispatchStub(T *pThis) : m_pThis(pThis)
	{}

	STDMETHOD(QueryClass)(REFIID iid, void ** ppvObject) throw()
	{
		if(ppvObject == NULL)return E_POINTER;
		return m_pThis->QueryInterface(iid, ppvObject);
	}

	// IDispatch
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
	{
		CBDispatchPtr pThis(m_pThis);

		if(pThis)return pThis->GetTypeInfoCount(pctinfo);

		return E_NOTIMPL;
	}

	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		CBDispatchPtr pThis(m_pThis);

		if(pThis)return pThis->GetTypeInfo(iTInfo, lcid, ppTInfo);

		return E_NOTIMPL;
	}

	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		CBDispatchPtr pThis(m_pThis);

		if(pThis)return pThis->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);

		return E_NOTIMPL;
	}

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		CBDispatchPtr pThis(m_pThis);

		if(pThis)return pThis->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

		return E_NOTIMPL;
	}

private:
	CBComPtr<IObjectStub> m_pThis;
};

template <class T, class baseT = CBComObject>
class CBDispatch : public baseT,
				public T
{
	INTERFACE_BEGIN_EX(baseT)
		INTERFACE_DEF_IID(IID_IDispatch, T)
		INTERFACE_DEF(T)
		INTERFACE_DEF_EX(__uuidof(IDispatchStub), getDispStub)
	INTERFACE_END

public:
	CBDispatch(void)
	{
		if(CBDispatch<T>::g_typeinfo.m_pTypeInfo == NULL && (CBTypeInfo*)baseT::GetTypeInfo())
			CBDispatch<T>::g_typeinfo.m_pTypeInfo = (CBTypeInfo*)baseT::GetTypeInfo();
	}

	CBTypeInfo* GetTypeInfo(void)
	{
		return &CBDispatch<T>::g_typeinfo;
	}

	HRESULT getDispStub(REFIID iid, void ** ppvObject)
	{
		*ppvObject = (IDispatchStub*) new CBDispatchStub(this);
		return S_OK;
	}

public:
	// IDispatch
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
	{
		if(pctinfo == NULL)
			return E_POINTER;

		*pctinfo = 1;
		return S_OK;
	}

	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		if(ppTInfo == NULL)
			return E_POINTER;

		if(iTInfo == 0)
			return CBDispatch<T>::g_typeinfo.QueryInterface(ppTInfo);

		return E_NOTIMPL;
	}

	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		return CBDispatch<T>::g_typeinfo.GetIDsOfNames(rgszNames, cNames, rgDispId);
	}

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		return CBDispatch<T>::g_typeinfo.Invoke((T*)this, dispIdMember, wFlags, pDispParams,
			pVarResult, pExcepInfo, puArgErr);
	}

public:
	static CBTypeInfo g_typeinfo;
};

const VARIANT VARIANT_ERROR = {VT_ERROR};
