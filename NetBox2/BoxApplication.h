
#pragma once

#include <BDictionary.h>

// CBoxApplication command target

class CBoxApplication : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxApplication)

public:
	CBoxApplication()
	{
		m_pContents.CreateInstance();
		m_pContents->put_ArrayMode(0);
	}

public:
	afx_msg VARIANT get_Item(VARIANT& varKey);
	afx_msg void put_Item(VARIANT& varKey, VARIANT& var);
	afx_msg LPDISPATCH get_Contents(void);

	afx_msg LPUNKNOWN get__NewEnum(void);

	afx_msg void Lock(void);
	afx_msg void Unlock(void);
	afx_msg BSTR get_ApplicationID(void);

	CString m_strHostKey;

	DECLARE_DISPATCH_MAP()

public:
	CBComPtr<CBDictionary> m_pContents;
};
