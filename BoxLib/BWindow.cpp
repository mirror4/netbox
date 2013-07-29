#include "StdAfx.h"
#include ".\bwindow.h"
#include "BThread.h"
#include "BFile.h"
#include "BPath.h"

static __declspec(thread) CBWindow* th_pRunWindow;
static TCHAR s_strClassName[128] = _T("NetBox window class #0");
static UINT s_nWinCount = 1;

ATL::CWndClassInfo& CBWindow::GetWndClassInfo()
{
	static ATL::CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), 0, StartWindowProc,
		  0, 0, NULL, NULL, NULL, NULL, NULL, s_strClassName, NULL },
		CAxWindow::GetWndClassName(), NULL, NULL, TRUE, 0, _T("")
	};

	wc.m_atom = 0;

	return wc;
}

CBWindow::CBWindow(void) :
	m_hRgn1(NULL),
	m_hRgn2(NULL),
	m_hRgn3(NULL),
	m_hRgn4(NULL),
	m_hIcon(NULL),
	m_hWndFocus(NULL),
	m_bAllowClose(TRUE),
	m_bForceClose(FALSE),
	m_pStubThis(this),
	m_sizeMinTrackSize(0, 0),
	m_sizeMaxTrackSize(0, 0)
{
	HWND hWnd;
	DWORD dwThreadID = ::GetCurrentThreadId();

	hWnd = ::GetTopWindow(NULL);
	while(hWnd)
	{
		if(::GetWindowThreadProcessId(hWnd, NULL) == dwThreadID && ::IsWindowEnabled(hWnd) && ::IsWindowVisible(hWnd))
			m_hWnds.Add(hWnd);
		hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
	}

	Create(NULL, 0, NULL, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL);
	SetForegroundWindow(m_hWnd);

	SetExternalDispatch(m_pStubThis);

	SetIcon(LoadIcon(L""));

	_itot(s_nWinCount ++, s_strClassName + 21, 10);
}

CBWindow::~CBWindow(void)
{
	ClearRgn();

	if(IsWindow())
	{
		Close();

		CBWindow* pOld = th_pRunWindow;

		th_pRunWindow = this;
		MSG msg;

		while(GetMessage( &msg, NULL, 0, 0 ))
			if(!PreTranslateMessage(&msg))
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

		th_pRunWindow = pOld;

		if(pOld && !pOld->IsWindow())
			PostQuitMessage(0);
	}

	if(m_hIcon)DestroyIcon(m_hIcon);
}

BOOL CBWindow::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
	   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	if(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST &&
		(pMsg->wParam == VK_F5 || pMsg->wParam == VK_F6))
		return FALSE;

	// give HTML page a chance to translate this message
	return (BOOL)::SendMessage(GetActiveWindow(), WM_FORWARDMSG, 0, (LPARAM)pMsg);
}

BOOL CBWindow::SaveFocusControl()
{
	HWND hWndFocus = ::GetFocus();
	if (hWndFocus != NULL && ::IsChild(m_hWnd, hWndFocus))
	{
		m_hWndFocus = hWndFocus;
		return TRUE;
	}
	return FALSE;
}

LRESULT CBWindow::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(LOWORD(wParam) == WA_INACTIVE)
		SaveFocusControl();
	else PostMessage(WM_DELAYACTION, WM_SETFOCUS);

	bHandled = FALSE;
	return 0;
}

LRESULT CBWindow::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if((wParam & 0xFFF0) == SC_MINIMIZE)
		SaveFocusControl();

	bHandled = FALSE;
	return 0;
}

LRESULT CBWindow::OnDelayAction(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
	case WM_SETFOCUS:
		if (!m_hWndFocus || !::IsWindow(m_hWndFocus) || !::IsChild(m_hWnd, m_hWndFocus))
		{
			m_hWndFocus = NULL;

			bHandled = FALSE;
			return 0;
		}

		::SetFocus(m_hWndFocus);
		break;
	}

	return 0;
}

LRESULT CBWindow::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostMessage(WM_DELAYACTION, WM_SETFOCUS);
	return 0;
}

LRESULT CBWindow::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(	(HIWORD(lParam) != WM_MOUSEMOVE) &&
		(HIWORD(lParam) != WM_MOUSEWHEEL) &&
		th_pRunWindow &&
		(th_pRunWindow != this) &&
		!IsWindowEnabled() &&
		(GetForegroundWindow() != th_pRunWindow->m_hWnd))
       	th_pRunWindow->SetActiveWindow();
	else bHandled = FALSE;

	return 0;
}

LRESULT CBWindow::OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == WM_DESTROY)PostMessage(WM_CLOSE);

	bHandled = FALSE;
	return 0;
}

BOOL CBWindow::OnClose_OnUnload(BOOL bComfirm = FALSE)
{
	CComPtr<IWebBrowser2> pWebBrowser2;
	HRESULT hr = QueryControl(IID_IWebBrowser2, (void**)&pWebBrowser2);
	if (FAILED(hr)) return FALSE;

	CComDispatchDriver pdddoc;
	hr = pWebBrowser2->get_Document(&pdddoc);
	if(FAILED(hr)) return FALSE;
	
	CComPtr<IHTMLDocument2> pidoc;
	pidoc = pdddoc;
	if (pidoc == NULL) return FALSE;

	CComPtr<IHTMLWindow2> piwin;
	hr = pidoc->get_parentWindow(&piwin);
	if (FAILED(hr)) return FALSE;

	CComVariant varEvent;
	hr = piwin->get_onbeforeunload(&varEvent);
	if (FAILED(hr)) return FALSE;
	if (varEvent.vt != VT_DISPATCH) return FALSE;

	CComDispatchDriver pddEvent;
	pddEvent = varEvent.pdispVal;
	if (pddEvent == NULL) return FALSE;

	CComVariant varRet;
	hr = pddEvent.Invoke0((DISPID)0, &varRet);
	if(FAILED(hr)) return FALSE;

	if (varRet.vt = VT_BSTR)
		if (SysStringLen(varRet.bstrVal) > 0)
			if (bComfirm)
				return MessageBoxW(m_hWnd, varRet.bstrVal, m_strTitle, MB_OKCANCEL) == IDCANCEL;
			else
				return TRUE;

	return FALSE;
}

LRESULT CBWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bForbidden_OnUnload = OnClose_OnUnload(m_bForceClose == FALSE && m_bAllowClose == TRUE);
	
	if (!m_bForceClose)
	{
		if(!m_bAllowClose || bForbidden_OnUnload)
		{
			bHandled = TRUE;
			return 0;
		}
	}

	if(th_pRunWindow == this && m_hWnds.GetCount())
	{
		::EnableWindow(m_hWnds[0], TRUE);
		::PostMessage(m_hWnds[0], WM_SETFOCUS, 0, 0);
	}

	bHandled = FALSE;

	CBComPtr<IOleObject> pObj;

	HRESULT hr = QueryControl(IID_IOleObject, (void**)&pObj);
	if(FAILED(hr))return 0;

	pObj->Close(0);

	return 0;
}

LRESULT CBWindow::OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CBWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(th_pRunWindow == this)
	{
		for(UINT i = 0; i < m_hWnds.GetCount(); i ++)
			::EnableWindow(m_hWnds[i], TRUE);
		m_hWnds.RemoveAll();
		PostQuitMessage(0);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CBWindow::OnWindowPosChinging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WINDOWPOS* pPos = (WINDOWPOS*)lParam;

	if(!(pPos->flags & SWP_NOZORDER) && !IsWindowEnabled())
	{
		pPos->flags |= SWP_NOZORDER;
		if(th_pRunWindow)th_pRunWindow->PostMessage(WM_SETFOCUS);
	}

	return 0;
}

LRESULT CBWindow::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* pMMInfo = (MINMAXINFO*)lParam;
	if(m_hRgn1)
	{
		pMMInfo->ptMinTrackSize.x = m_nMaskWidth;
		pMMInfo->ptMinTrackSize.y = m_nMaskHeight;
	}else
	{
		pMMInfo->ptMinTrackSize.x = m_sizeMinTrackSize.cx;
		pMMInfo->ptMinTrackSize.y = m_sizeMinTrackSize.cy;
	}

	if(m_sizeMaxTrackSize.cx)
		pMMInfo->ptMaxTrackSize.x = m_sizeMaxTrackSize.cx;

	if(m_sizeMaxTrackSize.cy)
		pMMInfo->ptMaxTrackSize.y = m_sizeMaxTrackSize.cy;

	bHandled = FALSE;
	return 0;
}

LRESULT CBWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_hRgn1 && lParam)
	{
		int cx, cy;
		HRGN hRgn, hRgn1;

		cx = LOWORD(lParam) - m_nMaskWidth;
		cy = HIWORD(lParam) - m_nMaskHeight;

		hRgn = ::CreateRectRgn(0, 0, 0, 0);
		hRgn1 = ::CreateRectRgn(0, 0, 0, 0);

		CombineRgn(hRgn, m_hRgn3, hRgn, RGN_COPY);
		if(cy)OffsetRgn(hRgn, 0, cy);

		CombineRgn(hRgn, m_hRgn2, hRgn, RGN_OR);
		if(cx)OffsetRgn(hRgn, cx, 0);

		CombineRgn(hRgn1, m_hRgn4, hRgn1, RGN_COPY);
		if(cy)OffsetRgn(hRgn1, 0, cy);

		CombineRgn(hRgn, hRgn1, hRgn, RGN_OR);
		CombineRgn(hRgn, m_hRgn1, hRgn, RGN_OR);
		DeleteObject(hRgn1);

		POINT points[4];

		if(cx)
		{
			ZeroMemory(points, sizeof(points));
			points[1].x = cx + m_nTop;
			points[2].y = points[3].y = m_nMaskHeight;
			points[2].x = cx + m_nBottom;

			hRgn1 = CreatePolygonRgn(points, 4, WINDING);
			CombineRgn(hRgn1, m_hRgnTop, hRgn1, RGN_AND);
			CombineRgn(hRgn, hRgn1, hRgn, RGN_OR);
			DeleteObject(hRgn1);

			hRgn1 = CreatePolygonRgn(points, 4, WINDING);
			CombineRgn(hRgn1, m_hRgnBottom, hRgn1, RGN_AND);
			if(cy)OffsetRgn(hRgn1, 0, cy);
			CombineRgn(hRgn, hRgn1, hRgn, RGN_OR);
			DeleteObject(hRgn1);
		}

		if(cy)
		{
			ZeroMemory(points, sizeof(points));
			points[1].x = points[2].x = m_nMaskWidth;
			points[2].y = cy + m_nRight;
			points[3].y = cy + m_nLeft;

			hRgn1 = CreatePolygonRgn(points, 4, WINDING);
			CombineRgn(hRgn1, m_hRgnLeft, hRgn1, RGN_AND);
			CombineRgn(hRgn, hRgn1, hRgn, RGN_OR);
			DeleteObject(hRgn1);

			hRgn1 = CreatePolygonRgn(points, 4, WINDING);
			CombineRgn(hRgn1, m_hRgnRight, hRgn1, RGN_AND);
			if(cx)OffsetRgn(hRgn1, cx, 0);
			CombineRgn(hRgn, hRgn1, hRgn, RGN_OR);
			DeleteObject(hRgn1);
		}

		if(cx && cy && m_bHasCross)
		{
			hRgn1 = ::CreateRectRgn(m_nCrossX, m_nCrossY, m_nCrossX + cx, m_nCrossY + cy);
			CombineRgn(hRgn, hRgn1, hRgn, RGN_OR);
			DeleteObject(hRgn1);
		}

		SetWindowRgn(hRgn, TRUE);
	}

	bHandled = FALSE;
	return 0;
}

HRESULT CBWindow::get_AllowClose(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = m_bAllowClose ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_AllowClose(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	m_bAllowClose = (newVal != VARIANT_FALSE);

	return S_OK;
}

HRESULT CBWindow::get_Border(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetStyle() & (WS_CAPTION | WS_THICKFRAME)) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_Border(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ModifyStyle(0, WS_CAPTION | WS_THICKFRAME);
	else ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0, SWP_FRAMECHANGED);

	return S_OK;
}

HRESULT CBWindow::get_Caption(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetStyle() & WS_DLGFRAME) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_Caption(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_NOTIMPL;

	if(newVal != VARIANT_FALSE)
		ModifyStyle(0, WS_DLGFRAME);
	else ModifyStyle(WS_DLGFRAME, 0, SWP_FRAMECHANGED);

	return S_OK;
}

HRESULT CBWindow::get_clientHeight(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetClientRect(&rect);

	*pVal = (short)(rect.bottom - rect.top);

	return S_OK;
}

HRESULT CBWindow::get_clientWidth(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetClientRect(&rect);

	*pVal = (short)(rect.right - rect.left);

	return S_OK;
}

HRESULT CBWindow::get_DropShadow(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetClassLong(m_hWnd, GCL_STYLE) & 0x00020000) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_DropShadow(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal)
		SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) | 0x00020000);
	else SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) & ~0x00020000);

	return S_OK;
}

HRESULT CBWindow::get_Height(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	*pVal = (short)(rect.bottom - rect.top);

	return S_OK;
}

HRESULT CBWindow::put_Height(short newVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	SetWindowPos(NULL, 0, 0, rect.right - rect.left, newVal, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::get_HWND(long *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (long)m_hWnd;

	return S_OK;
}

HRESULT CBWindow::get_Icon(BSTR* pVal)
{
	if(!IsWindow())return E_HANDLE;

	m_strIcon.AllocSysString();
	return S_OK;
}

HRESULT CBWindow::put_Icon(BSTR newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(m_strIcon.CompareNoCase(newVal))
	{
		HICON hIcon = m_hIcon;

		m_hIcon = LoadIcon(newVal);
		SetIcon(m_hIcon);
		if(hIcon)DestroyIcon(m_hIcon);

		m_strIcon = newVal;
	}

	return S_OK;
}

HRESULT CBWindow::get_LayeredAlpha(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	BOOL (__stdcall *GetLayeredWindowAttributes)(HWND hwnd, COLORREF *pcrKey, BYTE *pbAlpha, DWORD *pdwFlags);

	*pVal = 0;
	GetLayeredWindowAttributes = (BOOL (__stdcall *)(HWND,COLORREF *,BYTE *,DWORD *))GetProcAddress(LoadLibrary("USER32"), "GetLayeredWindowAttributes");
	if(GetLayeredWindowAttributes)
		GetLayeredWindowAttributes(m_hWnd, NULL, (BYTE*)pVal, NULL);

	return S_OK;
}

HRESULT CBWindow::put_LayeredAlpha(short newVal)
{
	if(!IsWindow())return E_HANDLE;
	if(newVal < 0 || newVal > 255)return E_INVALIDARG;

	if(newVal == 255)
		ModifyStyleEx(0x00080000, 0, TRUE);
	else
	{
		BOOL (__stdcall *SetLayeredWindowAttributes)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

		SetLayeredWindowAttributes = (BOOL(__stdcall *)(HWND,COLORREF,BYTE,DWORD))GetProcAddress(LoadLibrary("USER32"), "SetLayeredWindowAttributes");
		if(SetLayeredWindowAttributes)
		{
			ModifyStyleEx(0, 0x00080000);
			SetLayeredWindowAttributes(m_hWnd, 0, (BYTE)newVal, 0x00000002);
		}
	}

	return S_OK;
}

HRESULT CBWindow::get_Left(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	*pVal = (short)rect.left;

	return S_OK;
}

HRESULT CBWindow::put_Left(short newVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	SetWindowPos(NULL, newVal, rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::get_MaximizeBox(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetStyle() & WS_MAXIMIZEBOX) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_MaximizeBox(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ModifyStyle(0, WS_MAXIMIZEBOX);
	else ModifyStyle(WS_MAXIMIZEBOX, 0);

	return S_OK;
}

HRESULT CBWindow::get_Maximized(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = IsZoomed() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_Maximized(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ShowWindow(SW_MAXIMIZE);
	else ShowWindow(SW_RESTORE);

	return S_OK;
}

HRESULT CBWindow::get_MinimizeBox(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetStyle() & WS_MINIMIZEBOX) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_MinimizeBox(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ModifyStyle(0, WS_MINIMIZEBOX);
	else ModifyStyle(WS_MINIMIZEBOX, 0);

	return S_OK;
}

HRESULT CBWindow::get_Minimized(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = IsIconic() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_Minimized(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
	{
		SaveFocusControl();
		ShowWindow(SW_MINIMIZE);
	}else ShowWindow(SW_RESTORE);

	return S_OK;
}

HRESULT CBWindow::get_ResizAble(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetStyle() & WS_THICKFRAME) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_ResizAble(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ModifyStyle(0, WS_THICKFRAME);
	else ModifyStyle(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME, 0, SWP_FRAMECHANGED);

	return S_OK;
}

HRESULT CBWindow::get_ScrollBar(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetStyle() & (WS_VSCROLL | WS_HSCROLL)) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_ScrollBar(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	ShowScrollBar(SB_BOTH, newVal != VARIANT_FALSE);

	return S_OK;
}

HRESULT CBWindow::get_Title(BSTR* pVal)
{
	if(!IsWindow())return E_HANDLE;

	GetWindowText(*pVal);

	return S_OK;
}

HRESULT CBWindow::put_Title(BSTR newVal)
{
	if(!IsWindow())return E_HANDLE;

	m_strTitle = newVal;
	SetWindowTextW(m_hWnd, m_strTitle);

	return S_OK;
}

HRESULT CBWindow::get_ToolWindow(VARIANT_BOOL* pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetExStyle() & WS_EX_TOOLWINDOW) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_ToolWindow(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ModifyStyleEx(0, WS_EX_TOOLWINDOW);
	else ModifyStyleEx(WS_EX_TOOLWINDOW, 0, SWP_FRAMECHANGED);

	return S_OK;
}

HRESULT CBWindow::get_Top(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	*pVal = (short)rect.top;

	return S_OK;
}

HRESULT CBWindow::put_Top(short newVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	SetWindowPos(NULL, rect.left, newVal, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::get_TopMost(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetExStyle() & WS_EX_TOPMOST) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_TopMost(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	else SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	return S_OK;
}

HRESULT CBWindow::get_Width(short *pVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	*pVal = (short)(rect.right - rect.left);

	return S_OK;
}

HRESULT CBWindow::put_Width(short newVal)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect;

	GetWindowRect(&rect);

	SetWindowPos(NULL, 0, 0, newVal, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::get_WindowEdge(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetExStyle() & WS_EX_CLIENTEDGE) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_WindowEdge(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	if(newVal != VARIANT_FALSE)
		ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	else ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	return S_OK;
}

HRESULT CBWindow::get_Visible(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = IsWindowVisible() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::put_Visible(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	ShowWindow(newVal == VARIANT_FALSE ? SW_HIDE : SW_SHOW);

	return S_OK;
}

void CBWindow::ClearRgn(void)
{
	if(m_hRgn1)
	{
		DeleteObject(m_hRgn1);
		DeleteObject(m_hRgn2);
		DeleteObject(m_hRgn3);
		DeleteObject(m_hRgn4);

		DeleteObject(m_hRgnLeft);
		DeleteObject(m_hRgnTop);
		DeleteObject(m_hRgnRight);
		DeleteObject(m_hRgnBottom);
	}

	m_hRgn1 = NULL;
	m_hRgn2 = NULL;
	m_hRgn3 = NULL;
    m_hRgn4 = NULL;

	m_hRgnLeft = NULL;
	m_hRgnTop = NULL;
	m_hRgnRight = NULL;
	m_hRgnBottom = NULL;

	m_nCrossX = 0;
	m_nCrossY = 0;
	m_bHasCross = FALSE;
}

class clsWindowRgn
{
public:
	HRGN m_hRgn;
	short m_cx, m_cy;
};

static CBTls< CRBMap<CBKeyString, clsWindowRgn> > th_RegionPool;

HRESULT CBWindow::SetTransparentMask(BSTR newVal, short nLeft, short nTop, short nRight, short nBottom)
{
	if(!*newVal)
	{
		ClearRgn();
		SetWindowRgn(NULL, TRUE);
		return S_OK;
	}

	CBTlsPtr< CRBMap<CBKeyString, clsWindowRgn> > pRegionPool(th_RegionPool);
	HRGN hRgn;
	RECT rect;
	clsWindowRgn oRgn;;

	if(pRegionPool && pRegionPool->Lookup(newVal, oRgn))
	{
		hRgn = ::CreateRectRgn(0, 0, 0, 0);
		::CombineRgn(hRgn, oRgn.m_hRgn, hRgn, RGN_COPY);
	}else
	{
		CBComPtr<CBFile> pStream;
		CBComPtr<IPicture> pPicture;
		HRESULT hr;

		hr = pStream.CreateInstance();
		if(FAILED(hr))return hr;

		hr = pStream->Open(newVal);
		if(FAILED(hr))return hr;

		hr = ::OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (LPVOID *)&pPicture);
		if(FAILED(hr))return hr;

		pStream.Release();

		HDC hMemDC = CreateCompatibleDC(NULL);

		long cx, cy;

		pPicture->get_Width(&cx);
		pPicture->get_Height(&cy);

		const int HIMETRIC_INCH = 2540;

		// convert himetric to pixels
		oRgn.m_cx = MulDiv(cx, GetDeviceCaps(hMemDC, LOGPIXELSX), HIMETRIC_INCH);
		oRgn.m_cy = MulDiv(cy, GetDeviceCaps(hMemDC, LOGPIXELSY), HIMETRIC_INCH);

		BITMAPINFOHEADER bi =
		{sizeof(BITMAPINFOHEADER), oRgn.m_cx, oRgn.m_cy * 2, 1, 32, BI_RGB, 0, 0, 0, 0, 0};

		LPDWORD pbits32; 
		HBITMAP hMemBmp = CreateDIBSection(hMemDC, (LPBITMAPINFO)&bi, DIB_RGB_COLORS, (LPVOID*)&pbits32, NULL, 0);
		HGDIOBJ hBmpOld = SelectObject(hMemDC, hMemBmp);

		rect.left = 0;
		rect.top = 0;
		rect.right = oRgn.m_cx;
		rect.bottom = oRgn.m_cy;

		FillRect(hMemDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));

		pPicture->Render(hMemDC, 0, 0, oRgn.m_cx, oRgn.m_cy, 0, cy, cx, -cy, NULL);
		pPicture->Render(hMemDC, 0, oRgn.m_cy, oRgn.m_cx, oRgn.m_cy, 0, cy, cx, -cy, NULL);
		pPicture.Release();

		SelectObject(hMemDC, hBmpOld);
		DeleteDC(hMemDC);

		BOOL bColor, bLastColor;

		hRgn = CreateRectRgn(0, 0, 0, 0);

		for(cy = 0; cy < oRgn.m_cy; cy ++)
		{
			rect.bottom = oRgn.m_cy - cy;
			rect.top = rect.bottom - 1;

			bLastColor = TRUE;
			for(cx = 0; cx < oRgn.m_cx; cx++)
			{
				bColor = (pbits32[cy * oRgn.m_cx + cx] != pbits32[(cy + oRgn.m_cy) * oRgn.m_cx + cx]);

				if(bColor && !bLastColor)
				{
					rect.right = cx;

					oRgn.m_hRgn = CreateRectRgnIndirect(&rect);
					CombineRgn(hRgn, hRgn, oRgn.m_hRgn, RGN_OR);
					DeleteObject(oRgn.m_hRgn);

					bLastColor = bColor;
				}else if(!bColor && bLastColor)
				{
					rect.left = cx;
					bLastColor = bColor;
				}
			}

			if(!bLastColor)
			{
				rect.right = cx;

				oRgn.m_hRgn = CreateRectRgnIndirect(&rect);
				CombineRgn(hRgn, hRgn, oRgn.m_hRgn, RGN_OR);
				DeleteObject(oRgn.m_hRgn);
			}
		}

		DeleteObject(hMemBmp);

		if(pRegionPool)
		{
			oRgn.m_hRgn = ::CreateRectRgn(0, 0, 0, 0);
			::CombineRgn(oRgn.m_hRgn, hRgn, oRgn.m_hRgn, RGN_COPY);

			pRegionPool->SetAt(newVal, oRgn);
		}
	}

	ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0, SWP_FRAMECHANGED);
	ShowScrollBar(SB_BOTH, FALSE);

	ClearRgn();
	if(nLeft && nTop && nRight && nBottom)
	{
		DeleteObject(hRgn);

		m_hRgn1 = ::CreateRectRgn(0, 0, 0, 0);
		m_hRgn2 = ::CreateRectRgn(0, 0, 0, 0);
		m_hRgn3 = ::CreateRectRgn(0, 0, 0, 0);
		m_hRgn4 = ::CreateRectRgn(0, 0, 0, 0);

		m_hRgnLeft = ::CreateRectRgn(0, 0, 0, 0);
		m_hRgnTop = ::CreateRectRgn(0, 0, 0, 0);
		m_hRgnRight = ::CreateRectRgn(0, 0, 0, 0);
		m_hRgnBottom = ::CreateRectRgn(0, 0, 0, 0);

		m_nMaskWidth = oRgn.m_cx;
		m_nMaskHeight = oRgn.m_cy;

		m_nLeft = nLeft;
		m_nTop = nTop;
		m_nRight = nRight;
		m_nBottom = nBottom;

		POINT points[4];

		ZeroMemory(points, sizeof(points));
		points[0].x = m_nTop;
		points[1].x = m_nTop + 1;
		points[2].y = points[3].y = m_nMaskHeight;
		points[2].x = m_nBottom + 1;
		points[3].x = m_nBottom;

		hRgn = CreatePolygonRgn(points, 4, WINDING);
		CombineRgn(m_hRgnTop, oRgn.m_hRgn, hRgn, RGN_AND);
		DeleteObject(hRgn);

		ZeroMemory(points, sizeof(points));
		points[0].y = m_nLeft;
		points[3].y = m_nLeft + 1;
		points[1].y = m_nRight;
		points[2].y = m_nRight + 1;
		points[1].x = points[2].x = m_nMaskWidth;

		hRgn = CreatePolygonRgn(points, 4, WINDING);
		CombineRgn(m_hRgnLeft, oRgn.m_hRgn, hRgn, RGN_AND);
		DeleteObject(hRgn);

		hRgn = ::CreateRectRgn(0, 0, 0, 0);
		m_bHasCross = (CombineRgn(hRgn, m_hRgnLeft, m_hRgnTop, RGN_AND) != NULLREGION);
		if(m_bHasCross)
		{
			GetRgnBox(hRgn, &rect);

			m_nCrossX = rect.left;
			m_nCrossY = rect.top;
		}
		DeleteObject(hRgn);

		ZeroMemory(points, sizeof(points));
		points[1].x = points[2].x = m_nMaskWidth;
		points[2].y = m_nRight;
		points[3].y = m_nLeft;

		hRgn = CreatePolygonRgn(points, 4, WINDING);
		CombineRgn(m_hRgnBottom, m_hRgnTop, hRgn, RGN_DIFF);
		CombineRgn(m_hRgnTop, m_hRgnTop, hRgn, RGN_AND);
		CombineRgn(m_hRgn1, oRgn.m_hRgn, hRgn, RGN_AND);
		CombineRgn(m_hRgn4, oRgn.m_hRgn, hRgn, RGN_DIFF);
		DeleteObject(hRgn);

		ZeroMemory(points, sizeof(points));
		points[1].x = m_nTop;
		points[2].y = points[3].y = m_nMaskHeight;
		points[2].x = m_nBottom;

		hRgn = CreatePolygonRgn(points, 4, WINDING);
		CombineRgn(m_hRgnRight, m_hRgnLeft, hRgn, RGN_DIFF);
		CombineRgn(m_hRgnLeft, m_hRgnLeft, hRgn, RGN_AND);
		CombineRgn(m_hRgn2, m_hRgn1, hRgn, RGN_DIFF);
		CombineRgn(m_hRgn1, m_hRgn1, hRgn, RGN_AND);
		CombineRgn(m_hRgn3, m_hRgn4, hRgn, RGN_DIFF);
		CombineRgn(m_hRgn4, m_hRgn4, hRgn, RGN_AND);
		DeleteObject(hRgn);

		StretchRgn(m_hRgnTop);
		StretchRgn(m_hRgnBottom);
		StretchRgn(m_hRgnLeft, FALSE);
		StretchRgn(m_hRgnRight, FALSE);
	}else SetWindowRgn(hRgn, TRUE);

	SetWindowPos(NULL, 0, 0, oRgn.m_cx, oRgn.m_cy, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW);

	return S_OK;
}

void CBWindow::StretchRgn(HRGN hRgn, BOOL bX)
{
	WORD i, n, n1, n2;
	HRGN hRgnTemp, hRgnTemp1;

	hRgnTemp = ::CreateRectRgn(0, 0, 0, 0);
	n = 1;
	n1 = 0;
	for(i = 0; i < 20; i ++)
	{
		if(n1)OffsetRgn(hRgnTemp, bX ? n : 0, bX ? 0 : n);

		CombineRgn(hRgnTemp, hRgn, hRgnTemp, RGN_OR);

		hRgnTemp1 = hRgnTemp;
		hRgnTemp = hRgn;
		hRgn = hRgnTemp1;

		n2 = n;
		n += n1;
		n1 = n2;
	}
	DeleteObject(hRgnTemp);
}

HRESULT CBWindow::Center(void)
{
	if(!IsWindow())return E_HANDLE;

	RECT rect, rectScreen;

	GetWindowRect(&rect);
	::GetWindowRect(::GetDesktopWindow(), &rectScreen);

	SetWindowPos(NULL, (rectScreen.right - (rect.right - rect.left)) / 2,
		(rectScreen.bottom - (rect.bottom - rect.top)) / 2,
		0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::Close(void)
{
	if(!IsWindow())return E_HANDLE;

	m_bForceClose = TRUE;

	MSG msg;

	while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	PostMessage(WM_CLOSE);

	return S_OK;
}

HRESULT CBWindow::Drag(short nMode)
{
	if(!IsWindow())return E_HANDLE;

	if(nMode < 0 || nMode > 8)
		return DISP_E_TYPEMISMATCH;

	POINT pos;

	GetCursorPos(&pos);

	if(nMode)SendMessage(WM_SYSCOMMAND, SC_SIZE | nMode, MAKELONG(pos.x, pos.y));
	else SendMessage(WM_SYSCOMMAND, SC_MOVE | 2, MAKELONG(pos.x, pos.y));

	return S_OK;
}

HRESULT CBWindow::Focus(void)
{
	if(!IsWindow())return E_HANDLE;

	SetForegroundWindow(m_hWnd);
	SetFocus();

	return S_OK;
}

HRESULT CBWindow::isClosed(VARIANT_BOOL* pVal)
{
	*pVal = !IsWindow() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::isActived(VARIANT_BOOL* pVal)
{
	if(!IsWindow())return E_HANDLE;

	*pVal = (GetForegroundWindow() == m_hWnd) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT CBWindow::MoveTo(short left, short top)
{
	if(!IsWindow())return E_HANDLE;

	SetWindowPos(NULL, left, top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::MoveWindow(short left, short top, short width, short height)
{
	if(!IsWindow())return E_HANDLE;

	SetWindowPos(NULL, left, top, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::ResizeTo(short width, short height)
{
	if(!IsWindow())return E_HANDLE;

	SetWindowPos(NULL, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);

	return S_OK;
}

HRESULT CBWindow::SetMinTrackSize(short x, short y)
{
	m_sizeMinTrackSize.SetSize(x, y);
	return S_OK;
}

HRESULT CBWindow::SetMaxTrackSize(short x, short y)
{
	m_sizeMaxTrackSize.SetSize(x, y);
	return S_OK;
}

HICON CBWindow::LoadIcon(LPCWSTR newVal)
{
	HICON hIcon = ::LoadIcon((HINSTANCE)0x400000, _T("SMALL"));

	if(*newVal == 0)return hIcon;

	CBComPtr<CBFile> pStream;
	CBComPtr<IPicture> pPicture;
	CBPath path(newVal);
	HRESULT hr;

	hr = pStream.CreateInstance();
	if(FAILED(hr))return hIcon;

	hr = pStream->Open((BSTR)(LPCWSTR)path);
	if(FAILED(hr))return hIcon;

	hr = ::OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (LPVOID *)&pPicture);
	if(FAILED(hr))return hIcon;

	pStream.Release();

	long cx, cy, cx1, cy1;

	pPicture->get_Width(&cx1);
	pPicture->get_Height(&cy1);

	const int HIMETRIC_INCH = 2540;

	HDC hMemDC = CreateCompatibleDC(NULL);
	HDC hMemDC1 = CreateCompatibleDC(NULL);
	// convert himetric to pixels
	cx = MulDiv(cx1, GetDeviceCaps(hMemDC, LOGPIXELSX), HIMETRIC_INCH);
	cy = MulDiv(cy1, GetDeviceCaps(hMemDC, LOGPIXELSY), HIMETRIC_INCH);

	RECT rect = {0, 0, cx, cy};

	LPDWORD pbits32; 
	BITMAPINFOHEADER bi = {sizeof(BITMAPINFOHEADER), cx, cy, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	HBITMAP hMemBmp = CreateDIBSection(hMemDC, (LPBITMAPINFO)&bi, DIB_RGB_COLORS, (LPVOID*)&pbits32, NULL, 0);

	HBITMAP hMemBmp1 = CreateCompatibleBitmap(hMemDC, cx, cy);
	HGDIOBJ hBmpOld = SelectObject(hMemDC, hMemBmp1);
	FillRect(hMemDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	pPicture->Render(hMemDC, 0, 0, cx, cy, 0, cy1, cx1, -cy1, NULL);

	HBITMAP hMemBmp2 = CreateCompatibleBitmap(hMemDC1, cx, cy);
	HGDIOBJ hBmpOld1 = SelectObject(hMemDC1, hMemBmp2);
	FillRect(hMemDC1, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	pPicture->Render(hMemDC1, 0, 0, cx, cy, 0, cy1, cx1, -cy1, NULL);
	BitBlt(hMemDC, 0, 0, cx, cy, hMemDC1, 0, 0, SRCINVERT);
	SelectObject(hMemDC1, hBmpOld1);
	DeleteDC(hMemDC1);
	DeleteObject(hMemBmp2);

	SelectObject(hMemDC, hMemBmp);
	pPicture->Render(hMemDC, 0, 0, cx, cy, 0, cy1, cx1, -cy1, NULL);

	SelectObject(hMemDC, hBmpOld);
	DeleteDC(hMemDC);

	ICONINFO ii = {TRUE, 0, 0, hMemBmp1, hMemBmp};

	hIcon = CreateIconIndirect(&ii);

	DeleteObject(hMemBmp);
	DeleteObject(hMemBmp1);

	return hIcon;
}

HRESULT CBWindow::ShowDialog(VARIANT *pVal)
{
	if(!IsWindow())return E_HANDLE;

	AddRef();

	put_Visible(VARIANT_TRUE);

	m_varReturn.Clear();

	SetForegroundWindow(m_hWnd);
	SetFocus();

	CBWindow* pOld = th_pRunWindow;

	th_pRunWindow = this;
	MSG msg;

	for(UINT i = 0; i < m_hWnds.GetCount(); i ++)
		::EnableWindow(m_hWnds[i], FALSE);

	while( GetMessage( &msg, NULL, 0, 0 ))
		if(!PreTranslateMessage(&msg))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

	th_pRunWindow = pOld;

	if(pOld && !pOld->IsWindow())
		PostQuitMessage(0);

	HRESULT hr = VariantCopy(pVal, &m_varReturn);
	m_varReturn.Clear();

	Release();

	return hr;
}

HRESULT CBWindow::EndDialog(VARIANT retVal)
{
	if(!IsWindow())return E_HANDLE;

	if(retVal.vt != VT_ERROR)
		m_varReturn = retVal;

	Close();

	return S_OK;
}
