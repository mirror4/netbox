
#pragma once

#include "BoxObject.h"
#include "BoxDictionary.h"

// CBoxContents command target

class CBoxContents : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxContents)
public:
	CBoxContents(void)
	{
		m_pContents.CreateInstance();
	}

	template<class TYPE>
	void SetValue(LPCTSTR pstrKey, const TYPE value)
	{
		m_pContents->SetValue(pstrKey, value);
	}

public:
	afx_msg VARIANT get_Item(VARIANT& varKey);
	afx_msg void put_Item(VARIANT& varKey, VARIANT& var);
	afx_msg LPDISPATCH get_Contents(void);

	afx_msg LPUNKNOWN get__NewEnum(void);

	DECLARE_DISPATCH_MAP()

public:
	CBoxObject<CBoxDictionary> m_pContents;
};


