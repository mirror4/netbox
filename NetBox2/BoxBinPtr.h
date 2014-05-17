#pragma once

#include <atlsafe.h>

class CBoxBinPtr
{
public:
	CBoxBinPtr(VARIANT& rvar) :
		m_pData(NULL),
		m_nSize(0)
	{
		VARIANT* pvar = &rvar;
		m_var.vt = VT_ERROR;

		while(pvar->vt == (VT_VARIANT | VT_BYREF))
			pvar = pvar->pvarVal;

		if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
		{
			VariantInit(&m_var);
			VariantChangeType(&m_var, pvar, 0, VT_BSTR);
			pvar = &m_var;
		}

		if(pvar->vt == VT_BSTR)
		{
			m_pData = (BYTE *)pvar->pcVal;
			m_nSize = SysStringByteLen(pvar->bstrVal);
		}else if(pvar->vt & VT_ARRAY)
		{
			m_Array.Attach(pvar->parray);
			LPSAFEARRAY pArray = m_Array;

			m_pData = (BYTE *)pArray->pvData;
			m_nSize = m_Array.GetCount() * SafeArrayGetElemsize(pArray);
		}else if(pvar->vt != VT_EMPTY)
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);
	}

	CBoxBinPtr(int nSize) :
		m_pData(NULL),
		m_nSize(0)
	{
		m_var.vt = VT_ERROR;

		Alloc(nSize);
	}

	CBoxBinPtr(void * pData, int nSize) :
		m_pData(NULL),
		m_nSize(0)
	{
		m_var.vt = VT_ERROR;

		Alloc(nSize);
		CopyMemory(m_pData, pData, nSize);
	}

	~CBoxBinPtr(void)
	{
		Clear();
	}

	void Clear(void)
	{
		if(m_Array.m_psa != NULL)
			m_Array.Detach();

		if(m_var.vt != VT_ERROR)
			VariantClear(&m_var);
	}

	void Alloc(int nSize)
	{
		Clear();

		m_Array.Create(nSize);
		LPSAFEARRAY pArray = m_Array;

		if(pArray == NULL)
			AfxThrowMemoryException();

		m_pData = (BYTE *)pArray->pvData;
		m_nSize = nSize;
	}

	void CopyData(void * pData, int nSize = -1)
	{
		if(nSize != -1 && nSize != m_nSize)
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);

        CopyMemory(pData, m_pData, m_nSize);
	}

	VARIANT GetVariant(int nSize = -1)
	{
		VARIANT var;
		VariantInit(&var);

		if(nSize != -1 && nSize != m_nSize)
			m_Array.Resize(nSize);
		var.parray = m_Array.Detach();
		var.vt = VT_ARRAY | VT_UI1;

		return var;
	}

	operator VARIANT(void)
	{
		return GetVariant();
	}

	operator char*(void)
	{
		return (char*)m_pData;
	}

	operator BYTE*(void)
	{
		return m_pData;
	}

	operator void*(void)
	{
		return (void*)m_pData;
	}

	BYTE * m_pData;
	int m_nSize;

private:
	CComSafeArray<BYTE> m_Array;
	VARIANT m_var;
};

class CBoxBSTR
{
public:
	CBoxBSTR(){}

	CBoxBSTR(VARIANT& var)
	{
		Attach(var);
	}

	CBoxBSTR(VARIANT* pvar)
	{
		Attach(pvar);
	}

	void Attach(VARIANT* pvar)
	{
		if(pvar->vt == VT_ERROR)return;

		while(pvar->vt == (VT_VARIANT | VT_BYREF))
			pvar = pvar->pvarVal;

		if((pvar->vt & VT_ARRAY) || pvar->vt == VT_UNKNOWN)
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);

		if(pvar->vt == VT_BSTR)
			m_string = pvar->bstrVal;
		else 
		{
			CComVariant varStr;

			varStr.ChangeType(VT_BSTR, pvar);

			if(varStr.vt == VT_BSTR)
				m_string = varStr.bstrVal;
			else AfxThrowOleException(TYPE_E_TYPEMISMATCH);
		}
	}

	void Attach(VARIANT& var)
	{
		Attach(&var);
	}

	operator CString() const
	{
		return m_string;
	}

	CString m_string;
};