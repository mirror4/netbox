#pragma once


// CBuildBtn

class CBuildBtn : public CButton
{
	DECLARE_DYNAMIC(CBuildBtn)

public:
	CBuildBtn();
	virtual ~CBuildBtn();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnSysColorChange();
	void CheckMouse();

private:
	CBitmap m_hBmp;
	BITMAP m_bm;
	BOOL m_bInRect;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


