#include "StdAfx.h"
#include "BRequestDictionary.h"
#include "BEncoding.h"
#include "BCookie.h"

CBRequestDictionary::CUploadList CBRequestDictionary::m_listEmptyUpload;
CBRequestDictionary::CStringList CBRequestDictionary::m_listEmptyString;
CBCookie CBRequestDictionary::m_dictEmptyCookie;

STDMETHODIMP CBRequestDictionary::CStringList::get_Default(VARIANT *pVariantReturn)
{
	UINT i;

	i = m_arrayVariant.GetCount();
	if(i == 1)
		return VariantCopy(pVariantReturn, &m_arrayVariant[0]);

	if(i > 0)
	{
		CBString str;

		for(i = 0; i < m_arrayVariant.GetCount(); i ++)
		{
			if(i > 0)str.Append(L", ", 2);
			str += m_arrayVariant[i].bstrVal;
		}

		pVariantReturn->vt = VT_BSTR;
		pVariantReturn->bstrVal = str.AllocSysString();
	}

	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CUploadList::get_Default(VARIANT *pVariantReturn)
{
	if(m_arrayVariant.GetCount() == 1)
	{
		CBComPtr<CBUploadData> pData;

		pData = m_arrayVariant[0].pdispVal;
		if(pData != NULL)
			return pData->get_Item(pVariantReturn);
	}

	if(m_arrayVariant.GetCount() > 0)
		return DISP_E_PARAMNOTOPTIONAL;

	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CUploadList::get_FileName(BSTR *pStrReturn)
{
	if(m_arrayVariant.GetCount() == 1)
	{
		if(m_arrayVariant[0].vt == VT_DISPATCH)
		{
			CBComPtr<CBUploadData> pData;

			pData = m_arrayVariant[0].pdispVal;
			if(pData != NULL)
				return pData->get_FileName(pStrReturn);
		}
	}

	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CUploadList::get_Size(long* pVal)
{
	if(m_arrayVariant.GetCount() == 1)
	{
		if(m_arrayVariant[0].vt == VT_DISPATCH)
		{
			CBComPtr<CBUploadData> pData;

			pData = m_arrayVariant[0].pdispVal;
			if(pData != NULL)
				return pData->get_Size(pVal);
		}
	}

	*pVal = 0;

	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CUploadList::get_ContentType(BSTR *pStrReturn)
{
	if(m_arrayVariant.GetCount() == 1)
	{
		if(m_arrayVariant[0].vt == VT_DISPATCH)
		{
			CBComPtr<CBUploadData> pData;

			pData = m_arrayVariant[0].pdispVal;
			if(pData != NULL)
				return pData->get_ContentType(pStrReturn);
		}
	}

	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CBUploadData::get_Item(VARIANT *pVariantReturn)
{
	return VariantCopy(pVariantReturn, &m_varData);
}

STDMETHODIMP CBRequestDictionary::CBUploadData::get_FileName(BSTR *pStrReturn)
{
	*pStrReturn = m_strFileName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CBUploadData::get_Size(long* pVal)
{
	*pVal = m_nSize;
	return S_OK;
}

STDMETHODIMP CBRequestDictionary::CBUploadData::get_ContentType(BSTR *pStrReturn)
{
	*pStrReturn = m_strContentType.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBRequestDictionary::get_Default(VARIANT *pVariantReturn)
{
	pVariantReturn->vt = VT_BSTR;
	pVariantReturn->bstrVal = m_strDefault.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBRequestDictionary::get_Item(VARIANT VarKey, VARIANT *pVariantReturn)
{
	if(VarKey.vt == VT_ERROR)
		return get_Default(pVariantReturn);

	HRESULT hr;

	hr = m_dict.get_Item(VarKey, pVariantReturn);
	if(hr == S_FALSE)
	{
		switch(m_modeDict)
		{
		case modeStringList:*(CComVariant*)pVariantReturn = (IStringList*)&m_listEmptyString;break;
		case modeUploadList:*(CComVariant*)pVariantReturn = (IUploadList*)&m_listEmptyUpload;break;
		case modeReadCookie:*(CComVariant*)pVariantReturn = (IReadCookie*)&m_dictEmptyCookie;break;
		}
	}

	return hr;
}

STDMETHODIMP CBRequestDictionary::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBRequestDictionary::get_Count(long *cStrRet)
{
	*cStrRet = (long)m_dict.GetCount();
	return S_OK;
}

STDMETHODIMP CBRequestDictionary::get_Key(VARIANT VarKey, VARIANT *pvar)
{
	return m_dict.get_Key(VarKey, pvar);
}

STDMETHODIMP CBRequestDictionary::Exists(VARIANT VarKey, VARIANT_BOOL *pExists)
{
	return m_dict.Exists(VarKey, pExists);
}

STDMETHODIMP CBRequestDictionary::get_Items(VARIANT *pItemsArray)
{
	return m_dict.get_Items(pItemsArray);
}

STDMETHODIMP CBRequestDictionary::get_Keys(VARIANT *pKeysArray)
{
	return m_dict.get_Keys(pKeysArray);
}

HRESULT CBRequestDictionary::ParseUrlEncodeString(LPCSTR pstr, UINT nSize)
{
	LPCSTR pstrTemp;
	CBString strKey, strValue;

	m_strDefault = CBString(pstr, nSize);

	while(nSize)
	{
		pstrTemp = pstr;
		while(nSize && *pstr != '=')
		{
			pstr ++;
			nSize --;
		}

		if(pstr > pstrTemp)
			strKey = CBEncoding::UrlDecode(pstrTemp, (UINT)(pstr - pstrTemp));
		else strKey.Empty();

		if(nSize)
		{
			nSize --;
			pstr ++;
		}

		pstrTemp = pstr;
		while(nSize && *pstr != '&')
		{
			pstr ++;
			nSize --;
		}

		if(!strKey.IsEmpty())
			if(pstr > pstrTemp)
				strValue = CBEncoding::UrlDecode(pstrTemp, (UINT)(pstr - pstrTemp));
			else strValue.Empty();

		if(nSize)
		{
			nSize --;
			pstr ++;
		}

		if(!strKey.IsEmpty())
			AddValue(strKey, strValue);
	}

	return S_OK;
}

const void *newmemchr(const void *buf, int c, size_t count, LPCSTR pstr, UINT nSize)
{
	__try{
	//---------------
	return memchr(buf, c, count);
	//---------------
	}
	__except(1){
		char strFileName[128];

		SYSTEMTIME st;
		::GetLocalTime(&st);
		wsprintf(strFileName, "%04d%02d%02d %02d%02d%02d %02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		HANDLE hFile = ::CreateFile(strFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD written;
			::SetFilePointer(hFile, 0, NULL, FILE_END);
			::WriteFile(hFile, pstr, nSize, &written, NULL);
			::CloseHandle(hFile);
		}
	}
	return NULL;
}

HRESULT CBRequestDictionary::ParseUploadString(LPCSTR pstr, UINT nSize)
{
	CBStringA strName;
	CBStringA strFileName;
	CBStringA strContentType;
	const BYTE *p, *p1, *p2, *szQueryString;
	const BYTE *pstrSplit;
	UINT uiSplitSize, uiSize;
	BYTE ch;

/*
	HANDLE hFile = ::CreateFile("uploads.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD written;
		int nSize;
		char *buf;

		while (::ReadFile(hFile, &nSize, 4, &written, NULL) && written == 4)
		{
			buf = new char[nSize];
			::ReadFile(hFile, buf, nSize, &written, NULL);

			CBComPtr<CBRequestDictionary> pForm;

			pForm.CreateInstance();
			pForm->ParseUploadString(buf, written);

			delete buf;
		}
		::CloseHandle(hFile);
	}
*//*
	///////////////////////
	static CBCriticalSection s_cs;
	char *pFileName = "uploads.dat";
	char buf[128];
	CBLock l(&s_cs);

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD written;
		::SetFilePointer(hFile, 0, NULL, FILE_END);
		::WriteFile(hFile, &nSize, 4, &written, NULL);
		::WriteFile(hFile, pstr, nSize, &written, NULL);
		::CloseHandle(hFile);
	}
	///////////////////////
*/
	m_modeDict = modeUploadList;

	szQueryString = (const BYTE *)pstr;

	if(nSize < 2 || szQueryString[0] != '-' || szQueryString[1] != '-')
		return S_OK;

	p = szQueryString;
	while(nSize > 0 && *p >= ' ')
	{
		nSize --;
		p ++;
	}

	pstrSplit = szQueryString;
	uiSplitSize = (UINT)(p - szQueryString);
	szQueryString = p;

	while(nSize)
	{
		strFileName.Empty();
		strContentType.Empty();

		while(nSize > 0)
		{
			ch = *szQueryString ++;
			nSize --;
			if(ch != '\r' && ch != '\n')return S_OK;
			if(nSize > 0 && *szQueryString + ch == '\r' + '\n')
			{
				nSize --;
				szQueryString ++;
			}

			p = szQueryString;
			while(nSize > 0 && *p >= ' ')
			{
				nSize --;
				p ++;
			}

			if (nSize == 0) break;

			p1 = szQueryString;
			szQueryString = p;

			if(p != p1)
			{
				//Content-Disposition: form-data; name="textfi&lg;<<sseld"
				if(p1 + 20 < p && !_strnicmp((char*)p1, "Content-Disposition:", 20))
				{
					p1 += 20;
					while(p1 < p && *p1 == ' ')p1 ++;
					if(p1 + 10 >= p || _strnicmp((char*)p1, "form-data;", 10))
						return S_OK;

					p1 += 10;
					while(p1 < p && *p1 == ' ')p1 ++;
					if(p1 + 4 >= p || _strnicmp((char*)p1, "name", 4))
						return S_OK;

					p1 += 4;
					while(p1 < p && *p1 == ' ')p1 ++;
					if(p1 + 1 >= p || *p1 != '=')
						return S_OK;

					p1 ++;
					while(p1 < p && *p1 == ' ')p1 ++;

					ch = ';';
					if(*p1 == '\"')
					{
						p1 ++;
						ch = '\"';
					}

					p2 = p1;
					while(p1 < p && *p1 != ch)p1 ++;

					strName.SetString((char*)p2, (int)(p1 - p2));
					if(p1 < p && *p1 == '\"')p1 ++;
					if(p1 < p && *p1 == ';')p1 ++;

					while(p1 < p && *p1 == ' ')p1 ++;
					if(p1 + 8 < p && !_strnicmp((char*)p1, "filename", 8))
					{
						p1 += 8;
						while(p1 < p && *p1 == ' ')p1 ++;
						if(p1 + 1 >= p || *p1 != '=')
							return S_OK;

						p1 ++;
						while(p1 < p && *p1 == ' ')p1 ++;

						ch = ';';
						if(*p1 == '\"')
						{
							p1 ++;
							ch = '\"';
						}

						p2 = p1;
						while(p1 < p && *p1 != ch)p1 ++;

						strFileName.SetString((char*)p2, (int)(p1 - p2));
					}
				}else if(p1 + 13 < p && !_strnicmp((char*)p1, "Content-Type:", 13))
				{
					p1 += 13;
					while(p1 < p && *p1 == ' ')p1 ++;
					strContentType.SetString((char*)p1, (int)(p - p1));
				}
			}else
			{
				ch = *szQueryString ++;
				nSize --;
				if(ch != '\r' && ch != '\n')return S_OK;
				if(nSize > 0 && *szQueryString + ch == '\r' + '\n')
				{
					nSize --;
					szQueryString ++;
				}
				break;
			}
		}

		p = szQueryString;
		p1 = p + nSize;
		//while(p1 > p && (p = (BYTE*)newmemchr(p, '-', p1 - p, pstr, nSize)) &&
		while(p1 > p && (p = (BYTE*)memchr(p, '-', p1 - p)) &&
			p1 > p + uiSplitSize &&
			memcmp(p, pstrSplit, uiSplitSize))
			p ++;

		if(!p || p1 <= p + uiSplitSize)break;

		nSize = (int)(p1 - p - uiSplitSize);
		p1 = szQueryString;
		szQueryString = p + uiSplitSize;

		p --;
		ch = *p;
		if(ch != '\r' && ch != '\n')return S_OK;
		if(*(p - 1) + ch == '\r' + '\n')p --;

		if(!strName.IsEmpty())
		{
			CBComPtr<CBUploadData> pUploadData;

			uiSize = (UINT)(p - p1);
			if(strFileName.IsEmpty())
			{
				pUploadData.CreateInstance();
				pUploadData->m_varData = CBString((char*)p1, uiSize);
			}else
			{
				CBVarPtr varPtr;
				HRESULT hr;

				hr = varPtr.Create(uiSize);
				if(FAILED(hr))return hr;

				CopyMemory(varPtr.m_pData, p1, uiSize);

				pUploadData.CreateInstance();

				hr = varPtr.GetVariant(pUploadData->m_varData);
				if(FAILED(hr))return hr;

				pUploadData->m_strFileName = strFileName;
				pUploadData->m_nSize = uiSize;
				pUploadData->m_strContentType = strContentType;
			}

			CBComPtr<CUploadList> pList;
			CBDispatchPtr &Disp = m_dict[CBString(strName)];

			if(Disp == NULL)
				Disp.CreateInstance(CUploadList::GetClassObject());

			pList = Disp;

			if(pList != NULL)pList->AddValue(pUploadData);
		}
	}

	return S_OK;
}

HRESULT CBRequestDictionary::ParseCookieString(LPCSTR pstr, UINT nSize)
{
	CBString strKey;
	LPCSTR ptr;
	char ch;

	m_modeDict = modeReadCookie;

	m_strDefault = CBString(pstr, nSize);

	while(nSize)
	{
		while(nSize && isspace(*pstr))
		{
			nSize --;
			pstr ++;
		}
		if(!nSize)break;

		ptr = pstr;
		while(nSize && (ch = *pstr) != '=' && *pstr != ';')
		{
			nSize --;
			pstr ++;
		}
		if(!nSize)break;

		strKey = CBEncoding::UrlDecode(ptr, (UINT)(pstr - ptr));
		pstr ++;
		nSize --;

		if(ch == ';')continue;

		ptr = pstr;
		while(nSize && *pstr != ';')
		{
			nSize --;
			pstr ++;
		}

		if(!strKey.IsEmpty())
		{
			CBComPtr<CBCookie> pCookie;
			CBDispatchPtr &Disp = m_dict[strKey];

			if(Disp == NULL)
				Disp.CreateInstance(CBCookie::GetClassObject());

			pCookie = Disp;
			pCookie->ParseUrlEncodeString(ptr, (UINT)(pstr - ptr));
		}

		if(nSize)
		{
			pstr ++;
			nSize --;
		}
	}

	return S_OK;
}

CBStringA CBRequestDictionary::RanderCookie(void)
{
	CBStringA strRet;
	UINT nCount = m_dict.GetCount();
	UINT i;
	CBKeyString strKey;
	CBString strValue;
	CBDispatchPtr pDisp;
	CBComPtr<CBCookie> pCookie;

	if(nCount)
	{
		for(i = 0; i < nCount; i ++)
		{
			m_dict.GetAssoc(i, strKey, pDisp);
			pCookie = pDisp;

			if(pCookie->isModified())
			{
				strRet.Append("Set-Cookie: ", 12);
				strRet += CBEncoding::UrlEncode(CBStringA(strKey.m_str));
				strRet += '=';
				strRet += pCookie->RanderCookie();
				strRet.Append("\r\n", 2);
			}
		}
	}

	return strRet;
}

