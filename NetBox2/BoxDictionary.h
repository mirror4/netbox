
#pragma once

#include <afxtempl.h>

// CBoxDictionary command target

class CBoxDictionary : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxDictionary)
protected:
	class CAssoc
	{
		friend class CBoxDictionary;
		const CString key;
		CComVariant value;
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
	public:
		CAssoc( LPCTSTR keyval ) : key( keyval ) {}
	};

public:
// Construction
	CBoxDictionary(INT_PTR nBlockSize = 10);
	~CBoxDictionary();

	template<class TYPE>
	void SetDefaultValue(const TYPE value)
	{
		CSingleLock l(&m_cs, TRUE);
		m_varDefault = value;
	}
	template<class TYPE>
	void SetValue(LPCTSTR pstrKey, const TYPE value)
	{
		CSingleLock l(&m_cs, TRUE);
		Lookup(pstrKey) = value;
	}

	virtual void GetDefaultValue(CComVariant& rValue);

	BOOL GetValue(LPCTSTR key, CComVariant& rValue);
	BOOL GetValue(long index, CComVariant& rValue);
	BOOL GetKey(long index, CString& rKey);
	BOOL GetAssoc(long index, CString& rKey, CComVariant& rValue);

	void SetReadOnly(BOOL bReadOnly = TRUE);
	void SetAutoAdd(BOOL bAutoAdd = TRUE);

	BOOL Remove(LPCTSTR key);
	void RemoveAll();

	virtual void InitNewValue(CComVariant& var);

protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CCriticalSection m_cs;
	BOOL m_bReadOnly;
	BOOL m_bAutoAdd;
	CPtrArray m_pAssocArray;

	CAssoc* NewAssoc(LPCTSTR key);
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(LPCTSTR, UINT&, UINT&) const;

	UINT HashKey(LPCTSTR key) const;

	CComVariant& Lookup(LPCTSTR key);
	CAssoc* Lookup(VARIANT &varKey);

	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	CComVariant m_varDefault;

public:
	afx_msg long get_Count(void);
	afx_msg BSTR get_Key(long index);
	afx_msg VARIANT get_Item(VARIANT& varKey);
	afx_msg void put_Item(VARIANT& varKey, VARIANT& var);
	afx_msg LPUNKNOWN get__NewEnum(void);

	afx_msg BOOL Exists(LPCTSTR pstrKey);
	afx_msg void RemoveDISP(LPCTSTR pstrKey);
	afx_msg void RemoveAllDISP(void);

	DECLARE_DISPATCH_MAP()
};
