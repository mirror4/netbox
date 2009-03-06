#include "StdAfx.h"
#include "BImage.h"
#include "BStream.h"

class DLL_EXP CxStreamFile : public CxFile
	{
public:
	CxStreamFile(IStream* pStream)
	{
		m_pStream = pStream;
	}

	virtual bool Close()
	{
		m_pStream.Release();
		return TRUE;
	}
	virtual size_t	Read(void *buffer, size_t size, size_t count)
	{
		if (!m_pStream) return 0;

		if (size==0) return 0;
		ULONG n;
		
		HRESULT hr = m_pStream->Read(buffer, (ULONG)size*count, &n);
		if FAILED(hr) return 0;

		return n/size;
	}
//////////////////////////////////////////////////////////
	virtual size_t	Write(const void *buffer, size_t size, size_t count)
	{
		if (!m_pStream) return 0;

		if (size==0) return 0;
		ULONG n;
		
		HRESULT hr = m_pStream->Write(buffer, (ULONG)size*count, &n);
		if FAILED(hr) return 0;

		return n/size;
	}
//////////////////////////////////////////////////////////
	virtual bool Seek(long offset, int origin)
	{
		if (!m_pStream) return false;

		LARGE_INTEGER u;
		ULARGE_INTEGER n;

		n.QuadPart = 0;
		u.QuadPart = offset;
		HRESULT hr = m_pStream->Seek(u, origin, &n);
		if FAILED(hr) return false;
		return true;
	}
//////////////////////////////////////////////////////////
	virtual long Tell()
	{
		if (!m_pStream) return 0;

		LARGE_INTEGER u;
		ULARGE_INTEGER n;

		n.QuadPart = 0;
		u.QuadPart = 0;
		HRESULT hr = m_pStream->Seek(u, 1, &n);
		if FAILED(hr) return 0;
		return (long)n.QuadPart;
	}
//////////////////////////////////////////////////////////
	virtual long	Size()
	{
		if (!m_pStream) return 0;

		STATSTG stat;

		HRESULT hr = m_pStream->Stat(&stat, 1);
		if FAILED(hr) return 0;

		return (long)stat.cbSize.QuadPart;
	}
//////////////////////////////////////////////////////////
	virtual bool	Flush()
	{
		return false;
	}
//////////////////////////////////////////////////////////
	virtual bool	Eof()
	{
		return (Tell()>=Size());
	}
//////////////////////////////////////////////////////////
	virtual long	Error()
	{
		if (!m_pStream) return -1;
		return 0;
	}
//////////////////////////////////////////////////////////
	virtual bool PutC(unsigned char c)
	{
		if (!m_pStream) return false;

		ULONG n;
		
		HRESULT hr = m_pStream->Write((const void *)&c, (ULONG)1, &n);
		if FAILED(hr) return 0;

		return (n==1);
	}
//////////////////////////////////////////////////////////
	virtual long	GetC()
	{
		if (!m_pStream) return 0;
		if (Eof()) return EOF;

		ULONG n;
		unsigned char c;

		HRESULT hr = m_pStream->Read(&c, (ULONG)1, &n);
		if FAILED(hr) return 0;

		return (long)c;
	}
protected:
	CBComPtr<IStream> m_pStream;
	};

CBImage::CImagePaint::CImagePaint(CBImage* pImage) : m_pImage(pImage)
{
	LOGBRUSH lb = {(m_pImage->m_nBrushStyle > 0) ? BS_HATCHED : BS_SOLID, m_pImage->m_colocBrush, m_pImage->m_nBrushStyle - 1};

	if(m_pImage->m_nPenStyle == -1)m_hPen = (HPEN)GetStockObject(NULL_PEN);
	else m_hPen = CreatePen(m_pImage->m_nPenStyle, m_pImage->m_nPenWidth, m_pImage->m_colorPen);

	if(m_pImage->m_nBrushStyle == -1)m_hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	else m_hBrush = CreateBrushIndirect(&lb);

	m_pImage->m_cs.Enter();
	m_hOldPen = (HPEN)SelectObject(m_pImage->m_Image.hMemDC, m_hPen);
	m_hOldBrush = (HBRUSH)SelectObject(m_pImage->m_Image.hMemDC, m_hBrush);
}

CBImage::CImagePaint::~CImagePaint()
{
	if(m_hOldPen)SelectObject(m_pImage->m_Image.hMemDC, m_hOldPen);
	if(m_hOldBrush)SelectObject(m_pImage->m_Image.hMemDC, m_hOldBrush);
	m_pImage->m_cs.Leave();

	DeleteObject(m_hPen);
	DeleteObject(m_hBrush);
}

CBImage::CImagePoly::CImagePoly(VARIANT var) :
	m_pPoint(NULL), m_nCount(0), m_hr(S_OK)
{
	VARIANT* pvar = &var;
	CComVariant varTemp;

	if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
	{
		CBDispatchPtr pDisp;

		if(pvar->punkVal == NULL)
		{
			m_hr = DISP_E_UNKNOWNINTERFACE;
			return;
		}

		m_hr = pvar->punkVal->QueryInterface(&pDisp);
		if(FAILED(m_hr))return;

		m_hr = pDisp.GetProperty(0, &varTemp);
		if(FAILED(m_hr))return;

		pvar = &varTemp;
	}

	if(pvar->vt != (VT_ARRAY | VT_VARIANT))
	{
		m_hr = DISP_E_TYPEMISMATCH;
		return;
	}

	LPSAFEARRAY pArray = pvar->vt & VT_BYREF ? *pvar->pparray : pvar->parray;

	if((pArray->cDims != 1) || (pArray->rgsabound[0].cElements & 1))
	{
		m_hr = DISP_E_TYPEMISMATCH;
		return;
	}

	if(m_nCount = pArray->rgsabound[0].cElements / 2)
	{
		m_pPoint = new POINT[m_nCount];

		pvar = (VARIANT *)pArray->pvData;

		for(int i = 0; i < m_nCount; i ++)
		{
			m_pPoint[i].x = varGetNumbar(pvar[i * 2]);
			m_pPoint[i].y = varGetNumbar(pvar[i * 2 + 1]);
		}
	}
}

CBImage::CImagePoly::~CImagePoly()
{
	if(m_pPoint)delete m_pPoint;
}

CBImage::CBImage()
{
    memset(&m_lfFont, 0, sizeof(m_lfFont));

	m_lfFont.lfHeight        = 10; 
	m_lfFont.lfCharSet       = DEFAULT_CHARSET;
	m_lfFont.lfWeight        = FW_NORMAL;
	m_lfFont.lfWidth         = 0; 
	m_lfFont.lfEscapement    = 0; 
	m_lfFont.lfOrientation   = 0; 
	m_lfFont.lfItalic        = FALSE; 
	m_lfFont.lfUnderline     = FALSE; 
	m_lfFont.lfStrikeOut     = FALSE; 
	m_lfFont.lfOutPrecision  = OUT_DEFAULT_PRECIS; 
	m_lfFont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
	m_lfFont.lfQuality       = PROOF_QUALITY; 
	m_lfFont.lfPitchAndFamily= DEFAULT_PITCH | FF_DONTCARE ; 
    strcpy( m_lfFont.lfFaceName, _T("Arial"));

	m_colocBrush = RGB(255, 255, 255);
	m_nBrushStyle = 0;
	m_colorPen = RGB(255, 255, 255);
	m_nPenWidth = 1;
	m_nPenStyle = PS_SOLID;
}

STDMETHODIMP CBImage::Load(VARIANT VarSrc, short nType)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBComPtr<IPicture> pPicture;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	CxStreamFile sf(pStream);

	m_cs.Enter();
	if(!m_Image.Decode(&sf, nType))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBImage::Save(VARIANT VarDesc, short nType)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	CxStreamFile sf(pStream);

	m_cs.Enter();
	if(nType == 0)
		nType = (short)m_Image.GetType();
	if(!m_Image.Encode(&sf, nType))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBImage::Create(long nWidth, long nHeight, short Bpp)
{
	HRESULT hr = S_OK;

	m_cs.Enter();
	if(!m_Image.Create(nWidth, nHeight, Bpp))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBImage::Clear(void)
{
	m_cs.Enter();
	m_Image.Clear();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::get_Type(short *pvar)
{
	m_cs.Enter();
	*pvar = (short)m_Image.GetType();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::get_Height(long *pvar)
{
	m_cs.Enter();
	*pvar = m_Image.GetHeight();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::get_Width(long *pvar)
{
	m_cs.Enter();
	*pvar = m_Image.GetWidth();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::get_Bpp(short *pvar)
{
	*pvar = m_Image.GetBpp();
	return S_OK;
}

STDMETHODIMP CBImage::put_Bpp(short var)
{
	HRESULT hr = S_OK;

	m_cs.Enter();
	if (var > m_Image.GetBpp())
	{
		if (!m_Image.IncreaseBpp(var))
			hr = SetErrorInfo(m_Image.GetLastError());
	}
	else if(var < m_Image.GetBpp())
	{
		if (!m_Image.DecreaseBpp(var, false))
			hr = SetErrorInfo(m_Image.GetLastError());
	}
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::get_Quality(short *pvar)
{
	*pvar = m_Image.GetJpegQuality();
	return S_OK;
}

STDMETHODIMP CBImage::put_Quality(short var)
{
	m_Image.SetJpegQuality((BYTE)var);
	return S_OK;
}

STDMETHODIMP CBImage::get_TransparentColor(long *pvar)
{
	m_cs.Enter();
	*(RGBQUAD*)pvar = m_Image.GetTransColor();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::put_TransparentColor(long var)
{
	if((var & 0xffff0000) == 0xffff0000)
		var &= 0xffff;
	var &= 0xffffff;

	m_Image.SetTransColor(*(RGBQUAD*)&var);
	return S_OK;
}

STDMETHODIMP CBImage::IsTransparent(VARIANT_BOOL *pvar)
{
	*pvar = m_Image.IsTransparent() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBImage::Resample(long nWidth, long nHeight, short nMode, IImage **pvar)
{
	HRESULT hr = S_OK;
	CBComPtr<CBImage> pNewImage;

	pNewImage.CreateInstance();

	m_cs.Enter();
	if(!m_Image.Resample(nWidth, nHeight, nMode, &pNewImage->m_Image))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	if(FAILED(hr))return hr;

	return pNewImage.QueryInterface(pvar);
}

STDMETHODIMP CBImage::Clone(IImage **pvar)
{
	CBComPtr<CBImage> pNewImage;

	pNewImage.CreateInstance();

	m_cs.Enter();
	pNewImage->m_Image.Copy(m_Image);
	m_cs.Leave();

	return pNewImage.QueryInterface(pvar);
}

STDMETHODIMP CBImage::Crop(long left, long top, long right, long bottom)
{
	HRESULT hr = S_OK;

	m_cs.Enter();
	if(!m_Image.Crop(left, top, right, bottom))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::Flip(short intDirection)
{
	HRESULT hr = S_OK;

	m_cs.Enter();
	if(intDirection == 2)
	{
		if(!m_Image.Flip())
			hr = SetErrorInfo(m_Image.GetLastError());
	}else if(intDirection == 1)
	{
		if(!m_Image.Mirror())
			hr = SetErrorInfo(m_Image.GetLastError());
	}
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::Resize(long nWidth, long nHeight, short nMode)
{
	HRESULT hr = S_OK;

	m_cs.Enter();
	if(!m_Image.Resample(nWidth, nHeight, nMode))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBImage::Rotate(long fAngle)
{
	HRESULT hr = S_OK;

	m_cs.Enter();
	if(!m_Image.Rotate(((float)-fAngle) / 10))
		hr = SetErrorInfo(m_Image.GetLastError());
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBImage::get_FillColor(long *pvar)
{
	*pvar = m_colocBrush;
	return S_OK;
}

STDMETHODIMP CBImage::put_FillColor(long var)
{
	if((var & 0xffff0000) == 0xffff0000)
		var &= 0xffff;
	var &= 0xffffff;

	m_colocBrush = var;
	return S_OK;
}

STDMETHODIMP CBImage::get_FillStyle(short *pvar)
{
	*pvar = m_nBrushStyle;
	return S_OK;
}

STDMETHODIMP CBImage::put_FillStyle(short var)
{
	m_nBrushStyle = var;
	return S_OK;
}

STDMETHODIMP CBImage::get_FontBold(VARIANT_BOOL *pvar)
{
	*pvar = (m_lfFont.lfWeight == FW_BOLD) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBImage::put_FontBold(VARIANT_BOOL var)
{
	m_lfFont.lfWeight = (var != VARIANT_FALSE) ? FW_BOLD : FW_NORMAL;
	return S_OK;
}

STDMETHODIMP CBImage::get_FontBkColor(long *pvar)
{
	m_cs.Enter();
	*pvar = GetBkColor(m_Image.hMemDC);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::put_FontBkColor(long var)
{
	if((var & 0xffff0000) == 0xffff0000)
		var &= 0xffff;
	var &= 0xffffff;

	m_cs.Enter();
	SetBkColor(m_Image.hMemDC, var);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::get_FontBkMode(VARIANT_BOOL *pvar)
{
	m_cs.Enter();
	*pvar = (GetBkMode(m_Image.hMemDC) == OPAQUE) ? VARIANT_TRUE : VARIANT_FALSE;
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::put_FontBkMode(VARIANT_BOOL var)
{
	m_cs.Enter();
	SetBkMode(m_Image.hMemDC, var != VARIANT_FALSE ? OPAQUE : TRANSPARENT);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::get_FontColor(long *pvar)
{
	m_cs.Enter();
	*pvar = GetTextColor(m_Image.hMemDC);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::put_FontColor(long var)
{
	if((var & 0xffff0000) == 0xffff0000)
		var &= 0xffff;
	var &= 0xffffff;

	m_cs.Enter();
	SetTextColor(m_Image.hMemDC, var);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::get_FontEscapement(long *pvar)
{
	*pvar = m_lfFont.lfEscapement;
	return S_OK;
}

STDMETHODIMP CBImage::put_FontEscapement(long var)
{
	m_lfFont.lfEscapement = var;
	return S_OK;
}

STDMETHODIMP CBImage::get_FontItalic(VARIANT_BOOL *pvar)
{
	*pvar = m_lfFont.lfItalic ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBImage::put_FontItalic(VARIANT_BOOL var)
{
	m_lfFont.lfItalic = (var != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CBImage::get_FontName(BSTR *pvar)
{
	CBString str;

	m_cs.Enter();
	str = m_lfFont.lfFaceName;
	m_cs.Leave();

	*pvar = str.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBImage::put_FontName(BSTR var)
{
	CBStringA stra(var);

	m_cs.Enter();
	strncpy(m_lfFont.lfFaceName, stra, sizeof(m_lfFont.lfFaceName));
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::get_FontSize(short *pvar)
{
	*pvar = (short)m_lfFont.lfHeight;
	return S_OK;
}

STDMETHODIMP CBImage::put_FontSize(short var)
{
	m_lfFont.lfHeight = var;

	return S_OK;
}

STDMETHODIMP CBImage::get_FontUnderline(VARIANT_BOOL *pvar)
{
	*pvar = m_lfFont.lfUnderline ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBImage::put_FontUnderline(VARIANT_BOOL var)
{
	m_lfFont.lfUnderline = (var != VARIANT_FALSE);
	return S_OK;
}


STDMETHODIMP CBImage::get_PenColor(long *pvar)
{
	*pvar = m_colorPen;
	return S_OK;
}

STDMETHODIMP CBImage::put_PenColor(long var)
{
	if((var & 0xffff0000) == 0xffff0000)
		var &= 0xffff;
	var &= 0xffffff;

	m_colorPen = var;
	return S_OK;
}

STDMETHODIMP CBImage::get_PenStyle(short *pvar)
{
	*pvar = m_nPenStyle;
	return S_OK;
}

STDMETHODIMP CBImage::put_PenStyle(short var)
{
	m_nPenStyle = var;
	return S_OK;
}

STDMETHODIMP CBImage::get_PenWidth(short *pvar)
{
	*pvar = m_nPenWidth;
	return S_OK;
}

STDMETHODIMP CBImage::put_PenWidth(short var)
{
	m_nPenWidth = var;
	return S_OK;
}

STDMETHODIMP CBImage::get_TextAlignment(short *pvar)
{
	m_cs.Enter();
	*pvar = GetTextAlign(m_Image.hMemDC);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::put_TextAlignment(short var)
{
	m_cs.Enter();
	SetTextAlign(m_Image.hMemDC, var);
	m_cs.Leave();
	return S_OK;
}

STDMETHODIMP CBImage::GetPixelColor(long x, long y, long *pvar)
{
	m_cs.Enter();
	*pvar = GetPixel(m_Image.hMemDC, x, y);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::SetPixelColor(long x, long y, long var)
{
	if((var & 0xffff0000) == 0xffff0000)
		var &= 0xffff;
	var &= 0xffffff;

	m_cs.Enter();
	SetPixel(m_Image.hMemDC, x, y, var);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBImage::DrawChord(long x1, long y1, long x2, long y2, long fromDegree, long toDegree)
{
	double f, t;

	f = fromDegree * (3.1415926535 / 1800);
	t = toDegree * (3.1415926535 / 1800);

	CImagePaint p(this);

	Chord(m_Image.hMemDC, x1, y1, x2, y2, 
			(x1 + x2) / 2 + (long)floor(1024 * cos(f)), 
			(y1 + y2) / 2 - (long)floor(1024 * sin(f)), 
			(x1 + x2) / 2 + (long)floor(1024 * cos(t)), 
			(y1 + y2) / 2 - (long)floor(1024 * sin(t)));

	return S_OK;
}

STDMETHODIMP CBImage::DrawArc(long x1, long y1, long x2, long y2, long fromDegree, long toDegree)
{
	double f, t;

	f = fromDegree * (3.1415926535 / 1800);
	t = toDegree * (3.1415926535 / 1800);

	CImagePaint p(this);

	Arc(m_Image.hMemDC, x1, y1, x2, y2, 
			(x1 + x2) / 2 + (long)floor(1024 * cos(f)), 
			(y1 + y2) / 2 - (long)floor(1024 * sin(f)), 
			(x1 + x2) / 2 + (long)floor(1024 * cos(t)), 
			(y1 + y2) / 2 - (long)floor(1024 * sin(t)));

	return S_OK;
}

STDMETHODIMP CBImage::DrawEllipse(long x1, long y1, long x2, long y2)
{
	CImagePaint p(this);

	Ellipse(m_Image.hMemDC, x1, y1, x2, y2);

	return S_OK;
}

STDMETHODIMP CBImage::DrawLine(long x1, long y1, long x2, long y2)
{
	CImagePaint p(this);

	MoveToEx(m_Image.hMemDC, x1, y1, NULL);
	LineTo(m_Image.hMemDC, x2, y2);

	return S_OK;
}

STDMETHODIMP CBImage::DrawPie(long x1, long y1, long x2, long y2, long fromDegree, long toDegree)
{
	double f, t;

	f = fromDegree * (3.1415926535 / 1800);
	t = toDegree * (3.1415926535 / 1800);

	CImagePaint p(this);

	Pie(m_Image.hMemDC, x1, y1, x2, y2, 
			(x1 + x2) / 2 + (long)floor(1024 * cos(f)), 
			(y1 + y2) / 2 - (long)floor(1024 * sin(f)), 
			(x1 + x2) / 2 + (long)floor(1024 * cos(t)), 
			(y1 + y2) / 2 - (long)floor(1024 * sin(t)));

	return S_OK;
}

STDMETHODIMP CBImage::DrawPolyBezier(VARIANT var)
{
	CImagePoly ps(var);

	if(ps.m_pPoint)
	{
        CImagePaint p(this);

		PolyBezier(m_Image.hMemDC, ps.m_pPoint, ps.m_nCount);
	}

	return ps.m_hr;
}

STDMETHODIMP CBImage::DrawPolygon(VARIANT var)
{
	CImagePoly ps(var);

	if(ps.m_pPoint)
	{
        CImagePaint p(this);

		Polygon(m_Image.hMemDC, ps.m_pPoint, ps.m_nCount);
	}

	return ps.m_hr;
}

STDMETHODIMP CBImage::DrawPolyline(VARIANT var)
{
	CImagePoly ps(var);

	if(ps.m_pPoint)
	{
        CImagePaint p(this);

		Polyline(m_Image.hMemDC, ps.m_pPoint, ps.m_nCount);
	}

	return ps.m_hr;
}

STDMETHODIMP CBImage::DrawRectangle(long x1, long y1, long x2, long y2)
{
	CImagePaint p(this);

	Rectangle(m_Image.hMemDC, x1, y1, x2, y2);

	return S_OK;
}

STDMETHODIMP CBImage::DrawText(long x, long y, BSTR strText)
{
	HFONT hFont;
	HFONT pOldFont;
	CBStringA stra(strText);
	HRESULT hr = S_OK;

	hFont = CreateFontIndirect( &m_lfFont );

	m_cs.Enter();
	pOldFont = (HFONT)SelectObject(m_Image.hMemDC, hFont);
	TextOut(m_Image.hMemDC, x, y, stra, stra.GetLength());
	if (pOldFont) SelectObject(m_Image.hMemDC, pOldFont);
	m_cs.Leave();

	DeleteObject(hFont);

	return hr;
}

STDMETHODIMP CBImage::Paint(long x, long y, IImage *pImage)
{
	CBComPtr<CBImage> pSrcImage;

	pSrcImage = pImage;
	if(pSrcImage == NULL)return DISP_E_TYPEMISMATCH;

	pSrcImage->m_cs.Enter();
	m_cs.Enter();

	int mx, my;

	mx = pSrcImage->m_Image.head.biWidth;
	if(mx + x > m_Image.head.biWidth)
		mx = m_Image.head.biWidth - x;

	my = pSrcImage->m_Image.head.biHeight;
	if(my + y > m_Image.head.biHeight)
		my = m_Image.head.biHeight - y;

	if(pSrcImage->m_Image.IsTransparent())
	{
		RGBQUAD c, c1;

		c1 = pSrcImage->m_Image.GetTransColor();
		for(int iy = 0; iy < my; iy ++)
			for(int ix = 0; ix < mx; ix++)
			{
				c = pSrcImage->m_Image.GetPixelColor(ix, iy);
				if(*(DWORD*)&c1 != *(DWORD*)&c)
					m_Image.SetPixelColor(x + ix, y + iy, c);
			}
	}else
	{
		for(int iy = 0; iy < my; iy ++)
			for(int ix = 0; ix < mx; ix++)
				m_Image.SetPixelColor(x + ix, y + iy, pSrcImage->m_Image.GetPixelColor(ix, iy));
	}

	m_cs.Leave();
	pSrcImage->m_cs.Leave();

	return S_OK;
}

