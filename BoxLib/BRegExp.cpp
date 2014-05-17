#include "StdAfx.h"
#include "BRegExp.h"

STDMETHODIMP CBRegExp::get_Pattern(BSTR *pVal)
{
	m_cs.Enter();
	*pVal = m_strPattern.AllocSysString();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBRegExp::put_Pattern(BSTR Val)
{
	m_cs.Enter();
	m_strPattern = Val;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBRegExp::get_Global(VARIANT_BOOL *pVal)
{
	m_cs.Enter();
	*pVal = m_bGlobal;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBRegExp::put_Global(VARIANT_BOOL Val)
{
	m_cs.Enter();
	m_bGlobal = (Val != VARIANT_FALSE);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBRegExp::get_IgnoreCase(VARIANT_BOOL *pVal)
{
	m_cs.Enter();
	*pVal = m_bIgnoreCase;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBRegExp::put_IgnoreCase(VARIANT_BOOL Val)
{
	m_cs.Enter();
	m_bIgnoreCase = (Val != VARIANT_FALSE);
	m_cs.Leave();

	return S_OK;
}

HRESULT CBRegExp::Parse(CAtlRegExp<CBRECharTraits>& re)
{
	CBString strPattern;

	m_cs.Enter();
	strPattern = m_strPattern;
	m_cs.Leave();

	switch(re.Parse(strPattern, !m_bIgnoreCase))
	{
	case REPARSE_ERROR_OK:
		break;
	case REPARSE_ERROR_OUTOFMEMORY:
		return E_OUTOFMEMORY;
	case REPARSE_ERROR_BRACE_EXPECTED:
		return SetErrorInfo(L"A closing brace was expected.");
	case REPARSE_ERROR_PAREN_EXPECTED:
		return SetErrorInfo(L"A closing parenthesis was expected.");
	case REPARSE_ERROR_BRACKET_EXPECTED:
		return SetErrorInfo(L"A closing bracket was expected.");
	case REPARSE_ERROR_UNEXPECTED:
		return SetErrorInfo(L"An unspecified fatal error occurred.");
	case REPARSE_ERROR_EMPTY_RANGE:
		return SetErrorInfo(L"A range expression was empty.");
	case REPARSE_ERROR_INVALID_GROUP:
		return SetErrorInfo(L"A backreference was made to a group.");
	case REPARSE_ERROR_INVALID_RANGE:
		return SetErrorInfo(L"An invalid range was specified.");
	case REPARSE_ERROR_EMPTY_REPEATOP:
		return SetErrorInfo(L"A possibly empty * or + was detected.");
	default:
        return SetErrorInfo(L"The input string was invalid.");
	};

	return S_OK;
}

STDMETHODIMP CBRegExp::Execute(BSTR strText, IStringList **pVal)
{
	CBComPtr<CBList> pMatchs;
	CAtlRegExp<CBRECharTraits> re;
	CAtlREMatchContext<CBRECharTraits> mc;
	HRESULT hr;
	const WCHAR *pstrEnd;

	hr = Parse(re);
	if(FAILED(hr))return hr;

	pMatchs.CreateInstance();

	pstrEnd = strText;

	while(*pstrEnd && re.Match(pstrEnd, &mc, &pstrEnd))
	{
		CBComPtr<CBMatch> pMatch;

		pMatch.CreateInstance();
		pMatch->m_nIndex = mc.m_Match.szStart - strText;
		pMatch->m_strValue.SetString(mc.m_Match.szStart, mc.m_Match.szEnd - mc.m_Match.szStart);

		for(UINT nGroupIndex = 0; nGroupIndex < mc.m_uNumGroups; ++nGroupIndex)
		{
			const WCHAR* szStart = 0;
			const WCHAR* szEnd = 0;
			CBComPtr<CBSubMatch> pSubMatch;

			mc.GetMatch(nGroupIndex, &szStart, &szEnd);
			pSubMatch.CreateInstance();

			pSubMatch->m_nIndex = szStart - strText;
			pSubMatch->m_strValue.SetString(szStart, szEnd - szStart);

			pMatch->m_pSubMatchs->AddValue(pSubMatch);
		}

		pMatchs->AddValue((IMatch*)pMatch);

		if(!m_bGlobal)break;
	}

	return pMatchs->QueryInterface(pVal);
}

STDMETHODIMP CBRegExp::Replace(BSTR strText1, BSTR strText2, BSTR *pVal)
{
	CBString strResult;
	CAtlRegExp<CBRECharTraits> re;
	CAtlREMatchContext<CBRECharTraits> mc;
	int sLen = ::SysStringLen(strText2);
	HRESULT hr;
	const WCHAR *pstrEnd, *strText;

	hr = Parse(re);
	if(FAILED(hr))return hr;

	strText = pstrEnd = strText1;

	while(*pstrEnd && re.Match(pstrEnd, &mc, &pstrEnd))
	{
		strResult.Append(strText, mc.m_Match.szStart - strText);
		strResult.Append(strText2, sLen);

		if(!m_bGlobal)break;
		strText = pstrEnd;
	}

	strResult.Append(strText, pstrEnd - strText);

	*pVal = strResult.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBRegExp::Test(BSTR strText, VARIANT_BOOL *pVal)
{
	CAtlRegExp<CBRECharTraits> re;
	CAtlREMatchContext<CBRECharTraits> mc;
	HRESULT hr;

	hr = Parse(re);
	if(FAILED(hr))return hr;

	*pVal = re.Match(strText, &mc) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

