// BoxDictionary.cpp : implementation file
//

#include "stdafx.h"
#include "BoxDictionary.h"
#include "BoxDictEnum.h"
#include "BoxBinPtr.h"

// CBoxDictionary

IMPLEMENT_DYNCREATE(CBoxDictionary, CBoxSafeObject)

CBoxDictionary::CBoxDictionary(INT_PTR nBlockSize) :
	m_pHashTable(NULL),
	m_nHashTableSize(191),  // default size
	m_nCount(0),
	m_pFreeList(NULL),
	m_pBlocks(NULL),
	m_nBlockSize(nBlockSize),
	m_bReadOnly(FALSE)
{
	ASSERT(nBlockSize > 0);
}

CBoxDictionary::~CBoxDictionary()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

void CBoxDictionary::SetReadOnly(BOOL bReadOnly)
{
	CSingleLock l(&m_cs, TRUE);
	m_bReadOnly = bReadOnly;
	if(m_bReadOnly)m_bAutoAdd = FALSE;
}

void CBoxDictionary::SetAutoAdd(BOOL bAutoAdd)
{
	CSingleLock l(&m_cs, TRUE);
	m_bAutoAdd = bAutoAdd;
}

inline TCHAR TCL(TCHAR ch)
{
	if(ch >= _T('A') && ch <= _T('Z'))
		ch += _T('a') - _T('A');

	return ch;
}

inline UINT CBoxDictionary::HashKey(LPCTSTR key) const
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + TCL(*key++);
	return nHash;
}

void CBoxDictionary::InitHashTable(UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

void CBoxDictionary::RemoveAll()
{
	ASSERT_VALID(this);
	CSingleLock l(&m_cs, TRUE);

	if (m_pHashTable != NULL)
	{
		// destroy elements
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				pAssoc->~CAssoc();
				//DestructElement((CString*)&pAssoc->key);  // free up string data
				//DestructElement(&pAssoc->value);

			}
		}

		// free hash table
		delete [] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;

	m_pAssocArray.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// Assoc helpers
// same as CList implementation except we store CAssoc's not CNode's
//    and CAssoc's are singly linked all the time

CBoxDictionary::CAssoc*
CBoxDictionary::NewAssoc(LPCTSTR key)
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize,
							sizeof(CAssoc));
		// chain them into free list
		CAssoc* pAssoc =
				(CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (INT_PTR i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
#pragma push_macro("new")
#undef new
	::new(pAssoc) CAssoc(key);
#pragma pop_macro("new")
	m_pAssocArray.Add(pAssoc);
//	CString::Construct(&pAssoc->key);
//	ConstructElement(&pAssoc->value);

	return pAssoc;
}

void CBoxDictionary::FreeAssoc(CAssoc* pAssoc)
{
	long i;

	for(i = 0; i < m_nCount; i ++)
        if(m_pAssocArray[i] == pAssoc)
		{
			m_pAssocArray.RemoveAt(i);
			break;
		}

	pAssoc->~CAssoc();
//	DestructElement((CString*)&pAssoc->key);  // free up string data
//	DestructElement(&pAssoc->value);

	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow
	ASSERT(m_pAssocArray.GetCount() == m_nCount);

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

CBoxDictionary::CAssoc*
CBoxDictionary::GetAssocAt(LPCTSTR key, UINT& nHashBucket, UINT& nHashValue) const
// find association (or return NULL)
{
	nHashValue = HashKey(key);
	nHashBucket = nHashValue % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHashBucket]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if ((pAssoc->nHashValue == nHashValue) && (!pAssoc->key.CompareNoCase(key)))
			return pAssoc;
	}
	return NULL;
}

BOOL CBoxDictionary::GetValue(LPCTSTR key, CComVariant& rValue)
{
	ASSERT_VALID(this);
	CSingleLock l(&m_cs, TRUE);

	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

BOOL CBoxDictionary::GetValue(long index, CComVariant& rValue)
{
	ASSERT_VALID(this);
	CSingleLock l(&m_cs, TRUE);

	if(index < 0 || index >= m_nCount)
		return FALSE;

	rValue = ((CAssoc*)m_pAssocArray[index])->value;
	return TRUE;
}

BOOL CBoxDictionary::GetKey(long index, CString& rKey)
{
	ASSERT_VALID(this);
	CSingleLock l(&m_cs, TRUE);

	if(index < 0 || index >= m_nCount)
		return FALSE;

	rKey = ((CAssoc*)m_pAssocArray[index])->key;
	return TRUE;
}

BOOL CBoxDictionary::GetAssoc(long index, CString& rKey, CComVariant& rValue)
{
	ASSERT_VALID(this);
	CSingleLock l(&m_cs, TRUE);

	if(index < 0 || index >= m_nCount)
		return FALSE;

	rKey = ((CAssoc*)m_pAssocArray[index])->key;
	rValue = ((CAssoc*)m_pAssocArray[index])->value;
	return TRUE;
}

CBoxDictionary::CAssoc*
CBoxDictionary::Lookup(VARIANT &varKey)
{
	ASSERT_VALID(this);

	CString strKey;
	UINT nHashBucket, nHashValue;

	if(varKey.vt == VT_BSTR)
		strKey = varKey.bstrVal;
	else
	{
		CComVariant varTemp;

		varTemp.ChangeType(VT_I4, &varKey);

		if(varTemp.vt != VT_I4)
		{
			varTemp.ChangeType(VT_BSTR, &varKey);
			if(varTemp.vt != VT_BSTR)
				return NULL;
			strKey = varTemp.bstrVal;
		}else
		{
			if(varTemp.lVal < 0 || varTemp.lVal >= m_nCount)
				return NULL;
			return (CAssoc*)m_pAssocArray[varTemp.lVal];
		}
	}

	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(strKey, nHashBucket, nHashValue)) == NULL && m_bAutoAdd)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc(strKey);
		pAssoc->nHashValue = nHashValue;
		// 'pAssoc->value' is a constructed object, nothing more

		InitNewValue(pAssoc->value);

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHashBucket];
		m_pHashTable[nHashBucket] = pAssoc;
	}

	return pAssoc;
}

CComVariant& CBoxDictionary::Lookup(LPCTSTR key)
{
	ASSERT_VALID(this);

	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHashBucket, nHashValue)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc(key);
		pAssoc->nHashValue = nHashValue;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHashBucket];
		m_pHashTable[nHashBucket] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

BOOL CBoxDictionary::Remove(LPCTSTR key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);
	if(!key)return FALSE;

	CSingleLock l(&m_cs, TRUE);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	UINT nHashValue;
	nHashValue = HashKey(key);
	ppAssocPrev = &m_pHashTable[nHashValue%m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if ((pAssoc->nHashValue == nHashValue) && (!pAssoc->key.CompareNoCase(key)))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

void CBoxDictionary::InitNewValue(CComVariant& var)
{
}

BEGIN_DISPATCH_MAP(CBoxDictionary, CBoxSafeObject)
	DISP_PROPERTY_PARAM_ID(CBoxDictionary, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)
	DISP_FUNCTION(CBoxDictionary, "Add", put_Item, VT_EMPTY, VTS_VARIANT VTS_VARIANT)

	DISP_FUNCTION(CBoxDictionary, "Exists", Exists, VT_BOOL, VTS_BSTR)

	DISP_PROPERTY_EX_ID(CBoxDictionary, "_NewEnum", DISPID_NEWENUM, get__NewEnum, SetNotSupported, VT_UNKNOWN)

	DISP_PROPERTY_EX(CBoxDictionary, "Count", get_Count, SetNotSupported, VT_I4)

	DISP_FUNCTION(CBoxDictionary, "Remove", RemoveDISP, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxDictionary, "RemoveAll", RemoveAllDISP, VT_EMPTY, VTS_NONE)

	DISP_PROPERTY_PARAM(CBoxDictionary, "Key", get_Key, SetNotSupported, VT_BSTR, VTS_I4)
END_DISPATCH_MAP()

long CBoxDictionary::get_Count(void)
{
	CSingleLock l(&m_cs, TRUE);
	return (long)m_nCount;
}

BSTR CBoxDictionary::get_Key(long index)
{
	CSingleLock l(&m_cs, TRUE);

	if(index < 0 || index >= m_nCount)
		return NULL;
	return ((CAssoc*)m_pAssocArray[index])->key.AllocSysString();
}

void CBoxDictionary::GetDefaultValue(CComVariant& rValue)
{
	CSingleLock l(&m_cs, TRUE);
	rValue = m_varDefault;
}

VARIANT CBoxDictionary::get_Item(VARIANT& varKey)
{
	while(varKey.vt == (VT_VARIANT | VT_BYREF))
		varKey = *varKey.pvarVal;

	VARIANT var;
	::VariantInit(&var);

	if(varKey.vt == VT_ERROR)
		GetDefaultValue(*(CComVariant*)&var);
	else
	{
		CSingleLock l(&m_cs);

		CAssoc* pAssoc = Lookup(varKey);
		if(pAssoc != NULL)
			*(CComVariant*)&var = pAssoc->value;
	}

	return var;
}

void CBoxDictionary::put_Item(VARIANT& varKey, VARIANT& var)
{
	if(!m_bReadOnly)
	{
		CString strKey;

		while(var.vt == (VT_VARIANT | VT_BYREF))
			var = *var.pvarVal;

		if(varKey.vt == VT_ERROR)
			AfxThrowOleException(TYPE_E_TYPEMISMATCH);

		if(varKey.vt == VT_BSTR)
			strKey = varKey.bstrVal;
		else
		{
			CComVariant varBSTR;
			varBSTR.ChangeType(VT_BSTR, &varKey);
			if(varBSTR.vt != VT_BSTR)
				AfxThrowOleException(TYPE_E_TYPEMISMATCH);

			strKey = varBSTR.bstrVal;
		}
		SetValue(strKey, var);
	}
}

LPUNKNOWN CBoxDictionary::get__NewEnum(void)
{
	return new CBoxDictEnum(this);
}

BOOL CBoxDictionary::Exists(LPCTSTR pstrKey)
{
	if(!pstrKey)return FALSE;

	CSingleLock l(&m_cs, TRUE);
	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(pstrKey, nHashBucket, nHashValue);

	return pAssoc != NULL;
}

void CBoxDictionary::RemoveDISP(LPCTSTR pstrKey)
{
	if(!pstrKey)return;

	if(!m_bReadOnly)
		Remove(pstrKey);
}

void CBoxDictionary::RemoveAllDISP(void)
{
	if(!m_bReadOnly)
		RemoveAll();
}
