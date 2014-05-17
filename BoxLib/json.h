#pragma once

#include <time.h>

template <typename T>
class _parser
{
public:
	_parser(const T* s) : string(s), pos(0), linePos(0), lineNo(0)
	{
	}

	void skip()
	{
		T ch = string[pos];

		if(ch == '\n')
		{
			lineNo ++;
			pos ++;
			linePos = pos;
		}else if(ch != 0)
			pos ++;
	}

	void skipSpace()
	{
		T ch;

		while(ch = string[pos])
			if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t')
				skip();
			else break;
	}

	T get()
	{
		return string[pos];
	}

	BOOL skipChar(T ch)
	{
		skipSpace();
		if(string[pos] != ch)
			return FALSE;
		skip();
		skipSpace();
		return TRUE;
	}

	HRESULT throwErr(int ch)
	{
		CString s;

		s.Format("parser error (%d, %d): Syntax error, \'%c\' expected", lineNo + 1, pos - linePos + 1, ch);
		return CBComObject::SetErrorInfo(s);
	}

	HRESULT throwToken(LPCWSTR str, int n)
	{
		CString s;

		s.Format("parser error (%d, %d): Invalid token, \"%s\"", lineNo + 1, pos - linePos + 1, CString(str, n));
		return CBComObject::SetErrorInfo(s);
	}

	HRESULT throwErr(LPCWSTR str)
	{
		CString s;

		s.Format("parser error (%d, %d): %s", lineNo + 1, pos - linePos + 1, str);
		return CBComObject::SetErrorInfo(s);
	}

public:
	const T* string;
	int pos;
	int linePos;
	int lineNo;
};

class __declspec(uuid("94650000-0000-4469-6374-696F6E617888"))
IJSON : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE JSON_join( IStream *pStrm, int indent, CAtlArray<void*> &arrObjects) = 0;
	virtual HRESULT STDMETHODCALLTYPE JSON_split( _parser<WCHAR>* p ) = 0;
};


inline HRESULT JSON_putTabs(IStream *pStrm, int indent)
{
	static WCHAR s_tabs[] = L"                ";
	ULONG n, n1;
	HRESULT hr;

	indent *= 4;

	while(indent > 0)
	{
		if(indent > 16)
		{
			indent -= 16;
			n = 16;
		}else
		{
			n = indent;
			indent = 0;
		}

		hr = pStrm->Write(s_tabs, n * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;
	}

	return S_OK;
}

HRESULT JSON_putString(IStream *pStrm, LPCWSTR pStr);
HRESULT JSON_putInteger(IStream *pStrm, LONG n);
HRESULT JSON_putFloat(IStream *pStrm, double n);
HRESULT JSON_putVariant(IStream *pStrm, const VARIANT* pVar, int indent, CAtlArray<void*> &arrObjects);
HRESULT JSON_putArray(IStream *pStrm, const VARIANT* pArray, int nSize, int indent, CAtlArray<void*> &arrObjects);

HRESULT JSON_getVariant(_parser<WCHAR>* p, VARIANT* pVar);
HRESULT JSON_getArray(_parser<WCHAR>* p, CAtlArray<CComVariant>& arrayItems);
