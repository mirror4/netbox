// BoxEncoding.cpp : implementation file
//

#include "stdafx.h"
#include "BoxEncoding.h"
#include "BoxString.h"
#include "BoxBinPtr.h"
#include <openssl\evp.h>

// CBoxEncoding

IMPLEMENT_DYNAMIC(CBoxEncoding, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxEncoding, CBoxSafeObject)
	DISP_FUNCTION(CBoxEncoding, "HtmlEncode", HtmlEncode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "HtmlDecode", HtmlDecode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "UrlEncode", UrlEncode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "UrlDecode", UrlDecode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "JSEncode", JSEncode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "JSDecode", JSDecode, VT_BSTR, VTS_BSTR)

	DISP_FUNCTION(CBoxEncoding, "BinToStr", BinToStr, VT_BSTR, VTS_VARIANT VTS_VARIANT)
	DISP_FUNCTION(CBoxEncoding, "StrToBin", StrToBin, VT_VARIANT, VTS_VARIANT VTS_VARIANT)

	DISP_FUNCTION(CBoxEncoding, "Base64Decode", Base64Decode, VT_VARIANT, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "Base64Encode", Base64Encode, VT_BSTR, VTS_VARIANT)

	DISP_FUNCTION(CBoxEncoding, "HexDecode", HexDecode, VT_VARIANT, VTS_BSTR)
	DISP_FUNCTION(CBoxEncoding, "HexEncode", HexEncode, VT_BSTR, VTS_VARIANT)
END_DISPATCH_MAP()

// CBoxEncoding message handlers

BSTR CBoxEncoding::HtmlEncode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::HtmlEncode(strText);
	return strText.AllocSysString();
}

BSTR CBoxEncoding::HtmlDecode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::HtmlDecode(strText);
	return strText.AllocSysString();
}

BSTR CBoxEncoding::UrlEncode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::UrlEncode(strText);
	return strText.AllocSysString();
}

BSTR CBoxEncoding::UrlDecode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::UrlDecode(strText);
	return strText.AllocSysString();
}

BSTR CBoxEncoding::JSEncode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::JSEncode(strText);
	return strText.AllocSysString();
}

BSTR CBoxEncoding::JSDecode(LPCTSTR pstrText)
{
	CString strText(pstrText);

	CBoxEncode::JSDecode(strText);
	return strText.AllocSysString();
}

VARIANT CBoxEncoding::StrToBin(VARIANT& var, VARIANT &varCP)
{
	CBoxBinPtr varString(var, VT_BSTR);
	UINT nCodePage;

	if(varCP.vt == VT_ERROR)
		nCodePage = _AtlGetConversionACP();
	else
	{
		CComVariant varTemp;

		varTemp.ChangeType(VT_I4, &varCP);
		if(varTemp.vt == VT_I4)
			nCodePage = varTemp.lVal;
		else
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);
	}

	int _nTempCount = ::WideCharToMultiByte(nCodePage, 0, LPWSTR(varString.m_pData), varString.m_nSize, NULL, 0, NULL, NULL);
	CBoxBinPtr varPtr(_nTempCount);

	::WideCharToMultiByte(nCodePage, 0, LPWSTR(varString.m_pData), varString.m_nSize, varPtr, _nTempCount, NULL, NULL);

	return varPtr;
}

BSTR CBoxEncoding::BinToStr(VARIANT& var, VARIANT &varCP)
{
	CBoxBinPtr varPtr(var);
	UINT nCodePage;

	if(varCP.vt == VT_ERROR)
		nCodePage = _AtlGetConversionACP();
	else
	{
		CComVariant varTemp;

		varTemp.ChangeType(VT_I4, &varCP);
		if(varTemp.vt == VT_I4)
			nCodePage = varTemp.lVal;
		else
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);
	}

	int _nTempCount = ::MultiByteToWideChar(nCodePage, 0, varPtr, varPtr.m_nSize, NULL, NULL);
	BSTR bstr = ::SysAllocStringLen(NULL, _nTempCount);
	if(bstr != NULL)
		::MultiByteToWideChar(nCodePage, 0, varPtr, varPtr.m_nSize, bstr, _nTempCount);
	return bstr;
}

VARIANT CBoxEncoding::Base64Decode(LPCTSTR pstrText)
{
#ifdef _UNICODE
	CStringA str(pstrText);
	int len = str.GetLength();
	LPCSTR pstr = str;
#else
	int len = (int)_tcslen(pstrText);
	LPCSTR pstr = pstrText;
#endif

	CBoxBinPtr varPtr((len / 4) * 3);
	int nPos = 0, nPos1, nPos2, n = 0, nSize = 0;

	while(nPos < len)
	{
		nPos1 = nPos;
		while(nPos1 < len && pstr[nPos1] != '\n')
			nPos1 ++;

		nPos2 = nPos1;
		while(nPos2 > nPos && (unsigned char)pstr[nPos2 - 1] <= ' ')
			nPos2 --;

		n = EVP_DecodeBlock((unsigned char*)varPtr + nSize, (unsigned char*)pstr + nPos, nPos2 - nPos);
		if(n != -1)
		{
			if(nPos2 > nPos && pstr[nPos2 - 1] == '=')
				n --;
			nPos2 --;
			if(nPos2 > nPos && pstr[nPos2 - 1] == '=')
				n --;
			nSize += n;
		}

		nPos = nPos1 + 1;
	}

	return varPtr.GetVariant(nSize);
}

BSTR CBoxEncoding::Base64Encode(VARIANT& var)
{
	CBoxBinPtr varPtr(var);
	CStringA str;
	LPSTR pstr;
	int strSize;
	int nPos, i;

	strSize = ((varPtr.m_nSize + 2) / 3) * 4;
	strSize += (strSize / 64) * 2;
	pstr = str.GetBuffer(strSize);

	for(i = 0, nPos = 0; i < varPtr.m_nSize; i += 48)
		if(varPtr.m_nSize - i > 48)
		{
			EVP_EncodeBlock((unsigned char*)pstr + nPos, (unsigned char*)varPtr + i, 48);
			nPos += 64;
			pstr[nPos ++] = '\r';
			pstr[nPos ++] = '\n';
		}else
			EVP_EncodeBlock((unsigned char*)pstr + nPos, (unsigned char*)varPtr + i, varPtr.m_nSize - i);

	str.ReleaseBuffer(strSize);
	return str.AllocSysString();
}

VARIANT CBoxEncoding::HexDecode(LPCTSTR pstrText)
{
	int i, len = (int)_tcslen(pstrText) / 2;
	CBoxBinPtr varPtr(len);
	BYTE ch1, ch2;

	for(i = 0; i < len; i ++)
	{
		ch1 = pstrText[i * 2];
		ch2 = pstrText[i * 2 + 1];

		if((ch1 >= 'a' && ch1 <= 'f') || (ch1 >= 'A' && ch1 <= 'F'))
			ch1 = (ch1 & 0xf) + 9;
		else if(ch1 >= '0' && ch1 <= '9')
			ch1 &= 0xf;
		else break;

		if((ch2 >= 'a' && ch2 <= 'f') || (ch2 >= 'A' && ch2 <= 'F'))
			ch2 = (ch2 & 0xf) + 9;
		else if(ch2 >= '0' && ch2 <= '9')
			ch2 &= 0xf;
		else break;

		varPtr.m_pData[i] = (ch1 << 4) + ch2;
	}

	return varPtr.GetVariant(i);
}

BSTR CBoxEncoding::HexEncode(VARIANT& var)
{
	CBoxBinPtr varPtr(var);
	CString str;
	LPTSTR pstr;
	static char HexChar[] = "0123456789ABCDEF";
	int i;

	pstr = str.GetBuffer(varPtr.m_nSize * 2);
	for(i = 0; i < varPtr.m_nSize; i ++)
	{
		pstr[i * 2] = HexChar[varPtr.m_pData[i] >> 4];
		pstr[i * 2 + 1] = HexChar[varPtr.m_pData[i] & 0xf];
	}
	str.ReleaseBuffer(i * 2);

	return str.AllocSysString();
}
