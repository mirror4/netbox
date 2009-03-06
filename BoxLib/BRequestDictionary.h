#pragma once

#include "BCollection.h"
#include "BList.h"
#include "BCookie.h"

class __declspec(uuid("94650000-7374-4469-6374-696F6E617279"))
CBRequestDictionary : public CBDispatch<IRequestDictionary>
{
	DECLARE_CLASS(CBRequestDictionary)
	APARTMENT_MODE

private:
	class CBListFix : public CBList
	{
	public:
		STDMETHODIMP get_Item(VARIANT i, VARIANT *pVariantReturn)
		{
			if(i.vt == VT_ERROR)
				return get_Default(pVariantReturn);

			long n = varGetNumbar(i) - 1;

			if(n >=0 && n < (int)m_arrayVariant.GetCount())
				VariantCopy(pVariantReturn, &m_arrayVariant[n]);
			else return DISP_E_BADINDEX;

			return S_OK;
		}
	};

	class __declspec(uuid("94650000-0000-5374-7269-6E674C697374"))
	CStringList : public CBListFix
	{
		DECLARE_CLASS_EX(CStringList, CBListFix)
		APARTMENT_MODE

	public:
		STDMETHOD(get_Default)(VARIANT *pVariantReturn);
	};

	class __declspec(uuid("94650000-0000-5570-6C6F-61644C697374"))
	CUploadList : public CBDispatch<IUploadList, CBListFix>
	{
		DECLARE_CLASS_EX(CUploadList, CBListFix)
		APARTMENT_MODE

	public:
		// IUploadList
		STDMETHOD(get_FileName)(BSTR *pStrReturn);
		STDMETHOD(get_Size)(long* pVal);
		STDMETHOD(get_ContentType)(BSTR *pStrReturn);

		STDMETHOD(get_Default)(VARIANT *pVariantReturn);
	};

	class __declspec(uuid("94650000-0000-5570-6C6F-616444617461"))
	CBUploadData : public CBDispatch<IUploadData>
	{
		DECLARE_CLASS(CBUploadData)
		APARTMENT_MODE

	public:
		CBUploadData(void) : m_nSize(0)
		{}

	public:
		// IUploadData
        STDMETHOD(get_Item)(VARIANT *pVariantReturn);
		STDMETHOD(get_FileName)(BSTR *pStrReturn);
		STDMETHOD(get_Size)(long* pVal);
		STDMETHOD(get_ContentType)(BSTR *pStrReturn);

	public:
		CComVariant m_varData;
		CBString m_strFileName;
		CBString m_strContentType;
		long m_nSize;
	};

public:
	CBRequestDictionary(void) : m_modeDict(modeStringList)
	{}

	template<class TYPE>
	void AddValue(CBString strKey, const TYPE& value)
	{
		CBComPtr<CStringList> pList;
		CBDispatchPtr &Disp = m_dict[strKey];

		if(Disp == NULL)
			Disp.CreateInstance(CStringList::GetClassObject());

		pList = Disp;

		if(pList != NULL)pList->AddValue(value);
	}

public:
	// IRequestDictionary
	STDMETHOD(get_Item)(VARIANT VarKey, VARIANT *pVariantReturn);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get_Key)(VARIANT VarKey, VARIANT *pvar);

	STDMETHOD(Exists)(VARIANT VarKey, VARIANT_BOOL *pExists);

	STDMETHOD(get_Items)(VARIANT *pItemsArray);
	STDMETHOD(get_Keys)(VARIANT *pKeysArray);

	STDMETHOD(get_Default)(VARIANT *pVariantReturn);

	STDMETHODIMP get_ItemWrite(VARIANT VarKey, VARIANT *pVariantReturn)
	{
		if(VarKey.vt == VT_ERROR)
			return get_Default(pVariantReturn);

		CBDispatchPtr &pDisp = m_dict[VarKey];

		if(pDisp == NULL)
			pDisp.CreateInstance(CBCookie::GetClassObject());

		CBComPtr<IWriteCookie> pWriteCookie;
		pWriteCookie = pDisp;

		*(CComVariant*)pVariantReturn = pWriteCookie;

		return S_OK;
	}

public:
	BOOL Lookup(CBString strKey, VARIANT *pVariantReturn)
	{
		CBDispatchPtr *pDisp;

		if(m_dict.Lookup(strKey, &pDisp) != S_OK)
			return FALSE;

		*(CComVariant*)pVariantReturn = *pDisp;

		return TRUE;
	}

	HRESULT ParseUrlEncodeString(LPCSTR pstr, UINT nSize);
	HRESULT ParseUrlEncodeString(CBStringA str)
	{
		return ParseUrlEncodeString(str, str.GetLength());
	}

	HRESULT ParseUploadString(LPCSTR pstr, UINT nSize);
	HRESULT ParseUploadString(CBStringA str)
	{
		return ParseUploadString(str, str.GetLength());
	}

	HRESULT ParseCookieString(LPCSTR pstr, UINT nSize);
	HRESULT ParseCookieString(CBStringA str)
	{
		return ParseCookieString(str, str.GetLength());
	}

	CBStringA RanderCookie(void);

	enum DictionaryMode
	{
		modeStringList,
		modeUploadList,
		modeReadCookie
	};

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		return m_dict.GetEnum(rgVar, pos, count);
	}

private:
	CBDict<CBDispatchPtr> m_dict;

	static CUploadList m_listEmptyUpload;
	static CStringList m_listEmptyString;
	static CBCookie m_dictEmptyCookie;
	CBString m_strDefault;

	DictionaryMode m_modeDict;
};

class __declspec(uuid("94650000-7374-4469-6374-696F6E61727A"))
CBWriteCookieDictionary : public CBDispatch<IRequestDictionary>
{
	DECLARE_CLASS(CBWriteCookieDictionary)

public:
	// IRequestDictionary
	STDMETHOD(get_Item)(VARIANT VarKey, VARIANT *pVariantReturn)
	{
		return m_pReadCookies->get_ItemWrite(VarKey, pVariantReturn);
	}

	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn)
	{
		return m_pReadCookies->get__NewEnum(ppEnumReturn);
	}

	STDMETHOD(get_Count)(long *cStrRet)
	{
		return m_pReadCookies->get_Count(cStrRet);
	}

	STDMETHOD(get_Key)(VARIANT VarKey, VARIANT *pvar)
	{
		return m_pReadCookies->get_Key(VarKey, pvar);
	}

	STDMETHOD(Exists)(VARIANT VarKey, VARIANT_BOOL *pExists)
	{
		return m_pReadCookies->Exists(VarKey, pExists);
	}

	STDMETHOD(get_Items)(VARIANT *pItemsArray)
	{
		return m_pReadCookies->get_Items(pItemsArray);
	}

	STDMETHOD(get_Keys)(VARIANT *pKeysArray)
	{
		return m_pReadCookies->get_Keys(pKeysArray);
	}

public:
	CBComPtr<CBRequestDictionary> m_pReadCookies;
};

