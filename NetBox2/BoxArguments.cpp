// BoxArguments.cpp : implementation file
//

#include "stdafx.h"
#include "BoxArguments.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBoxArguments

IMPLEMENT_DYNCREATE(CBoxArguments, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxArguments, CBoxSafeObject)
	DISP_PROPERTY_PARAM_ID(CBoxArguments, "Item", 0, get_Item, SetNotSupported, VT_BSTR, VTS_I4)
	DISP_PROPERTY_EX(CBoxArguments, "Length", get_Length, SetNotSupported, VT_I4)
	DISP_FUNCTION(CBoxArguments, "Count", get_Length, VT_I4, VTS_NONE)
	DISP_PROPERTY_EX(CBoxArguments, "CommandLine", GetNotSupported, put_CommandLine, VT_BSTR)
END_DISPATCH_MAP()

// CBoxArguments message handlers

BSTR CBoxArguments::get_Item(long ItemNo)
{
	if(ItemNo < 0 || ItemNo >= m_astrArgs.GetCount())
		return NULL;

	return m_astrArgs[ItemNo].AllocSysString();
}

long CBoxArguments::get_Length(void)
{
	return (long)m_astrArgs.GetCount();
}

void CBoxArguments::put_CommandLine(LPCTSTR pstrCommandLine)
{
	LPCTSTR pstr;
	BOOL bIsLong;

	m_astrArgs.RemoveAll();

	while(pstrCommandLine && *pstrCommandLine)
	{
		while(IsBlankChar(*pstrCommandLine))
			pstrCommandLine ++;

		if(bIsLong = (*pstrCommandLine == '\"'))
			pstrCommandLine ++;

		pstr = pstrCommandLine;

		if(bIsLong)
			while(*pstrCommandLine != 0 && *pstrCommandLine != '\"')
				pstrCommandLine ++;
		else
			while(*pstrCommandLine != 0 && !IsBlankChar(*pstrCommandLine))
				pstrCommandLine ++;

		if(pstrCommandLine != pstr)
		{
			CString str;

			str.SetString(pstr, (int)(pstrCommandLine - pstr));
			m_astrArgs.Add(str);
		}

		if(bIsLong && *pstrCommandLine == '\"')
			pstrCommandLine ++;
	}
}
