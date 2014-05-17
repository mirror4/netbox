#include "stdafx.h"
#include <atlbase.h>
#include <atlutil.h>
#include <atlcom.h>

#include "BoxString.h"

void CBoxEncode::HtmlEncode(CString &str)
{
	TCHAR* pszEncoded = new TCHAR[str.GetLength() * 6 + 1];
	int nLength = 0;
	TCHAR ch;

	for(int i = 0; i < str.GetLength(); i ++)
	{
		switch(ch = str[i])
		{
		case _T('\"'):
			_tcscpy(pszEncoded + nLength, _T("&quot;"));
			nLength += 6;
			break;
		case _T('<'):
			_tcscpy(pszEncoded + nLength, _T("&lt;"));
			nLength += 4;
			break;
		case _T('>'):
			_tcscpy(pszEncoded + nLength, _T("&gt;"));
			nLength += 4;
			break;
		case _T('&'):
			_tcscpy(pszEncoded + nLength, _T("&amp;"));
			nLength += 5;
			break;
		default:
			pszEncoded[nLength ++] = ch;
		}
	}

	pszEncoded[nLength] = 0;

	str = pszEncoded;
	delete pszEncoded;
}
