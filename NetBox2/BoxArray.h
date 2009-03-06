#pragma once

#include "BoxBinPtr.h"

class CBoxArray
{
public:

	CBoxArray(void) :
		m_pData(NULL),
		m_nSize(0)
	{
	}

	~CBoxArray(void)
	{
		if(m_pData != NULL)
			delete m_pData;
	}

	void Clear(void)
	{
		if(m_pData != NULL)
			delete m_pData;

		m_pData = NULL;
		m_nSize = 0;
	}

	void SetData(void * pData, int nSize)
	{
		Alloc(nSize);

		if(m_pData != NULL)
			CopyMemory(m_pData, pData, nSize);
	}

	void Alloc(int nSize)
	{
		Clear();

		if(nSize != 0)
		{
			m_pData = new BYTE[nSize];
			m_nSize = nSize;
		}
	}

	VARIANT GetVariant(void)
	{

		if(m_pData == NULL)
			AfxThrowMemoryException();

		CBoxBinPtr varPtr(m_nSize);

		CopyMemory(varPtr, m_pData, m_nSize);

		return varPtr;
	}

	operator VARIANT(void)
	{
		return GetVariant();
	}

	void SetVariant(VARIANT& v, int nSize = -1)
	{
		CBoxBinPtr varPtr(v);

		if(nSize != -1 && nSize != varPtr.m_nSize)
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);

		SetData(varPtr, varPtr.m_nSize);
	}

	void operator=(VARIANT& v)
	{
		SetVariant(v);
	}

	BYTE * m_pData;
	int m_nSize;
};


