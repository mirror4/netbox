#include "StdAfx.h"
#include "BVarType.h"
#include "BEncoding.h"
#include "BThread.h"
#include "BMStream.h"
#include "BCodepage.h"
#include "BDate.h"
#include <mshtml.h>
#include "zlib\zlib.h"
#include "zlib\zutil.h"

#ifndef Z_BUFSIZE
#define Z_BUFSIZE 16384
#endif

static HRESULT baseDecode(const char *pdecodeTable, UINT dwBits, BSTR baseString, VARIANT *retVal)
{
	UINT nWritten = 0, len = SysStringLen(baseString);
	CBVarPtr varPtr;

	varPtr.Create(len * dwBits / 8);

	if(len)
	{
		BSTR szEnd = baseString + len;
		DWORD dwCurr = 0;
		int nBits = 0;
		WCHAR ch;

		while(baseString < szEnd)
		{
			ch = *baseString++;
			int nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

			if (nCh != -1)
			{
				dwCurr <<= dwBits;
				dwCurr |= nCh;
				nBits += dwBits;

				while(nBits >= 8)
				{
					varPtr.m_pData[nWritten ++] = (BYTE) (dwCurr >> (nBits - 8));
					nBits -= 8;
				}
			}
		}
	}

	return varPtr.GetVariant(retVal, nWritten);
}

STDMETHODIMP CBEncoding::Base32Decode(BSTR base32String, VARIANT *retVal)
{
	static const char decodeTable[] =
	{
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 2x  !"#$%&'()*+,-./   */
		-1,-1,26,27,28,29,30,31,-1,-1,-1,-1,-1,-1,-1,-1, /* 3x 0123456789:;<=>?   */
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, /* 4x @ABCDEFGHIJKLMNO   */
		15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1, /* 5X PQRSTUVWXYZ[\]^_   */
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, /* 6x `abcdefghijklmno   */
		15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1  /* 7X pqrstuvwxyz{\}~DEL */
	};

	return baseDecode(decodeTable, 5, base32String, retVal);
}

static HRESULT baseEncode(const char *pEncodingTable, UINT dwBits, VARIANT varData, short nLineBreak, BSTR *retVal)
{
	CBVarPtr varPtr;
	UINT i, len = 0, len1, bits = 0;
	DWORD dwData = 0;
	DWORD dwSize;
	BYTE bMask = 0xff >> (8 - dwBits);

	if(nLineBreak < 0)nLineBreak = 0;
	if(nLineBreak > 76)nLineBreak = 76;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	if(dwBits == 6)dwSize = (varPtr.m_nSize + 2) / 3 * 4;
	else if(dwBits == 5)dwSize = (varPtr.m_nSize + 4) / 5 * 8;

	if(nLineBreak)dwSize += (dwSize / nLineBreak) * 2;
	*retVal = ::SysAllocStringLen(NULL, dwSize);

	len1 = 0;
	for(i = 0; i < (UINT)varPtr.m_nSize; i ++)
	{
		dwData <<= 8;
		dwData |= varPtr.m_pData[i];
		bits += 8;

		while(bits >= dwBits)
		{
			(*retVal)[len ++] = pEncodingTable[(dwData >> (bits - dwBits)) & bMask];
			if(nLineBreak && ++ len1 >= (DWORD)nLineBreak)
			{
				(*retVal)[len ++] = 13;
				(*retVal)[len ++] = 10;
				len1 = 0;
			}
			bits -= dwBits;
		}
	}

	if(bits)
	{
		(*retVal)[len ++] = pEncodingTable[(dwData << (dwBits - bits)) & bMask];
		if(nLineBreak && ++ len1 >= (DWORD)nLineBreak)
		{
			(*retVal)[len ++] = 13;
			(*retVal)[len ++] = 10;
			len1 = 0;
		}
	}

	while(len < dwSize)
	{
		(*retVal)[len ++] = '=';
		if(nLineBreak && ++ len1 >= (DWORD)nLineBreak)
		{
			(*retVal)[len ++] = 13;
			(*retVal)[len ++] = 10;
			len1 = 0;
		}
	}

	return S_OK;
}

STDMETHODIMP CBEncoding::Base32Encode(VARIANT varData, short nLineBreak, BSTR *retVal)
{
	return baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZ234567", 5, varData, nLineBreak, retVal);
}

STDMETHODIMP CBEncoding::Base64Decode(BSTR base64String, VARIANT *retVal)
{
	static const char decodeTable[] =
	{
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,62,-1,63, /* 2x  !"#$%&'()*+,-./   */
		52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1, /* 3x 0123456789:;<=>?   */
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, /* 4x @ABCDEFGHIJKLMNO   */
		15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,63, /* 5X PQRSTUVWXYZ[\]^_   */
		-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40, /* 6x `abcdefghijklmno   */
		41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1  /* 7X pqrstuvwxyz{\}~DEL */
	};

	return baseDecode(decodeTable, 6, base64String, retVal);
}

STDMETHODIMP CBEncoding::Base64Encode(VARIANT varData, short nLineBreak, BSTR *retVal)
{
	return baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", 6, varData, nLineBreak, retVal);
}

STDMETHODIMP CBEncoding::BinToStr(VARIANT varData, long nCodePage, BSTR *retVal)
{
	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	if(nCodePage == -1)
	{
		*retVal = ::SysAllocStringByteLen(NULL, varPtr.m_nSize);
		CopyMemory(*retVal, varPtr.m_pData, varPtr.m_nSize);
		return S_OK;
	}

	if(nCodePage == 0)nCodePage = _AtlGetConversionACP();

	int _nTempCount = ::MultiByteToWideChar(nCodePage, 0, (LPCSTR)varPtr.m_pData, varPtr.m_nSize, NULL, NULL);
	*retVal = ::SysAllocStringLen(NULL, _nTempCount);
	if(*retVal != NULL)
		::MultiByteToWideChar(nCodePage, 0, (LPCSTR)varPtr.m_pData, varPtr.m_nSize, *retVal, _nTempCount);

	return S_OK;
}

STDMETHODIMP CBEncoding::StrToBin(BSTR strData, long nCodePage, VARIANT *retVal)
{
	CBVarPtr varPtr;
	UINT strLen = SysStringByteLen(strData);

	if(nCodePage == -1)
	{
		varPtr.Create(strLen);
		CopyMemory(varPtr.m_pData, strData, strLen);
		return varPtr.GetVariant(retVal);
	}

	if(nCodePage == 0)nCodePage = _AtlGetConversionACP();

	int _nTempCount = ::WideCharToMultiByte(nCodePage, 0, strData, strLen / 2, NULL, 0, NULL, NULL);
	varPtr.Create(_nTempCount);

	::WideCharToMultiByte(nCodePage, 0, strData, strLen / 2, (LPSTR)varPtr.m_pData, _nTempCount, NULL, NULL);

	return varPtr.GetVariant(retVal);
}

STDMETHODIMP CBEncoding::CsvDecode(BSTR bstrText, VARIANT* retVal)
{
	CAtlArray<VARIANT> arrayVariant;
	CBString strTemp;
	HRESULT hr = S_OK;
	BOOL bString;
	WCHAR ch;
	VARIANT varTemp = {VT_BSTR};
	BSTR ptr, bstrTextEnd = bstrText + ::SysStringLen(bstrText);

	while(bstrText < bstrTextEnd)
	{
		bString = FALSE;
		if(*bstrText == L'\"')
		{
			bString = TRUE;
			bstrText ++;
		}
		ptr = bstrText;

		while(bstrText < bstrTextEnd)
		{
			ch = *bstrText ++;

			if(bString)
			{
				if(ch == L'\"')
					if(*bstrText == L'\"')
					{
						if(bstrText - ptr)
							strTemp.Append(ptr, bstrText - ptr);
						bstrText ++;
						ptr = bstrText;
					}else
					{
						if(bstrText - ptr - 1)
							strTemp.Append(ptr, bstrText - ptr - 1);
						ptr = bstrText;
						bString = FALSE;
					}
			}else if(ch == L',')
			{
				if(bstrText - ptr - 1)
					strTemp.Append(ptr, bstrText - ptr - 1);
				ptr = bstrText;

				varTemp.bstrVal = strTemp.AllocSysString();
				arrayVariant.Add(varTemp);
				strTemp.Empty();

				break;
			}
		}

		if(bString)
		{
			for(size_t i = 0; i < arrayVariant.GetCount(); i ++)
				::VariantClear(&arrayVariant[i]);

			return HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
		}
	}

	if(bstrText - ptr)
		strTemp.Append(ptr, bstrText - ptr);

	varTemp.bstrVal = strTemp.AllocSysString();
	arrayVariant.Add(varTemp);

	CComSafeArray<VARIANT> arrayResult;

	arrayResult.Create(arrayVariant.GetCount());
	LPSAFEARRAY pArray = arrayResult;

	if(!pArray)
	{
		for(size_t i = 0; i < arrayVariant.GetCount(); i ++)
			::VariantClear(&arrayVariant[i]);

		return E_OUTOFMEMORY;
	}
	else if(arrayVariant.GetCount())
		CopyMemory(pArray->pvData, &arrayVariant[0], arrayVariant.GetCount() * sizeof(VARIANT));

	retVal->vt = VT_ARRAY | VT_VARIANT;
	retVal->parray = arrayResult.Detach();

	return S_OK;
}

STDMETHODIMP CBEncoding::CsvEncode(VARIANT varArray, BSTR* retVal)
{
	HRESULT hr = S_OK;
	VARIANT* pvar = &varArray;
	CComVariant varTemp;
	CBString str;
	CBString strResult;

	if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
	{
		CBDispatchPtr pDisp;

		if(pvar->punkVal == NULL)
			return DISP_E_UNKNOWNINTERFACE;

		hr = pvar->punkVal->QueryInterface(&pDisp);
		if(FAILED(hr))return hr;

		hr = pDisp.GetProperty(0, &varTemp);
		if(FAILED(hr))return hr;

		pvar = &varTemp;
	}

	if((pvar->vt & (VT_ARRAY | VT_TYPEMASK)) == (VT_ARRAY | VT_VARIANT))
	{
		LPSAFEARRAY pArray = pvar->vt & VT_BYREF ? *pvar->pparray : pvar->parray;

		if(pArray->cDims != 1 || pArray->rgsabound[0].lLbound != 0)
			return TYPE_E_TYPEMISMATCH;

		pvar = (VARIANT *)pArray->pvData;

		for(ULONG i = 0; i < pArray->rgsabound[0].cElements; i ++)
		{
			if(pvar[i].vt == VT_BSTR)
				str = pvar[i].bstrVal;
			else
			{
				CComVariant varTemp1;

				hr = VariantChangeType(&varTemp1, &pvar[i], VARIANT_ALPHABOOL, VT_BSTR);
				if(FAILED(hr))return hr;

				str = varTemp1.bstrVal;
			}

			if(str.GetLength())
			{
				BOOL bString = (pvar[i].vt == VT_BSTR);

				if(bString)
					if(str.Find(L'\"') != -1)
						str.Replace(L"\"", L"\"\"");
					else if(str.Find(L',') == -1 && str.Find(L'\r') == -1 && str.Find(L'\n') == -1)
						bString = FALSE;

				if(bString)
				{
					if(i > 0)strResult.Append(L",\"", 2);
					else strResult.AppendChar(L'\"');
					strResult.Append(str);
					strResult.AppendChar(L'\"');
				}else
				{
					if(i > 0)strResult.AppendChar(L',');
					strResult.Append(str);
				}
			}else strResult.AppendChar(L',');
		}
	}else return TYPE_E_TYPEMISMATCH;

	*retVal = strResult.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBEncoding::HexDecode(BSTR HexString, VARIANT *retVal)
{
	UINT i, pos, len = SysStringLen(HexString);
	CBVarPtr varPtr;
	WCHAR ch1, ch2;

	varPtr.Create(len / 2);

	pos = 0;
	for(i = 0; i < len; i ++)
	{
		ch1 = HexString[i];

		if((ch1 >= 'a' && ch1 <= 'f') || (ch1 >= 'A' && ch1 <= 'F'))
			ch1 = (ch1 & 0xf) + 9;
		else if(ch1 >= '0' && ch1 <= '9')
			ch1 &= 0xf;
		else continue;

		i ++;
		if(i < len)
		{
			ch2 = HexString[i];
			if((ch2 >= 'a' && ch2 <= 'f') || (ch2 >= 'A' && ch2 <= 'F'))
				ch2 = (ch2 & 0xf) + 9;
			else if(ch2 >= '0' && ch2 <= '9')
				ch2 &= 0xf;
			else
			{
				ch2 = ch1;
				ch1 = 0;
			}
		}else
		{
			ch2 = ch1;
			ch1 = 0;
		}

		varPtr.m_pData[pos ++] = (ch1 << 4) + ch2;
	}

	return varPtr.GetVariant(retVal, pos);
}

STDMETHODIMP CBEncoding::HexEncode(VARIANT varData, short nLineBreak, BSTR *retVal)
{
	CBVarPtr varPtr;
	LPWSTR pstr;
	static char HexChar[] = "0123456789ABCDEF";
	UINT i, pos, len1;

	if(nLineBreak < 0)nLineBreak = 0;
	nLineBreak &= 0xfffffffe;
	if(nLineBreak > 76)nLineBreak = 76;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	i = varPtr.m_nSize * 2;
	if(nLineBreak)i += (i / nLineBreak) * 2;
	pstr = *retVal = ::SysAllocStringLen(NULL, i);

	len1 = 0;
	pos = 0;
	if(*retVal != NULL)
		for(i = 0; i < varPtr.m_nSize; i ++)
		{
			pstr[pos * 2] = HexChar[varPtr.m_pData[i] >> 4];
			pstr[pos * 2 + 1] = HexChar[varPtr.m_pData[i] & 0xf];
			pos ++;
			len1 += 2;
			if(nLineBreak && len1 >= (DWORD)nLineBreak)
			{
				pstr[pos * 2] = 13;
				pstr[pos * 2 + 1] = 10;
				pos ++;
				len1 = 0;
			}
		}

	return S_OK;
}

STDMETHODIMP CBEncoding::HtmlEncode(BSTR TextString, BSTR *retVal)
{
	CBAutoPtr<WCHAR> pszEncoded;
	UINT i, len = SysStringLen(TextString);
	int nLength = 0;
	WCHAR ch;

	pszEncoded.Allocate(len * 6 + 1);

	for(i = 0; i < len; i ++)
	{
		switch(ch = TextString[i])
		{
		case '\"':
			wcsncpy(pszEncoded + nLength, L"&quot;", 6);
			nLength += 6;
			break;
		case '<':
			wcsncpy(pszEncoded + nLength, L"&lt;", 4);
			nLength += 4;
			break;
		case '>':
			wcsncpy(pszEncoded + nLength, L"&gt;", 4);
			nLength += 4;
			break;
		case '&':
			wcsncpy(pszEncoded + nLength, L"&amp;", 5);
			nLength += 5;
			break;
		default:
			pszEncoded[nLength ++] = ch;
		}
	}

	*retVal = ::SysAllocStringLen(pszEncoded, nLength);
	return S_OK;
}

STDMETHODIMP CBEncoding::xmlEncode(BSTR TextString, BSTR *retVal)
{
	CBAutoPtr<WCHAR> pszEncoded;
	UINT i, len = SysStringLen(TextString);
	int nLength = 0;
	WCHAR ch;

	pszEncoded.Allocate(len * 6 + 1);

	for(i = 0; i < len; i ++)
	{
		switch(ch = TextString[i])
		{
		case '\'':
			wcsncpy(pszEncoded + nLength, L"&apos;", 6);
			nLength += 6;
			break;
		case '\"':
			wcsncpy(pszEncoded + nLength, L"&quot;", 6);
			nLength += 6;
			break;
		case '<':
			wcsncpy(pszEncoded + nLength, L"&lt;", 4);
			nLength += 4;
			break;
		case '>':
			wcsncpy(pszEncoded + nLength, L"&gt;", 4);
			nLength += 4;
			break;
		case '&':
			wcsncpy(pszEncoded + nLength, L"&amp;", 5);
			nLength += 5;
			break;
		default:
			pszEncoded[nLength ++] = ch;
		}
	}

	*retVal = ::SysAllocStringLen(pszEncoded, nLength);
	return S_OK;
}

STDMETHODIMP CBEncoding::JSEncode(BSTR TextString, BSTR *retVal)
{
	CBAutoPtr<WCHAR> pszEncoded;
	UINT i, len = SysStringLen(TextString);
	UINT nPos = 0;
	WCHAR ch;

	pszEncoded.Allocate(len * 2);

	for(i = 0; i < len; i ++)
		switch(ch = TextString[i])
		{
		case '\\':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = '\\';
			break;
		case '/':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = '/';
			break;
		case '\r':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = 'r';
			break;
		case '\n':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = 'n';
			break;
		case '\t':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = 't';
			break;
		case '\'':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = '\'';
			break;
		case '\"':
			pszEncoded[nPos ++] = '\\';
			pszEncoded[nPos ++] = '\"';
			break;
		default:
			pszEncoded[nPos ++] = ch;
			break;
		}

	*retVal = ::SysAllocStringLen(pszEncoded, nPos);
	return S_OK;
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

STDMETHODIMP CBEncoding::SQLEncode(VARIANT varValue, BSTR *retVal)
{
	VARIANT* pvar = &varValue;
	VARIANT varTemp = {VT_EMPTY};
	HRESULT hr;

	if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
	{
		CBDispatchPtr pDisp;

		if(pvar->punkVal == NULL)
			return DISP_E_UNKNOWNINTERFACE;

		hr = pvar->punkVal->QueryInterface(&pDisp);
		if(FAILED(hr))return hr;

		hr = pDisp.GetProperty(0, &varTemp);
		if(FAILED(hr))return hr;

		pvar = &varTemp;

		if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
		{
			VariantClear(&varTemp);
			return DISP_E_BADVARTYPE;
		}
	}

	if(pvar->vt == VT_NULL)
	{
		*retVal = ::SysAllocStringLen(L"NULL", 4);
		return S_OK;
	}else if(pvar->vt == VT_DATE)
	{
		WCHAR wstr[20]; // "9999-99-99 99:99:99"
		WCHAR *pstr = wstr;
		CBDate d(pvar->date);
		SYSTEMTIME st;

		st = d;

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
			if(pstr != wstr)
				*pstr ++ = ' ';

			pstr = iToStr(st.wHour, pstr, 2);
			*pstr ++ = ':';

			pstr = iToStr(st.wMinute, pstr, 2);
			*pstr ++ = ':';

			pstr = iToStr(st.wSecond, pstr, 2);
		}

		*retVal = ::SysAllocStringLen(wstr, pstr - wstr);

		return S_OK;
	}else if(pvar->vt != VT_BSTR)
	{
		hr = VariantChangeType(&varTemp, pvar, 0, VT_BSTR);
		if(FAILED(hr))return hr;

		*retVal = varTemp.bstrVal;
		return S_OK;
	}

	BSTR TextString = pvar->bstrVal;

	if(TextString == NULL)
	{
		*retVal = ::SysAllocStringLen(L"", 0);
		return S_OK;
	}

	CBAutoPtr<WCHAR> pszEncoded;
	UINT i, len = SysStringLen(TextString);
	UINT nPos = 0;
	WCHAR ch;

	pszEncoded.Allocate(len * 2);

	for(i = 0; i < len; i ++)
	{
		ch = TextString[i];

		if(ch == '\'')
			pszEncoded[nPos ++] = '\'';
		pszEncoded[nPos ++] = ch;
	}

	if(varTemp.vt == VT_BSTR)
		SysFreeString(varTemp.bstrVal);

	*retVal = ::SysAllocStringLen(pszEncoded, nPos);
	return S_OK;
}

#define HEX_ESCAPE '%'
static unsigned char isAcceptable[] =
{
	0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1, /* 2x  !"#$%&'()*+,-./   */
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0, /* 3x 0123456789:;<=>?   */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 4x @ABCDEFGHIJKLMNO   */
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, /* 5X PQRSTUVWXYZ[\]^_   */
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 6x `abcdefghijklmno   */
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0  /* 7X pqrstuvwxyz{\}~DEL */
};
#define ACCEPTABLE(a)	( a>=32 && a<128 && (isAcceptable[a-32]))
static const char *hex = "0123456789ABCDEF";
#define HEXDATA(ch)		((ch) >= '0' && (ch) <= '9' ? (ch) - '0' : \
						(ch) >= 'a' && (ch) <= 'f' ? (ch) - 'a' + 10 : \
						(ch) >= 'A' && (ch) <= 'F' ? (ch) - 'A' + 10 : 0)
#define ISHEXCHA(ch)	(((ch) >= '0' && (ch) <= '9') || \
						((ch) >= 'a' && (ch) <= 'f') || \
						((ch) >= 'A' && (ch) <= 'F'))

CBString CBEncoding::UrlDecode(LPCSTR urlString, UINT len)
{
	CBString str;
	CBAutoPtr<char> pszDecoded;
	char ch;
	UINT pos;

	pszDecoded.Allocate(len + 1);
	pos = 0;
	while(len--)
	{
		switch(ch = *urlString ++)
		{
		case '+':
			pszDecoded[pos ++] = ' ';
			break;
		case '%':
			if(len >= 2 && ISHEXCHA(urlString[0]) && ISHEXCHA(urlString[1]))
			{
				pszDecoded[pos ++] = (char)((HEXDATA(urlString[0]) << 4) + HEXDATA(urlString[1]));
				urlString += 2;
				len -=2;
			}else if(len >= 5 && (urlString[0] == 'u' || urlString[0] == 'U') &&
					ISHEXCHA(urlString[1]) &&
					ISHEXCHA(urlString[2]) &&
					ISHEXCHA(urlString[3]) &&
					ISHEXCHA(urlString[4]))
			{
				urlString ++;
				len --;

				if(pos)
				{
					pszDecoded[pos] = 0;
					str += pszDecoded;
					pos = 0;
				}

				str.AppendChar((WCHAR)( (HEXDATA(urlString[0]) << 12) + (HEXDATA(urlString[1]) << 8) +
					(HEXDATA(urlString[2]) << 4) + HEXDATA(urlString[3])));

				urlString += 4;
				len -=4;
			}
			else pszDecoded[pos ++] = '%';
			break;
		default:
			pszDecoded[pos ++] = ch;
		}
	}

	if(pos)
	{
		pszDecoded[pos] = 0;
		str += pszDecoded;
		pos = 0;
	}

	return str;
}

STDMETHODIMP CBEncoding::UrlDecode(BSTR urlString, BSTR *retVal)
{
	*retVal = UrlDecode(CStringA(urlString, SysStringLen(urlString))).AllocSysString();
	return S_OK;
}

CBStringA CBEncoding::UrlEncode(LPCSTR urlString, UINT len, BOOL bEncodeDBMS)
{
	CBStringA str;
	LPSTR pszEncoded;
	UINT i, nLength = 0;
	BYTE ch;

	pszEncoded = str.GetBuffer(len * 3 + 1);

	for(i = 0; i < len; i ++)
	{
		ch = (BYTE)urlString[i];

		if(!bEncodeDBMS)
		{
			if(CBCodePage::IsDBCSLeadByte(ch))
			{
				pszEncoded[nLength ++] = ch;
				if(i < len)
				{
					i ++;
					pszEncoded[nLength ++] = (BYTE)urlString[i];
				}

				continue;
			}
		}

		if (!ACCEPTABLE(ch))
		{
			pszEncoded[nLength ++] = HEX_ESCAPE;	/* Means hex commming */
			pszEncoded[nLength ++] = hex[(ch >> 4) & 15];
			pszEncoded[nLength ++] = hex[ch & 15];
		}
		else
			pszEncoded[nLength ++] = ch;
	}

	str.ReleaseBuffer(nLength);

	return str;
}

STDMETHODIMP CBEncoding::UrlEncode(BSTR urlString, BSTR *retVal)
{
	*retVal = UrlEncode(CStringA(urlString, SysStringLen(urlString))).AllocSysString();
	return S_OK;
}

STDMETHODIMP CBEncoding::Deflate(VARIANT varData, VARIANT* retVal)
{
	CBVarPtr varPtr;
	Byte Buffer[Z_BUFSIZE];
	z_stream stream;
	int err;
	uInt len;
	int done = 0;
	CBTempStream mStream;

	ZeroMemory(&stream, sizeof(stream));

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if (err != Z_OK)return E_OUTOFMEMORY;

	stream.next_out = Buffer;
	stream.avail_out = Z_BUFSIZE;

	stream.next_in = (Bytef*)varPtr.m_pData;
	stream.avail_in = varPtr.m_nSize;

	while (stream.avail_in != 0)
	{
		if (stream.avail_out == 0)
		{
			hr = mStream.Write(Buffer, Z_BUFSIZE, NULL);
			if(FAILED(hr))
			{
				deflateEnd(&stream);
				return hr;
			}
			stream.next_out = Buffer;
			stream.avail_out = Z_BUFSIZE;
		}

		if (deflate(&stream, Z_NO_FLUSH) != Z_OK)
		{
			deflateEnd(&stream);
			return E_OUTOFMEMORY;
		}
	}

	for (;;)
	{
		len = Z_BUFSIZE - stream.avail_out;

		if (len != 0)
		{
			hr = mStream.Write(Buffer, len, NULL);
			if(FAILED(hr))
			{
				deflateEnd(&stream);
				return hr;
			}
			stream.next_out = Buffer;
			stream.avail_out = Z_BUFSIZE;
		}
		if (done) break;
		err = deflate(&stream, Z_FINISH);

		if (len == 0 && err == Z_BUF_ERROR)err = Z_OK;

		done = (stream.avail_out != 0 || err == Z_STREAM_END);

		if (err != Z_OK && err != Z_STREAM_END)
		{
			deflateEnd(&stream);
			return E_OUTOFMEMORY;
		}
	}

	deflateEnd(&stream);

	return mStream.GetVariant(retVal);
}

STDMETHODIMP CBEncoding::Inflate(VARIANT varData, VARIANT* retVal)
{
	CBVarPtr varPtr;
	Byte Buffer[Z_BUFSIZE];
	z_stream stream;
	int err;
	CBTempStream mStream;

	ZeroMemory(&stream, sizeof(stream));

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	err = inflateInit2(&stream, -MAX_WBITS);
	if (err != Z_OK)return E_OUTOFMEMORY;

	stream.next_out = Buffer;
	stream.avail_out = Z_BUFSIZE;

	stream.next_in = (Bytef*)varPtr.m_pData;
	stream.avail_in = varPtr.m_nSize;

	while (err != Z_STREAM_END && err != Z_BUF_ERROR)
	{
		if (stream.avail_out == 0)
		{
			hr = mStream.Write(Buffer, Z_BUFSIZE, NULL);
			if(FAILED(hr))
			{
				inflateEnd(&stream);
				return hr;
			}
			stream.next_out = Buffer;
			stream.avail_out = Z_BUFSIZE;
		}

		err = inflate(&stream, Z_NO_FLUSH);

		if (err != Z_OK && err != Z_STREAM_END && err != Z_BUF_ERROR)
		{
			inflateEnd(&stream);
			if(err == Z_MEM_ERROR)return E_OUTOFMEMORY;
			return E_INVALIDARG;
		}
	}

	if (stream.avail_out < Z_BUFSIZE)
	{
		hr = mStream.Write(Buffer, Z_BUFSIZE - stream.avail_out, NULL);
		if(FAILED(hr))
		{
			inflateEnd(&stream);
			return hr;
		}
	}

	inflateEnd(&stream);

	return mStream.GetVariant(retVal);
}

HRESULT CBEncoding::FormatMessage(LPCWSTR strFmtString, UINT len, BSTR bstr1, BSTR bstr2, BSTR bstr3, BSTR bstr4, BSTR bstr5, BSTR bstr6, BSTR bstr7, BSTR bstr8, BSTR *bstrMessage)
{
	CBString strResult;
	LPCWSTR ptr, ptr1, ptrEnd;
	BSTR* pbstrParam = &bstr1;

	ptr = strFmtString;
	ptrEnd = ptr + len;

	while(ptr < ptrEnd)
	{
		ptr1 = ptr;
		while(ptr1 < ptrEnd && *ptr1 != '%')
			ptr1 ++;

		strResult.Append(ptr, ptr1 - ptr);
		ptr = ptr1 + 1;

		if(ptr < ptrEnd)
		{
			int ch = *ptr - '1';

			if(ch >= 0 && ch <= 7)
			{
				strResult.Append(pbstrParam[ch], ::SysStringLen(pbstrParam[ch]));
				ptr ++;
			}else if(*ptr == 's' || *ptr == 'S' || *ptr == '0')
			{
				strResult.Append(pbstrParam[0], ::SysStringLen(pbstrParam[0]));
				ptr ++;
			}else if(*ptr == '%')
			{
				strResult.AppendChar('%');
				ptr ++;
			}
		}
	}

	*bstrMessage = strResult.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBEncoding::FormatMessage(BSTR bstrFmtString, BSTR bstr1, BSTR bstr2, BSTR bstr3, BSTR bstr4, BSTR bstr5, BSTR bstr6, BSTR bstr7, BSTR bstr8, BSTR *bstrMessage)
{
	return FormatMessage(bstrFmtString, ::SysStringLen(bstrFmtString), bstr1, bstr2, bstr3, bstr4, bstr5, bstr6, bstr7, bstr8, bstrMessage);
}
