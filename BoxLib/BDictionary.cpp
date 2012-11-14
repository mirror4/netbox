#include "StdAfx.h"
#include "BDictionary.h"
#include "BVarType.h"
#include "BMStream.h"
#include "BStream.h"
#include <atlsafe.h>

#include "json.h"


CBCriticalSection CBDictionary::m_csContents;

STDMETHODIMP CBDictionary::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hr = CBDispatch<IVariantDictionary>::g_typeinfo.GetIDsOfNames(rgszNames, cNames, rgDispId);
	if(SUCCEEDED(hr))return hr;

	UINT i, j, count;
	CComVariant varEmpty;
	CBVariant varKey;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;

	varKey.vt = VT_BSTR;

	m_cs.Enter();

	for(i = 0; i < cNames; i ++)
	{
		count = (int)m_posArray.GetCount();

		varKey.bstrVal = rgszNames[i];
		pPair = m_mapItems.Lookup(varKey);

		if(pPair != NULL)
		{
			for(j = 0; j < count; j ++)
				if(m_posArray[j] == pPair)
				{
					rgDispId[i] = j | 0x65000000;
					break;
				}
		}else
		{
			m_cs.Leave();

			varKey.vt = VT_EMPTY;

			return DISP_E_MEMBERNOTFOUND;
		}
	}

	m_cs.Leave();

	varKey.vt = VT_EMPTY;

	return S_OK;
}

STDMETHODIMP CBDictionary::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if((dispIdMember & 0xff000000) != 0x65000000)
		return CBDispatch<IVariantDictionary>::g_typeinfo.Invoke((IVariantDictionary*)this, dispIdMember, wFlags,
			pDispParams, pVarResult, pExcepInfo, puArgErr);

	dispIdMember &= 0xffffff;

	CBLock l(&m_cs);

	if(dispIdMember < (int)m_posArray.GetCount())
	{
		if((wFlags & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF)) && pDispParams->cArgs == 1)
		{
			VARIANT* pvar = pDispParams->rgvarg;

			while(pvar->vt == (VT_VARIANT | VT_BYREF))
				pvar = pvar->pvarVal;

			if(!(wFlags & DISPATCH_PROPERTYPUTREF) &&
				(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH))
			{
				l.Unlock();

				if(pvar->punkVal == NULL)
					return DISP_E_UNKNOWNINTERFACE;

				CBDispatchPtr pDisp;
				CComVariant var;

				HRESULT hr = pvar->punkVal->QueryInterface(&pDisp);
				if(FAILED(hr))return hr;

				hr = pDisp.GetProperty(0, &var);
				if(FAILED(hr))return hr;

				l.Lock();
				if(dispIdMember < (int)m_posArray.GetCount())
				{
					var.Detach(&m_posArray[dispIdMember]->m_value);
					return S_OK;
				}else return DISP_E_MEMBERNOTFOUND;
			}else return VariantCopyInd(&m_posArray[dispIdMember]->m_value, pvar);
		}else if(pDispParams->cArgs == 0)
		{
			if(pVarResult != NULL)
				return VariantCopy(pVarResult, &m_posArray[dispIdMember]->m_value);

			return S_OK;
		}

		VARIANT *pvar = &m_posArray[dispIdMember]->m_value;

		if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
		{
			CBDispatchPtr pDisp;

			HRESULT hr = pvar->punkVal->QueryInterface(&pDisp);
			if(FAILED(hr))
				return DISP_E_BADPARAMCOUNT;

			l.Unlock();
			return pDisp->Invoke(0, riid, lcid, wFlags, pDispParams,
					pVarResult, pExcepInfo, puArgErr);
		}

		return DISP_E_BADPARAMCOUNT;
	}else return DISP_E_MEMBERNOTFOUND;

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Item(VARIANT VarKey, VARIANT *pvar)
{
	if(VarKey.vt & VT_ARRAY)
		return E_INVALIDARG;

	if(VarKey.vt == VT_ERROR)
		return WriteObjectToVariant((IVariantDictionary*)this, pvar);

	m_cs.Enter();
	if(m_bArrayMode && (VarKey.vt == VT_I4 || VarKey.vt == VT_I2))
	{
		long i = varGetNumber(VarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			VariantCopy(pvar, &m_posArray[i]->m_value);
			m_cs.Leave();
			return S_OK;
		}else
		{
			m_cs.Leave();
			return DISP_E_BADINDEX;
		}
	}

	m_mapItems.Lookup(*(CBVariant*)&VarKey, *(CComVariant*)pvar);
	m_cs.Leave();

	return S_OK;
}

HRESULT CBDictionary::putItem(VARIANT* pvarKey, VARIANT* pvar)
{
	if(pvarKey->vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(pvarKey->vt & VT_ARRAY)return E_INVALIDARG;

	if(pvarKey->vt == VT_BSTR)
	{
		if(pvarKey->bstrVal == NULL)
			return DISP_E_TYPEMISMATCH;

		UINT len = SysStringByteLen(pvarKey->bstrVal);

		if(len & 1)
			return DISP_E_TYPEMISMATCH;

		for(UINT i = 0; i < len / 2; i ++)
			if(pvarKey->bstrVal[i] == 0)
				return DISP_E_TYPEMISMATCH;
	}

	if(pvar->vt == VT_ERROR)
		return DISP_E_PARAMNOTOPTIONAL;

	m_cs.Enter();
	if(m_bArrayMode && (pvarKey->vt == VT_I4 || pvarKey->vt == VT_I2))
	{
		long i = varGetNumber(*pvarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			VariantCopyInd(&m_posArray[i]->m_value, pvar);
			m_cs.Leave();
			return S_OK;
		}
	}

	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)pvarKey);

	if(pPair == NULL)
	{
		static VARIANT varTemp;

		pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(*(CBVariant*)pvarKey, varTemp);
		m_posArray.Add(pPair);
	}

	VariantCopyInd(&pPair->m_value, pvar);

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::put_Item(VARIANT VarKey, VARIANT pvar)
{
	return putItem(&VarKey, &pvar);
}

STDMETHODIMP CBDictionary::putref_Item(VARIANT VarKey, VARIANT pvar)
{
	return putItem(&VarKey, &pvar);
}

STDMETHODIMP CBDictionary::get_Key(VARIANT VarKey, VARIANT *pvar)
{
	if(VarKey.vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(VarKey.vt & VT_ARRAY)return E_INVALIDARG;

	m_cs.Enter();
	if(m_bArrayMode && (VarKey.vt == VT_I4 || VarKey.vt == VT_I2))
	{
		long i = varGetNumber(VarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			VariantCopy(pvar, (VARIANT*)&m_posArray[i]->m_key);
			m_cs.Leave();
			return S_OK;
		}else
		{
			m_cs.Leave();
			return DISP_E_BADINDEX;
		}
	}

	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&VarKey);

	if(pPair != NULL)
		VariantCopy(pvar, (VARIANT*)&pPair->m_key);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = (long)m_posArray.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBDictionary::Remove(VARIANT VarKey)
{
	if(VarKey.vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(VarKey.vt & VT_ARRAY)return E_INVALIDARG;

	m_cs.Enter();
	if(m_bArrayMode && (VarKey.vt == VT_I4 || VarKey.vt == VT_I2))
	{
		long i = varGetNumber(VarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			m_mapItems.RemoveAt(m_posArray[i]);
			m_posArray.RemoveAt(i);

			m_cs.Leave();
			return S_OK;
		}else
		{
			m_cs.Leave();
			return DISP_E_BADINDEX;
		}
	}

	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&VarKey);

	if(pPair != NULL)
	{
		for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
			if(m_posArray[i] == pPair)
			{
				m_posArray.RemoveAt(i);
				break;
			}
		m_mapItems.RemoveAt(pPair);
	}
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::RemoveAll(void)
{
	m_cs.Enter();
	m_posArray.RemoveAll();
	m_mapItems.RemoveAll();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_ArrayMode(VARIANT_BOOL *pval)
{
	m_cs.Enter();
	*pval = m_bArrayMode ? VARIANT_TRUE : VARIANT_FALSE;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::put_ArrayMode(VARIANT_BOOL val)
{
	m_cs.Enter();
	m_bArrayMode = (val != VARIANT_FALSE);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Items(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;

	m_cs.Enter();

	hr = bstrArray.Create((ULONG)m_posArray.GetCount());
	if(FAILED(hr))
	{
		m_cs.Leave();
		return hr;
	}

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		hr = VariantCopy(&pVar[i], &m_posArray[i]->m_value);
		if(FAILED(hr))
		{
			m_cs.Leave();
			bstrArray.Destroy();
			return hr;
		}
	}

	m_cs.Leave();

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Keys(VARIANT *pKeysArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;

	m_cs.Enter();

	hr = bstrArray.Create((ULONG)m_posArray.GetCount());
	if(FAILED(hr))
	{
		m_cs.Leave();
		return hr;
	}

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		hr = VariantCopy(&pVar[i], (VARIANT*)&m_posArray[i]->m_key);
		if(FAILED(hr))
		{
			m_cs.Leave();
			bstrArray.Destroy();
			return hr;
		}
	}

	m_cs.Leave();

	pKeysArray->vt = VT_ARRAY | VT_VARIANT;
	pKeysArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBDictionary::Add(VARIANT VarKey, VARIANT var)
{
	return putItem(&VarKey, &var);
}

STDMETHODIMP CBDictionary::Exists(VARIANT VarKey, VARIANT_BOOL *pExists)
{
	if(VarKey.vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(VarKey.vt & VT_ARRAY)return E_INVALIDARG;

	m_cs.Enter();
	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&VarKey);

	*pExists = (pPair != NULL) ? VARIANT_TRUE : VARIANT_FALSE;

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::Sort(VARIANT_BOOL bMode)
{
	POSITION pos;
	UINT apos;

	m_cs.Enter();

	pos = m_mapItems.GetHeadPosition();

	if(!bMode)
	{
		apos = m_posArray.GetCount() - 1;
		while(pos)
			m_posArray[apos --] = m_mapItems.GetNext(pos);
	}else
	{
		apos = 0;
		while(pos)
			m_posArray[apos ++] = m_mapItems.GetNext(pos);
	}

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::Load(VARIANT VarSrc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBDictionary* pThis = this;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return LoadObjectFromStream(pStream, CLSID_NULL, (void**)&pThis);
}

STDMETHODIMP CBDictionary::Save(VARIANT VarDesc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	if (mode & 256)
	{
		CAtlArray<void*> arrObjects;
		return JSON_join(pStream, mode&512?1:0, arrObjects);
	}
	else
	{
		SetPersistMode(mode);
		hr = WriteObjectToStream((IVariantDictionary*)this, pStream);
		if(FAILED(hr))return hr;
		if (mode & 0x4000)
			return pStream->Commit(STGC_DEFAULT);
		return hr;
	}
}

STDMETHODIMP CBDictionary::toJson(int intStyle, BSTR* pvar)
{
	HRESULT hr;
	CAtlArray<void*> arrObjects;
	CBTempStream mStream;

	hr = JSON_join(&mStream, intStyle, arrObjects);
	if(FAILED(hr))return hr;

	*pvar = ::SysAllocStringByteLen(NULL, (ULONG)mStream.GetLength());
	mStream.SeekToBegin();
	mStream.Read(*pvar, (ULONG)mStream.GetLength());
	return S_OK;
}

STDMETHODIMP CBDictionary::fromJson(BSTR bstrJson)
{
	_parser<WCHAR> p(bstrJson);
	RemoveAll();
	return JSON_split(&p);
}

STDMETHODIMP CBDictionary::Join(BSTR bstrKeyDelimiter, BSTR bstrDelimiter, BSTR* pvar)
{
	HRESULT hr;
	CBString strValue;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;

	CBLock l(&m_cs);
	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		if(i > 0)strValue.Append(bstrDelimiter);

		pPair = m_posArray[i];

		if(pPair->m_key.vt == VT_BSTR)
			strValue.Append(pPair->m_key.bstrVal);
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, (VARIANTARG*)&pPair->m_key, VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))
				return hr;
			strValue.Append(var.bstrVal);
		}

		strValue.Append(bstrKeyDelimiter);

		if(pPair->m_value.vt == VT_BSTR)
			strValue.Append(pPair->m_value.bstrVal);
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, &pPair->m_value, VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))
				return hr;
			strValue.Append(var.bstrVal);
		}
	}

	*pvar = strValue.AllocSysString();

	return S_OK;
}

inline BSTR findstr(BSTR bstr, int nCount, LPCWSTR strKey)
{
	BSTR bstrTemp;
	UINT nCountTemp;
	LPCWSTR strKeyTemp;

	while(nCount)
	{
		bstrTemp = bstr;
		nCountTemp = nCount;
		strKeyTemp = strKey;

		while(nCountTemp && *strKeyTemp && *bstrTemp == *strKeyTemp)
		{
			nCountTemp --;
			strKeyTemp ++;
			bstrTemp ++;
		}

		if(!*strKeyTemp)return bstr;

		bstr ++;
		nCount --;
	}

	return bstr;
}

STDMETHODIMP CBDictionary::Split(BSTR bstrExpression, BSTR bstrKeyDelimiter, BSTR bstrDelimiter)
{
	int nCount;
	int strKeyDelimiterLength = ::SysStringLen(bstrKeyDelimiter);
	int strDelimiterLength = ::SysStringLen(bstrDelimiter);
	BSTR pstr, pstr1;
	CBVariant varKey;
	VARIANT varValue;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;
	static VARIANT varEmpty;

	varKey.vt = VT_BSTR;
	VariantInit(&varValue);
	varValue.vt = VT_BSTR;

	nCount = ::SysStringLen(bstrExpression);

	m_cs.Enter();

	while(nCount > 0)
	{
		pstr = findstr(bstrExpression, nCount, bstrDelimiter);
		pstr1 = findstr(bstrExpression, pstr - bstrExpression, bstrKeyDelimiter);

		varKey.bstrVal = ::SysAllocStringLen(bstrExpression, pstr1 - bstrExpression);
		if(pstr1 < pstr)pstr1 += strKeyDelimiterLength;
		varValue.bstrVal = ::SysAllocStringLen(pstr1, pstr - pstr1);

		pPair = m_mapItems.Lookup(varKey);

		if(pPair == NULL)
		{
			pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(varKey, varEmpty);
			m_posArray.Add(pPair);
		}else VariantClear(&pPair->m_value);

		SysFreeString(varKey.bstrVal);

		CopyMemory(&pPair->m_value, &varValue, sizeof(VARIANT));

		nCount -= pstr + strDelimiterLength - bstrExpression;
		bstrExpression = pstr + strDelimiterLength;
	}

	m_cs.Leave();

	varKey.vt = VT_EMPTY;

	return S_OK;
}

STDMETHODIMP CBDictionary::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBDictionary);
	return S_OK;
}

STDMETHODIMP CBDictionary::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBDictionary::Load(IStream *pStm)
{
	HRESULT hr;
	CBVariant var;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;
	CComVariant varEmpty;

	CBLock l(&m_cs);

	m_bArrayMode = FALSE;
	hr = ReadStream(pStm, &m_bArrayMode, 1);
	if(FAILED(hr))return hr;

	while(TRUE)
	{
		hr = var.LoadFromStream(pStm);
		if(FAILED(hr))return hr;

		if(hr == 0xffff)
			break;

		if(var.vt == VT_ERROR || (var.vt & VT_ARRAY))
			return E_INVALIDARG;

		pPair = m_mapItems.Lookup(var);

		if(pPair == NULL)
		{
			pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(var, varEmpty);
			m_posArray.Add(pPair);
		}

		hr = ((CBVariant*)&pPair->m_value)->LoadFromStream(pStm);

		if(FAILED(hr))return hr;
	}

	return S_OK;
}

STDMETHODIMP CBDictionary::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	size_t i;

	CBLock l(&m_cs);

	hr = pStm->Write(&m_bArrayMode, 1, NULL);
	if(FAILED(hr))return hr;

	for(i = 0; i < m_posArray.GetCount(); i ++)
	{
		hr = m_posArray[i]->m_key.WriteToStream(pStm);
		if(FAILED(hr))return hr;

		hr = ((CBVariant*)&m_posArray[i]->m_value)->WriteToStream(pStm);
		if(FAILED(hr))return hr;
	}

	i = 0xffff;
	return pStm->Write(&i, sizeof(VARTYPE), NULL);
}

STDMETHODIMP CBDictionary::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBDictionary::InitNew(void)
{
	return RemoveAll();
}
/*
HRESULT CBDictionary::UTF82VARIANT(const char *pstr, int nCount, VARIANT* pvar)
{
	if (pvar==NULL) return E_INVALIDARG;

	if (nCount == 0)
		nCount = lstrlen(pstr);
	
	int _nTempCount = ::MultiByteToWideChar(65001, 0, pstr, nCount, NULL, NULL);
	CComBSTR bstr(_nTempCount);

	if (bstr==NULL) return E_OUTOFMEMORY;
	::MultiByteToWideChar(65001, 0, pstr, nCount, bstr, _nTempCount);

	pvar->vt = VT_BSTR;
	pvar->bstrVal = bstr.Detach();
	return S_OK;
}

LPSTR CBDictionary::BSTR2UTF8(BSTR strData, int *pCount)
{
	UINT strLen = SysStringByteLen(strData);
	int _nTempCount = ::WideCharToMultiByte(65001, 0, strData, strLen / 2, NULL, 0, NULL, NULL);
	LPSTR pstr = new char[_nTempCount+1];
	::WideCharToMultiByte(65001, 0, strData, strLen / 2, pstr, _nTempCount, NULL, NULL);
	pstr[_nTempCount] = 0;
	if (pCount!=NULL) *pCount = _nTempCount;
	return pstr;
}
*/
STDMETHODIMP CBDictionary::JSON_join( IStream *pStrm, int indent, CAtlArray<void*> &arrObjects)
{
	POSITION pos;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;
	HRESULT hr;
	ULONG n1;
	int indent1 = indent >= 0 ? indent + 1 : indent;

	hr = pStrm->Write(L"{\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
	if(FAILED(hr))return hr;

	CBLock l(&m_cs);
	arrObjects.Add((void *)(IDispatch *)this);

	pos = m_mapItems.GetHeadPosition();
	while(pos)
	{
		pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.GetNext(pos);

		hr = JSON_putTabs(pStrm, indent1);
		if(FAILED(hr))return hr;

		if(pPair->m_key.vt == VT_BSTR)
			hr = JSON_putVariant(pStrm, &pPair->m_key, indent1, arrObjects);
		else
		{
			CComVariant var;

			hr = ::VariantChangeType(&var, (VARIANTARG*)&pPair->m_key, VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))return hr;

			hr = JSON_putVariant(pStrm, &var, indent1, arrObjects);
		}
		if(FAILED(hr))return hr;

		if(indent != -1)
			hr = pStrm->Write(L": ", 2 * sizeof(WCHAR), &n1);
		else
			hr = pStrm->Write(L":", 1 * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;

		hr = JSON_putVariant(pStrm, &pPair->m_value, indent1, arrObjects);
		if(FAILED(hr))return hr;

		if(pos)
			hr = pStrm->Write(L",\r\n", (indent != -1 ? 3 : 1) * sizeof(WCHAR), &n1);
		else if(indent != -1)
			hr = pStrm->Write(L"\r\n", 2 * sizeof(WCHAR), &n1);
		if(FAILED(hr))return hr;
	}

	hr = JSON_putTabs(pStrm, indent);
	if(FAILED(hr))return hr;
	hr = pStrm->Write(L"}", 1 * sizeof(WCHAR), &n1);
	if(FAILED(hr))return hr;

	arrObjects.RemoveAt(arrObjects.GetCount()-1);
	return S_OK;
}

STDMETHODIMP CBDictionary::JSON_split( _parser<WCHAR>* p )
{
	WCHAR ch;
	HRESULT hr;
	CComVariant varEmpty;

	if(!p->skipChar('{'))
		return p->throwErr('{');

	CBLock l(&m_cs);
	m_bArrayMode = VARIANT_FALSE;

	while((ch = p->get()) == '\"')
	{
		CRBMap<CBVariant, CComVariant>::CPair* pPair;
		CBVariant varKey;
		VARIANT var = {VT_EMPTY};

		hr = JSON_getVariant(p, &varKey);
		if(FAILED(hr))
			return hr;

		if(!p->skipChar(':'))
			return p->throwErr(':');

		hr = JSON_getVariant(p, &var);
		if(FAILED(hr))
			return hr;

		pPair = m_mapItems.Lookup(varKey);

		if(pPair == NULL)
		{
			pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(varKey, varEmpty);
			m_posArray.Add(pPair);
		}
		else VariantClear(&pPair->m_value);
		CopyMemory(&pPair->m_value, &var, sizeof(VARIANT));

		if(!p->skipChar(','))
			break;

		if(p->get() == '}')
			return p->throwErr(L"Unexpected comma");
	}

	if(!p->skipChar('}'))
		return p->throwErr('}');

	return S_OK;
}
