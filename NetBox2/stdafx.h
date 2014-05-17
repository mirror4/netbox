// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <winsock2.h>
#include <afxmt.h>
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afx.h>

void TimeToString(const SYSTEMTIME& st, CString &strTime);

#define IsBlankChar(c) (((c) == ' ') || ((c) == '\t'))
#define IsLineChar(c) (((c) == '\r') || ((c) == '\n'))
#define IsBlank(c) (IsBlankChar(c) || IsLineChar(c))

#ifdef _UNICODE
	#define BOX_CT2CA(str)		CStringA(str)
	#define BOX_CA2CT(str)		CString(str)

	#define BOX_CT2CW(str)		(str)
	#define BOX_CW2CT(str)		(str)
#else
	#define BOX_CT2CA(str)		(str)
	#define BOX_CA2CT(str)		(str)

	#define BOX_CT2CW(str)		CStringW(str)
	#define BOX_CW2CT(str)		CString(str)
#endif

BOOL _is_NT();

#define IS_WINNT		_is_NT()

#include "BoxSafeObject.h"

BOOL inline CheckFileExt(LPCTSTR pstr, char ch1, char ch2, char ch3)
{
	return pstr && pstr[0] == '.' &&
		tolower(pstr[1]) == ch1 &&
		tolower(pstr[2]) == ch2 &&
		tolower(pstr[3]) == ch3;
}

BOOL inline IsBoxFile(LPCTSTR pstr)
{
	return CheckFileExt(pstr, 'b', 'o', 'x');
}

BOOL inline IsAspFile(LPCTSTR pstr)
{
	return CheckFileExt(pstr, 'a', 's', 'p');
}

BOOL inline IsWscFile(LPCTSTR pstr)
{
	return CheckFileExt(pstr, 'w', 's', 'c');
}

BOOL inline IsExeFile(LPCTSTR pstr)
{
	return IsBoxFile(pstr) || IsAspFile(pstr) || IsWscFile(pstr);
}