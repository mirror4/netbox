#pragma once

#include "BList.h"
#include "BThread.h"
#include <atlrx.h>

class __declspec(uuid("94650000-0000-0000-0000-526567457870"))
CBRegExp : public CBDispatch<IRegExp>
{
	class CBRECharTraits : public CAtlRECharTraitsW
	{
	public:
		static const WCHAR** GetAbbrevs()
		{
			static const WCHAR *s_szAbbrevs[] = 
			{
				L"b([ \\t])",		// white space (blank)
				L"B([^ \\t])",		// not white space (blank)
				L"c([a-zA-Z])",	// alpha
				L"C(^[a-zA-Z])",	// not alpha
				L"d([0-9])",		// digit
				L"D(^[0-9])",		// not digit
				L"f(\f)",		// form-feed
				L"h([0-9a-fA-F])",	// hex digit
				L"n(\n)",	// newline
				L"q(\"[^\"]*\")|(\'[^\']*\')",	// quoted string
				L"r(\r))",	// newline
				L"s([ \f\n\r\t\v])",	// whitespace character
				L"S([^ \f\n\r\t\v])",	// not whitespace character
				L"t(\t)",	// tab
				L"v(\v)",	// vertical tab
				L"w([A-Za-z0-9_])",	// word
				L"W([^A-Za-z0-9_])",	// word
				L"z([0-9]+)",		// integer
				NULL
			};

			return s_szAbbrevs;
		}
	};

	class __declspec(uuid("94650000-0000-0000-5375-624D61746368"))
	CBSubMatch : public CBDispatch<ISubMatch>
	{
	public:
		STDMETHOD(get_Value)(BSTR *pVal)
		{
			*pVal = m_strValue.AllocSysString();
			return S_OK;
		}

        STDMETHOD(get_FirstIndex)(long *pVal)
		{
			*pVal = m_nIndex;
			return S_OK;
		}

        STDMETHOD(get_Length)(long *pVal)
		{
			*pVal = m_strValue.GetLength();
			return S_OK;
		}

		CBString m_strValue;
		long m_nIndex;
	};

	class __declspec(uuid("94650000-0000-0000-0000-004D61746368"))
	CBMatch : public CBDispatch<IMatch, CBSubMatch>
	{
	public:
		CBMatch()
		{
			m_pSubMatchs.CreateInstance();
		}

	public:
		STDMETHOD(get_SubMatches)(IStringList **pVal)
		{
			return m_pSubMatchs->QueryInterface(pVal);
		}

		CBComPtr<CBList> m_pSubMatchs;
	};

	DECLARE_CLASS(CBRegExp)

public:
	CBRegExp() :
		m_bIgnoreCase(FALSE),
		m_bGlobal(FALSE)
	{}

public:
	STDMETHOD(get_Pattern)(BSTR *pVal);
	STDMETHOD(put_Pattern)(BSTR Val);
	STDMETHOD(get_Global)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Global)(VARIANT_BOOL Val);
	STDMETHOD(get_IgnoreCase)(VARIANT_BOOL *pVal);
	STDMETHOD(put_IgnoreCase)(VARIANT_BOOL Val);
	STDMETHOD(Execute)(BSTR strText, IStringList **pVal);
	STDMETHOD(Replace)(BSTR strText1, BSTR strText2, BSTR *pVal);
	STDMETHOD(Test)(BSTR strText, VARIANT_BOOL *pVal);

private:
	HRESULT Parse(CAtlRegExp<CBRECharTraits>& re);

private:
	CBCriticalSection m_cs;
	CBString m_strPattern;
	BOOL m_bIgnoreCase;
	BOOL m_bGlobal;
};

