// NBRDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NBR.h"
#include "NBRDlg.h"
#include "LogDialog.h"
#include "SiDlg.h"
#include <shlwapi.h>
#include "DlgLang.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNBRDlg dialog



CNBRDlg::CNBRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNBRDlg::IDD, pParent), m_sizeMin(0, 0), m_sizeMain(0, 0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNBRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT, m_wndOutput);
	DDX_Control(pDX, IDC_SOURCE, m_wndSource);
	DDX_Control(pDX, IDC_SELECT, m_wndSelect);
	DDX_Control(pDX, IDC_SRCFOLDER, m_wndSrcFolder);
	DDX_Control(pDX, IDC_NAME, m_wndName);
	DDX_Control(pDX, IDC_LOGO, m_wndLogo);
	DDX_Control(pDX, IDC_PBNB, m_wndPBNB);
	DDX_Control(pDX, IDOK, m_wndBuild);
	DDX_Control(pDX, IDC_STARTUP, m_wndStartup);
	DDX_Control(pDX, IDC_ICONFILE, m_wndIcon);
	DDX_Control(pDX, IDC_PRI, m_wndPRI);

	DDX_Control(pDX, IDC_FILEDESCRIPTION, m_wndFileDescription);
	DDX_Control(pDX, IDC_FILEVERSION, m_wndFileVersion);
	DDX_Control(pDX, IDC_PRODUCTVERSION, m_wndProductVersion);
	DDX_Control(pDX, IDC_PRODUCTNAME, m_wndProductName);
	DDX_Control(pDX, IDC_LEGALCOPYRIGHT, m_wndLegalCopyright);
}

BEGIN_MESSAGE_MAP(CNBRDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSER, OnBnClickedBrowser)
	ON_BN_CLICKED(IDC_FOLDER, OnBnClickedFolder)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_SRCFOLDER, OnCbnSelchangeSrcfolder)
	ON_BN_CLICKED(IDC_APPLICATION, OnBnClickedApplication)
	ON_BN_CLICKED(IDC_LIBRARY, OnBnClickedLibrary)
//	ON_BN_CLICKED(IDC_VIEWCERT, OnBnClickedViewcert)
	ON_BN_CLICKED(IDC_SYSINFO, OnBnClickedSysinfo)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SOURCE, OnLvnItemchangedSource)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CONSOLE, &CNBRDlg::OnBnClickedConsole)


END_MESSAGE_MAP()

#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

HBITMAP nbLoadSysColorBitmap(int id)
{
	HINSTANCE hInstImageWell = AfxFindResourceHandle((LPCTSTR)id, RT_BITMAP);
	HRSRC hRsrc = ::FindResource(hInstImageWell, (LPCTSTR)id, RT_BITMAP);
	if (hRsrc == NULL)
		return NULL;

	HGLOBAL hglb;
	if ((hglb = LoadResource(theApp.m_hInstance, hRsrc)) == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 256;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		if (pColorTable[iColor] == RGB(0xC0, 0xC0, 0xC0))
			pColorTable[iColor] =
				CLR_TO_RGBQUAD(::GetSysColor(COLOR_BTNFACE));
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);

	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

static struct
{
	int		id;
	int		left;
	int		top;
	int		width;
	int		height;
}s_ctlStyle[] = 
{
	{IDC_LOGO, 1, 0, 0, 0},
	{IDC_HR1, 0, 0, 1, 0},

	{IDC_SRCFOLDER, 0, 0, 1, 0},
	{IDC_FOLDER, 1, 0, 0, 0},
	{IDC_SOURCE, 0, 0, 1, 1},

	{IDT_FILETYPE, 0, 1, 0, 0},
	{IDC_SELECT, 0, 1, 1, 0},
	{IDC_ADD, 1, 1, 0, 0},
	{IDC_REMOVE, 1, 1, 0, 0},
	{IDC_HR2, 0, 1, 1, 0},

	{IDT_OUTFILE, 0, 1, 0, 0},
	{IDC_OUTPUT, 0, 1, 1, 0},
	{IDC_BROWSER, 1, 1, 0, 0},
	{IDC_HR3, 0, 1, 1, 0},

	{IDT_PROTYPE, 0, 1, 0, 0},
	{IDC_APPLICATION, 0, 1, 0, 0},
	{IDC_CONSOLE, 0, 1, 0, 0},
	{IDC_LIBRARY, 0, 1, 0, 0},
	{IDOK, 1, 1, 0, 0},
	{IDT_STARTUP, 0, 1, 1, 0},
	{IDC_STARTUP, 0, 1, 0, 0},
	{IDC_HR4, 0, 1, 1, 0},

	{IDT_ICON, 0, 1, 0, 0},
	{IDC_ICONFILE, 0, 1, 0, 0},
	{IDT_PRI, 0, 1, 0, 0},
	{IDC_PRI, 0, 1, 0, 0},
	{IDC_HR6, 0, 1, 1, 0},

	{IDT_FILEDESCRIPTION, 0, 1, 0, 0},
	{IDC_FILEDESCRIPTION, 0, 1, 1, 0},
	{IDT_FILEVERSION, 0, 1, 0, 0},
	{IDC_FILEVERSION, 0, 1, 0, 0},
	{IDT_PRODUCTVERSION, 0, 1, 0, 0},
	{IDC_PRODUCTVERSION, 0, 1, 0, 0},
	{IDT_PRODUCTNAME, 0, 1, 0, 0},
	{IDC_PRODUCTNAME, 0, 1, 1, 0},
	{IDT_LEGALCOPYRIGHT, 0, 1, 0, 0},
	{IDC_LEGALCOPYRIGHT, 0, 1, 1, 0},
	{IDC_HR7, 0, 1, 1, 0},
	
//	{IDC_VIEWCERT, 0, 1, 0, 0},
	{IDC_SYSINFO, 0, 1, 0, 0},
	{IDC_EMBEDVBS, 0, 1, 0, 0},
	{IDC_PBNB, 1, 1, 0, 0},
	{IDCANCEL, 1, 1, 0, 0}
};

// CNBRDlg message handlers

BOOL CNBRDlg::OnInitDialog()
{
	CRect r;
	CSize s;

	GetWindowRect(&r);
	m_sizeMin = r.Size();
	m_sizeMain = m_sizeMin;
	GetClientRect(&r);
	s = r.Size();

	CDialog::OnInitDialog();

	int i, j;

	for(i = 0; i < sizeof(s_ctlStyle) / sizeof(s_ctlStyle[0]); i ++)
	{
		CWnd* pWin;

		if(pWin = GetDlgItem(s_ctlStyle[i].id))
		{
			pWin->GetWindowRect(&r);
			ScreenToClient(&r);

			if(s_ctlStyle[i].left || s_ctlStyle[i].top)
			{
				if(s_ctlStyle[i].left)s_ctlStyle[i].left = r.left - s.cx;
				else s_ctlStyle[i].left = r.left;
				if(s_ctlStyle[i].top)s_ctlStyle[i].top = r.top - s.cy;
				else s_ctlStyle[i].top = r.top;
			}

			if(s_ctlStyle[i].width || s_ctlStyle[i].height)
			{
				if(s_ctlStyle[i].width)s_ctlStyle[i].width = r.Width() - s.cx;
				else s_ctlStyle[i].width = r.Width();
				if(s_ctlStyle[i].height)s_ctlStyle[i].height = r.Height() - s.cy;
				else s_ctlStyle[i].height = r.Height();
			}
		}
	}

	SetWindowPos(NULL, 0, 0,
		theApp.GetProfileInt(_T("IDE"), "Width", m_sizeMin.cx),
		theApp.GetProfileInt(_T("IDE"), "Height", m_sizeMin.cy),
		SWP_NOZORDER | SWP_NOMOVE);

	m_wndName.SetBitmap(nbLoadSysColorBitmap(IDB_NAME));
	m_wndLogo.SetBitmap(nbLoadSysColorBitmap(IDB_LOGO));
	m_wndPBNB.SetBitmap(nbLoadSysColorBitmap(IDB_PBNB));
	m_wndBuild.OnSysColorChange();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	m_menuLang.CreatePopupMenu();
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);

		for(i = 0; i < (int)theApp.m_arrayLang.GetCount(); i ++)
			m_menuLang.AppendMenu(MF_STRING, IDM_LANGUAGE + i * 0x10, CString(theApp.m_arrayLang[i]));
		pSysMenu->AppendMenu(MF_POPUP, (UINT_PTR)m_menuLang.m_hMenu, _T("Language"));

		pSysMenu->AppendMenu(MF_SEPARATOR);
//		pSysMenu->AppendMenu(MF_STRING, IDM_VIEWCERT, _T("View Certificate ..."));
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, _T("&About NBDW..."));
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_VIEWTEAM, _T("NetBox Team ..."));
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_wndSelect.SetCurSel(-1);
	CheckRadioButton(IDC_LIBRARY, IDC_CONSOLE, IDC_APPLICATION);

	m_wndSource.InsertColumn(0, _T("Name"), LVCFMT_LEFT, theApp.GetProfileInt(_T("List"), _T("Column_0"), 200));
	m_wndSource.InsertColumn(1, _T("Size"), LVCFMT_RIGHT, theApp.GetProfileInt(_T("List"), _T("Column_1"), 60));
	m_wndSource.InsertColumn(2, _T("Type"), LVCFMT_LEFT, theApp.GetProfileInt(_T("List"), _T("Column_2"), 80));
	m_wndSource.InsertColumn(3, _T("Date"), LVCFMT_LEFT, theApp.GetProfileInt(_T("List"), _T("Column_3"), 100));
	m_wndSource.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	m_wndSelect.AddString(_T("Backup File(*.bak)"));
	m_wndSelect.AddString(_T("Compressed File(*.rar;*.zip;*.cab;*.arj;*.lzh;*.ace;*.tar;*.gzip)"));
	m_wndSelect.AddString(_T("Database File(*.mdb;*.db;*.dbf;*.mdf;*.ldf)"));
	m_wndSelect.AddString(_T("Executable File(*.exe;*.com;*.dll;*.ocx;*.sys)"));
	m_wndSelect.AddString(_T("Media File(*.mp3;*.wav;*.mid;*.mpeg;*.mpg;*.avi;*.rm;*.ram;*.mov)"));

	m_wndPRI.AddString(_T("asInvoker"));
	m_wndPRI.SetCurSel(0);
	m_wndPRI.AddString(_T("requireAdministrator"));

	CString str;
	for(int i = 0; i < 128; i ++)
	{
		str.Format(_T("item_%d"), i);
		str = theApp.GetProfileString(_T("Project"), str);
		if(str.IsEmpty())break;
		if(PathIsDirectory(str))m_wndSrcFolder.AddString(str);
	}

	str = theApp.GetProfileString(_T("IDE"), _T("Language"));

	for(j = 0; j < (int)theApp.m_arrayLang.GetCount(); j ++)
		if(!theApp.m_arrayLang[j].CompareNoCase(CBString(str)))
		{
			theApp.setLanguage(j);
			break;
		}

	if(theApp.m_nLanguage == -1)
	{
		CDlgLang lang;

		lang.DoModal();
	}
	m_menuLang.CheckMenuItem(IDM_LANGUAGE + theApp.m_nLanguage * 0x10, MF_CHECKED);
	setLanguage();

	if(!theApp.m_strStartPath.IsEmpty())
	{
		m_strSrcFolder = theApp.m_strStartPath;
		m_strSrcFolder.StripPath();
		m_wndSrcFolder.SetWindowText(CBStringA(m_strSrcFolder.m_strPath));
		theApp.m_strStartPath.Empty();
		OnCbnSelchangeSrcfolder();

		if(theApp.m_bBuild)
		{
			OnBnClickedOk();
			EndDialog(0);
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNBRDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if(m_sizeMin.cx && m_sizeMin.cy)
	{
		lpMMI->ptMinTrackSize.x = m_sizeMin.cx;
		lpMMI->ptMinTrackSize.y = m_sizeMin.cy;
	}
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CNBRDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(!(nType & SIZE_MINIMIZED))
	{
		int i, x, y;

		for(i = 0; i < sizeof(s_ctlStyle) / sizeof(s_ctlStyle[0]); i ++)
		{
			CWnd* pWin;

			if(pWin = GetDlgItem(s_ctlStyle[i].id))
			{
				if(s_ctlStyle[i].left || s_ctlStyle[i].top)
				{
					if(s_ctlStyle[i].left > 0)x = s_ctlStyle[i].left;
					else x = cx + s_ctlStyle[i].left;
					if(s_ctlStyle[i].top > 0)y = s_ctlStyle[i].top;
					else y = cy + s_ctlStyle[i].top;
					pWin->SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				}

				if(s_ctlStyle[i].width || s_ctlStyle[i].height)
				{
					if(s_ctlStyle[i].width > 0)x = s_ctlStyle[i].width;
					else x = cx + s_ctlStyle[i].width;
					if(s_ctlStyle[i].height > 0)y = s_ctlStyle[i].height;
					else y = cy + s_ctlStyle[i].height;
					pWin->SetWindowPos(NULL, 0, 0, x, y, SWP_NOZORDER | SWP_NOMOVE);
				}
			}
		}

		InvalidateRect(NULL, FALSE);
	}

	if(!(nType & SIZE_MINIMIZED) && !(nType & SIZE_MAXIMIZED))
	{
		CRect r;

		GetWindowRect(&r);
		m_sizeMain = r.Size();
	}
}

void CNBRDlg::setLanguage(void)
{
	SetDlgItemText(IDT_SRCFOLDER, GetMessage(L"Srcfolder"));
	SetDlgItemText(IDC_FOLDER, GetMessage(L"SelFolder") + _T(" ..."));
	SetDlgItemText(IDT_FILETYPE, GetMessage(L"fileType") + _T(":"));
	SetDlgItemText(IDT_OUTFILE, GetMessage(L"outFile") + _T(":"));
	SetDlgItemText(IDC_BROWSER, GetMessage(L"Browse"));
	SetDlgItemText(IDT_PROTYPE, GetMessage(L"proType") + _T(":"));
	SetDlgItemText(IDC_APPLICATION, GetMessage(L"Application"));
	SetDlgItemText(IDC_CONSOLE, GetMessage(L"Console"));
	SetDlgItemText(IDC_LIBRARY, GetMessage(L"Library"));
	SetDlgItemText(IDT_STARTUP, GetMessage(L"Startup") + _T(":"));
	SetDlgItemText(IDCANCEL, GetMessage(L"Close"));
//	SetDlgItemText(IDC_VIEWCERT, GetMessage(L"ViewCert") + _T(" ..."));
	SetDlgItemText(IDC_SYSINFO, GetMessage(L"SysInfo") + _T(" ..."));

	SetDlgItemText(IDT_LEGALCOPYRIGHT, GetMessage(L"Copyright"));
	SetDlgItemText(IDT_PRODUCTVERSION, GetMessage(L"ProductVersion"));
	SetDlgItemText(IDT_PRODUCTNAME, GetMessage(L"ProductName"));
	SetDlgItemText(IDT_FILEVERSION, GetMessage(L"FileVersion"));
	SetDlgItemText(IDT_FILEDESCRIPTION, GetMessage(L"FileDescription"));
	SetDlgItemText(IDT_ICON, GetMessage(L"Icon"));
	SetDlgItemText(IDT_PRI, GetMessage(L"PRI"));

	LVCOLUMN col;
	CString str;

	col.mask = LVCF_TEXT;

	str = GetMessage(L"Name");
	col.pszText = (LPTSTR)(LPCTSTR)str;
	m_wndSource.SetColumn(0, &col);

	str = GetMessage(L"Size");
	col.pszText = (LPTSTR)(LPCTSTR)str;
	m_wndSource.SetColumn(1, &col);

	str = GetMessage(L"Type");
	col.pszText = (LPTSTR)(LPCTSTR)str;
	m_wndSource.SetColumn(2, &col);

	str = GetMessage(L"Date");
	col.pszText = (LPTSTR)(LPCTSTR)str;
	m_wndSource.SetColumn(3, &col);
}

void CNBRDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
/*	}else if ((nID & 0xFFF0) == IDM_VIEWCERT)
	{
		CCertView dlgCert;
		dlgCert.DoModal();*/
	}else if ((nID & 0xFFF0) == IDM_VIEWTEAM)
		ShellExecute(NULL, "open", "http://www.9465.net/technical/team.htm", NULL, NULL, SW_SHOW);
	else if(((nID & 0xFFF0) >= IDM_LANGUAGE) &&
		((((nID & 0xFFF0) - IDM_LANGUAGE) >> 4) < (int)theApp.m_arrayLang.GetCount()))
	{
		m_menuLang.CheckMenuItem(IDM_LANGUAGE + theApp.m_nLanguage * 0x10, MF_UNCHECKED);
		theApp.setLanguage(((nID & 0xFFF0) - IDM_LANGUAGE) >> 4);
		m_menuLang.CheckMenuItem(IDM_LANGUAGE + theApp.m_nLanguage * 0x10, MF_CHECKED);
		setLanguage();
	}
	else CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNBRDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNBRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNBRDlg::OnBnClickedBrowser()
{
	LPCTSTR pstrFilter = _T("NetBox Application(*.exe)|*.exe|All Files (*.*)|*.*||");
	LPCTSTR pstrExt = _T("exe");

	if(IsDlgButtonChecked(IDC_LIBRARY))
	{
		pstrFilter = _T("NetBox Library(*.nbl)|*.nbl|All Files (*.*)|*.*||");
		pstrExt = _T("nbl");
	}

	CFileDialog tFileDlg(FALSE, pstrExt, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, pstrFilter, this);

	tFileDlg.DoModal();

	m_wndOutput.SetWindowText(tFileDlg.GetPathName());
}

static TCHAR szDir[MAX_PATH];

static INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
		break;
	}
	return 0;
}

void CNBRDlg::OnBnClickedFolder()
{
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	CComPtr<IMalloc> pMalloc;

	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		ZeroMemory(&bi, sizeof(bi));
		bi.lpfn = BrowseCallbackProc;
		bi.hwndOwner = m_hWnd;

		pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			if(SHGetPathFromIDList(pidl, szDir))
			{
				m_strSrcFolder = szDir;
				m_strSrcFolder.AddBackslash();

				m_wndSrcFolder.SetCurSel(-1);
				m_wndSrcFolder.SetWindowText(szDir);

				OnCbnSelchangeSrcfolder();
			}
			pMalloc->Free(pidl);
		}
	}
}

void CNBRDlg::OnCbnSelchangeSrcfolder()
{
	int nSel = m_wndSrcFolder.GetCurSel();
	int i;

	if(nSel != -1)
	{
		CString str;

		m_wndSrcFolder.GetLBText(nSel, str);
		m_strSrcFolder = str;
		m_strSrcFolder.AddBackslash();
	}

	if(!m_strSrcFolder.IsDirectory())
	{
		if(nSel != -1)
		{
			m_wndSrcFolder.SetCurSel(-1);
			m_wndSrcFolder.DeleteString(nSel);
		}

		return;
	}

	m_wndOutput.SetWindowText(_T(""));
	m_wndSource.DeleteAllItems();
	m_wndStartup.ResetContent();
	m_strStartup.Empty();
	m_wndStartup.SetCurSel(0);

	m_wndIcon.ResetContent();
	m_strIcon.Empty();
	m_wndIcon.SetCurSel(0);

	m_wndSelect.SetCurSel(-1);
	CheckRadioButton(IDC_LIBRARY, IDC_CONSOLE, IDC_APPLICATION);

	m_imgs.DeleteImageList();
	m_imgs.Create(16, 16, ILC_COLORDDB, 0, 0);
	m_arrayType.RemoveAll();
	m_typeIndex.RemoveAll();

	m_wndSource.SetImageList(&m_imgs, LVSIL_SMALL);

	GetSourceFolder(CBStringA(m_strSrcFolder));
	m_wndStartup.SelectString(-1, _T("main.box"));

	m_wndIcon.SelectString(-1, _T("main.ico"));

	m_wndPRI.SetCurSel(0);

	m_wndFileDescription.SetWindowText(_T(""));
	m_wndFileVersion.SetWindowText(_T(""));
	m_wndProductVersion.SetWindowText(_T(""));
	m_wndProductName.SetWindowText(_T(""));
	m_wndLegalCopyright.SetWindowText(_T(""));

	CStringArray straMake;
	CString str;

	LoadFileArray(CBStringA(m_strSrcFolder) + _T("netbox.dwf"), straMake);

	for(i = 0; i < straMake.GetCount(); i ++)
	{
		str = straMake[i];
		if(!str.Left(8).CompareNoCase(_T("STARTUP ")))
		{
			m_strStartup = str.Mid(8);
			m_strStartup.Trim();
			m_wndStartup.SelectString(-1, m_strStartup);
		}else if(!str.Left(7).CompareNoCase(_T("OUTPUT ")))
		{
			str = str.Mid(7);
			str.Trim();
			m_wndOutput.SetWindowText(str);
		}else if(!str.Left(5).CompareNoCase(_T("TYPE ")))
		{
			str = str.Mid(5);
			str.Trim();
			if(!str.CompareNoCase(_T("Library")))
			{
				CheckRadioButton(IDC_LIBRARY, IDC_CONSOLE, IDC_LIBRARY);
				m_wndStartup.EnableWindow(FALSE);
				m_wndIcon.EnableWindow(FALSE);
				m_wndPRI.EnableWindow(FALSE);
				m_wndFileDescription.EnableWindow(FALSE);
				m_wndFileVersion.EnableWindow(FALSE);
				m_wndProductVersion.EnableWindow(FALSE);
				m_wndProductName.EnableWindow(FALSE);
				m_wndLegalCopyright.EnableWindow(FALSE);
			}else
			{
				CheckRadioButton(IDC_LIBRARY, IDC_CONSOLE, str.CompareNoCase(_T("Application"))?IDC_CONSOLE:IDC_APPLICATION);
				m_wndStartup.EnableWindow();
				m_wndIcon.EnableWindow();
				m_wndPRI.EnableWindow();
				m_wndFileDescription.EnableWindow();
				m_wndFileVersion.EnableWindow();
				m_wndProductVersion.EnableWindow();
				m_wndProductName.EnableWindow();
				m_wndLegalCopyright.EnableWindow();
			}
		}else if(!str.Left(5).CompareNoCase(_T("ICON ")))
		{
			m_strIcon = str.Mid(5);
			m_strIcon.Trim();
			m_wndIcon.SelectString(-1, m_strIcon);
		}else if(!str.Left(4).CompareNoCase(_T("PRI ")))
		{
			str = str.Mid(4);
			str.Trim();
			m_wndPRI.SelectString(-1, str);
		}else if(!str.Left(16).CompareNoCase(_T("FILEDESCRIPTION ")))
		{
			str = str.Mid(16);
			str.Trim();
			m_wndFileDescription.SetWindowText(str);
		}else if(!str.Left(12).CompareNoCase(_T("FILEVERSION ")))
		{
			str = str.Mid(12);
			str.Trim();
			m_wndFileVersion.SetWindowText(str);
		}else if(!str.Left(15).CompareNoCase(_T("PRODUCTVERSION ")))
		{
			str = str.Mid(15);
			str.Trim();
			m_wndProductVersion.SetWindowText(str);
		}else if(!str.Left(12).CompareNoCase(_T("PRODUCTNAME ")))
		{
			str = str.Mid(12);
			str.Trim();
			m_wndProductName.SetWindowText(str);
		}else if(!str.Left(15).CompareNoCase(_T("LEGALCOPYRIGHT ")))
		{
			str = str.Mid(15);
			str.Trim();
			m_wndLegalCopyright.SetWindowText(str);
		}else if(!str.Left(8).CompareNoCase(_T("EXCLUDE ")))
		{
			LVFINDINFO info;
			int nIndex;

			str = str.Mid(7);
			str.Trim();
			str.MakeLower();

			info.flags = LVFI_PARTIAL|LVFI_STRING;
			info.psz = str;

			nIndex = m_wndSource.FindItem(&info);
			if(nIndex != -1)m_wndSource.SetCheck(nIndex, FALSE);
		}else if(!str.Left(17).CompareNoCase(_T("PACKAGELOADFIRST ")))
		{
			str = str.Mid(17);
			str.Trim();
			CheckDlgButton(IDC_EMBEDVBS, str.CompareNoCase(_T("TRUE"))?TRUE:FALSE);
		}
	}
}

void CNBRDlg::GetSourceFolder(LPCTSTR szPath)
{
	CFileFind find;
	CString str(szPath), str1;

	str += _T("\\*.*");

	BOOL bOK = find.FindFile(str);

	while(bOK)
	{
		bOK = find.FindNextFile();

		if(!find.IsHidden() && !find.IsDots())
		{
			int n = m_wndSource.GetItemCount();
			int typeIndex;
			LONGLONG size;
			CTime tm;
			BOOL bNotMake;
			LVFINDINFO lf;

			str = find.GetFilePath();
			str.MakeLower();

			m_wndSource.InsertItem(n, (LPCTSTR)str + CBStringA(m_strSrcFolder).GetLength());

			ZeroMemory(&lf, sizeof(lf));
			lf.flags = LVFI_STRING;
			lf.psz = (LPCTSTR)str + CBStringA(m_strSrcFolder).GetLength();
			n = m_wndSource.FindItem(&lf);

			bNotMake = _tcscmp((LPCTSTR)str + CBStringA(m_strSrcFolder).GetLength(), _T("netbox.dwf"));

			str1 = find.GetFileName();
			int p = str1.Find('.');
			if(p != -1)
			{
				str1 = str1.Mid(p);
				if(str1.Compare(_T(".exe")) && str1.Compare(_T(".ico")))
					str = str1;
			}else str = _T("");

			if(!m_typeIndex.Lookup(str, typeIndex))
			{
				SHFILEINFO sfi;

				ZeroMemory(&sfi,sizeof(sfi));
				if(!find.IsDirectory())
					SHGetFileInfo(str, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_TYPENAME|SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_ICON);
				else SHGetFileInfo(_T(""), FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi), SHGFI_TYPENAME|SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_ICON);

				typeIndex = (int)m_arrayType.GetCount();
				m_imgs.SetImageCount(typeIndex + 1);
				m_imgs.Replace(typeIndex, sfi.hIcon);
				m_arrayType.Add(sfi.szTypeName);
				m_typeIndex.SetAt(str, typeIndex);
				//DeleteObject(sfi.hIcon);
			}

			m_wndSource.SetItem(n, 0, LVIF_IMAGE, NULL, typeIndex, 0, 0, 0);

			size = (find.GetLength() + 1023) / 1024;
			str = _T(" KB");
			while(size > 1000)
			{
				str1.Format(_T(",%03d"), size % 1000);
				size /= 1000;
				str = str1 + str;
			}
			str1.Format(_T("%d"), size);
			str = str1 + str;

			if(!find.IsDirectory())
				m_wndSource.SetItemText(n, 1, str);

			m_wndSource.SetItemText(n, 2, m_arrayType[typeIndex]);

			find.GetLastWriteTime(tm);
			str.Format(_T("%d-%d-%d %d:%d"), tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute());
			m_wndSource.SetItemText(n, 3, str);

			m_wndSource.SetCheck(n, bNotMake);
		}

		if(!find.IsHidden() && !find.IsDots() && find.IsDirectory())
			GetSourceFolder(find.GetFilePath());
	}
}

#define UP(c)	(((c) >= 'A' && (c) <= 'Z') ? ((c) + ('a' - 'A')) : (c))

static BOOL CheckString(LPCTSTR ChkStr, LPCTSTR FmtStr)
{	int i, l = (int)_tcslen(ChkStr);

	if(!*ChkStr && !*FmtStr)return TRUE;
	if(!*ChkStr && *FmtStr)
	{
		while(*FmtStr == '?' || *FmtStr == '*')FmtStr ++;
		if(*FmtStr)return FALSE;
		return TRUE;
	}

	switch(*FmtStr)
	{
	case '?':
		return CheckString(ChkStr + 1, FmtStr + 1);
	case '*':
		for(i = 0; i < l; i ++)
			if(CheckString(ChkStr + i + 1, FmtStr + 1))return TRUE;
		return FALSE;
	default:
		if(UP(*ChkStr) != UP(*FmtStr))return FALSE;
		return CheckString(ChkStr + 1, FmtStr + 1);
	}
}

void CNBRDlg::ChangeSelect(BOOL bCheck)
{
	static LPCTSTR pstrBackup[] = {
		_T(".bak"),
		NULL
	};
	static LPCTSTR pstrCompress[] = {
		_T(".rar"),
		_T(".zip"),
		_T(".cab"),
		_T(".arj"),
		_T(".lzh"),
		_T(".ace"),
		_T(".tar"),
		_T(".gzip"),
		NULL
	};
	static LPCTSTR pstrDBase[] = {
		_T(".mdb"),
		_T(".db"),
		_T(".dbf"),
		_T(".mdf"),
		_T(".ldf"),
		NULL
	};
	static LPCTSTR pstrExe[] = {
		_T(".exe"),
		_T(".com"),
		_T(".dll"),
		_T(".ocx"),
		_T(".sys"),
		NULL
	};
	static LPCTSTR pstrMedia[] = {
		_T(".mp3"),
		_T(".wav"),
		_T(".mid"),
		_T(".mpeg"),
		_T(".mpg"),
		_T(".avi"),
		_T(".rm"),
		_T(".ram"),
		_T(".mov"),
		NULL
	};
	int n = m_wndSelect.GetCurSel();
	LPCTSTR *pstrSelect;
	CString str, str1;
	int i, j;

	switch(n)
	{
	case 0:pstrSelect = pstrBackup;break;
	case 1:pstrSelect = pstrCompress;break;
	case 2:pstrSelect = pstrDBase;break;
	case 3:pstrSelect = pstrExe;break;
	case 4:pstrSelect = pstrMedia;break;
	case -1:
		n = m_wndSource.GetItemCount();
		m_wndSelect.GetWindowText(str1);

		for(i = 0; i < n; i ++)
		{
			str = m_wndSource.GetItemText(i, 0);
			if(CheckString(str.Mid(str.ReverseFind(_T('\\')) + 1), str1))
				m_wndSource.SetCheck(i, bCheck);
		}
	default:
		m_wndSelect.SetCurSel(-1);
		return;
	}

	m_wndSelect.SetCurSel(-1);

	n = m_wndSource.GetItemCount();
	for(i = 0; i < n; i ++)
	{
		str = m_wndSource.GetItemText(i, 0);
		for(j = 0; pstrSelect[j]; j ++)
			if(!str.Right((int)_tcslen(pstrSelect[j])).Compare(pstrSelect[j]))
			{
				m_wndSource.SetCheck(i, bCheck);
				break;
			}
	}
}

void CNBRDlg::OnBnClickedRemove()
{
	ChangeSelect(FALSE);
}

void CNBRDlg::OnBnClickedAdd()
{
	ChangeSelect(TRUE);
}

void CNBRDlg::OnBnClickedOk()
{
/*	if(!theApp.m_certinfo.m_bValid)
	{
		CCertView dlgCert;

		dlgCert.DoModal();
	}

	if(!theApp.m_certinfo.m_bValid)
		return;
*/
	CLogDialog logDlg(this);
	CStringArray straMake;
	CString str;
	int i, n = m_wndSource.GetItemCount();

	for(i = 0; i < n; i ++)
		if(!m_wndSource.GetCheck(i))
			straMake.Add(_T("EXCLUDE ") + m_wndSource.GetItemText(i, 0));

	if(straMake.GetCount() == n)
	{
		AfxMessageBox(_T("Please enter Source Folder!"));
		m_wndSource.SetFocus();
		return;
	}

	m_wndOutput.GetWindowText(str);
	str.Trim();
	if(str.IsEmpty())
	{
		AfxMessageBox(_T("Please enter Output File!"));
		m_wndOutput.SetFocus();
		return;
	}
	straMake.InsertAt(0, _T("OUTPUT ") + str);

	if(!IsDlgButtonChecked(IDC_LIBRARY))
	{
		if(IsDlgButtonChecked(IDC_APPLICATION))
			str = _T("Application");
		else
			str = _T("ConsoleApp");
	}
	else str = _T("Library");
	straMake.InsertAt(1, _T("TYPE ") + str);

	if(!IsDlgButtonChecked(IDC_LIBRARY))
	{
		m_wndStartup.GetWindowText(str);

		if(str.IsEmpty())
		{
			AfxMessageBox(_T("Please select Startup!"));
			m_wndStartup.SetFocus();
			return;
		}

		straMake.InsertAt(1, _T("STARTUP ") + str);

		m_wndIcon.GetWindowText(str);
		straMake.InsertAt(1, _T("ICON ") + str);

		m_wndPRI.GetWindowText(str);
		straMake.InsertAt(1, _T("PRI ") + str);

		m_wndFileDescription.GetWindowText(str);
		straMake.InsertAt(1, _T("FILEDESCRIPTION ") + str);
		m_wndFileVersion.GetWindowText(str);
		straMake.InsertAt(1, _T("FILEVERSION ") + str);
		m_wndProductVersion.GetWindowText(str);
		straMake.InsertAt(1, _T("PRODUCTVERSION ") + str);
		m_wndProductName.GetWindowText(str);
		straMake.InsertAt(1, _T("PRODUCTNAME ") + str);
		m_wndLegalCopyright.GetWindowText(str);
		straMake.InsertAt(1, _T("LEGALCOPYRIGHT ") + str);

		straMake.InsertAt(1, _T("PACKAGELOADFIRST ") + IsDlgButtonChecked(IDC_EMBEDVBS)?_T("TRUE"):_T("FALSE"));
	}

	str = m_strSrcFolder;

	SaveFileArray(str + _T("netbox.dwf"), straMake);

	while((n = m_wndSrcFolder.FindString(0, str)) != CB_ERR)
		m_wndSrcFolder.DeleteString(n);

	m_wndSrcFolder.InsertString(0, str);

	m_wndSrcFolder.SetCurSel(-1);
	m_wndSrcFolder.SetWindowText(str);

	logDlg.DoModal();
}

void CNBRDlg::LoadFileArray(LPCTSTR pstrFile, CStringArray& strArray)
{
	CFile file;

	if(file.Open(pstrFile, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary))
	{
		char *pBuffer, *ptr, *ptr1;
		int nSize = (int)file.GetLength();

		pBuffer = new char[nSize + 1];
		if(pBuffer)
		{
			CString str;

			file.Read(pBuffer, nSize);
			pBuffer[nSize] = 0;
			file.Close();

			ptr = pBuffer;
			while(*ptr)
			{
				ptr1 = ptr;
				while(*ptr1 && *ptr1 != '\n' && *ptr1 != '\r')
					ptr1 ++;
				str.SetString(ptr, (int)(ptr1 - ptr));
				str.Trim();

				if(!str.IsEmpty())strArray.Add(str);

				while(*ptr1 && *ptr1 == '\n' || *ptr1 == '\r')
					ptr1 ++;

				ptr = ptr1;
			}

			delete pBuffer;
		}
	}
}

void CNBRDlg::SaveFileArray(LPCTSTR pstrFile, CStringArray& strArray)
{
	CFile file;

	if(file.Open(pstrFile, CFile::shareDenyNone | CFile::modeWrite | CFile::typeBinary | CFile::modeCreate))
	{
		CString str;
		int i;

		for(i = 0; i < strArray.GetCount(); i ++)
			str += strArray[i] + _T("\r\n");

		file.Write((LPCTSTR)str, str.GetLength());
	}
}

void CNBRDlg::OnDestroy()
{
	DeleteFile(theApp.m_pszProfileName);

	int i, n, n1;
	CString str, str1;

	theApp.WriteProfileString(_T("IDE"), _T("Language"), CString(theApp.m_pMsgColl->m_strLanguage));
	theApp.WriteProfileInt(_T("IDE"), _T("Width"), m_sizeMain.cx);
	theApp.WriteProfileInt(_T("IDE"), _T("Height"), m_sizeMain.cy);

	theApp.WriteProfileInt(_T("List"), _T("Column_0"), m_wndSource.GetColumnWidth(0));
	theApp.WriteProfileInt(_T("List"), _T("Column_1"), m_wndSource.GetColumnWidth(1));
	theApp.WriteProfileInt(_T("List"), _T("Column_2"), m_wndSource.GetColumnWidth(2));
	theApp.WriteProfileInt(_T("List"), _T("Column_3"), m_wndSource.GetColumnWidth(3));

	n = m_wndSrcFolder.GetCount();

	n1 = 0;
	for(i = 0; i < n; i ++)
	{
		m_wndSrcFolder.GetLBText(i, str);
		if(PathIsDirectory(str))
		{
			str1.Format(_T("item_%d"), n1 ++);
			theApp.WriteProfileString(_T("Project"), str1, str);
		}
	}

	CDialog::OnDestroy();
}

void CNBRDlg::OnBnClickedConsole()
{
	m_wndOutput.SetWindowText(_T(""));
	m_wndStartup.EnableWindow();
	m_wndIcon.EnableWindow();
	m_wndPRI.EnableWindow();
	m_wndFileDescription.EnableWindow();
	m_wndFileVersion.EnableWindow();
	m_wndProductVersion.EnableWindow();
	m_wndProductName.EnableWindow();
	m_wndLegalCopyright.EnableWindow();
}

void CNBRDlg::OnBnClickedApplication()
{
	m_wndOutput.SetWindowText(_T(""));
	m_wndStartup.EnableWindow();
	m_wndIcon.EnableWindow();
	m_wndPRI.EnableWindow();
	m_wndFileDescription.EnableWindow();
	m_wndFileVersion.EnableWindow();
	m_wndProductVersion.EnableWindow();
	m_wndProductName.EnableWindow();
	m_wndLegalCopyright.EnableWindow();
}

void CNBRDlg::OnBnClickedLibrary()
{
	m_wndOutput.SetWindowText(_T(""));
	m_wndStartup.EnableWindow(FALSE);
	m_wndIcon.EnableWindow(FALSE);
	m_wndPRI.EnableWindow(FALSE);
	m_wndFileDescription.EnableWindow(FALSE);
	m_wndFileVersion.EnableWindow(FALSE);
	m_wndProductVersion.EnableWindow(FALSE);
	m_wndProductName.EnableWindow(FALSE);
	m_wndLegalCopyright.EnableWindow(FALSE);
}

void CNBRDlg::OnBnClickedViewcert()
{
/*	CCertView dlgCert;
	dlgCert.DoModal();*/
}

void CNBRDlg::OnBnClickedSysinfo()
{
	CSiDlg siDlg;

	siDlg.DoModal();
}

BOOL CNBRDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	POINT pos;

	GetCursorPos(&pos);
	ScreenToClient(&pos);
	CWnd *pChildWnd = ChildWindowFromPoint(pos);

	if(pChildWnd && pChildWnd->GetDlgCtrlID() == IDC_PBNB)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return 0;
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CNBRDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd *pChildWnd = ChildWindowFromPoint(point);

	if(pChildWnd && pChildWnd->GetDlgCtrlID() == IDC_PBNB)
		ShellExecute(NULL, "open", "http://www.9465.net/", NULL, NULL, SW_SHOW);

	CDialog::OnLButtonUp(nFlags, point);
}

void CNBRDlg::OnSysColorChange()
{
	CDialog::OnSysColorChange();

	m_wndName.SetBitmap(nbLoadSysColorBitmap(IDB_NAME));
	m_wndLogo.SetBitmap(nbLoadSysColorBitmap(IDB_LOGO));
	m_wndPBNB.SetBitmap(nbLoadSysColorBitmap(IDB_PBNB));
	m_wndBuild.OnSysColorChange();
}

void CNBRDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_wndBuild.CheckMouse();
	CDialog::OnMouseMove(nFlags, point);
}

void CNBRDlg::OnLvnItemchangedSource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if((pNMLV->uChanged & LVIF_STATE) && 
		((pNMLV->uNewState ^ pNMLV->uOldState) & INDEXTOSTATEIMAGEMASK(LVIS_SELECTED)))
	{
		CString str;

		str = m_wndSource.GetItemText(pNMLV->iItem, 0);
		if(str.Find(_T('\\')) == -1 && !str.Right(4).Compare(".box"))
		{
			int nIndex = m_wndStartup.FindString(-1, str);

			if((pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(LVIS_SELECTED)) && nIndex == CB_ERR)
				m_wndStartup.AddString(str);
			else if(nIndex != CB_ERR)m_wndStartup.DeleteString(nIndex);

			m_wndStartup.SetCurSel(0);
			m_wndStartup.SelectString(-1, _T("main.box"));
			if(!m_strStartup.IsEmpty())
				m_wndStartup.SelectString(-1, m_strStartup);
		}
		else if (str.Find(_T('\\')) == -1 && !str.Right(4).Compare(".ico"))
		{
			int nIndex = m_wndIcon.FindString(-1, str);

			if((pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(LVIS_SELECTED)) && nIndex == CB_ERR)
				m_wndIcon.AddString(str);
			else if(nIndex != CB_ERR)m_wndIcon.DeleteString(nIndex);

			m_wndIcon.SetCurSel(0);
			m_wndIcon.SelectString(-1, _T("main.ico"));
			if(!m_strIcon.IsEmpty())
				m_wndIcon.SelectString(-1, m_strIcon);
		}
	}
}

void CNBRDlg::OnDropFiles(HDROP hDropInfo)
{
	CString str;

	str.ReleaseBuffer(DragQueryFile(hDropInfo, 0, str.GetBuffer(MAX_PATH), MAX_PATH));

	m_strSrcFolder = str;

	if(!m_strSrcFolder.IsDirectory())
		m_strSrcFolder.StripPath();

	if(!m_strSrcFolder.IsDirectory())return;

	m_strSrcFolder.AddBackslash();

	str = m_strSrcFolder;

	m_wndSrcFolder.SetCurSel(-1);
	m_wndSrcFolder.SetWindowText(str);

	OnCbnSelchangeSrcfolder();
}

