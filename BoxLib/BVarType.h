#pragma once

#include <atlsafe.h>

class CBVarPtr
{
public:
	CBVarPtr(void) :
		m_pData(NULL),
		m_nSize(0)
	{
	}

	~CBVarPtr(void)
	{
	}

	HRESULT Attach(VARIANT* pvar)
	{
		if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
		{
			HRESULT hr;
			CBDispatchPtr pDisp;

			if(pvar->punkVal == NULL)
				return DISP_E_UNKNOWNINTERFACE;

			hr = pvar->punkVal->QueryInterface(&pDisp);
			if(FAILED(hr))return hr;

			hr = pDisp.GetProperty(0, &m_varTemp);
			if(FAILED(hr))return hr;

			pvar = &m_varTemp;
		}

		if(pvar->vt == VT_BSTR)
		{
			m_pData = (BYTE *)pvar->pcVal;
			m_nSize = SysStringByteLen(pvar->bstrVal);
		}else if(pvar->vt & VT_ARRAY)
		{
			LPSAFEARRAY pArray = pvar->vt & VT_BYREF ? *pvar->pparray : pvar->parray;
			ULONG i;

			m_nSize = 0;
			switch (pvar->vt & VT_TYPEMASK)
			{
			case VT_UI1:
			case VT_I1:
				m_nSize = sizeof(BYTE);
				break;
			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
				m_nSize = sizeof(short);
				break;
			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
			case VT_ERROR:
				m_nSize = sizeof(long);
				break;
			case VT_I8:
			case VT_UI8:
				m_nSize = sizeof(LONGLONG);
				break;
			case VT_R8:
			case VT_CY:
			case VT_DATE:
				m_nSize = sizeof(double);
				break;
			default:
				return TYPE_E_TYPEMISMATCH;
			}

			if(pArray->cDims > 0)
				for(i = 0; i < pArray->cDims; i ++)
					m_nSize *= pArray->rgsabound[i].cElements;
			else m_nSize = 0;

			m_pData = (BYTE *)pArray->pvData;
		}else if(pvar->vt != VT_EMPTY)
			return TYPE_E_TYPEMISMATCH;

		return S_OK;
	}

	HRESULT Attach(VARIANT& var)
	{
		return Attach(&var);
	}

	HRESULT Create(UINT nSize)
	{
		m_Array.Destroy();

		m_Array.Create(nSize);
		LPSAFEARRAY pArray = m_Array;

		if(pArray == NULL)
			return E_OUTOFMEMORY;

		m_pData = (BYTE *)pArray->pvData;
		m_nSize = nSize;

		return S_OK;
	}

	HRESULT GetVariant(VARIANT& var, int nSize = -1)
	{
		if(nSize != -1 && nSize != m_nSize)
			m_Array.Resize(nSize);
		var.parray = m_Array.Detach();
		var.vt = VT_ARRAY | VT_UI1;

		return S_OK;
	}

	HRESULT GetVariant(VARIANT* var, int nSize = -1)
	{
		return GetVariant(*var, nSize);
	}

	BYTE * m_pData;
	UINT m_nSize;

private:
	CComVariant m_varTemp;
	CComSafeArray<BYTE> m_Array;
};

template <class T>
HRESULT inline varGetString(VARIANT* pvar, T& str)
{
	if(pvar->vt == VT_EMPTY)
	{
		str.Empty();
		return S_OK;
	}

	if(pvar->vt == VT_ERROR)
		return E_INVALIDARG;

	if(pvar->vt == VT_BSTR)
	{
		str = pvar->bstrVal;
		return S_OK;
	}

	HRESULT hr;
	CComVariant varTemp;

	hr = VariantChangeType(&varTemp, pvar, VARIANT_ALPHABOOL, VT_BSTR);
	if(FAILED(hr))return hr;

	str = varTemp.bstrVal;

	return S_OK;
}

template <class T>
HRESULT inline varGetString(VARIANT& var, T& str)
{
	return varGetString(&var, str);
}

long inline varGetNumber(VARIANT* pvar, long nDefault = 0)
{
	if(pvar->vt == VT_ERROR)
		return nDefault;

	switch (pvar->vt)
	{
	case VT_UI1:
	case VT_I1:
		return pvar->ulVal & 0xff;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		return pvar->ulVal & 0xffff;
	case VT_I4:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
		return pvar->ulVal;
	}

	CComVariant var;

	if(FAILED(var.ChangeType(VT_I4, pvar)))
		return nDefault;

	return var.lVal;
}

long inline varGetNumber(VARIANT& v, long nDefault = 0)
{
	return varGetNumber(&v, nDefault);
}

