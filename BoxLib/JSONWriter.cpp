#include "StdAfx.h"
#include "json.h"
#include "BDictionary.h"
#include "BCollection.h"
#include "BListex.h"
#include "msado15.tlh"


HRESULT JSON_putString(IStream *pStrm, LPCWSTR pStr)
{
	HRESULT hr;
	LPCWSTR p1 = pStr;
	ULONG n1;
	WCHAR ch;
	WCHAR chs[2] = L"\\";

	if(!pStr)
		return pStrm->Write(L"\"\"", 2 * sizeof(WCHAR), &n1);

	hr = pStrm->Write(L"\"", 1 * sizeof(WCHAR), &n1);
	if(FAILED(hr))return hr;

	while(ch = *pStr)
	{
		switch(ch)
		{
		case '\"':
			chs[1] = '\"';
			break;
		case '\\':
			chs[1] = '\\';
			break;
		case '\b':
			chs[1] = 'b';
			break;
		case '\f':
			chs[1] = 'f';
			break;
		case '\n':
			chs[1] = 'n';
			break;
		case '\r':
			chs[1] = 'r';
			break;
		case '\t':
			chs[1] = 't';
			break;
		default:
			chs[1] = 0;
		}

		if(chs[1])
		{
			if(p1 < pStr)
			{
				hr = pStrm->Write(p1, (ULONG)((pStr - p1) * sizeof(WCHAR)), &n1);
				if(FAILED(hr))return hr;
			}

			hr = pStrm->Write(chs, 2 * sizeof(WCHAR), &n1);
			if(FAILED(hr))return hr;

			pStr ++;
			p1 = pStr;
		}else pStr ++;
	}

	if(p1 < pStr)
	{
		hr = pStrm->Write(p1, (ULONG)((pStr - p1) * sizeof(WCHAR)), &n1);
		if(FAILED(hr))return hr;
	}

	return pStrm->Write(L"\"", 1 * sizeof(WCHAR), &n1);
}

HRESULT JSON_putInteger(IStream *pStrm, LONG n)
{
	ULONG n1;
	WCHAR s[16];

	_ltow_s(n, s, 16, 10);

	return pStrm->Write(s, (ULONG)(wcslen(s) * sizeof(WCHAR)), &n1);
}

HRESULT JSON_putUInteger(IStream *pStrm, ULONG n)
{
	ULONG n1;
	WCHAR s[16];

	_ultow_s(n, s, 16, 10);

	return pStrm->Write(s, (ULONG)(wcslen(s) * sizeof(WCHAR)), &n1);
}

HRESULT JSON_putInt64(IStream *pStrm, LONGLONG n)
{
	ULONG n1;
	WCHAR s[32];

	_i64tow_s(n, s, 32, 10);

	return pStrm->Write(s, (ULONG)(wcslen(s) * sizeof(WCHAR)), &n1);
}

HRESULT JSON_putUInt64(IStream *pStrm, ULONGLONG n)
{
	ULONG n1;
	WCHAR s[32];

	_ui64tow_s(n, s, 32, 10);

	return pStrm->Write(s, (ULONG)(wcslen(s) * sizeof(WCHAR)), &n1);
}

HRESULT JSON_putFloat(IStream *pStrm, double n)
{
	WCHAR s[_CVTBUFSIZE];
	ULONG n1;

	n1 = swprintf_s(s, _CVTBUFSIZE, L"%.17e", n);
	return pStrm->Write(s, (ULONG)(n1 * sizeof(WCHAR)), &n1);
}

inline WCHAR* iToStr(int n, WCHAR* pstr, int len)
{
	int len1 = len;

	while(len)
	{
		len --;
		pstr[len] = n % 10 + '0';
		n /= 10;
	}

	return pstr + len1;
}

HRESULT JSON_putDate(IStream *pStrm, DATE n)
{
	WCHAR s[32];
	WCHAR *pstr = s;
	ULONG n1;
	SYSTEMTIME st;

	::VariantTimeToSystemTime(n, &st);

	*pstr ++ = '\"';
	if(st.wYear != 1899 || st.wMonth != 12 || st.wDay != 30)
	{
		pstr = iToStr(st.wYear, pstr, 4);
		*pstr ++ = '-';

		pstr = iToStr(st.wMonth, pstr, 2);
		*pstr ++ = '-';

		pstr = iToStr(st.wDay, pstr, 2);
	}

	if(st.wHour || st.wMinute || st.wSecond)
	{
		if(pstr != s + 1)
			*pstr ++ = ' ';

		pstr = iToStr(st.wHour, pstr, 2);
		*pstr ++ = ':';

		pstr = iToStr(st.wMinute, pstr, 2);
		*pstr ++ = ':';

		pstr = iToStr(st.wSecond, pstr, 2);
	}
	*pstr ++ = '\"';

	return pStrm->Write(s, (ULONG)((pstr - s) * sizeof(WCHAR)), &n1);
}

HRESULT JSON_putObject(IStream *pStrm, IUnknown* o, int indent, CAtlArray<void*> &arrObjects)
{
	if(o == NULL)
		return DISP_E_UNKNOWNINTERFACE;

	for(int j=0;j<(int)arrObjects.GetCount();j++)
	{
		if (arrObjects[j] == (void *)o)
		{
			return CBComObject::SetErrorInfo(L"Duplicate object.");
		}
	}

	CBComPtr<IJSON> pJson;

	pJson = o;

	if(pJson)
		return pJson->JSON_join(pStrm, indent, arrObjects);

	HRESULT hr;

	_RecordsetPtr prs = o;
	if (prs)
	{
		ULONG n1;
		int indent1 = indent >= 0 ? indent + 1 : indent;
		LONG nCount;
	    _variant_t Index;
		Index.vt = VT_I2;
		CAtlArray<FieldPtr> flds;
		
		hr = pStrm->Write(L"[\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;

		nCount = prs->Fields->Count;
		hr = pStrm->Write(L"{\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;
		for (int j=0;j<nCount;j++)
		{
			Index.iVal = j;
			_bstr_t bstrname = prs->Fields->GetItem(Index)->Name;
			//flds.Add(prs->Fields->GetItem(Index));

			hr = JSON_putTabs(pStrm, indent1);
			if(FAILED(hr))return hr;

			JSON_putString(pStrm, bstrname);
			if(FAILED(hr))return hr;

			if(indent != -1)
				hr = pStrm->Write(L": ", 2 * sizeof(WCHAR), &n1);
			else
				hr = pStrm->Write(L":", 1 * sizeof(WCHAR), &n1);
			if(FAILED(hr))return hr;

			JSON_putInteger(pStrm, j);
			if(FAILED(hr))return hr;

			if(j < nCount - 1)
				hr = pStrm->Write(L",\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
			else if(indent != -1)
				hr = pStrm->Write(L"\r\n", 2 * sizeof(WCHAR), &n1);
			if(FAILED(hr))return hr;
		}
		hr = JSON_putTabs(pStrm, indent);
		if(FAILED(hr))return hr;
		hr = pStrm->Write(L"}", 1 * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;

		prs->MoveFirst();
		if (!prs->GetEndOfFile())
		{
			hr = pStrm->Write(L",", 1 * sizeof(WCHAR), &n1);
			if(FAILED(hr))return hr;

			//_variant_t avarRecords = prs->GetRows(-1);
			//JSON_putVariant(pStrm, &avarRecords, indent, arrObjects);
			while (true)
			{
				hr = pStrm->Write(L"[\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
				if(FAILED(hr))return hr;

				for (int j=0;j<nCount;j++)
				{
					Index.iVal = j;
					_variant_t value = prs->Fields->Item[Index]->Value;
					//_variant_t value = flds[j]->Value;

					hr = JSON_putTabs(pStrm, indent1);
					if(FAILED(hr))return hr;

					JSON_putVariant(pStrm, &value, indent, arrObjects);
					if(FAILED(hr))return hr;

					if(j < nCount - 1)
						hr = pStrm->Write(L",\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
					else if(indent != -1)
						hr = pStrm->Write(L"\r\n", 2 * sizeof(WCHAR), &n1);
					if(FAILED(hr))return hr;
				}

				hr = JSON_putTabs(pStrm, indent);
				if(FAILED(hr))return hr;
				hr = pStrm->Write(L"]", 1 * sizeof(WCHAR), &n1);
				if(FAILED(hr))return hr;

				prs->MoveNext();
				if (prs->GetEndOfFile()) break;
				hr = pStrm->Write(L",", 1 * sizeof(WCHAR), &n1);
				if(FAILED(hr))return hr;
			}
		}
		hr = JSON_putTabs(pStrm, indent);
		if(FAILED(hr))return hr;
		hr = pStrm->Write(L"]", 1 * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;
		return S_OK;
	}

	CComDispatchDriver pDisp;
	CComVariant varTemp;

	hr = o->QueryInterface(&pDisp);
	if(FAILED(hr))return hr;

	hr = pDisp.GetProperty(0, &varTemp);
	if(FAILED(hr))return hr;

	return JSON_putVariant(pStrm, &varTemp, indent, arrObjects);
}

HRESULT JSON_putArray(IStream *pStrm, const VARIANT* pArray, int nSize, int indent, CAtlArray<void*> &arrObjects)
{
	HRESULT hr;
	ULONG n1;
	int indent1 = indent >= 0 ? indent + 1 : indent;
	int i;

	hr = pStrm->Write(L"[\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
	if(FAILED(hr))return hr;

	for(i = 0; i < nSize; i ++)
	{
		hr = JSON_putTabs(pStrm, indent1);
		if(FAILED(hr))return hr;

		hr = JSON_putVariant(pStrm, &pArray[i], indent1, arrObjects);
		if(FAILED(hr))return hr;

		if(i < nSize - 1)
			hr = pStrm->Write(L",\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
		else if(indent != -1)
			hr = pStrm->Write(L"\r\n", 2 * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;
	}

	hr = JSON_putTabs(pStrm, indent);
	if(FAILED(hr))return hr;
	hr = pStrm->Write(L"]", 1 * sizeof(WCHAR), &n1);
	if(FAILED(hr))return hr;

	return S_OK;
}

HRESULT JSON_putSafeArray(IStream *pStrm, const SAFEARRAY* pArray, int indent, CAtlArray<void*> &arrObjects)
{
	int nSize = 1, i;

	if(pArray->cDims > 0)
		for(i = 0; i < pArray->cDims; i ++)
			nSize *= pArray->rgsabound[i].cElements;
	else nSize = 0;

	return JSON_putArray(pStrm, (const VARIANT *)pArray->pvData, nSize, indent, arrObjects);
}

HRESULT JSON_putVariant(IStream *pStrm, const VARIANT* pVar, int indent, CAtlArray<void*> &arrObjects)
{
	while(pVar && pVar->vt == (VT_VARIANT | VT_BYREF))
		pVar = pVar->pvarVal;

	if(!pVar)return DISP_E_TYPEMISMATCH;

	ULONG n1;
	CComVariant var;
	HRESULT hr;

	switch(pVar->vt)
	{
	case VT_BSTR:
		return JSON_putString(pStrm, pVar->bstrVal);
	case VT_EMPTY:
	case VT_NULL:
	case VT_ERROR:
		return pStrm->Write(L"null", 4 * sizeof(WCHAR), &n1);
	case VT_BOOL:
		if(pVar->boolVal == VARIANT_TRUE)
			return pStrm->Write(L"true", 4 * sizeof(WCHAR), &n1);
		else
			return pStrm->Write(L"false", 5 * sizeof(WCHAR), &n1);
		break;
	case VT_I1:
		return JSON_putInteger(pStrm, pVar->bVal);
	case VT_I2:
		return JSON_putInteger(pStrm, pVar->iVal);
	case VT_UI2:
		return JSON_putInteger(pStrm, pVar->uiVal);
	case VT_I4:
	case VT_INT:
		return JSON_putInteger(pStrm, pVar->lVal);
	case VT_UI4:
	case VT_UINT:
		return JSON_putUInteger(pStrm, pVar->ulVal);
	case VT_R4:
		return JSON_putFloat(pStrm, pVar->fltVal);
	case VT_R8:
		return JSON_putFloat(pStrm, pVar->dblVal);
	case VT_DATE:
		return JSON_putDate(pStrm, pVar->date);
	case VT_I8:
		return JSON_putInt64(pStrm, pVar->llVal);
	case VT_UI8:
		return JSON_putUInt64(pStrm, pVar->ullVal);
	case VT_CY:
		hr = VariantChangeType(&var, (VARIANTARG *)pVar, VARIANT_ALPHABOOL, VT_R8);
		if FAILED(hr) return hr;
		return JSON_putFloat(pStrm, var.dblVal);
	case VT_DECIMAL:
		hr = VariantChangeType(&var, (VARIANTARG *)pVar, VARIANT_ALPHABOOL, VT_I8);
		if FAILED(hr) return hr;
		return JSON_putInt64(pStrm, var.llVal);
	case VT_DISPATCH:
	case VT_UNKNOWN:
		return JSON_putObject(pStrm, pVar->punkVal, indent, arrObjects);
	case VT_ARRAY | VT_VARIANT:
		if((pVar->vt & VT_TYPEMASK) != VT_VARIANT || pVar->parray == NULL)
			return TYPE_E_TYPEMISMATCH;

		return JSON_putSafeArray(pStrm, pVar->vt & VT_BYREF ? *pVar->pparray : pVar->parray, indent, arrObjects);
	}

	return S_OK;
}
