#include "StdAfx.h"
#include "BMessageManager.h"
#include "BStream.h"
#include "BCodepage.h"
#include "BEncoding.h"

void CBMessageManager::GetLanguage(LPCWSTR bstrLanguage, CColl** pMessageColl)
{
	CBComPtr<CColl> pColl;

	m_cs.Enter();
	m_mapLang.Lookup(bstrLanguage, pColl);
	m_cs.Leave();

	*pMessageColl = pColl.Detach();
}

STDMETHODIMP CBMessageManager::GetLanguage(BSTR bstrLanguage, IMessageColl** pMessageColl)
{
	CBComPtr<CColl> pColl;

	m_cs.Enter();
	m_mapLang.Lookup(bstrLanguage, pColl);
	m_cs.Leave();

	*pMessageColl = pColl.Detach();
	return S_OK;
}

STDMETHODIMP CBMessageManager::AddLanguage(BSTR bstrLanguage, long CodePage, IMessageColl** pMessageColl)
{
	CBComPtr<CColl> pColl;

	pColl.CreateInstance();
	pColl->m_strLanguage = bstrLanguage;
	pColl->m_nCodePage = CodePage;

	m_cs.Enter();
	m_mapLang.SetAt(bstrLanguage, pColl);
	m_cs.Leave();

	*pMessageColl = pColl.Detach();
	return S_OK;
}

STDMETHODIMP CBMessageManager::RemoveLanguage(BSTR bstrLanguage)
{
	m_cs.Enter();
	m_mapLang.RemoveKey(bstrLanguage);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBMessageManager::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = m_mapLang.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBMessageManager::get_Languages(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;
	POSITION pos;
	int i = 0;

	CBLock l(&m_cs);

	hr = bstrArray.Create((ULONG)m_mapLang.GetCount());
	if(FAILED(hr))return hr;

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	pos = m_mapLang.GetHeadPosition();
	while(pos)
	{
		*(CComVariant*)&pVar[i ++] = ((CRBMap<CBKeyString, CBComPtr<CColl> >::CPair*)pos)->m_value;
		m_mapLang.GetNext(pos);
	}

	l.Unlock();

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBMessageManager::Load(VARIANT VarSrc)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBComPtr<CBStream> pBStream;
	CBComPtr<CColl> pColl;
	CBStringA strLine;
	CBCodePage cp(CP_UTF8);
	int n;
	BOOL bFirstLine = TRUE;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	pBStream = pStream;
	if(pBStream == NULL)
	{
		pBStream.CreateInstance();
		pBStream->Attach(pStream);
	}

	CBLock l(&m_cs);

	m_mapLang.RemoveAll();

	while(1)
	{
		hr = pBStream->ReadLine(strLine);
		if(hr == HRESULT_FROM_WIN32(ERROR_HANDLE_EOF))
			break;
		if(FAILED(hr))return hr;

		if(bFirstLine)
		{
			bFirstLine = FALSE;
			if(((BYTE)strLine[0] == 0xEF) && ((BYTE)strLine[1] == 0xBB) && ((BYTE)strLine[2] == 0xBF))
				strLine = strLine.Mid(3);
		}

		strLine.Trim();
		if(!strLine.IsEmpty())
			if(strLine[0] == '[' && strLine[strLine.GetLength() - 1] == ']')
			{
				strLine = strLine.Mid(1, strLine.GetLength() - 2);
				n = strLine.Find(':');
				if(n != -1)
				{
					CBString strLang;

					strLang = strLine.Left(n);
					strLang.Trim();
					if(!strLang.IsEmpty())
					{
						pColl.CreateInstance();
						pColl->m_strLanguage = strLang;
						pColl->m_nCodePage = atoi((LPCTSTR)strLine + n + 1);
						m_mapLang.SetAt(strLang, pColl);

					}
				}
			}else if(pColl != NULL)
			{
				n = strLine.Find('=');
				if(n != -1)
				{
					CBString strName, strValue;

					strName = strLine.Left(n).Trim();
					if(!strName.IsEmpty())
					{
						LPWSTR ptr, ptr1, ptr2, ptr0;
						WCHAR ch;

						strValue = strLine.Mid(n + 1).Trim();
						ptr0 = ptr = ptr2 = strValue.GetBuffer();
						ptr1 = ptr + strValue.GetLength();

						while(ptr < ptr1)
						{
							ch = *ptr++;
							if(ch != '\\')*ptr2++ = ch;
							else if(ptr < ptr1)
								switch(*ptr++)
								{
								case '\\':*ptr2++ = L'\\';break;
								case 'n':*ptr2++ = L'\n';break;
								case 'r':*ptr2++ = L'\r';break;
								case 't':*ptr2++ = L'\t';break;
								}
						}

						strValue.ReleaseBuffer(ptr2 - ptr0);
						pColl->m_mapMessage.SetAt(strName, strValue);
					}
				}
			}
	}

	return S_OK;
}

STDMETHODIMP CBMessageManager::Save(VARIANT VarDesc)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBComPtr<CColl> pColl;
	CBStringA strLine;
	CBString strLineW;
	POSITION pos, pos1;
	LPCWSTR ptr, ptr1, ptr2;
	CBCodePage cp(CP_UTF8);

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	CBLock l(&m_cs);

	pos = m_mapLang.GetHeadPosition();
	while(pos)
	{
		pColl = ((CRBMap<CBKeyString, CBComPtr<CColl> >::CPair*)pos)->m_value;


		strLine = '[';
		strLine += pColl->m_strLanguage;
		strLine.AppendFormat(":%d]\r\n", pColl->m_nCodePage);
		hr = pStream->Write((LPCSTR)strLine, strLine.GetLength(), NULL);
		if(FAILED(hr))return hr;

		CBLock l1(&pColl->m_cs);

		pos1 = pColl->m_mapMessage.GetHeadPosition();
		while(pos1)
		{
			strLineW = ((CRBMap<CBKeyString, CBString>::CPair*)pos1)->m_key.m_str;
			strLineW.AppendChar(L'=');

			ptr = ((CRBMap<CBKeyString, CBString>::CPair*)pos1)->m_value;
			ptr1 = ptr + ((CRBMap<CBKeyString, CBString>::CPair*)pos1)->m_value.GetLength();
			while(ptr < ptr1)
			{
				ptr2 = ptr;
				while(ptr < ptr1 && *ptr != '\n' && *ptr != '\r' && *ptr != '\\')
					ptr ++;

				strLineW.Append(ptr2, (int)(ptr - ptr2));

				while(ptr < ptr1)
					if(*ptr == '\r')
					{
						strLineW.Append(L"\\r", 2);
						ptr ++;
					}else if(*ptr == '\n')
					{
						strLineW.Append(L"\\n", 2);
						ptr ++;
					}else if(*ptr == '\\')
					{
						strLineW.Append(L"\\\\", 2);
						ptr ++;
					}else break;
			}

			strLineW.Append(L"\r\n", 2);

			strLine = strLineW;

			hr = pStream->Write((LPCSTR)strLine, strLine.GetLength(), NULL);
			if(FAILED(hr))return hr;

			pColl->m_mapMessage.GetNext(pos1);
		}

		hr = pStream->Write("\r\n", 2, NULL);
		if(FAILED(hr))return hr;

		m_mapLang.GetNext(pos);
	}

	return S_OK;
}

CBString CBMessageManager::CColl::GetMessage(LPCWSTR bstrKey)
{
	CBString str;

	m_cs.Enter();
	m_mapMessage.Lookup(bstrKey, str);
	m_cs.Leave();

	return str;
}

STDMETHODIMP CBMessageManager::CColl::GetMessage(BSTR bstrKey, BSTR *bstrMessage)
{
	CBString str;

	m_cs.Enter();
	m_mapMessage.Lookup(bstrKey, str);
	m_cs.Leave();

	*bstrMessage = str.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::FormatMessage(BSTR bstrKey, BSTR bstr1, BSTR bstr2, BSTR bstr3, BSTR bstr4, BSTR bstr5, BSTR bstr6, BSTR bstr7, BSTR bstr8, BSTR *bstrMessage)
{
	CBString str;

	m_cs.Enter();
	m_mapMessage.Lookup(bstrKey, str);
	m_cs.Leave();

	return CBEncoding::FormatMessage(str, str.GetLength(), bstr1, bstr2, bstr3, bstr4, bstr5, bstr6, bstr7, bstr8, bstrMessage);
}

STDMETHODIMP CBMessageManager::CColl::AddMessage(BSTR bstrKey, BSTR bstrMessage)
{
	m_cs.Enter();
	m_mapMessage.SetAt(bstrKey, bstrMessage);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::RemoveMessage(BSTR bstrKey)
{
	m_cs.Enter();
	m_mapMessage.RemoveKey(bstrKey);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::get_Language(BSTR *pVal)
{
	*pVal = m_strLanguage.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::get_CodePage(long *pCodePage)
{
	*pCodePage = m_nCodePage;
	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = (long)m_mapMessage.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::get_Names(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;
	POSITION pos;
	int i = 0;

	CBLock l(&m_cs);

	hr = bstrArray.Create((ULONG)m_mapMessage.GetCount());
	if(FAILED(hr))return hr;

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	pos = m_mapMessage.GetHeadPosition();
	while(pos)
	{
		*(CComVariant*)&pVar[i ++] = ((CRBMap<CBKeyString, CBString>::CPair*)pos)->m_key.m_str;
		m_mapMessage.GetNext(pos);
	}

	l.Unlock();

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBMessageManager::CColl::get_Messages(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;
	POSITION pos;
	int i = 0;

	CBLock l(&m_cs);

	hr = bstrArray.Create((ULONG)m_mapMessage.GetCount());
	if(FAILED(hr))return hr;

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	pos = m_mapMessage.GetHeadPosition();
	while(pos)
	{
		*(CComVariant*)&pVar[i ++] = ((CRBMap<CBKeyString, CBString>::CPair*)pos)->m_value;
		m_mapMessage.GetNext(pos);
	}

	l.Unlock();

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

