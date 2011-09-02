// NBR.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NBR.h"
#include "NBRDlg.h"

#include <activscp.h>
#include <openssl\rand.h>
#include <openssl\err.h>
#include <openssl\bio.h>
#include <openssl\conf.h>
#include <openssl\md5.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNBRApp

BEGIN_MESSAGE_MAP(CNBRApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNBRApp construction

CNBRApp::CNBRApp() : m_bBuild(FALSE), m_nLanguage(-1)
{
}


// The one and only CNBRApp object

CNBRApp theApp;

void CNBRApp::LoadLic(void)
{
/*	m_certinfo.Clear();

	CFile file;

	if(file.Open(m_strBasePath + _T("nbdw.lic"), CFile::modeRead | CFile::shareDenyNone))
	{
		char buffer[512];

		file.Read(buffer, 512);

		file.Close();

		if(!m_certinfo.SetLic(buffer))
		{
			m_certinfo.m_strError = _T("CertErr");
			return;
		}

		CBDate d;

		d.GetTime();
		d.Add(CBDate::modeDay, 2);

		m_certinfo.CheckDate(d);

		if(!m_certinfo.m_bValid)
		{
			m_certinfo.m_strError = _T("CertExp");
			return;
		}

		for(int i = 0; i < 16; i ++)
			if(m_aHash[i] != m_certinfo.m_aHash[i])
			{
				m_certinfo.m_bValid = FALSE;
				m_certinfo.m_strError = _T("CertHost");
				return;
			}

		m_certinfo.m_strError = _T("CertInfo");
	}else m_certinfo.m_strError = _T("NoFile");*/
}

// CNBRApp initialization

BOOL CNBRApp::InitInstance()
{
	if(__argc == 2 && (!_stricmp(__targv[1], "/REGSERVER") || !_stricmp(__targv[1], "/UNREGSERVER")))
	{
		SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSHNOWAIT,0, 0);
		return false;
	}

	if(__argc >= 2)
	{
		m_strStartPath = __targv[1];

		if(__argc == 3 && !_tcsicmp(__targv[2], _T("-build")))
			m_bBuild = TRUE;
	}

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxOleInit();
	AfxEnableControlContainer();

	CComPtr<IActiveScript> pActiveScript;

	pActiveScript.CoCreateInstance(L"VBScript");
	pActiveScript.Release();
	pActiveScript.CoCreateInstance(L"JScript");
	pActiveScript.Release();

	m_pSysInfo.CreateInstance();

	m_pSysInfo->AppendUserData(L"NBDW_Version", L"1.8");

	m_strBasePath.ReleaseBuffer(::GetModuleFileName(NULL, m_strBasePath.GetBuffer(_MAX_PATH), _MAX_PATH));
	m_strBasePath = m_strBasePath.Left(m_strBasePath.ReverseFind('\\') + 1);

	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(m_strBasePath + _T("NBDW.INI"));

	LoadStdProfileSettings(4);

	CNBRDlg dlg;
	m_pMainWnd = &dlg;

	RAND_screen();

	CBString strInfo = m_pSysInfo->Lookup(L"NBDW_Version") + m_pSysInfo->Lookup(L"OS_Platform") + m_pSysInfo->Lookup(L"OS_BuildNumber") + m_pSysInfo->Lookup(L"IE_Version") + m_pSysInfo->Lookup(L"System_Name") + m_pSysInfo->Lookup(L"CPU_Name") + m_pSysInfo->Lookup(L"CPU_Serial") + m_pSysInfo->Lookup(L"CPU_Number") + m_pSysInfo->Lookup(L"RAM");
	CBString strKey;
	int i;

	for(i = 0; i < 16; i ++)
	{
		strKey.Format(L"IDE_%d", i);
		strInfo.Append(m_pSysInfo->Lookup(strKey));
	}

	for(i = 0; i < 16; i ++)
	{
		strKey.Format(L"SCSI_%d", i);
		strInfo.Append(m_pSysInfo->Lookup(strKey));
	}

	for(i = 0; i < 16; i ++)
	{
		strKey.Format(L"Volume_%d", i);
		strInfo.Append(m_pSysInfo->Lookup(strKey));
	}

	MD5_CTX ctxMD5;

	MD5_Init(&ctxMD5);

	MD5_Update(&ctxMD5, (LPCWSTR)strInfo, strInfo.GetLength() * 2);

	MD5_Final(m_aHash, &ctxMD5);

	m_pSysInfo->Save(&m_mStream, FALSE);

	LoadLic();

	CComVariant var;

	m_pMsgMan.CreateInstance();
	var = m_strBasePath + _T("nbdw.lng");
	m_pMsgMan->Load(var);

	m_pMsgMan->GetLanguages(m_arrayLang);
	if(!m_arrayLang.GetCount())
	{
		AfxMessageBox(_T("The language resource is invalid. Repair or reinstall NetBox Deployment Wizard."));
		return FALSE;
	}

	INT_PTR nResponse = dlg.DoModal();

	RAND_cleanup();
	CONF_modules_unload(1);
    CRYPTO_cleanup_all_ex_data();
	ERR_remove_state(0);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

HRESULT GetBoxStream(LPCWSTR bstrFile, IStream **ppStream)
{
	return E_NOTIMPL;
}

extern "C" HANDLE __stdcall NewCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

extern "C" HANDLE __stdcall NewCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
