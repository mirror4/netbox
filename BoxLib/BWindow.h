#pragma once

#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>
#include "BGdi.h"

#define WM_DELAYACTION	(WM_USER + 1)

class __declspec(uuid("94650000-0000-0000-0000-57696E646F77"))
CBWindow : public CBDispatch<IWindow>,
		   public CWindowImpl<CBWindow, CAxWindow>
{
	DECLARE_CLASS(CBWindow)
	APARTMENT_MODE

public:
	static ATL::CWndClassInfo& GetWndClassInfo();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CBWindow)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_DELAYACTION, OnDelayAction)
		MESSAGE_HANDLER(WM_PARENTNOTIFY, OnParentNotify)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChinging)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
	END_MSG_MAP()

	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDelayAction(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnWindowPosChinging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CBString	m_strTitle;

private:
	HWND m_hWndFocus;
	CComVariant m_varReturn;

	BOOL SaveFocusControl();

	CBComPtr<IDispatchStub> m_pStubThis;

	void ClearRgn(void);
	static void StretchRgn(HRGN hRgn, BOOL bX = TRUE);

	HRGN m_hRgn1, m_hRgn2, m_hRgn3, m_hRgn4;
	HRGN m_hRgnLeft, m_hRgnTop, m_hRgnRight, m_hRgnBottom;
	int m_nMaskWidth, m_nMaskHeight;
	int m_nLeft, m_nTop, m_nRight, m_nBottom;
	int m_nCrossX, m_nCrossY;
	BOOL m_bHasCross;

	CBSize m_sizeMinTrackSize, m_sizeMaxTrackSize;

	HICON m_hIcon;

	BOOL m_bAllowClose, m_bForceClose;
	CBString m_strIcon;

	CAtlArray<HWND> m_hWnds;

public:
	CBWindow(void);
	~CBWindow(void);

	static HICON LoadIcon(LPCWSTR newVal);
	BOOL CBWindow::OnClose_OnUnload(BOOL bComfirm);

public:
	//	IWindow
	STDMETHOD(get_AllowClose)(VARIANT_BOOL *pVal);
	STDMETHOD(put_AllowClose)(VARIANT_BOOL newVal);
	STDMETHOD(get_Border)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Border)(VARIANT_BOOL newVal);
	STDMETHOD(get_Caption)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Caption)(VARIANT_BOOL newVal);
	STDMETHOD(get_clientHeight)(short *pVal);
	STDMETHOD(get_clientWidth)(short *pVal);
	STDMETHOD(get_DropShadow)(VARIANT_BOOL *pVal);
	STDMETHOD(put_DropShadow)(VARIANT_BOOL newVal);
	STDMETHOD(get_Height)(short *pVal);
	STDMETHOD(put_Height)(short newVal);
	STDMETHOD(get_HWND)(long *pVal);
	STDMETHOD(get_Icon)(BSTR* pVal);
	STDMETHOD(put_Icon)(BSTR newVal);
	STDMETHOD(get_LayeredAlpha)(short *pVal);
	STDMETHOD(put_LayeredAlpha)(short newVal);
	STDMETHOD(get_Left)(short *pVal);
	STDMETHOD(put_Left)(short newVal);
	STDMETHOD(get_MaximizeBox)(VARIANT_BOOL *pVal);
	STDMETHOD(put_MaximizeBox)(VARIANT_BOOL newVal);
	STDMETHOD(get_Maximized)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Maximized)(VARIANT_BOOL newVal);
	STDMETHOD(get_MinimizeBox)(VARIANT_BOOL *pVal);
	STDMETHOD(put_MinimizeBox)(VARIANT_BOOL newVal);
	STDMETHOD(get_Minimized)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Minimized)(VARIANT_BOOL newVal);
	STDMETHOD(get_ResizAble)(VARIANT_BOOL *pVal);
	STDMETHOD(put_ResizAble)(VARIANT_BOOL newVal);
	STDMETHOD(get_ScrollBar)(VARIANT_BOOL *pVal);
	STDMETHOD(put_ScrollBar)(VARIANT_BOOL newVal);
	STDMETHOD(get_Title)(BSTR* pVal);
	STDMETHOD(put_Title)(BSTR newVal);
	STDMETHOD(get_ToolWindow)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ToolWindow)(VARIANT_BOOL newVal);
	STDMETHOD(get_Top)(short *pVal);
	STDMETHOD(put_Top)(short newVal);
	STDMETHOD(get_TopMost)(VARIANT_BOOL *pVal);
	STDMETHOD(put_TopMost)(VARIANT_BOOL newVal);
	STDMETHOD(get_Width)(short *pVal);
	STDMETHOD(put_Width)(short newVal);
	STDMETHOD(get_WindowEdge)(VARIANT_BOOL *pVal);
	STDMETHOD(put_WindowEdge)(VARIANT_BOOL newVal);
	STDMETHOD(get_Visible)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal);

	STDMETHOD(Center)(void);
	STDMETHOD(Close)(void);
	STDMETHOD(Drag)(short nMode = 0);
	STDMETHOD(Focus)(void);
	STDMETHOD(isClosed)(VARIANT_BOOL* pVal);
	STDMETHOD(isActived)(VARIANT_BOOL* pVal);
	STDMETHOD(MoveTo)(short left, short top);
	STDMETHOD(MoveWindow)(short left, short top, short width, short height);
	STDMETHOD(ResizeTo)(short width, short height);
	STDMETHOD(ShowDialog)(VARIANT *pVal);
	STDMETHOD(EndDialog)(VARIANT retVal = VARIANT_ERROR);

	STDMETHOD(SetTransparentMask)(BSTR newVal, short nLeft = 0, short nTop = 0, short nRight = 0, short nBottom = 0);
	STDMETHOD(SetMinTrackSize)(short x = 0, short y = 0);
	STDMETHOD(SetMaxTrackSize)(short x = 0, short y = 0);
};

