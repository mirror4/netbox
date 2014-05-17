#pragma once

#include "BStruct.h"
#include "json.h"

class __declspec(uuid("94650000-0000-0000-0000-00004C697374"))
CBListEx : public CBDispatch<IVariantList, CBStruct>
{
	DECLARE_CLASS(CBListEx)

public:
	// IPersist
	STDMETHOD(GetClassID)(CLSID *pClassID);

public:
	// IPersistStream
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);

public:
	// IPersistStreamInit
	STDMETHOD(InitNew)(void);

public:
	// IVariantList
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Items)(VARIANT *pItemsArray);

	STDMETHOD(get_Count)(long *cStrRet);

	STDMETHOD(Add)(VARIANT var);
	STDMETHOD(Insert)(long i, VARIANT var);
	STDMETHOD(Remove)(long i);
	STDMETHOD(RemoveAll)(void);
	STDMETHOD(Sort)(VARIANT_BOOL bMode = -1);
	STDMETHOD(Join)(BSTR bstrDelimiter, BSTR* pvar);
	STDMETHOD(Split)(BSTR bstrExpression, BSTR bstrDelimiter);

	STDMETHOD(toJson)(int intStyle, BSTR* pvar);
	STDMETHOD(fromJson)(BSTR bstrJson);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		ULONG i = 0;

		m_cs.Enter();
		while(count > 0 && pos < (int)m_arrayVariant.GetCount())
		{
			VariantCopy(rgVar, &m_arrayVariant[pos]);
			count --;
			pos ++;
			rgVar ++;
			i ++;
		}
		m_cs.Leave();

		return i;
	}
	
};

