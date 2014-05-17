
#pragma once


// CBoxConsole command target

class CBoxConsole : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxConsole)
public:
	CBoxConsole();

	void AllocConsole();

	CString ReadString(void);

public:
	afx_msg void Write(LPCTSTR pstr);
	afx_msg void WriteLine(LPCTSTR pstr);
	afx_msg void WriteBlankLines(long nLine);
	afx_msg long Read(void);
	afx_msg BSTR ReadLine(void);
	afx_msg BSTR ReadPassword(void);
	afx_msg void put_Caption(LPCTSTR pstrTitle);
	afx_msg BSTR get_Caption();

	DECLARE_DISPATCH_MAP()

private:
	HANDLE m_hStdin, m_hStdout;
};
