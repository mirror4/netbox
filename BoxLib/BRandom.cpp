#include "StdAfx.h"
#include "BRandom.h"
#include "BVarType.h"
#include <openssl\rand.h>

STDMETHODIMP CBRandom::Generate(short nSize, VARIANT* pvar)
{
	CBVarPtr varPtr;
	HRESULT hr;

	hr = varPtr.Create(nSize);
	if(FAILED(hr))return hr;

	RAND_bytes(varPtr.m_pData, nSize);

	return varPtr.GetVariant(pvar);
}

