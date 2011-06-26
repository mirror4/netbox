#include "StdAfx.h"
#include "BList.h"
#include "BVarType.h"

CBList::~CBList(void)
{
	RemoveAll();
}

STDMETHODIMP CBList::get_Default(VARIANT *pVariantReturn)
{
	return DISP_E_PARAMNOTOPTIONAL;
}

STDMETHODIMP CBList::get_Item(VARIANT i, VARIANT *pVariantReturn)
{
	if(i.vt == VT_ERROR)
		return get_Default(pVariantReturn);

	long n = varGetNumbar(i);

	if(n >=0 && n < (int)m_arrayVariant.GetCount())
		VariantCopy(pVariantReturn, &m_arrayVariant[n]);
	else return DISP_E_BADINDEX;

	return S_OK;
}

STDMETHODIMP CBList::get_Count(long *cStrRet)
{
	*cStrRet = (long)m_arrayVariant.GetCount();

	return S_OK;
}

STDMETHODIMP CBList::get_Items(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;

	hr = bstrArray.Create((ULONG)m_arrayVariant.GetCount());
	if(FAILED(hr))
		return hr;

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
	{
		hr = VariantCopy(&pVar[i], &m_arrayVariant[i]);
		if(FAILED(hr))
		{
			bstrArray.Destroy();
			return hr;
		}
	}

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBList::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

static int staticCompare(VARIANT* pvar1, VARIANT* pvar2)
{
	CBString str1, str2;

	varGetString(pvar1, str1);
	varGetString(pvar2, str2);
	
	return str1.CompareNoCase(str2);
}

void CBList::Sort(void)
{
	qsort(m_arrayVariant.GetData(), m_arrayVariant.GetCount(), sizeof(VARIANT),
		(int (__cdecl *)(const void *,const void *))staticCompare);
}

