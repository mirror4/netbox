
#pragma once

#include <mshtmlc.h>
#include <atlsafe.h>

// CBoxEncoding command target

class CBoxEncoding : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxEncoding)

public:
	afx_msg BSTR HtmlEncode(LPCTSTR pstrText);
	afx_msg BSTR HtmlDecode(LPCTSTR pstrText);

	afx_msg BSTR UrlEncode(LPCTSTR pstrText);
	afx_msg BSTR UrlDecode(LPCTSTR pstrText);

	afx_msg BSTR JSEncode(LPCTSTR pstrText);
	afx_msg BSTR JSDecode(LPCTSTR pstrText);

	afx_msg VARIANT StrToBin(VARIANT& var, VARIANT &varCP);
	afx_msg BSTR BinToStr(VARIANT& var, VARIANT &varCP);

	afx_msg VARIANT Base64Decode(LPCTSTR pstrText);
	afx_msg BSTR Base64Encode(VARIANT& var);

	afx_msg VARIANT HexDecode(LPCTSTR pstrText);
	afx_msg BSTR HexEncode(VARIANT& var);

	DECLARE_DISPATCH_MAP()

private:
	enum FMTLEVEL
	{
		FL_STYLE = 1,
		FL_FORM = 2,
		FL_FRAME = 4,
		FL_IMAGE = 8,
		FL_SCRIPT = 16
	};
};


