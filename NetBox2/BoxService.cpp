// BoxService.cpp : implementation file
//

#include "stdafx.h"
#include "BoxService.h"
#include "BoxScript.h"
#include "BoxFile.h"
#include "NetBox2.h"
#include <BWindow.h>

#include <openssl\err.h>

// CBoxService

#define SRV_RESTART		0x10
#define WM_CHGICON		(WM_USER + 1)
#define WM_POPUPMENU	(WM_USER + 2)

CBoxService::CBoxService() :
	m_uElapse(0),
	m_bCheckReg(FALSE),
	m_sshStatusHandle(0),
	m_bDesktop(FALSE),
	m_bTrayIcon(TRUE),
	m_nAutoRestart(0),
	m_nMenuDelay(0),
	m_bInBLCLK(FALSE),
	m_bServiceType(1)
{
	ZeroMemory(&m_tnd, sizeof(m_tnd));
	m_tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	m_tnd.uID = (UINT)IDI_WINLOGO;
	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
	m_tnd.uCallbackMessage = WM_USER;
	m_tnd.hIcon = CBWindow::LoadIcon(L"");
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_nFreq);
}

CBoxService::~CBoxService()
{
	if(m_tnd.hIcon)DestroyIcon(m_tnd.hIcon);
}

IMPLEMENT_DYNAMIC(CBoxService, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxService, CBoxSafeObject)
	DISP_FUNCTION_ID(CBoxService, "RunService", 0, RunService, VT_EMPTY, VTS_BSTR VTS_VARIANT VTS_VARIANT)

	DISP_PROPERTY(CBoxService, "Name", m_strName, VT_BSTR)
	DISP_PROPERTY(CBoxService, "DisplayName", m_strDisplayName, VT_BSTR)
	DISP_PROPERTY(CBoxService, "Description", m_strDescription, VT_BSTR)

	DISP_PROPERTY(CBoxService, "Desktop", m_bDesktop, VT_BOOL)
	DISP_PROPERTY(CBoxService, "TrayIcon", m_bTrayIcon, VT_BOOL)
	DISP_PROPERTY(CBoxService, "AutoRestart", m_nAutoRestart, VT_I4)
	DISP_PROPERTY(CBoxService, "Type", m_bServiceType, VT_I4)
	DISP_PROPERTY_EX(CBoxService, "Icon", get_Icon, put_Icon, VT_BSTR)

	DISP_PROPERTY_EX(CBoxService, "Installed", get_Installed, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX(CBoxService, "Running", get_Running, SetNotSupported, VT_BOOL)

	DISP_FUNCTION(CBoxService, "Install", Install, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxService, "Remove", Remove, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxService, "Restart", Restart, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxService, "Start", Start, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxService, "Stop", Stop, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxService, "SendCommand", SendCommand, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CBoxService, "AppendCommand", AppendCommand, VT_EMPTY, VTS_I2 VTS_VARIANT)
	DISP_FUNCTION(CBoxService, "LogEvent", LogEvent, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CBoxService, "SetTimer", SetTimer, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CBoxService, "DoEvents", DoEvents, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// CBoxService message handlers

void CBoxService::RunService(LPCTSTR pstrName, VARIANT* pVarDisplay, VARIANT* pVarDescription)
{
	m_strName = pstrName;
	if(pVarDisplay->vt == VT_BSTR)
		m_strDisplayName = pVarDisplay->bstrVal;
	if(pVarDescription->vt == VT_BSTR)
		m_strDescription = pVarDescription->bstrVal;

	if(theApp.m_pArguments->GetCount() > 1)
	{
		CString str;

		str = theApp.m_pArguments->GetString(1);

		if(!str.CompareNoCase(_T("-install")))
		{
			CComVariant v(1);
			Install(v);
			theApp.Quit(0);

			return;
		}else if(!str.CompareNoCase(_T("-remove")))
		{
			Remove();
			theApp.Quit(0);

			return;
		}else if(!str.CompareNoCase(_T("-start")))
		{
			Start();
			theApp.Quit(0);

			return;
		}else if(!str.CompareNoCase(_T("-stop")))
		{
			Stop();
			theApp.Quit(0);

			return;
		}else if(!str.CompareNoCase(_T("-restart")))
		{
			Restart();
			theApp.Quit(0);

			return;
		}
	}

	DoEvents();
}

BOOL CBoxService::get_Installed(void)
{
	BOOL bIsInstalled = FALSE;

	if(IS_WINNT && m_bServiceType)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;

		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager)
		{
			schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

			if(schService)
			{
				bIsInstalled = TRUE;
				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}else
	{
		HKEY hk;
		BOOL bResult;

		bResult = m_bServiceType ? (RegOpenKey(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"),
				&hk ) == ERROR_SUCCESS) : (RegOpenKey(HKEY_CURRENT_USER,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
				&hk ) == ERROR_SUCCESS);

		if (bResult)
		{
			TCHAR szPath[512] = _T("");
			DWORD dwSize = sizeof(szPath);
			DWORD dwType = REG_SZ;

			if(RegQueryValueEx(hk, m_strName, 0, &dwType, (LPBYTE)szPath, &dwSize) == ERROR_SUCCESS)
				bIsInstalled = TRUE;
			RegCloseKey(hk);
		}
	}

	return bIsInstalled;
}

BOOL CBoxService::get_Running(void)
{
	BOOL bIsRunning = FALSE;

	if(IS_WINNT && m_bServiceType)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;
		SERVICE_STATUS ssStatus;

		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager)
		{
			schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

			if(schService)
			{
				if(QueryServiceStatus(schService, &ssStatus) && ssStatus.dwCurrentState == SERVICE_RUNNING)
					bIsRunning = TRUE;

				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}

	if(!bIsRunning && FindWindow(m_strName, NULL))
		bIsRunning = TRUE;

	return bIsRunning;
}

void CBoxService::Install(VARIANT& varType)
{
	while(varType.vt == (VT_VARIANT | VT_BYREF))
		varType = *varType.pvarVal;

	int iAutoRun = 1;
	if(varType.vt != VT_ERROR)
	{
		CComVariant v;
		v.ChangeType(VT_I4, &varType);
		iAutoRun = v.lVal?1:0;
	}

	CString str;

	str = _T('\"') + g_pFile->m_strExeName + _T('\"');

	if(g_pFile->m_strAppName.Compare(g_pFile->m_strExeName))
		str += _T(" \"") + g_pFile->m_strAppName + _T('\"');
	else str = g_pFile->m_strAppName;

	if(m_bServiceType)str += _T(" -Dispatch ");
	else str += _T(" -AutoRun ");
	str += m_strName;

	if(IS_WINNT && m_bServiceType)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;

		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if ( schSCManager )
		{
			schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

			if(schService)
			{
				ChangeServiceConfig(schService, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				if (iAutoRun)
					StartService(schService, 0, NULL);
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return;
			}

			schService = CreateService(schSCManager, m_strName,
				m_strDisplayName.IsEmpty() ? m_strName : m_strDisplayName,
				SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | (m_bDesktop ? SERVICE_INTERACTIVE_PROCESS : 0),
				SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, str, NULL, NULL, NULL, NULL, NULL);

			if ( schService )
			{
				SERVICE_DESCRIPTION sd;

				sd.lpDescription = (LPTSTR)(LPCTSTR)m_strDescription;

				BOOL (WINAPI *pfnChangeServiceConfig2A)(SC_HANDLE hService, DWORD dwInfoLevel, LPVOID lpInfo);
				pfnChangeServiceConfig2A = (BOOL (WINAPI *)(SC_HANDLE, DWORD, LPVOID))GetProcAddress(GetModuleHandle(_T("ADVAPI32.DLL")), "ChangeServiceConfig2A");
				if(pfnChangeServiceConfig2A)
				{
					pfnChangeServiceConfig2A( schService, SERVICE_CONFIG_DESCRIPTION, &sd);
					if (m_nAutoRestart)
					{
						SERVICE_FAILURE_ACTIONS sfa;
						SC_ACTION sca;

						sca.Delay = m_nAutoRestart<60000?60000:m_nAutoRestart;
						sca.Type = SC_ACTION_RESTART;

						sfa.dwResetPeriod = 0;
						sfa.lpRebootMsg = NULL;
						sfa.cActions = sizeof(sca)/sizeof(SC_ACTION);
						sfa.lpsaActions = &sca;
						sfa.lpCommand = NULL;

						pfnChangeServiceConfig2A( schService, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);
					}
				}

				if (iAutoRun)
					StartService( schService, 0, NULL);
				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}else
	{
		HKEY hk;
		BOOL bResult;

		bResult = m_bServiceType ? (RegOpenKey(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"),
				&hk ) == ERROR_SUCCESS) : (RegOpenKey(HKEY_CURRENT_USER,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
				&hk ) == ERROR_SUCCESS);

		if (!bResult)
			return;

		RegSetValueEx(hk, m_strName, 0, REG_SZ, (CONST LPBYTE)(LPCTSTR)str, (DWORD)str.GetLength() + 1);

		RegCloseKey(hk);
		if (iAutoRun)
			WinExec(str, SW_SHOW);
	}
}

void CBoxService::Remove(void)
{
	Stop();

	if(IS_WINNT && m_bServiceType)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;

		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager)
		{
			schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

			if(schService)
			{
				DeleteService(schService);
				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}else
	{
		HKEY hk;
		BOOL bResult;

		bResult = m_bServiceType ? (RegOpenKey(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"),
				&hk ) == ERROR_SUCCESS) : (RegOpenKey(HKEY_CURRENT_USER,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
				&hk ) == ERROR_SUCCESS);

		if (!bResult)
			return;

		RegDeleteValue(hk, m_strName);
		RegCloseKey(hk);
	}
}

void CBoxService::Start(void)
{
	if(IS_WINNT && m_bServiceType)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;

		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if ( schSCManager )
		{
			schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

			if(schService)
			{
				StartService( schService, 0, NULL);
				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}else
	{
		HKEY hk;
		TCHAR szPath[512] = _T("");
		DWORD dwSize = sizeof(szPath);
		DWORD dwType = REG_SZ;
		BOOL bResult;

		bResult = m_bServiceType ? (RegOpenKey(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"),
				&hk ) == ERROR_SUCCESS) : (RegOpenKey(HKEY_CURRENT_USER,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
				&hk ) == ERROR_SUCCESS);

		if (!bResult)
			return;

		if(RegQueryValueEx(hk, m_strName, 0, &dwType, (LPBYTE)szPath, &dwSize) == ERROR_SUCCESS)
			WinExec(BOX_CT2CA(szPath), SW_SHOW);

		RegCloseKey(hk);
	}
}

void CBoxService::Stop(void)
{
	if(IS_WINNT && m_bServiceType)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;
		SERVICE_STATUS ssStatus;

		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager)
		{
			schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

			if(schService)
			{
				if(ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
				{
					Sleep(1);

					while(QueryServiceStatus(schService, &ssStatus))
					{
						if (ssStatus.dwCurrentState != SERVICE_STOPPED)
							Sleep(1);
						else break;
					}
				}

				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}else
	{
		int i;
		HWND hwnd;

		hwnd = FindWindow(m_strName, NULL);
		if(hwnd)
		{
			i = 0;
			while(hwnd && i < 100)
			{
				Sleep(1);

				PostMessage(hwnd, WM_CLOSE, 0, 0);
				hwnd = FindWindow(m_strName, NULL);
			}
		}
	}
}

void CBoxService::Restart(void)
{
	HWND hwnd;

	hwnd = FindWindow(m_strName, NULL);
	if(hwnd)PostMessage(hwnd, WM_COMMAND, SRV_RESTART, 0);
	else
	{
		Stop();
		Start();
	}
}

void CBoxService::SendCommand(short uCtrl)
{
	if(uCtrl < 256)
	{
		HWND hwnd;

		hwnd = FindWindow(m_strName, NULL);
		if(hwnd)PostMessage(hwnd, WM_COMMAND, uCtrl, 0);
		else if(IS_WINNT)
		{
			SC_HANDLE   schService;
			SC_HANDLE   schSCManager;
			SERVICE_STATUS ssStatus;

			schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (schSCManager)
			{
				schService = OpenService(schSCManager, m_strName, SERVICE_ALL_ACCESS);

				if(schService)
				{
					ControlService(schService, uCtrl, &ssStatus);
					CloseServiceHandle(schService);
				}
				CloseServiceHandle(schSCManager);
			}
		}
	}
}

void CBoxService::LogEvent(long nType, LPCTSTR pstrMsg)
{
	HANDLE hEventSource;

	if(!m_bCheckReg)
	{
		HKEY hk;
		CString str(_T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\"));

		if(m_strName.IsEmpty())str += _T("NetBox");
		else str += m_strName;
		if (RegOpenKey(HKEY_LOCAL_MACHINE, str, &hk ) != ERROR_SUCCESS &&
			RegCreateKey(HKEY_LOCAL_MACHINE, str, &hk) != ERROR_SUCCESS)
			return;

		RegSetValueEx(hk, _T("EventMessageFile"), 0, REG_SZ, (CONST BYTE*)(LPCTSTR)g_pFile->m_strExeName, (g_pFile->m_strExeName.GetLength() + 1) * sizeof(TCHAR));

		RegCloseKey(hk);

		m_bCheckReg = TRUE;
	}
 
	if(m_strName.IsEmpty())
		hEventSource = ::RegisterEventSource(NULL, _T("NetBox"));
	else hEventSource = ::RegisterEventSource(NULL, m_strName);
	if (hEventSource != NULL)
	{
		::ReportEvent(hEventSource, (WORD)nType, 0, 0, NULL, 1, 0, (LPCTSTR*) &pstrMsg, NULL);
		::DeregisterEventSource(hEventSource); 
	}
}

BSTR CBoxService::get_Icon(void)
{
	return m_strIcon.AllocSysString();
}

void CBoxService::put_Icon(LPCTSTR pstrName)
{
	if(m_strIcon.CompareNoCase(pstrName))
	{
		HICON hIcon = m_tnd.hIcon;

		m_tnd.hIcon = CBWindow::LoadIcon(CBString(pstrName));
		SendMessage(m_tnd.hWnd, WM_CHGICON, 0, 0);
		m_strIcon = pstrName;

		if(hIcon)DestroyIcon(hIcon);
	}
}

void CBoxService::SetTimer(long uElapse)
{
	m_uElapse = uElapse;
}

void CBoxService::Dispatch(LPCTSTR pstrName)
{
	m_strName = pstrName;

	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ NULL, service_main},
		{ NULL, NULL }
	};

	m_bDesktop = theApp.m_bIsShell;
	dispatchTable[0].lpServiceName = (LPTSTR)(LPCTSTR)m_strName;

	if (!StartServiceCtrlDispatcher(dispatchTable))
	{
		DWORD dw = GetLastError();
		theApp.m_pService->LogEvent(EVENTLOG_ERROR_TYPE, "StartServiceCtrlDispatcher Error.");
		
		LPSTR pMsgBuf = NULL;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
		{
			theApp.m_pService->LogEvent(EVENTLOG_ERROR_TYPE, pMsgBuf);
			LocalFree(pMsgBuf);
		}
	}
}

void CBoxService::DoEvents(void)
{
	if(FindWindow(m_strName, NULL))
		return;

	WNDCLASS wc;
	MSG msg;
	HINSTANCE hInstance;
	void (WINAPI *RegisterServiceProcess)(DWORD dwProcessId, DWORD dwType);

	ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0);

	RegisterServiceProcess = (void (WINAPI *)(DWORD, DWORD))GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")), "RegisterServiceProcess");

	if(RegisterServiceProcess)
		RegisterServiceProcess(GetCurrentProcessId(), 1);

	hInstance = (HINSTANCE)0x400000;

	ZeroMemory(&wc, sizeof(wc));

	wc.lpszClassName = m_strName;
	wc.lpfnWndProc = StaticMainWndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	RegisterClass( &wc );

	strncpy(m_tnd.szTip, m_strDisplayName.IsEmpty() ? m_strName : m_strDisplayName, sizeof(m_tnd.szTip));

	CreateWindow(m_strName, m_strDisplayName.IsEmpty() ? m_strName : m_strDisplayName,
				WS_SYSMENU | WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

	ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);

	while( GetMessage( &msg, NULL, 0, 0 ))
	{
		BOOL bContinue = TRUE;
		if((msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)) 
			//||(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST))
		{
			// give HTML page a chance to translate this message
			switch( msg.wParam )
			{
			case VK_F5:
			case VK_F6:
				break;
			default:
				bContinue = !(BOOL)::SendMessage(GetActiveWindow(), WM_FORWARDMSG, 0, (LPARAM)&msg);
				break;
			}
		}
		if( bContinue )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		
	}

	OnStop();
}

void WINAPI CBoxService::service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	theApp.m_pService->Main();
}

void CBoxService::Main()
{
	m_dwError = 0;

	m_sshStatusHandle = RegisterServiceCtrlHandler( m_strName, service_ctrl);

	if (m_sshStatusHandle)
	{
		m_ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | (m_bDesktop ? SERVICE_INTERACTIVE_PROCESS : 0);
		m_ssStatus.dwServiceSpecificExitCode = 0;

		ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);

		theApp.Start();

		ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);

		m_sshStatusHandle = 0;
	}
	else
	{
		DWORD dw = GetLastError();
		theApp.m_pService->LogEvent(EVENTLOG_ERROR_TYPE, "RegisterServiceCtrlHandler Error.");
		
		LPSTR pMsgBuf = NULL;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
		{
			theApp.m_pService->LogEvent(EVENTLOG_ERROR_TYPE, pMsgBuf);
			LocalFree(pMsgBuf);
		}
	}
}

void WINAPI CBoxService::service_ctrl(DWORD dwCtrlCode)
{
	theApp.m_pService->Control(dwCtrlCode);
}

void CBoxService::Control(DWORD dwCtrlCode)
{
	::SendMessage(m_tnd.hWnd, WM_COMMAND, dwCtrlCode, 0);
}

void CBoxService::ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;

	if(dwCurrentState == SERVICE_START_PENDING)
		m_ssStatus.dwControlsAccepted = 0;
	else
		m_ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	m_ssStatus.dwCurrentState = dwCurrentState;
	m_ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	m_ssStatus.dwWaitHint = dwWaitHint;

	if((dwCurrentState == SERVICE_RUNNING) ||
		 (dwCurrentState == SERVICE_STOPPED))
		m_ssStatus.dwCheckPoint = 0;
	else
		m_ssStatus.dwCheckPoint = dwCheckPoint++;

	if(m_sshStatusHandle)
		SetServiceStatus( m_sshStatusHandle, &m_ssStatus);
}

void CBoxService::OnStart(void)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		if(pNowScript->CallEvent(_T("OnServiceStart")))
			::PostMessage(m_tnd.hWnd, WM_CLOSE, 0, 0);
}

void CBoxService::OnStop(void)
{
	ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);

	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		if(pNowScript->CallEvent(_T("OnServiceStop")))
			::PostMessage(m_tnd.hWnd, WM_CLOSE, 0, 0);
}

void CBoxService::OnPause(void)
{
	if(m_ssStatus.dwCurrentState == SERVICE_RUNNING)
	{
		ReportStatusToSCMgr(SERVICE_PAUSE_PENDING, NO_ERROR, 0);

		CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

		if(pNowScript != NULL)
			if(pNowScript->CallEvent(_T("OnServicePause")))
				::PostMessage(m_tnd.hWnd, WM_CLOSE, 0, 0);

		EnableMenuItem(m_hMenu, SERVICE_CONTROL_CONTINUE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hMenu, SERVICE_CONTROL_PAUSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

		ReportStatusToSCMgr(SERVICE_PAUSED, NO_ERROR, 0);
	}
}

void CBoxService::OnResume(void)
{
	if(m_ssStatus.dwCurrentState == SERVICE_PAUSED)
	{
		ReportStatusToSCMgr(SERVICE_CONTINUE_PENDING, NO_ERROR, 0);

		CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

		if(pNowScript != NULL)
			if(pNowScript->CallEvent(_T("OnServiceResume")))
				::PostMessage(m_tnd.hWnd, WM_CLOSE, 0, 0);

		EnableMenuItem(m_hMenu, SERVICE_CONTROL_CONTINUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		EnableMenuItem(m_hMenu, SERVICE_CONTROL_PAUSE, MF_BYCOMMAND | MF_ENABLED);

		ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);
	}
}

void CBoxService::OnTimer(void)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		if(pNowScript->CallEvent(_T("OnServiceTimer")))
			::PostMessage(m_tnd.hWnd, WM_CLOSE, 0, 0);
}

void CBoxService::OnCommand(long dwCtrlCode)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		if(pNowScript->CallCommand(dwCtrlCode))
			::PostMessage(m_tnd.hWnd, WM_CLOSE, 0, 0);
}

static BOOL InMessage95;

LRESULT CALLBACK CBoxService::StaticMainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return theApp.m_pService->MainWndProc(hwnd, message, wParam, lParam);
}

void CBoxService::AppendCommand(short uCtrl, VARIANT* pVarName)
{
	CString str;

	varGetString(pVarName, str);

	m_bUserMenu = TRUE;

	if(uCtrl < 0)
		InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, 0);
	else if(uCtrl == 0 || uCtrl > 0x1f)
	{
		InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, uCtrl, str);
		if(!uCtrl)SetMenuDefaultItem(m_hMenu, 0, 0);
	}
}

LRESULT CBoxService::MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int n;
	static int hTimer;
	HWND hShellWnd;

	switch(message)
	{
	case WM_CREATE:
		m_hMenu = GetSystemMenu(hwnd, FALSE);
		n = GetMenuItemCount(m_hMenu);
		while(n --  > 1)DeleteMenu(m_hMenu, 0, MF_BYPOSITION);

		m_tnd.hWnd = hwnd;

		m_bUserMenu = FALSE;
		OnStart();

		if(!m_bUserMenu)
		{
			InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING | MF_DISABLED | MF_GRAYED, SERVICE_CONTROL_CONTINUE, _T("Start Service"));
			InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SERVICE_CONTROL_PAUSE, _T("Pause Service"));
			InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, 0);
			InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SRV_RESTART, _T("Restart Service"));
		}
		InsertMenu(m_hMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, 0);

		hTimer = (int)::SetTimer(hwnd, 1, 50, NULL);
		m_hShellWnd = NULL;
		if(m_uElapse)
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&m_nTimer);
			m_nTimer = m_nTimer * 1000 / m_nFreq;
			m_nTimer += m_uElapse;
		}else m_nTimer = 0;

		PostMessage(hwnd, WM_TIMER, 0, 0);
		break;

	case WM_TIMER:
		if(m_bTrayIcon)
		{
			hShellWnd = FindWindow(_T("progman"), NULL);
			if(hShellWnd)
			{
				if(hShellWnd != m_hShellWnd)
				{
					m_hShellWnd = hShellWnd;
					Shell_NotifyIcon(NIM_ADD, &m_tnd);
				}
			}else m_hShellWnd = NULL;

			if(m_nMenuDelay)
			{
				__int64 n;

				QueryPerformanceCounter((LARGE_INTEGER*)&n);
				n = n * 1000 / m_nFreq;

				POINT pos;

				GetCursorPos(&pos);
				if(pos.x != m_mPos.x || pos.y != m_mPos.y || n > m_nMenuDelay)
				{
					m_nMenuDelay = 0;
					SetForegroundWindow(hwnd);
					PostMessage(hwnd, WM_POPUPMENU, 0, 0);
				}
			}
		}else if(m_hShellWnd)
		{
			m_hShellWnd = NULL;
			Shell_NotifyIcon(NIM_DELETE, &m_tnd);
		}

		if(m_ssStatus.dwCurrentState == SERVICE_RUNNING && m_uElapse != 0)
		{
			__int64 n;
			static BOOL s_IsInTimer = FALSE;

			QueryPerformanceCounter((LARGE_INTEGER*)&n);
			n = n * 1000 / m_nFreq;
			if(n > m_nTimer && !s_IsInTimer)
			{
				m_nTimer = n + m_uElapse;

				s_IsInTimer = TRUE;
				OnTimer();
				s_IsInTimer = FALSE;
			}
		}
		break;

	case WM_USER:
		switch(lParam)
		{
		case WM_LBUTTONUP:
			if(!m_bInBLCLK)
			{
				GetCursorPos(&m_mPos);
				QueryPerformanceCounter((LARGE_INTEGER*)&m_nMenuDelay);
				m_nMenuDelay = m_nMenuDelay * 1000 / m_nFreq;
				m_nMenuDelay += GetDoubleClickTime();
				PostMessage(hwnd, WM_TIMER, 0, 0);
			}else m_bInBLCLK = FALSE;
			break;
		case WM_MOUSEMOVE:
			if(m_nMenuDelay)
			{
				POINT pos;

				GetCursorPos(&pos);
				if(pos.x != m_mPos.x || pos.y != m_mPos.y)
				{
					m_nMenuDelay = 0;
					SetForegroundWindow(hwnd);
					PostMessage(hwnd, WM_POPUPMENU, 0, 0);
				}
			}
			break;
		case WM_RBUTTONUP:
			GetCursorPos(&m_mPos);
			m_nMenuDelay = 0;
			m_bInBLCLK = FALSE;
			SetForegroundWindow(hwnd);
			PostMessage(hwnd, WM_POPUPMENU, 0, 0);
			break;
		case WM_LBUTTONDBLCLK:
			m_bInBLCLK = TRUE;
			m_nMenuDelay = 0;
			PostMessage(hwnd, WM_COMMAND, 0, 0);
			break;
		}
		break;

	case WM_CHGICON:
		if(m_bTrayIcon)
			Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
		break;

	case WM_POPUPMENU:
		TrackPopupMenu(m_hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON,
				m_mPos.x, m_mPos.y, 0, hwnd, NULL);
		break;

	case WM_SYSCOMMAND:
	case WM_COMMAND:
		switch(wParam)
		{
		case SRV_RESTART:
			theApp.m_bRunSelfAtExit = TRUE;
		case SERVICE_CONTROL_STOP:
		case SC_CLOSE:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case SERVICE_CONTROL_CONTINUE:
			OnResume();
			break;
		case SERVICE_CONTROL_PAUSE:
			OnPause();
			break;
		default:
			OnCommand(wParam);
		}
		return DefWindowProc( hwnd, message, wParam, lParam);

	case WM_CLOSE:
		{
			CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

			if(pNowScript != NULL)
				pNowScript->CallEvent(_T("OnServiceClose"));
		}
				
		KillTimer(hwnd, hTimer);

		if(m_bTrayIcon)
			Shell_NotifyIcon(NIM_DELETE, &m_tnd);

		DestroyWindow(hwnd);

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc( hwnd, message, wParam, lParam);
	}
	return 0;
}

void CBoxService::Halt(void)
{
	if(m_bTrayIcon)
		Shell_NotifyIcon(NIM_DELETE, &m_tnd);

	if(m_sshStatusHandle)
		ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
	else theApp.PostThreadMessage(WM_QUIT, 0, 0);
}
