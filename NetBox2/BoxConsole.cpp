// BoxConsole.cpp : implementation file
//

#include "stdafx.h"
#include "BoxConsole.h"
#include "BoxSystem.h"
#include "NetBox2.h"

#include <openssl\rand.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBoxConsole

IMPLEMENT_DYNAMIC(CBoxConsole, CBoxSafeObject)

CBoxConsole::CBoxConsole() : m_hStdin(NULL), m_hStdout(NULL)
{
}

BEGIN_DISPATCH_MAP(CBoxConsole, CBoxSafeObject)
	DISP_FUNCTION(CBoxConsole, "Read", Read, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxConsole, "ReadLine", ReadLine, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CBoxConsole, "ReadPassword", ReadPassword, VT_BSTR, VTS_NONE)

	DISP_FUNCTION(CBoxConsole, "Write", Write, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxConsole, "WriteLine", WriteLine, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxConsole, "WriteBlankLines", WriteBlankLines, VT_EMPTY, VTS_I4)

	DISP_PROPERTY_EX(CBoxConsole, "Caption", get_Caption, put_Caption, VT_BSTR)
END_DISPATCH_MAP()

HWND g_hWndConsole;

void CBoxConsole::AllocConsole()
{
	if(m_hStdin == NULL && m_hStdout == NULL)
	{
		BYTE randstr[11];

		::AllocConsole();

		m_hStdin = ::GetStdHandle(STD_INPUT_HANDLE);
		m_hStdout = ::GetStdHandle(STD_OUTPUT_HANDLE);

		RAND_bytes(randstr, 10);
		for(int i = 0; i < 10; i ++)
			randstr[i] = (randstr[i] % 26) + 'A';
		randstr[10] = 0;
		::SetConsoleTitle((LPSTR)randstr);
		g_hWndConsole = ::FindWindow(NULL, (LPSTR)randstr);
		::SetConsoleTitle(CBoxSystem::getVersion());

		::SetForegroundWindow(g_hWndConsole);
	}
}

// CBoxConsole message handlers
void CBoxConsole::Write(LPCTSTR pstr)
{
	if(!pstr)return;

	DWORD dwSize = 0;
	DWORD dwStrLen = (DWORD)_tcslen(pstr);

	while(dwStrLen > 1024)
	{
		::WriteConsole(m_hStdout, pstr, 1024, &dwSize, NULL);
		dwSize = 0;
		pstr += 1024;
		dwStrLen -= 1024;
	}

	::WriteConsole(m_hStdout, pstr, dwStrLen, &dwSize, NULL);
}

void CBoxConsole::WriteLine(LPCTSTR pstr)
{
	DWORD dwSize;

	if(!pstr)return;

	Write(pstr);
	::WriteConsole(m_hStdout, _T("\r\n"), 2, &dwSize, NULL);
}

void CBoxConsole::WriteBlankLines(long nLine)
{
	if(nLine > 0)
	{
		TCHAR strNewLine[1024];
		DWORD dwSize;

		for(int i = 0; i < 1024; i += 2)
		{
			strNewLine[i] = _T('\r');
			strNewLine[i + 1] = _T('\n');
		}

		while(nLine > 512)
		{
			::WriteConsole(m_hStdout, strNewLine, 1024, &dwSize, NULL);
			nLine -= 512;
		}

		if(nLine > 0)
			::WriteConsole(m_hStdout, strNewLine, nLine * 2, &dwSize, NULL);
	}
}

long CBoxConsole::Read(void)
{
	TCHAR ch;
	DWORD dwSize;

	::ReadConsole(m_hStdin, &ch, 1, &dwSize, NULL);
	return (long)ch;
}

BSTR CBoxConsole::ReadPassword(void)
{
	DWORD dwMode;
	DWORD dwSize;
	
	GetConsoleMode(m_hStdin, &dwMode);
	SetConsoleMode(m_hStdin, ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
	BSTR bstr = ReadLine();
	SetConsoleMode(m_hStdin, dwMode);

	::WriteConsole(m_hStdout, _T("\r\n"), 2, &dwSize, NULL);

	return bstr;
}

CString CBoxConsole::ReadString(void)
{
	CString str;
	LPTSTR lpstr = str.GetBuffer(1024 + 1);
	DWORD dwSize;

	::ReadConsole(m_hStdin, lpstr, 1024, &dwSize, NULL);
	if(dwSize >= 2)dwSize -= 2;
	else dwSize = 0;
	lpstr[dwSize] = 0;

	str.ReleaseBuffer();

	return str;
}

BSTR CBoxConsole::ReadLine(void)
{
	return ReadString().AllocSysString();
}

void CBoxConsole::put_Caption(LPCTSTR pstrTitle)
{
	::SetConsoleTitle(pstrTitle);
}

BSTR CBoxConsole::get_Caption()
{
	TCHAR strTitle[1024];

	::GetConsoleTitle(strTitle, 1024);
	CComBSTR bstr(strTitle);
	return bstr.Detach();
}
