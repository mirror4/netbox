// BuildBtn.cpp : implementation file
//

#include "stdafx.h"
#include "NBR.h"
#include "BuildBtn.h"
#include <Uxtheme.h>
#include <commctrl.h>
#include ".\buildbtn.h"


HBITMAP nbLoadSysColorBitmap(int id);

struct _ThemeHelper
{
	typedef BOOL(__stdcall *PFNISAPPTHEMED)();
	typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
	typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
	typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
	typedef HRESULT(__stdcall *PFNGETTHEMEPARTSIZE)(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, RECT * pRect, enum THEMESIZE eSize, OUT SIZE *psz);

	static void* GetProc(LPCSTR szProc, void* pfnFail)
	{
		static HMODULE hThemeDll = LoadLibrary(_T("UxTheme.dll"));

		void* pRet = pfnFail;
		if (hThemeDll != NULL)
		{
			pRet = GetProcAddress(hThemeDll, szProc);
		}
		return pRet;
	}

	static BOOL IsAppThemedFail()
	{
		return FALSE;
	}
	static BOOL IsAppThemed()
	{
		static PFNISAPPTHEMED pfnIsAppThemed = (PFNISAPPTHEMED)GetProc("IsAppThemed", IsAppThemedFail);
		return (*pfnIsAppThemed)();
	}

	static HTHEME OpenThemeDataFail(HWND , LPCWSTR )
	{
		return NULL;
	}
	static HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
	{
		static PFNOPENTHEMEDATA pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProc("OpenThemeData", OpenThemeData);
		return (*pfnOpenThemeData)(hwnd, pszClassList);
	}
	static HRESULT CloseThemeDataFail(HTHEME)
	{
		return E_FAIL;
	}
	static HRESULT CloseThemeData(HTHEME hTheme)
	{
		static PFNCLOSETHEMEDATA pfnCloseThemeData = (PFNCLOSETHEMEDATA)GetProc("CloseThemeData", CloseThemeDataFail);
		return (*pfnCloseThemeData)(hTheme);
	}

	static HRESULT DrawThemeBackgroundFail(HTHEME, HDC, int, int, const RECT *, const RECT *)
	{
		return E_FAIL;
	}

	static HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect)
	{
		static PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground = 
			(PFNDRAWTHEMEBACKGROUND)GetProc("DrawThemeBackground", DrawThemeBackgroundFail);
		return (*pfnDrawThemeBackground)(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	}

	static HRESULT GetThemePartSizeFail(HTHEME, HDC, int, int, RECT *, enum THEMESIZE, SIZE *)
	{
		return E_FAIL;
	}
	static HRESULT GetThemePartSize(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, RECT * pRect, enum THEMESIZE eSize, SIZE *psz)
	{
		static PFNGETTHEMEPARTSIZE pfnGetThemePartSize = 
			(PFNGETTHEMEPARTSIZE)GetProc("GetThemePartSize", GetThemePartSizeFail);
		return (*pfnGetThemePartSize)(hTheme, hdc, iPartId, iStateId, pRect, eSize, psz);
	}
};

// CBuildBtn

IMPLEMENT_DYNAMIC(CBuildBtn, CButton)
CBuildBtn::CBuildBtn()
{
	m_bInRect = FALSE;
	m_hBmp.Attach(nbLoadSysColorBitmap(IDB_BUILD));
	m_hBmp.GetBitmap(&m_bm);
}

CBuildBtn::~CBuildBtn()
{
}

void CBuildBtn::OnSysColorChange()
{
	m_hBmp.DeleteObject();

	if(_ThemeHelper::IsAppThemed())
	{
		ModifyStyle(BS_BITMAP, BS_OWNERDRAW);
		m_hBmp.LoadBitmap(IDB_BUILD);
		CheckMouse();
	}else
	{
		ModifyStyle(BS_OWNERDRAW, BS_BITMAP);
		m_hBmp.Attach(nbLoadSysColorBitmap(IDB_BUILD));
		SetBitmap(m_hBmp);
	}
}

BEGIN_MESSAGE_MAP(CBuildBtn, CButton)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CBuildBtn message handlers

void CBuildBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	CRect rect;
	rect.CopyRect(&lpDrawItemStruct->rcItem);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	pDC->FillRect(&lpDrawItemStruct->rcItem, CBrush::FromHandle(GetSysColorBrush(COLOR_BTNFACE)));

	if (_ThemeHelper::IsAppThemed())
	{
		HTHEME hTheme = _ThemeHelper::OpenThemeData(m_hWnd, L"Button");
		if (hTheme != NULL)
		{
			int iStateId = 0;

			if (lpDrawItemStruct->itemState & ODS_SELECTED)
				iStateId = 3;
			else if(m_bInRect)
				iStateId = 2;
			else if (lpDrawItemStruct->itemState & ODS_FOCUS)
				iStateId = 5;

			_ThemeHelper::DrawThemeBackground(hTheme, lpDrawItemStruct->hDC, 1, iStateId, &lpDrawItemStruct->rcItem, 0);

			if (lpDrawItemStruct->itemState & ODS_FOCUS)
			{
				rect.DeflateRect(3, 3);
				pDC->DrawFocusRect(&rect);
			}
		}
	}

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOld = memDC.SelectObject(&m_hBmp);
	if (pOld == NULL)
		return;     // destructors will clean up

	pDC->TransparentBlt(rect.left + (rect.Width() - m_bm.bmWidth) / 2, rect.top + (rect.Height() - m_bm.bmHeight) / 2, m_bm.bmWidth, m_bm.bmHeight,
		&memDC, 0, 0, m_bm.bmWidth, m_bm.bmHeight, RGB(192, 192, 192));
	memDC.SelectObject(pOld);
}

void CBuildBtn::CheckMouse()
{
	BOOL bNewVal;

	POINT pos;
	CRect rectWin;

	::GetCursorPos(&pos);
	GetWindowRect(&rectWin);
	bNewVal = rectWin.PtInRect(pos);

	if(bNewVal != m_bInRect)
	{
		m_bInRect = bNewVal;
		InvalidateRect(NULL, FALSE);
	}else m_bInRect = bNewVal;
}

void CBuildBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	CheckMouse();
	CButton::OnMouseMove(nFlags, point);
}
