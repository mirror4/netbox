#include "StdAfx.h"
#include "BQueue.h"
#include "BVarType.h"
#include "BMStream.h"
#include "BStream.h"
#include <atlsafe.h>

STDMETHODIMP CBQueue::get_Item(VARIANT *pvar)
{
	return WriteObjectToVariant((IQueue*)this, pvar);
}

STDMETHODIMP CBQueue::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = (long)m_listItems.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::get_Items(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;
	POSITION pos;
	int i;

	m_cs.Enter();

	hr = bstrArray.Create((ULONG)m_listItems.GetCount());
	if(FAILED(hr))
	{
		m_cs.Leave();
		return hr;
	}

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	pos = m_listItems.GetHeadPosition();
	i = 0;

	while(pos)
	{
		hr = VariantCopy(&pVar[i ++], &m_listItems.GetNext(pos));
		if(FAILED(hr))
		{
			m_cs.Leave();
			return hr;
		}
	}

	m_cs.Leave();

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBQueue::AddHead(VARIANT var)
{
	VARIANT varTemp = {VT_EMPTY};

	VariantCopyInd(&varTemp, &var);

	m_cs.Enter();
	m_listItems.AddHead(varTemp);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::AddTail(VARIANT var)
{
	VARIANT varTemp = {VT_EMPTY};

	VariantCopyInd(&varTemp, &var);

	m_cs.Enter();
	m_listItems.AddTail(varTemp);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::GetHead(VARIANT *var)
{
	m_cs.Enter();
	if(m_listItems.GetCount())
		VariantCopy(var, &m_listItems.GetHead());
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::GetTail(VARIANT *var)
{
	m_cs.Enter();
	if(m_listItems.GetCount())
		VariantCopy(var, &m_listItems.GetTail());
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::RemoveHead(VARIANT *var)
{
	m_cs.Enter();
	if(m_listItems.GetCount())
		*var = m_listItems.RemoveHead();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::RemoveTail(VARIANT *var)
{
	m_cs.Enter();
	if(m_listItems.GetCount())
		*var = m_listItems.RemoveTail();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::RemoveAll(void)
{
	HRESULT hr;

	m_cs.Enter();

	if(m_listItems.GetCount())
	{
		POSITION pos = m_listItems.GetHeadPosition();

		while(pos)
		{
			hr = VariantClear(&m_listItems.GetNext(pos));
			if(FAILED(hr))
			{
				m_cs.Leave();
				return hr;
			}
		}

		m_listItems.RemoveAll();
	}

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::Load(VARIANT VarSrc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBQueue* pThis = this;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return LoadObjectFromStream(pStream, CLSID_NULL, (void**)&pThis);
}

STDMETHODIMP CBQueue::Save(VARIANT VarDesc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return WriteObjectToStream((IQueue*)this, pStream);
}

STDMETHODIMP CBQueue::Join(BSTR bstrDelimiter, BSTR* pvar)
{
	HRESULT hr;
	CBString strValue;
	POSITION pos;
	VARIANT *pValue;
	BOOL bFirst = TRUE;

	m_cs.Enter();

	pos = m_listItems.GetHeadPosition();

	while(pos)
	{
		pValue = &m_listItems.GetNext(pos);

		if(!bFirst)strValue.Append(bstrDelimiter);
		bFirst = FALSE;

		if(pValue->vt == VT_BSTR)
			strValue.Append(pValue->bstrVal);
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, pValue, VARIANT_ALPHABOOL, VT_BSTR);
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

STDMETHODIMP CBQueue::Split(BSTR bstrExpression, BSTR bstrDelimiter)
{
	LPCWSTR pstr, pstr1;
	VARIANT varTemp = {VT_EMPTY};
	BOOL bTrim = TRUE;
	int bstrDelimiterLength = ::SysStringLen(bstrDelimiter);

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

		m_listItems.AddTail(varTemp);

		if(*pstr1)bstrExpression = (BSTR)pstr1 + bstrDelimiterLength;
		else bstrExpression = (BSTR)pstr1;
	}

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBQueue);
	return S_OK;
}

STDMETHODIMP CBQueue::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBQueue::Load(IStream *pStm)
{
	HRESULT hr;
	VARIANT var;

	m_cs.Enter();

	while(TRUE)
	{
		ZeroMemory(&var, sizeof(VARIANT));
		hr = ((CBVariant*)&var)->LoadFromStream(pStm);
		if(FAILED(hr))
		{
			m_cs.Leave();
			return hr;
		}

		if(hr == 0xffff)
			break;

		m_listItems.AddTail(var);
	}

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBQueue::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	size_t i;
	POSITION pos;

	m_cs.Enter();

	pos = m_listItems.GetHeadPosition();

	while(pos)
	{
		hr = ((CBVariant*)&m_listItems.GetNext(pos))->WriteToStream(pStm);
		if(FAILED(hr))
		{
			m_cs.Leave();
			return hr;
		}
	}

	m_cs.Leave();

	i = 0xffff;
	return pStm->Write(&i, sizeof(VARTYPE), NULL);
}

STDMETHODIMP CBQueue::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBQueue::InitNew(void)
{
	return RemoveAll();
}

