#pragma once

class CBCodePage
{
public:
	CBCodePage(UINT uiNowCP = 0);
	~CBCodePage(void);

public:
	static BOOL IsDBCSLeadByte(BYTE TestChar);
	static void setCodePage(UINT nCodePage);
	static UINT CodePageFromCharSet(LPCSTR strCharSet);

private:
	UINT m_nCodePage;
};

