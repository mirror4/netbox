#include "StdAfx.h"
#include "BListex.h"
#include "BMStream.h"
#include "BStream.h"

STDMETHODIMP CBListEx::get_Items(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;

	m_cs.Enter();

	hr = bstrArray.Create((ULONG)m_arrayVariant.GetCount());
	if(FAILED(hr))
	{
		m_cs.Leave();
		return hr;
	}

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
	{
		hr = VariantCopy(&pVar[i], &m_arrayVariant[i]);
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

STDMETHODIMP CBListEx::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBListEx::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = (long)m_arrayVariant.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBListEx::Add(VARIANT var)
{
	VARIANT varTemp = {VT_EMPTY};

	VariantCopyInd(&varTemp, &var);

	m_cs.Enter();
	m_arrayVariant.Add(varTemp);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBListEx::Insert(long i, VARIANT var)
{
	VARIANT varTemp = {VT_EMPTY};
	HRESULT hr = S_OK;

	VariantCopyInd(&varTemp, &var);

	m_cs.Enter();
	if(i < 0 || i > (int)m_arrayVariant.GetCount())
		hr = DISP_E_BADINDEX;
	else m_arrayVariant.InsertAt(i, varTemp);
	m_cs.Leave();

	if(FAILED(hr))
		VariantClear(&varTemp);

	return hr;
}

STDMETHODIMP CBListEx::Remove(long i)
{
	if(i < 0)return DISP_E_BADINDEX;

	m_cs.Enter();
	if(i < (int)m_arrayVariant.GetCount())
	{
		VariantClear(&m_arrayVariant[i]);
		m_arrayVariant.RemoveAt(i);
		m_cs.Leave();
		return S_OK;
	}
	m_cs.Leave();

	return DISP_E_BADINDEX;
}

STDMETHODIMP CBListEx::RemoveAll(void)
{
	m_cs.Enter();

	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
		VariantClear(&m_arrayVariant[i]);

	m_arrayVariant.RemoveAll();

	m_cs.Leave();

	return S_OK;
}

static int staticCompare(VARIANT* pvar1, VARIANT* pvar2)
{
	return ((CBVariant*)pvar1)->Compare(pvar2);
}

static int staticCompareDesc(VARIANT* pvar1, VARIANT* pvar2)
{
	return ((CBVariant*)pvar2)->Compare(pvar1);
}

STDMETHODIMP CBListEx::Sort(VARIANT_BOOL bMode)
{
	m_cs.Enter();
	qsort(m_arrayVariant.GetData(), m_arrayVariant.GetCount(), sizeof(VARIANT),
		(int (__cdecl *)(const void *,const void *))(bMode ? staticCompare : staticCompareDesc));
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBListEx::Join(BSTR bstrDelimiter, BSTR* pvar)
{
	HRESULT hr;
	CBString strValue;

	m_cs.Enter();

	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
	{
		if(i > 0)strValue.Append(bstrDelimiter);

		if(m_arrayVariant[i].vt == VT_BSTR)
			strValue.Append(m_arrayVariant[i].bstrVal);
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, &m_arrayVariant[i], VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))
			{
				m_cs.Leave();
				return hr;
			}

			strValue.Append(var.bstrVal);
		}
	}

	m_cs.Leave();

	*pvar = strValue.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBListEx::Split(BSTR bstrExpression, BSTR bstrDelimiter)
{
	LPCWSTR pstr, pstr1;
	VARIANT varTemp = {VT_EMPTY};
	BOOL bTrim = TRUE;
	int strDelimiterLength = ::SysStringLen(bstrDelimiter);

	varTemp.vt = VT_BSTR;

	if(wcscmp(bstrDelimiter, L" "))
		bTrim = FALSE;

	m_cs.Enter();

	while(*bstrExpression)
	{
		if(bTrim)
			while(*bstrExpression && iswspace(*bstrExpression))
				bstrExpression ++;

		pstr = wcsstr((LPCWSTR)bstrExpression, bstrDelimiter);

		if(!pstr)
			pstr = bstrExpression + wcslen(bstrExpression);

		pstr1 = pstr;

		if(bTrim)
			while(pstr > bstrExpression && iswspace(*(pstr - 1)))
				pstr --;

		varTemp.bstrVal = ::SysAllocStringLen(bstrExpression, pstr - bstrExpression);

		m_arrayVariant.Add(varTemp);

		if(*pstr1)bstrExpression = (BSTR)pstr1 + strDelimiterLength;
		else bstrExpression = (BSTR)pstr1;
	}

	m_cs.Leave();

	return S_OK;
}


STDMETHODIMP CBListEx::Load(IStream *pStm)
{
	HRESULT hr;
	VARIANT var = {VT_EMPTY};
 
	CBLock l(&m_cs);

	while(TRUE)
	{
		ZeroMemory(&var, sizeof(VARIANT));
		hr = ((CBVariant*)&var)->LoadFromStream(pStm);
		if(FAILED(hr))return hr;

		if(hr == 0xffff)
			break;

		m_arrayVariant.Add(var);
	}

	return S_OK;
}

STDMETHODIMP CBListEx::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	size_t i;

	CBLock l(&m_cs);

	for(i = 0; i < m_arrayVariant.GetCount(); i ++)
	{
		hr = ((CBVariant*)&m_arrayVariant[i])->WriteToStream(pStm);
		if(FAILED(hr))return hr;
	}

	i = 0xffff;
	return pStm->Write(&i, sizeof(VARTYPE), NULL);
}

STDMETHODIMP CBListEx::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBListEx);
	return S_OK;
}

STDMETHODIMP CBListEx::InitNew(void)
{
	return RemoveAll();
}
