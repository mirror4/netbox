#include "StdAfx.h"
#include "BArguments.h"

STDMETHODIMP CBArguments::get_Default(VARIANT *pVariantReturn)
{
	pVariantReturn->bstrVal = m_strCmdLine.AllocSysString();
	pVariantReturn->vt = VT_BSTR;

	return S_OK;
}

STDMETHODIMP CBArguments::get_CommandLine(BSTR* newVal)
{
	*newVal = m_strCmdLine.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBArguments::put_CommandLine(BSTR newVal)
{
	m_strCmdLine = newVal;

	LPCWSTR pstr;
	BOOL bIsLong;

	RemoveAll();

	while(newVal && *newVal)
	{
		while(iswspace(*newVal))
			newVal ++;

		if(bIsLong = (*newVal == '\"'))
			newVal ++;

		pstr = newVal;

		if(bIsLong)
			while(*newVal != 0 && *newVal != '\"')
				newVal ++;
		else
			while(*newVal != 0 && !iswspace(*newVal))
				newVal ++;

		if(newVal != pstr)
		{
			CBString str;

			str.SetString(pstr, (int)(newVal - pstr));
			AddValue(str);
		}

		if(bIsLong && *newVal == '\"')
			newVal ++;
	}

	return S_OK;
}

