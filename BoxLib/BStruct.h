#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-0000-0000-0042-537472756374"))
CBStruct : public CBDispatch<IVariantStruct>,
				public IPersistStreamInit
{
	DECLARE_CLASS(CBStruct)

	INTERFACE_BEGIN_EX(CBDispatch<IVariantStruct>)
		PERSIST_INTERFACE
	INTERFACE_END

public:
	class CBStructFactory : public CBClass<IClassFactory>
	{
	private:
		class CBStructTypeInfo : public CBTypeInfo
		{
		public:
			CLSID Init(LPCWSTR pstrName, LPCWSTR pstrInit);

			STDMETHOD(Invoke)(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, 
					VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

		public:
			CAtlArray<CBString> m_arrarName;
			CAtlArray<METHOD_ENTRY> m_arrayMethod;
		};

	public:
		CBStructFactory(LPCWSTR pstrName, LPCWSTR pstrInit);
	public:
		// IClassFactory
		STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj);
		STDMETHOD(LockServer)(BOOL fLock)
		{
			return S_OK;
		}

		CLSID m_clsid;
		CBComPtr<CBStructTypeInfo> m_pTypeInfo;
	};

public:
	CBStruct(void)
	{}

	~CBStruct(void);

public:
	// IPersist
	STDMETHOD(GetClassID)(CLSID *pClassID);

public:
	// IPersistStream
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

public:
	// IPersistStreamInit
	STDMETHOD(InitNew)(void);

public:
	// IVariantStruct
	STDMETHOD(get_Item)(long id, VARIANT *pvar);
	STDMETHOD(put_Item)(long id, VARIANT pvar);
	STDMETHOD(putref_Item)(long id, VARIANT pvar);

	STDMETHOD(Load)(VARIANT VarSrc, short mode = 7);
	STDMETHOD(Save)(VARIANT VarDesc, short mode = 7);


	void setCount(UINT n)
	{
		m_arrayVariant.SetCount(n);
		ZeroMemory(&m_arrayVariant[0], sizeof(VARIANT) * n);
	}

private:
	void ClearAll(void);

protected:
	CBCriticalSection m_cs;
	CAtlArray<VARIANT> m_arrayVariant;
};
