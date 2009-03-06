#pragma once

#include "BThread.h"
#include "CxImage\ximage.h"

class __declspec(uuid("94650000-0000-0000-0000-00496D616765"))
CBImage : public CBDispatch<IImage>
{
	DECLARE_CLASS(CBImage)

private:
	class CImagePaint
	{
	public:
		CImagePaint(CBImage* pImage);
		~CImagePaint();

	private:
		CBImage* m_pImage;
		HPEN m_hPen;
		HPEN m_hOldPen;
		HBRUSH m_hBrush;
		HBRUSH m_hOldBrush;
	};

	class CImagePoly
	{
	public:
		CImagePoly(VARIANT var);
		~CImagePoly();

		LPPOINT m_pPoint;
		int m_nCount;
		HRESULT m_hr;
	};

public:
	CBImage();

public:
	// IImage
	STDMETHOD(Load)(VARIANT VarSrc, short nType = 0);
	STDMETHOD(Save)(VARIANT VarDesc, short nType = 0);
	STDMETHOD(Create)(long nWidth, long nHeight, short Bpp);
	STDMETHOD(Clear)(void);
	STDMETHOD(get_Type)(short *pvar);
	STDMETHOD(get_Height)(long *pvar);
	STDMETHOD(get_Width)(long *pvar);
	STDMETHOD(get_Bpp)(short *pvar);
	STDMETHOD(put_Bpp)(short var);
	STDMETHOD(get_Quality)(short *pvar);
	STDMETHOD(put_Quality)(short var);
	STDMETHOD(get_TransparentColor)(long *pvar);
	STDMETHOD(put_TransparentColor)(long var);
	STDMETHOD(IsTransparent)(VARIANT_BOOL *pvar);
	STDMETHOD(Resample)(long nWidth, long nHeight, short nMode, IImage **pvar);
	STDMETHOD(Clone)(IImage **pvar);
	STDMETHOD(Crop)(long left, long top, long right, long bottom);
	STDMETHOD(Flip)(short intDirection);
	STDMETHOD(Resize)(long nWidth, long nHeight, short nMode = 1);
	STDMETHOD(Rotate)(long fAngle);
	STDMETHOD(get_FillColor)(long *pvar);
	STDMETHOD(put_FillColor)(long var);
	STDMETHOD(get_FillStyle)(short *pvar);
	STDMETHOD(put_FillStyle)(short var);
	STDMETHOD(get_FontBold)(VARIANT_BOOL *pvar);
	STDMETHOD(put_FontBold)(VARIANT_BOOL var);
	STDMETHOD(get_FontBkColor)(long *pvar);
	STDMETHOD(put_FontBkColor)(long var);
	STDMETHOD(get_FontBkMode)(VARIANT_BOOL *pvar);
	STDMETHOD(put_FontBkMode)(VARIANT_BOOL var);
	STDMETHOD(get_FontColor)(long *pvar);
	STDMETHOD(put_FontColor)(long var);
	STDMETHOD(get_FontEscapement)(long *pvar);
	STDMETHOD(put_FontEscapement)(long var);
	STDMETHOD(get_FontItalic)(VARIANT_BOOL *pvar);
	STDMETHOD(put_FontItalic)(VARIANT_BOOL var);
	STDMETHOD(get_FontName)(BSTR *pvar);
	STDMETHOD(put_FontName)(BSTR var);
	STDMETHOD(get_FontSize)(short *pvar);
	STDMETHOD(put_FontSize)(short var);
	STDMETHOD(get_FontUnderline)(VARIANT_BOOL *pvar);
	STDMETHOD(put_FontUnderline)(VARIANT_BOOL var);
	STDMETHOD(get_PenColor)(long *pvar);
	STDMETHOD(put_PenColor)(long var);
	STDMETHOD(get_PenStyle)(short *pvar);
	STDMETHOD(put_PenStyle)(short var);
	STDMETHOD(get_PenWidth)(short *pvar);
	STDMETHOD(put_PenWidth)(short var);
	STDMETHOD(get_TextAlignment)(short *pvar);
	STDMETHOD(put_TextAlignment)(short var);
	STDMETHOD(GetPixelColor)(long x, long y, long *pvar);
	STDMETHOD(SetPixelColor)(long x, long y, long var);
	STDMETHOD(DrawArc)(long x1, long y1, long x2, long y2, long fromDegree, long toDegree);
	STDMETHOD(DrawChord)(long x1, long y1, long x2, long y2, long fromDegree, long toDegree);
	STDMETHOD(DrawEllipse)(long x1, long y1, long x2, long y2);
	STDMETHOD(DrawLine)(long x1, long y1, long x2, long y2);
	STDMETHOD(DrawPie)(long x1, long y1, long x2, long y2, long fromDegree, long toDegree);
	STDMETHOD(DrawPolyBezier)(VARIANT var);
	STDMETHOD(DrawPolygon)(VARIANT var);
	STDMETHOD(DrawPolyline)(VARIANT var);
	STDMETHOD(DrawRectangle)(long x1, long y1, long x2, long y2);
	STDMETHOD(DrawText)(long x, long y, BSTR strText);
	STDMETHOD(Paint)(long x, long y, IImage *pImage);

private:
	CBCriticalSection m_cs;
	CxImage m_Image;
	LOGFONT m_lfFont;
	short m_align;
	COLORREF m_colocBrush;
	short m_nBrushStyle;
	COLORREF m_colorPen;
	short m_nPenWidth;
	short m_nPenStyle;
};

