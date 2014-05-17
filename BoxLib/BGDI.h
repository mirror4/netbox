#pragma once

#include <commctrl.h>

class CBSize;
class CBPoint;
class CBRect;

class CBSize : public tagSIZE
{
public:
	CBSize() throw()
	{}

	CBSize(int initCX, int initCY) throw()
	{
		cx = initCX; cy = initCY;
	}

	CBSize(SIZE initSize) throw()
	{
		*(SIZE*)this = initSize;
	}

	CBSize(POINT initPt) throw()
	{
		*(POINT*)this = initPt;
	}

	CBSize(DWORD dwSize) throw()
	{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
	}

	BOOL operator==(SIZE size) const throw()
	{
		return (cx == size.cx && cy == size.cy);
	}

	BOOL operator!=(SIZE size) const throw()
	{
		return (cx != size.cx || cy != size.cy);
	}

	void operator+=(SIZE size) throw()
	{
		cx += size.cx; cy += size.cy;
	}

	void operator-=(SIZE size) throw()
	{
		cx -= size.cx; cy -= size.cy;
	}

	void SetSize(int CX, int CY) throw()
	{
		cx = CX; cy = CY;
	}

	CBSize operator+(SIZE size) const throw()
	{
		return CBSize(cx + size.cx, cy + size.cy);
	}

	CBSize operator-(SIZE size) const throw()
	{
		return CBSize(cx - size.cx, cy - size.cy);
	}

	CBSize operator-() const throw()
	{
		return CBSize(-cx, -cy);
	}

	CBPoint operator+(POINT point) const throw();
	CBPoint operator-(POINT point) const throw();
	CBRect operator+(const RECT* lpRect) const throw();
	CBRect operator-(const RECT* lpRect) const throw();
};

class CBPoint : public tagPOINT
{
public:
	CBPoint() throw()
	{}

	CBPoint(int initX, int initY) throw()
	{
		x = initX; y = initY;
	}

	CBPoint(POINT initPt) throw()
	{
		*(POINT*)this = initPt;
	}

	CBPoint(SIZE initSize) throw()
	{
		*(SIZE*)this = initSize;
	}

	CBPoint(LPARAM dwPoint) throw()
	{
		x = (short)LOWORD(dwPoint);
		y = (short)HIWORD(dwPoint);
	}

	void Offset(int xOffset, int yOffset) throw()
	{
		x += xOffset; y += yOffset;
	}

	void Offset(POINT point) throw()
	{
		x += point.x; y += point.y;
	}

	void Offset(SIZE size) throw()
	{
		x += size.cx; y += size.cy;
	}

	void SetPoint(int X, int Y) throw()
	{
		x = X; y = Y;
	}

	BOOL operator==(POINT point) const throw()
	{
		return (x == point.x && y == point.y);
	}

	BOOL operator!=(POINT point) const throw()
	{
		return (x != point.x || y != point.y);
	}

	void operator+=(SIZE size) throw()
	{
		x += size.cx; y += size.cy;
	}

	void operator-=(SIZE size) throw()
	{
		x -= size.cx; y -= size.cy;
	}

	void operator+=(POINT point) throw()
	{
		x += point.x; y += point.y;
	}

	void operator-=(POINT point) throw()
	{
		x -= point.x; y -= point.y;
	}

	CBPoint operator+(SIZE size) const throw()
	{
		return CBPoint(x + size.cx, y + size.cy);
	}

	CBPoint operator-(SIZE size) const throw()
	{
		return CBPoint(x - size.cx, y - size.cy);
	}

	CBPoint operator-() const throw()
	{
		return CBPoint(-x, -y);
	}

	CBPoint operator+(POINT point) const throw()
	{
		return CBPoint(x + point.x, y + point.y);
	}

	CBSize operator-(POINT point) const throw()
	{
		return CBSize(x - point.x, y - point.y);
	}

	CBRect operator+(const RECT* lpRect) const throw();
	CBRect operator-(const RECT* lpRect) const throw();
};

class CBRect : public tagRECT
{
public:
	CBRect() throw()
	{}

	CBRect(int l, int t, int r, int b) throw()
	{
		left = l; top = t; right = r; bottom = b;
	}

	CBRect(const RECT& srcRect) throw()
	{
		*this = srcRect;
	}

	CBRect(LPCRECT lpSrcRect) throw()
	{
		*this = *lpSrcRect;
	}

	CBRect(POINT point, SIZE size) throw()
	{
		right = (left = point.x) + size.cx;
		bottom = (top = point.y) + size.cy;
	}

	CBRect(POINT topLeft, POINT bottomRight) throw()
	{
		left = topLeft.x;
		top = topLeft.y;
		right = bottomRight.x;
		bottom = bottomRight.y;
	}

	int Width() const throw()
	{
		return right - left;
	}

	int Height() const throw()
	{
		return bottom - top;
	}

	CBSize Size() const throw()
	{
		return CBSize(right - left, bottom - top);
	}

	CBPoint& TopLeft() throw()
	{
		return *((CBPoint*)this);
	}

	CBPoint& BottomRight() throw()
	{
		return *((CBPoint*)this+1);
	}

	const CBPoint& TopLeft() const throw()
	{
		return *((CBPoint*)this);
	}

	const CBPoint& BottomRight() const throw()
	{
		return *((CBPoint*)this + 1);
	}

	CBPoint CenterPoint() const throw()
	{
		return CBPoint((left + right) / 2, (top + bottom) / 2);
	}

	void SwapLeftRight() throw()
	{
		LONG temp = left; left = right; right = temp;
	}

	operator LPRECT() throw()
	{
		return this;
	}

	operator LPCRECT() const throw()
	{
		return this;
	}

	BOOL IsEmpty() const throw()
	{
		return left >= right || top >= bottom;
	}

	BOOL IsNull() const throw()
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}

	BOOL PtInRect(POINT point) const throw()
	{
		return point.x >= left && point.x < right &&
				point.y >= top  && point.y < bottom;
	}

	void Set(int x1, int y1, int x2, int y2) throw()
	{
		left = x1;
		top = y1;
		right = x2;
		bottom = y2;
	}

	void Set(POINT topLeft, POINT bottomRight) throw()
	{
		Set(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	}

	void SetEmpty() throw()
	{
		ZeroMemory(this, sizeof(RECT));
	}

	void Copy(LPCRECT lpSrcRect) throw()
	{
		*this = *lpSrcRect;
	}

	BOOL Equal(LPCRECT lpRect) const throw()
	{
		return *this == *lpRect;
	}

	void Inflate(int x, int y) throw()
	{
		left -= x;
		right += x;
		top -= y;
		bottom += y;
	}

	void Inflate(SIZE size) throw()
	{
		Inflate(size.cx, size.cy);
	}

	void Inflate(int l, int t, int r, int b) throw()
	{
		left -= l;
		top -= t;
		right += r;
		bottom += b;
	}

	void Inflate(LPCRECT lpRect) throw()
	{
		Inflate(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}

	void Deflate(int x, int y) throw()
	{
		Inflate(-x, -y);
	}

	void Deflate(SIZE size) throw()
	{
		Inflate(-size.cx, -size.cy);
	}

	void Deflate(int l, int t, int r, int b) throw()
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	void Deflate(LPCRECT lpRect) throw()
	{
		Deflate(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}

	void Offset(int x, int y) throw()
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}

	void Offset(SIZE size) throw()
	{
		Offset(size.cx, size.cy);
	}

	void Offset(POINT point) throw()
	{
		Offset(point.x, point.y);
	}

	void Normalize() throw()
	{
		int nTemp;

		if (left > right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}

		if (top > bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

	void MoveToY(int y) throw()
	{
		bottom = Height() + y;
		top = y;
	}

	void MoveToX(int x) throw()
	{
		right = Width() + x;
		left = x;
	}

	void MoveTo(int x, int y) throw()
	{
		MoveToX(x);
		MoveToY(y);
	}

	void MoveTo(POINT point) throw()
	{
		MoveToX(point.x);
		MoveToY(point.y);
	}

	BOOL Intersect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{
		left  = max(lpRect1->left, lpRect2->left);
		right = min(lpRect1->right, lpRect2->right);

		if (left < right) {

			top = max(lpRect1->top, lpRect2->top);
			bottom = min(lpRect1->bottom, lpRect2->bottom);

			if (top < bottom)
				return TRUE;
		}

		SetEmpty();
	
		return FALSE;
	}

	BOOL Union(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{
		BOOL frc1Empty, frc2Empty;

		frc1Empty = ((lpRect1->left >= lpRect1->right) ||
				(lpRect1->top >= lpRect1->bottom));

		frc2Empty = ((lpRect2->left >= lpRect2->right) ||
				(lpRect2->top >= lpRect2->bottom));

		if (frc1Empty && frc2Empty)
		{
			SetEmpty();
			return FALSE;
		}

		if (frc1Empty)
		{
			*this = *lpRect2;
			return TRUE;
		}

		if (frc2Empty)
		{
			*this = *lpRect1;
			return TRUE;
		}

		left   = min(lpRect1->left,   lpRect2->left);
		top    = min(lpRect1->top,    lpRect2->top);
		right  = max(lpRect1->right,  lpRect2->right);
		bottom = max(lpRect1->bottom, lpRect2->bottom);

		return TRUE;
	}

	BOOL Subtract(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw()
	{
		int cSidesOut;
		BOOL fIntersect;
		RECT rcInt;

		fIntersect = IntersectRect(&rcInt, lpRectSrc1, lpRectSrc2);

		*this = *lpRectSrc1;

		if (fIntersect)
		{
			cSidesOut = 0;
			if (rcInt.left   <= lpRectSrc1->left)
				cSidesOut++;
			if (rcInt.top    <= lpRectSrc1->top)
				cSidesOut++;
			if (rcInt.right  >= lpRectSrc1->right)
				cSidesOut++;
			if (rcInt.bottom >= lpRectSrc1->bottom)
				cSidesOut++;

			if (cSidesOut == 4)
			{
				SetEmpty();
				return FALSE;
			}

			if (cSidesOut == 3)
			{
				if (rcInt.left > lpRectSrc1->left)
					right = rcInt.left;

				else if (rcInt.right < lpRectSrc1->right)
					left = rcInt.right;

				else if (rcInt.top > lpRectSrc1->top)
					bottom = rcInt.top;

				else if (rcInt.bottom < lpRectSrc1->bottom)
					top = rcInt.bottom;
			}
		}

		if ((left >= right) || (top >= bottom))
			return FALSE;

		return TRUE;
	}

	void operator=(const RECT& srcRect) throw()
	{
		*this = srcRect;
	}

	BOOL operator==(const RECT& rect) const throw()
	{
		return Equal(&rect);
	}

	BOOL operator!=(const RECT& rect) const throw()
	{
		return !Equal(&rect);
	}

	void operator+=(POINT point) throw()
	{
		Offset(point);
	}

	void operator+=(SIZE size) throw()
	{
		Offset(size);
	}

	void operator+=(LPCRECT lpRect) throw()
	{
		Inflate(lpRect);
	}

	void operator-=(POINT point) throw()
	{
		Offset(-point.x, -point.y);
	}

	void operator-=(SIZE size) throw()
	{
		Offset(-size.cx, -size.cy);
	}

	void operator-=(LPCRECT lpRect) throw()
	{
		Deflate(lpRect);
	}

	void operator&=(const RECT& rect) throw()
	{
		Intersect(this, &rect);
	}

	void operator|=(const RECT& rect) throw()
	{
		Union(this, &rect);
	}

	CBRect operator+(POINT point) const throw()
	{
		CBRect rect(*this);
		rect += point;
		return rect;
	}

	CBRect operator-(POINT point) const throw()
	{
		CBRect rect(*this);
		rect -= point;
		return rect;
	}

	CBRect operator+(LPCRECT lpRect) const throw()
	{
		CBRect rect(*this);
		rect += lpRect;
		return rect;
	}

	CBRect operator+(SIZE size) const throw()
	{
		CBRect rect(*this);
		rect += size;
		return rect;
	}

	CBRect operator-(SIZE size) const throw()
	{
		CBRect rect(*this);
		rect -= size;
		return rect;
	}

	CBRect operator-(LPCRECT lpRect) const throw()
	{
		CBRect rect(*this);
		rect -= lpRect;
		return rect;
	}

	CBRect operator&(const RECT& rect2) const throw()
	{
		CBRect rect(*this);
		rect &= rect2;
		return rect;
	}

	CBRect operator|(const RECT& rect2) const throw()
	{
		CBRect rect(*this);
		rect |= rect2;
		return rect;
	}
};

inline CBPoint CBSize::operator+(POINT point) const throw()
{
	return CBPoint(cx + point.x, cy + point.y);
}

inline CBPoint CBSize::operator-(POINT point) const throw()
{
	return CBPoint(cx - point.x, cy - point.y);
}

inline CBRect CBSize::operator+(const RECT* lpRect) const throw()
{
	return CBRect(lpRect) + *this;
}

inline CBRect CBSize::operator-(const RECT* lpRect) const throw()
{
	return CBRect(lpRect) - *this;
}

inline CBRect CBPoint::operator+(const RECT* lpRect) const throw()
{
	return CBRect(lpRect) + *this;
}

inline CBRect CBPoint::operator-(const RECT* lpRect) const throw()
{
	return CBRect(lpRect) - *this;
}

class CBGdiObject
{
public:
	CBGdiObject(void) : m_hObject(NULL)
	{}

	~CBGdiObject()
	{
		DeleteObject();
	}

	operator HGDIOBJ() const
	{
		return m_hObject;
	}

	HGDIOBJ* operator&() throw()
	{
		return &m_hObject;
	}

	BOOL Attach(HGDIOBJ hObject)
	{
		if(hObject == NULL)
			return FALSE;

		DeleteObject();
		m_hObject = hObject;

		return TRUE;
	}

	HGDIOBJ Detach()
	{
		HGDIOBJ hObject = m_hObject;
		m_hObject = NULL;
		return hObject;
	}

	void DeleteObject()
	{
		if (m_hObject)
			::DeleteObject(Detach());
	}

	int GetObject(int nCount, LPVOID lpObject) const
	{
		ASSERT(m_hObject != NULL);
		return ::GetObject(m_hObject, nCount, lpObject);
	}

	UINT GetObjectType() const
	{
		ASSERT(m_hObject != NULL);
		return (UINT)::GetObjectType(m_hObject);
	}

	BOOL CreateStockObject(int nIndex)
	{
		ASSERT(m_hObject != NULL);
		return (m_hObject = ::GetStockObject(nIndex)) != NULL;
	}

public:
	HGDIOBJ m_hObject;
};

class CBPen : public CBGdiObject
{
public:
	CBPen(){}

	CBPen(int nPenStyle, int nWidth, COLORREF crColor)
	{
		Attach(::CreatePen(nPenStyle, nWidth, crColor));
	}

	CBPen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
			int nStyleCount, const DWORD* lpStyle)
	{
		Attach(::ExtCreatePen(nPenStyle, nWidth, pLogBrush, nStyleCount,
				lpStyle));
	}

	operator HPEN() const
	{
		return (HPEN)m_hObject;
	}

	HPEN* operator&() throw()
	{
		return (HPEN*)&m_hObject;
	}

	BOOL CreatePen(int nPenStyle, int nWidth, COLORREF crColor)
	{
		return Attach(::CreatePen(nPenStyle, nWidth, crColor));
	}

    BOOL CreatePenIndirect(LPLOGPEN lpLogPen)
	{
		return Attach(::CreatePenIndirect(lpLogPen));
	}

    BOOL CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
			int nStyleCount, const DWORD* lpStyle)
	{
		return Attach(::ExtCreatePen(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle));
	}

    int GetExtLogPen(EXTLOGPEN* pLogPen)
	{
		return ::GetObject(m_hObject, sizeof(EXTLOGPEN), pLogPen);
	}

    int GetLogPen(LOGPEN* pLogPen)
	{
		return ::GetObject(m_hObject, sizeof(LOGPEN), pLogPen);
	}
};

class CBBrush : public CBGdiObject
{
public:
	CBBrush(){};

	CBBrush(COLORREF crColor)
	{
		Attach(::CreateSolidBrush(crColor));
	}

	CBBrush(int nIndex, COLORREF crColor)
	{
		Attach(::CreateHatchBrush(nIndex, crColor));
	}

	CBBrush(HBITMAP hBitmap)
	{
		Attach(::CreatePatternBrush(hBitmap));
	}

	BOOL CreateSolidBrush(COLORREF crColor)
	{
		return Attach(::CreateSolidBrush(crColor));
	}

	BOOL CreateHatchBrush(int nIndex, COLORREF crColor)
	{
		return Attach(::CreateHatchBrush(nIndex, crColor));
	}

	BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush)
	{
		return Attach(::CreateBrushIndirect(lpLogBrush));
	}

	BOOL CreatePatternBrush(HBITMAP hBitmap)
	{
		return Attach(::CreatePatternBrush(hBitmap));
	}

	BOOL CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage)
	{
		const void* lpPackedDIB = ::GlobalLock(hPackedDIB);
		BOOL bResult = Attach(::CreateDIBPatternBrushPt(lpPackedDIB, nUsage));
		::GlobalUnlock(hPackedDIB);

		return bResult;
	}

	BOOL CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage)
	{
		return Attach(::CreateDIBPatternBrushPt(lpPackedDIB, nUsage));
	}

	BOOL CreateSysColorBrush(int nIndex)
	{
		return Attach(::GetSysColorBrush(nIndex));
	}

	operator HBRUSH() const
	{
		return (HBRUSH)m_hObject;
	}

	HBRUSH* operator&() throw()
	{
		return (HBRUSH*)&m_hObject;
	}

	int GetLogBrush(LOGBRUSH* pLogBrush)
	{
		return ::GetObject(m_hObject, sizeof(LOGBRUSH), pLogBrush);
	}
};

class CBFont : public CBGdiObject
{
public:
	BOOL CreateFontIndirect(const LOGFONT* lpLogFont)
	{
		return Attach(::CreateFontIndirect(lpLogFont));
	}

	BOOL CreateFont(int nHeight, int nWidth, int nEscapement,
			int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline,
			BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
			BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
			LPCTSTR lpszFacename)
	{
		return Attach(::CreateFont(nHeight, nWidth, nEscapement,
		nOrientation, nWeight, bItalic, bUnderline, cStrikeOut,
		nCharSet, nOutPrecision, nClipPrecision, nQuality,
		nPitchAndFamily, lpszFacename));
	}

	BOOL CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, HDC hDC = NULL)
	{
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfHeight = nPointSize;
		lstrcpyn(logFont.lfFaceName, lpszFaceName, _countof(logFont.lfFaceName));

		return CreatePointFontIndirect(&logFont, hDC);
	}

	BOOL CreatePointFontIndirect(const LOGFONT* lpLogFont, HDC hDC = NULL)
	{
		HDC hDC1;

		if(hDC == NULL)
			hDC1 = ::GetDC(NULL);
		else hDC1 = hDC;

		LOGFONT logFont = *lpLogFont;
		POINT pt;
		pt.y = ::GetDeviceCaps(hDC1, LOGPIXELSY) * logFont.lfHeight;
		pt.y /= 720;    // 72 points/inch, 10 decipoints/point
		pt.x = 0;
		::DPtoLP(hDC1, &pt, 1);
		POINT ptOrg = { 0, 0 };
		::DPtoLP(hDC1, &ptOrg, 1);
		logFont.lfHeight = -abs(pt.y - ptOrg.y);

		if (hDC == NULL)
			ReleaseDC(NULL, hDC1);

		return CreateFontIndirect(&logFont);
	}

	operator HFONT() const
	{
		return (HFONT)m_hObject;
	}

	HFONT* operator&() throw()
	{
		return (HFONT*)&m_hObject;
	}

	int GetLogFont(LOGFONT* pLogFont)
	{
		return ::GetObject(m_hObject, sizeof(LOGFONT), pLogFont);
	}
};

class CBBitmap : public CBGdiObject
{
public:
	BOOL LoadBitmap(LPCTSTR lpszResourceName)
	{
		return Attach(::LoadBitmap(hAppInstance, lpszResourceName));
	}

	BOOL LoadBitmap(UINT nIDResource)
	{
		return LoadBitmap(MAKEINTRESOURCE(nIDResource));
	}

	BOOL LoadOEMBitmap(UINT nIDBitmap)
	{
		return Attach(::LoadBitmap(NULL, MAKEINTRESOURCE(nIDBitmap)));
	}

	BOOL LoadMappedBitmap(UINT nIDBitmap, UINT nFlags = 0, LPCOLORMAP lpColorMap = NULL, int nMapSize = 0)
	{
		return Attach(::CreateMappedBitmap(hAppInstance, nIDBitmap, (WORD)nFlags,
			lpColorMap, nMapSize));
	}

	BOOL CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitcount,
			const void* lpBits)
	{
		return Attach(::CreateBitmap(nWidth, nHeight, nPlanes, nBitcount, lpBits));
	}

	BOOL CreateBitmapIndirect(LPBITMAP lpBitmap)
	{
		return Attach(::CreateBitmapIndirect(lpBitmap));
	}

	BOOL CreateCompatibleBitmap(HDC hDC, int nWidth, int nHeight)
	{
		return Attach(::CreateCompatibleBitmap(hDC, nWidth, nHeight));
	}

	BOOL CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight)
	{
		return Attach(::CreateDiscardableBitmap(hDC, nWidth, nHeight));
	}

	BOOL Copy(HBITMAP hBitmap)
	{
		return Attach(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG));
	}

	operator HBITMAP() const
	{
		return (HBITMAP)m_hObject;
	}

	HBITMAP* operator&() throw()
	{
		return (HBITMAP*)&m_hObject;
	}

	int GetBitmap(BITMAP* pBitMap)
	{
		return ::GetObject(m_hObject, sizeof(BITMAP), pBitMap);
	}

	DWORD SetBitmapBits(DWORD dwCount, const void* lpBits)
	{
		return ::SetBitmapBits((HBITMAP)m_hObject, dwCount, lpBits);
	}

	DWORD GetBitmapBits(DWORD dwCount, LPVOID lpBits) const
	{
		return ::GetBitmapBits((HBITMAP)m_hObject, dwCount, lpBits);
	}

	CBSize SetBitmapDimension(int nWidth, int nHeight)
	{
		SIZE size;
		::SetBitmapDimensionEx((HBITMAP)m_hObject, nWidth, nHeight, &size);
		return size;
	}

	CBSize GetBitmapDimension() const
	{
		SIZE size;
		::GetBitmapDimensionEx((HBITMAP)m_hObject, &size);
		return size;
	}
};

class CBPalette : public CBGdiObject
{
public:
	BOOL CreatePalette(LPLOGPALETTE lpLogPalette)
	{
		return Attach(::CreatePalette(lpLogPalette));
	}

	BOOL CreateHalftonePalette(HDC hDC)
	{
		return Attach(::CreateHalftonePalette(hDC));
	}

	operator HPALETTE() const
	{
		return (HPALETTE)m_hObject;
	}

	HPALETTE* operator&() throw()
	{
		return (HPALETTE*)&m_hObject;
	}

	int GetEntryCount()
	{
		WORD nEntries;
		GetObject(sizeof(WORD), &nEntries);
		return (int)nEntries;
	}

	UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors) const
	{
		ASSERT(m_hObject != NULL);
		return ::GetPaletteEntries((HPALETTE)m_hObject, nStartIndex, nNumEntries, lpPaletteColors);
	}

	UINT SetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors)
	{
		ASSERT(m_hObject != NULL);
		return ::SetPaletteEntries((HPALETTE)m_hObject, nStartIndex, nNumEntries, lpPaletteColors);
	}

	void AnimatePalette(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors)
	{
		ASSERT(m_hObject != NULL);
		::AnimatePalette((HPALETTE)m_hObject, nStartIndex, nNumEntries,	lpPaletteColors);
	}

	UINT GetNearestPaletteIndex(COLORREF crColor) const
	{
		ASSERT(m_hObject != NULL);
		return ::GetNearestPaletteIndex((HPALETTE)m_hObject, crColor);
	}

	BOOL ResizePalette(UINT nNumEntries)
	{
		ASSERT(m_hObject != NULL);
		return ::ResizePalette((HPALETTE)m_hObject, nNumEntries);
	}
};

class CBRgn : public CBGdiObject
{
public:
	BOOL CreateRectRgn(int x1, int y1, int x2, int y2)
	{
		return Attach(::CreateRectRgn(x1, y1, x2, y2));
	}

	BOOL CreateRectRgnIndirect(LPCRECT lpRect)
	{
		return Attach(::CreateRectRgnIndirect(lpRect));
	}

	BOOL CreateEllipticRgn(int x1, int y1, int x2, int y2)
	{
		return Attach(::CreateEllipticRgn(x1, y1, x2, y2));
	}

	BOOL CreateEllipticRgnIndirect(LPCRECT lpRect)
	{
		return Attach(::CreateEllipticRgnIndirect(lpRect));
	}

	BOOL CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode)
	{
		return Attach(::CreatePolygonRgn(lpPoints, nCount, nMode));
	}

	BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode)
	{
		return Attach(::CreatePolyPolygonRgn(lpPoints, lpPolyCounts, nCount, nPolyFillMode));
	}

	BOOL CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		return Attach(::CreateRoundRectRgn(x1, y1, x2, y2, x3, y3));
	}

	BOOL CreateFromPath(HDC hDC)
	{
		return Attach(::PathToRegion(hDC));
	}

	BOOL CreateFromData(const XFORM* lpXForm, int nCount, const RGNDATA* pRgnData)
	{
		return Attach(::ExtCreateRegion(lpXForm, nCount, pRgnData));
	}

	void SetRect(int x1, int y1, int x2, int y2)
	{
		ASSERT(m_hObject != NULL);
		::SetRectRgn((HRGN)m_hObject, x1, y1, x2, y2);
	}

	void SetRect(LPCRECT lpRect)
	{
		SetRect(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}

	int Combine(CBRgn* pRgn1, CBRgn* pRgn2, int nCombineMode)
	{
		ASSERT(m_hObject != NULL);
		return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgn1->m_hObject, (HRGN)pRgn2->m_hObject, nCombineMode);
	}

	int Copy(CBRgn* pRgnSrc)
	{
		ASSERT(m_hObject != NULL);
		return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgnSrc->m_hObject, NULL, RGN_COPY);
	}

	BOOL Equal(CBRgn* pRgn) const
	{
		ASSERT(m_hObject != NULL);
		return ::EqualRgn((HRGN)m_hObject, (HRGN)pRgn->m_hObject);
	}

	int Offset(int x, int y)
	{
		ASSERT(m_hObject != NULL);
		return ::OffsetRgn((HRGN)m_hObject, x, y);
	}

	int Offset(POINT point)
	{
		ASSERT(m_hObject != NULL);
		return ::OffsetRgn((HRGN)m_hObject, point.x, point.y);
	}

	int GetRgnBox(LPRECT lpRect) const
	{
		ASSERT(m_hObject != NULL);
		return ::GetRgnBox((HRGN)m_hObject, lpRect);
	}

	BOOL PtInRegion(int x, int y) const
	{
		ASSERT(m_hObject != NULL);
		return ::PtInRegion((HRGN)m_hObject, x, y);
	}

	BOOL PtInRegion(POINT point) const
	{
		return PtInRegion(point.x, point.y);
	}

	BOOL RectInRegion(LPCRECT lpRect) const
	{
		ASSERT(m_hObject != NULL);
		return ::RectInRegion((HRGN)m_hObject, lpRect);
	}

	int GetRegionData(LPRGNDATA lpRgnData, int nDataSize) const
	{
		ASSERT(m_hObject != NULL);
		return (int)::GetRegionData((HRGN)m_hObject, nDataSize, lpRgnData);
	}

	operator HRGN() const
	{
		return (HRGN)m_hObject;
	}

	HRGN* operator&() throw()
	{
		return (HRGN*)&m_hObject;
	}
};

class CBIcon
{
public:
	CBIcon(void) : m_hObject(NULL)
	{}

	~CBIcon()
	{
		DeleteObject();
	}

	BOOL LoadIcon(LPCTSTR lpszResourceName)
	{
		return Attach(::LoadIcon(hAppInstance, lpszResourceName));
	}

	BOOL LoadIcon(UINT nIDResource)
	{
		return LoadIcon(MAKEINTRESOURCE(nIDResource));
	}

	operator HICON() const
	{
		return m_hObject;
	}

	HICON* operator&() throw()
	{
		return &m_hObject;
	}

	BOOL Attach(HICON hObject)
	{
		if(hObject == NULL)
			return FALSE;

		DeleteObject();
		m_hObject = hObject;

		return TRUE;
	}

	HICON Detach()
	{
		HICON hObject = m_hObject;
		m_hObject = NULL;
		return hObject;
	}

	void DeleteObject()
	{
		if (m_hObject)
			::DestroyIcon(Detach());
	}

	void Copy(HICON hIcon)
	{
		Attach(CopyIcon(hIcon));
	}

	BOOL GetIconInfo(ICONINFO *piconinfo)
	{
		ASSERT(m_hObject != NULL);
		return ::GetIconInfo(m_hObject, piconinfo);
	}


public:
	HICON	m_hObject;
};


