#pragma once

#include "BCollection.h"
#include "BDate.h"

class __declspec(uuid("94650000-0000-0000-0000-436F6F6B6965"))
CBCookie : public CBDispatch<IReadCookie>
{
	DECLARE_CLASS(CBCookie)
	APARTMENT_MODE

	INTERFACE_BEGIN_EX(CBDispatch<IReadCookie>)
		INTERFACE_DEF_AGGREGATE(IID_IWriteCookie, m_pWriteCookie)
	INTERFACE_END

private:
	class __declspec(uuid("94650000-0057-7269-7465-436F6F6B6965"))
	CBWriteCookie : public CBDispatch<IWriteCookie>
	{
		DECLARE_CLASS(CBWriteCookie)
		APARTMENT_MODE

		INTERFACE_BEGIN_EX(CBDispatch<IWriteCookie>)
			INTERFACE_DEF_AGGREGATE(IID_IReadCookie, m_pCookie)
		INTERFACE_END

	public:
		// IWriteCookie
		STDMETHOD(put_Item)(VARIANT Key, BSTR rhs);
		STDMETHOD(put_Expires)(DATE rhs);
		STDMETHOD(put_Domain)(BSTR rhs);
		STDMETHOD(put_Path)(BSTR rhs);
		STDMETHOD(put_Secure)(VARIANT_BOOL rhs);
		STDMETHOD(get_HasKeys)(VARIANT_BOOL *pfHasKeys);
		STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);

		STDMETHOD(get_Items)(VARIANT *pItemsArray);
		STDMETHOD(get_Keys)(VARIANT *pKeysArray);

		CBComPtr<IObjectStub> m_pCookie;
	};

public:
	CBCookie(void) : m_bSecure(FALSE), m_strPath(L'/'), m_bModified(FALSE)
	{
		m_pWriteCookie.CreateInstance();
		m_pWriteCookie->m_pCookie = this;
	}

public:
	// IReadCookie
	STDMETHOD(get_Item)(VARIANT Key, VARIANT* pVariantReturn);
	STDMETHOD(get_HasKeys)(VARIANT_BOOL *pfHasKeys);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get_Key)(VARIANT VarKey, VARIANT *pvar);

	STDMETHOD(get_Items)(VARIANT *pItemsArray);
	STDMETHOD(get_Keys)(VARIANT *pKeysArray);

	STDMETHOD(put_Item)(VARIANT Key, BSTR rhs);
	STDMETHOD(put_Expires)(DATE rhs);
	STDMETHOD(put_Domain)(BSTR rhs);
	STDMETHOD(put_Path)(BSTR rhs);
	STDMETHOD(put_Secure)(VARIANT_BOOL rhs);

public:
	void AddValue(CBString strKey, const CBString strValue)
	{
		CBString *pstrValue;

		m_dict.Lookup(strKey, &pstrValue, TRUE);
		*pstrValue = strValue;
	}

	CBStringA RanderCookie(void);

	HRESULT ParseUrlEncodeString(LPCSTR pstr, UINT nSize);
	HRESULT ParseUrlEncodeString(CBStringA str)
	{
		return ParseUrlEncodeString(str, str.GetLength());
	}

	BOOL isModified(void)
	{
		return m_bModified;
	}

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		return m_dict.GetEnum(rgVar, pos, count);
	}

private:
	CBComPtr<CBWriteCookie> m_pWriteCookie;
	CBDict<CBString> m_dict;
	CBString m_strDefault;

	CBDate m_dateExpires;
	CBString m_strDomain;
	CBString m_strPath;
	BOOL m_bSecure;
	BOOL m_bModified;
};

