#include "StdAfx.h"
#include "BoxDictEnum.h"

CBoxDictEnum::CBoxDictEnum(CBoxDictionary* pDict) : m_dwRef(1), m_nPos(0)
{
	m_pDict = pDict;
}

CBoxDictEnum::CBoxDictEnum(CBoxDictEnum* pEnum) : m_dwRef(1)
{
	m_pDict = pEnum->m_pDict;
	m_nPos = pEnum->m_nPos;
}

CBoxDictEnum::~CBoxDictEnum(void)
{
}

STDMETHODIMP_(ULONG) CBoxDictEnum::AddRef()
{
	return InterlockedIncrement(&m_dwRef);
}

STDMETHODIMP_(ULONG) CBoxDictEnum::Release()
{
	if (m_dwRef == 0)
		return 0;

	LONG lResult = InterlockedDecrement(&m_dwRef);
	if (lResult == 0)
		delete this;

	return lResult;
}

STDMETHODIMP CBoxDictEnum::QueryInterface(REFIID iid, LPVOID far* ppvObj)     
{
	*ppvObj = NULL;

	if (IsEqualIID(iid, IID_IUnknown))
		*ppvObj = (IUnknown*)this;
	else if (IsEqualIID(iid, IID_IEnumVARIANT))
		*ppvObj = (IEnumVARIANT*)this;

	if(*ppvObj)
	{
		AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}

STDMETHODIMP CBoxDictEnum::Next(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
{
	ULONG l;

	if(pCeltFetched != NULL)
		*pCeltFetched = 0;
	else if(celt > 1)
		return ResultFromScode( E_INVALIDARG ) ;   

	for(l=0; l < celt; l++)
		VariantInit( &rgVar[l] );

	long nCount = m_pDict->get_Count();

	for(l = 0; m_nPos < nCount && celt != 0; l ++, m_nPos ++)
	{
		celt-- ;
		rgVar[l].vt = VT_BSTR;
		rgVar[l].bstrVal = m_pDict->get_Key(m_nPos);
		if(pCeltFetched != NULL)(*pCeltFetched)++;
	}

	if(celt != 0)
		return ResultFromScode(S_FALSE);

    return S_OK;
}

STDMETHODIMP CBoxDictEnum::Skip(ULONG celt)
{
	long nCount = m_pDict->get_Count();
	m_nPos += celt;
	if(m_nPos > nCount)
		m_nPos = nCount;
	return S_OK;
}

STDMETHODIMP CBoxDictEnum::Reset(void)
{
	m_nPos = 0;
	return S_OK;
}

STDMETHODIMP CBoxDictEnum::Clone(IEnumVARIANT **ppEnum)
{
	*ppEnum = new CBoxDictEnum(this);
	return S_OK;
}
