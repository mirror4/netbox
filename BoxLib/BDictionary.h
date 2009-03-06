#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-0000-4469-6374-696F6E617279"))
CBDictionary : public CBDispatch<IVariantDictionary>,
					public IPersistStreamInit
{
	DECLARE_CLASS(CBDictionary)

	INTERFACE_BEGIN_EX(CBDispatch<IVariantDictionary>)
		PERSIST_INTERFACE
	INTERFACE_END

public:
	CBDictionary(void) : m_bArrayMode(TRUE)
	{}

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
	// IDispatch
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

public:
	// IVariantDictionary
	STDMETHOD(get_Item)(VARIANT VarKey, VARIANT *pvar);
	STDMETHOD(put_Item)(VARIANT VarKey, VARIANT pvar);
	STDMETHOD(putref_Item)(VARIANT VarKey, VARIANT pvar);
	STDMETHOD(get_Key)(VARIANT VarKey, VARIANT *pvar);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);

	STDMETHOD(Remove)(VARIANT VarKey);
	STDMETHOD(RemoveAll)(void);

	STDMETHOD(get_Items)(VARIANT *pItemsArray);
	STDMETHOD(get_Keys)(VARIANT *pKeysArray);
	STDMETHOD(get_ArrayMode)(VARIANT_BOOL *pval);
	STDMETHOD(put_ArrayMode)(VARIANT_BOOL val);

	STDMETHOD(Add)(VARIANT VarKey, VARIANT var);
	STDMETHOD(Exists)(VARIANT VarKey, VARIANT_BOOL *pExists);
	STDMETHOD(Sort)(VARIANT_BOOL bMode = -1);
	STDMETHOD(Load)(VARIANT VarSrc, short mode = 7);
	STDMETHOD(Save)(VARIANT VarDesc, short mode = 7);
	STDMETHOD(Join)(BSTR bstrKeyDelimiter, BSTR bstrDelimiter, BSTR* pvar);
	STDMETHOD(Split)(BSTR bstrExpression, BSTR bstrKeyDelimiter, BSTR bstrDelimiter);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		ULONG i = 0;
		m_cs.Enter();
		while(count > 0 && pos < (int)m_posArray.GetCount())
		{
			VariantCopy(rgVar, (VARIANT*)&m_posArray[pos]->m_key);
			count --;
			pos ++;
			rgVar ++;
			i ++;
		}
		m_cs.Leave();

		return i;
	}

protected:
	HRESULT putItem(VARIANT* pvarKey, VARIANT* pvar);

	CBCriticalSection m_cs;
	CRBMap<CBVariant, CComVariant> m_mapItems;
	CAtlArray<CRBMap<CBVariant, CComVariant>::CPair*> m_posArray;
	BOOL m_bArrayMode;

public:
	static CBCriticalSection m_csContents;
};

#define DECLARE_CONTENTS \
private: \
	CBComPtr<CBDictionary> m_pContents; \
public: \
	STDMETHOD(get_Value)(VARIANT VarKey, VARIANT *pvar) \
	{	if(VarKey.vt == VT_ERROR) \
			return DISP_E_PARAMNOTOPTIONAL; \
		return get_Contents()->get_Item(VarKey, pvar); \
	} \
	STDMETHOD(put_Value)(VARIANT VarKey, VARIANT pvar) \
	{	return get_Contents()->put_Item(VarKey, pvar);} \
	STDMETHOD(putref_Value)(VARIANT VarKey, VARIANT pvar) \
	{	return get_Contents()->putref_Item(VarKey, pvar);} \
	STDMETHOD(get_Contents)(IVariantDictionary **ppProperties) \
	{	return get_Contents()->QueryInterface(ppProperties);} \
	CBDictionary* get_Contents(void) \
	{if(m_pContents == NULL) \
		{	CBDictionary::m_csContents.Enter(); \
			if(m_pContents == NULL)m_pContents.CreateInstance(); \
			CBDictionary::m_csContents.Leave();} \
		return m_pContents;}

