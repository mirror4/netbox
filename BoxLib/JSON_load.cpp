#include "StdAfx.h"
#include "JSON.h"
#include <atlsafe.h>
#include <math.h>
#include "BDictionary.h"
#include "BCollection.h"
#include "BListex.h"

HRESULT JSON_getValue(_parser<WCHAR>* p, VARIANT* pVar)
{
	int pos = 1;
	LPCWSTR s = p->string + p->pos;
	WCHAR ch;

	while((ch = s[pos]) && ch >= 'a' && ch <= 'z')pos ++;

	if(pos == 4 && !wcsncmp(s, L"true", 4))
	{
		pVar->vt = VT_BOOL;
		pVar->boolVal = VARIANT_TRUE;
		p->pos += 4;
		return S_OK;
	}

	if(pos == 5 && !wcsncmp(s, L"false", 5))
	{
		pVar->vt = VT_BOOL;
		pVar->boolVal = VARIANT_FALSE;
		p->pos += 5;
		return S_OK;
	}

	if(pos == 4 && !wcsncmp(s, L"null", 4))
	{
		pVar->vt = VT_EMPTY;
		p->pos += 4;
		return S_OK;
	}

	return p->throwToken(s, pos);
}

inline int getInt(LPCWSTR s, int& pos)
{
	WCHAR ch;
	int n = 0;

	while((ch = s[pos]) && ch >= '0' && ch <= '9')
	{
		n = n * 10 + ch - '0';
		pos ++;
	}

	return n;
}

HRESULT JSON_getNumber(_parser<WCHAR>* p, VARIANT* pVar)
{
	__int64 digit, frac = 0, exp = 0;
	double v, div = 1.0;
	BOOL bNeg, bExpNeg;
	WCHAR ch;

	if(bNeg = (p->get() == '-'))
		p->pos++;

	int pos = 0;
	LPCWSTR s = p->string + p->pos;

	digit = getInt(s, pos);

	ch = s[pos];
	if(ch == '.')
	{
		pos ++;

		while((ch = s[pos]) && ch >= '0' && ch <= '9')
		{
			frac = frac * 10 + ch - '0';
			div *= 10;
			pos ++;
		}
	}

	if(ch == 'e' || ch == 'E')
	{
		pos ++;

		bExpNeg = false;
		if(s[pos] == '+')
			pos ++;
		else if(s[pos] == '-')
		{
			bExpNeg = true;
			pos ++;
		}

		exp = getInt(s, pos);
		if(bExpNeg)exp = -exp;
	}

	if(div > 1 || exp < 0)
	{
		v = digit + (frac / div);
		if(bNeg)v = -v;

		if(exp != 0)
			v *= pow((double)10, (int)exp);

		p->pos += pos;
		pVar->vt = VT_R8;
		pVar->dblVal = v;

		return S_OK;
	}

	if(bNeg)digit = -digit;
	if(exp != 0)
		digit *= (__int64)pow((double)10, (int)exp);

	p->pos += pos;
	if(digit >= SHRT_MIN && digit <= SHRT_MAX)
	{
		pVar->vt = VT_I2;
		pVar->intVal = (int)digit;
		return S_OK;
	}

	if(digit >= INT_MIN && digit <= INT_MAX)
	{
		pVar->vt = VT_I4;
		pVar->intVal = (int)digit;
		return S_OK;
	}

	pVar->vt = VT_R8;
	pVar->dblVal = (DOUBLE)digit;

	return S_OK;
}

HRESULT JSON_getDate(_parser<WCHAR>* p, VARIANT* pVar)
{
	p->pos++;

	SYSTEMTIME st = {1899, 12, 0, 30};
	int pos = 0;
	LPCWSTR s = p->string + p->pos;

	st.wYear = getInt(s, pos);
	if(s[pos] == '-')
	{
		pos ++;
		st.wMonth = getInt(s, pos);

		if(s[pos ++] != '-')
		{
			p->pos += pos - 1;
			return p->throwErr(L"Invalid datetime character");
		}

		st.wDay = getInt(s, pos);

		p->pos += pos;
		p->skipSpace();

		pos = 0;
		s = p->string + p->pos;

		if(s[pos] == '#')
		{
			p->pos ++;
			pVar->vt = VT_DATE;
			::SystemTimeToVariantTime(&st, &pVar->date);
			return S_OK;
		}

		st.wHour = getInt(s, pos);
	}else
	{
		st.wHour = st.wYear;
		st.wYear = 1899;
	}

	if(s[pos ++] != ':')
	{
		p->pos += pos - 1;
		return p->throwErr(L"Invalid datetime character");
	}
	st.wMinute = getInt(s, pos);

	if(s[pos ++] != ':')
	{
		p->pos += pos - 1;
		return p->throwErr(L"Invalid datetime character");
	}
	st.wSecond = getInt(s, pos);

	if(s[pos ++] != '#')
	{
		p->pos += pos - 1;
		return p->throwErr(L"Invalid datetime character");
	}

	p->pos += pos;

	pVar->vt = VT_DATE;
	::SystemTimeToVariantTime(&st, &pVar->date);

	return S_OK;
}

inline bool isHex(WCHAR ch)
{
	return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

inline int Hex2Num(WCHAR ch)
{
	return (ch >= '0' && ch <= '9') ? ch - '0' : (ch & 0xf) + 9;
}

HRESULT JSON_getString(_parser<WCHAR>* p, VARIANT* pVar)
{
	p->pos ++;

	int pos = 0, pos1, pos2, ecs = 0, i;
	LPCWSTR s = p->string + p->pos;
	BSTR s1;
	WCHAR ch;

	while((ch = s[pos]) && ch != '\"' && ch != '\r' && ch != '\n')
	{
		if(ch == '\\')
		{
			ecs ++;
			pos ++;

			ch = s[pos];
			if(ch == 'u')
			{
				ecs += 4;
				for(i = 0; i < 4; i ++)
				{
					pos ++;
					if(!isHex(s[pos]))
					{
						p->pos += pos;
						return p->throwErr(L"Invalid hexadecimal character");
					}
				}
			}else if(ch != '\"' && ch != '\\' && ch != '/' && ch != 'b' && ch != 'f' && ch != 'n' && ch != 'r' && ch != 't')
			{
				p->pos += pos;
				return p->throwErr(L"Invalid escapement character");
			}
		}
		pos ++;
	}

	if(ch != '\"')
	{
		p->pos += pos;
		return p->throwErr('\"');
	}

	pVar->vt = VT_BSTR;
	pVar->bstrVal = s1 = ::SysAllocStringLen(NULL, pos - ecs);

	pos1 = pos2 = 0;
	while(pos1 < pos)
	{
		ch = s[pos1];
		if(ch == '\\')
		{
			pos1 ++;
			switch(s[pos1])
			{
			case '\"':
				s1[pos2 ++] = '\"';
				break;
			case '\\':
				s1[pos2 ++] = '\\';
				break;
			case '/':
				s1[pos2 ++] = '/';
				break;
			case 'b':
				s1[pos2 ++] = '\b';
				break;
			case 'f':
				s1[pos2 ++] = '\f';
				break;
			case 'n':
				s1[pos2 ++] = '\n';
				break;
			case 'r':
				s1[pos2 ++] = '\r';
				break;
			case 't':
				s1[pos2 ++] = '\t';
				break;
			case 'u':
				s1[pos2 ++] = (Hex2Num(s[pos1 + 1]) << 12) | (Hex2Num(s[pos1 + 2]) << 8) | (Hex2Num(s[pos1 + 3]) << 4) | Hex2Num(s[pos1 + 4]);
				pos1 += 4;
				break;
			}
		}else s1[pos2 ++] = ch;
		pos1 ++;
	}

	p->pos += pos + 1;
	return S_OK;
}

HRESULT JSON_getArray(_parser<WCHAR>* p, CAtlArray<CComVariant>& arrayItems)
{
	WCHAR ch;
	HRESULT hr;
	int n = (int)arrayItems.GetCount();

	if(!p->skipChar('['))
		return p->throwErr('[');

	if(p->get() != ']')
	{
		while(ch = p->get())
		{
			if(ch == ']')
				return p->throwErr(L"Unexpected comma");

			arrayItems.Add();
			hr = JSON_getVariant(p, &arrayItems[n ++]);
			if(FAILED(hr))
				return hr;

			if(!p->skipChar(','))
				break;
		}
	}

	if(!p->skipChar(']'))
		return p->throwErr(']');

	return S_OK;
}

HRESULT JSON_getArray(_parser<WCHAR>* p, VARIANT* pVar)
{
	HRESULT hr;
	CAtlArray<CComVariant> arrayItems;

	hr = JSON_getArray(p, arrayItems);
	if(FAILED(hr))
		return hr;

	CComSafeArray<VARIANT> varArray;
	int i;
	int n = (int)arrayItems.GetCount();

	hr = varArray.Create(n);
	if(FAILED(hr))
		return hr;

	CopyMemory(varArray.m_psa->pvData, &arrayItems[0], sizeof(VARIANT) * n);

	for(i = 0; i < n; i ++)
		arrayItems[i].vt = VT_EMPTY;

	pVar->vt = VT_ARRAY | VT_VARIANT;
	pVar->parray = varArray.Detach();

	return S_OK;
}

inline HRESULT getObject(WCHAR ch, VARIANT* pVar)
{
	CComDispatchDriver pDisp;
	HRESULT hr;

	if(ch == '{')
		hr = pDisp.CoCreateInstance(__uuidof(CBDictionary));
	else if(ch == '[')
		hr = pDisp.CoCreateInstance(__uuidof(CBListEx));
	else return S_FALSE;

	if(FAILED(hr))
		return hr;

	pVar->vt = VT_DISPATCH;
	pVar->pdispVal = pDisp.Detach();

	return S_OK;
}

HRESULT JSON_getVariant(_parser<WCHAR>* p, VARIANT* pVar)
{
	WCHAR ch = p->get();

	if(ch == '\"')
		return JSON_getString(p, pVar);

	if(ch == '#')
		return JSON_getDate(p, pVar);

	if(ch >= 'a' && ch <= 'z')
		return JSON_getValue(p, pVar);

	if(ch == '-' || (ch >= '0' && ch <= '9'))
		return JSON_getNumber(p, pVar);

	if(getObject(ch, pVar) == S_OK)
	{
		CBComPtr<IJSON> pJson;

		pJson = pVar->pdispVal;

		if(pJson)
			return pJson->JSON_split(p);

		pVar->pdispVal->Release();
		pVar->vt = VT_EMPTY;

		return DISP_E_UNKNOWNINTERFACE;
	}


	if(ch == '[')
		return JSON_getArray(p, pVar);

	return p->throwErr(L"Expected a value");
}
