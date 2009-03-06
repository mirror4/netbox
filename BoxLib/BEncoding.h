#pragma once

class CBEncoding : public CBDispatch<IEncoding>
{
public:
	STDMETHOD(Base32Decode)(BSTR base32String, VARIANT *retVal);
	STDMETHOD(Base32Encode)(VARIANT varData, short nLineBreak, BSTR *retVal);

	STDMETHOD(Base64Decode)(BSTR base64String, VARIANT *retVal);
	STDMETHOD(Base64Encode)(VARIANT varData, short nLineBreak, BSTR *retVal);

	STDMETHOD(BinToStr)(VARIANT varData, long nCodePage, BSTR *retVal);
	STDMETHOD(StrToBin)(BSTR strData, long nCodePage, VARIANT *retVal);

	STDMETHOD(CsvDecode)(BSTR bstrText, VARIANT* retVal);
	STDMETHOD(CsvEncode)(VARIANT varArray, BSTR* retVal);

	STDMETHOD(HexDecode)(BSTR HexString, VARIANT *retVal);
	STDMETHOD(HexEncode)(VARIANT varData, short nLineBreak, BSTR *retVal);

	STDMETHOD(HtmlEncode)(BSTR TextString, BSTR *retVal);
	STDMETHOD(HtmlFormat)(BSTR TextString, BSTR* retVal);

	STDMETHOD(JSEncode)(BSTR TextString, BSTR *retVal);

	STDMETHOD(SQLEncode)(VARIANT varValue, BSTR *retVal);

	STDMETHOD(UrlDecode)(BSTR urlString, BSTR *retVal);
	STDMETHOD(UrlEncode)(BSTR urlString, BSTR *retVal);

	STDMETHOD(xmlEncode)(BSTR TextString, BSTR *retVal);

	static CBString UrlDecode(LPCSTR urlString, UINT len);
	static CBString UrlDecode(CBStringA str)
	{
		return UrlDecode(str, str.GetLength());
	}

	static CBStringA UrlEncode(LPCSTR urlString, UINT len, BOOL bEncodeDBMS = FALSE);
	static CBStringA UrlEncode(CBStringA str, BOOL bEncodeDBMS = FALSE)
	{
		return UrlEncode(str, str.GetLength(), bEncodeDBMS);
	}

	STDMETHOD(Deflate)(VARIANT varData, VARIANT* retVal);
	STDMETHOD(Inflate)(VARIANT varData, VARIANT* retVal);

	static HRESULT FormatMessage(LPCWSTR strFmtString, UINT len, BSTR bstr1, BSTR bstr2, BSTR bstr3, BSTR bstr4, BSTR bstr5, BSTR bstr6, BSTR bstr7, BSTR bstr8, BSTR *bstrMessage);
	STDMETHOD(FormatMessage)(BSTR bstrFmtString, BSTR bstr1, BSTR bstr2, BSTR bstr3, BSTR bstr4, BSTR bstr5, BSTR bstr6, BSTR bstr7, BSTR bstr8, BSTR *bstrMessage);
};

