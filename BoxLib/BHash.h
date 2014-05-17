#pragma once

#include "BVarType.h"
#include "BCollection.h"

class __declspec(uuid("94650000-0000-0000-0000-000048617368"))
CBHash : public CBDispatch<IHash>
{
	DECLARE_CLASS(CBHash)
	APARTMENT_MODE

public:
	CBHash(void) : m_iAlgo(-1)
	{}

public:
	// IHash
	STDMETHOD(get__NewEnum)(IUnknown** ppEnumReturn);
	STDMETHOD(get_Name)(BSTR *pVal);
	STDMETHOD(get_HashSize)(short *pVal);
	STDMETHOD(Create)(BSTR bstrAlgo = L"MD5", VARIANT varKey = VARIANT_ERROR);
	STDMETHOD(Update)(VARIANT varData);
	STDMETHOD(Final)(VARIANT varData, VARIANT *retVal);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count);

private:
	BYTE m_ctx[256];
	int m_iAlgo;
	CBVarPtr m_varKey;
};

