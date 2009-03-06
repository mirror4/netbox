
#pragma once


// CBoxArguments command target

class CBoxArguments : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxArguments)

public:
	CStringArray m_astrArgs;

public:
	afx_msg BSTR get_Item(long ItemNo);
	afx_msg long get_Length(void);
	afx_msg void put_CommandLine(LPCTSTR pstrCommandLine);
	DECLARE_DISPATCH_MAP()
};
