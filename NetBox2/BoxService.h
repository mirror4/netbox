
#pragma once

#include <winsvc.h>


// CBoxService command target

class CBoxService : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxService)
public:
	CBoxService();
	~CBoxService();

public:
	void Main();
	static void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);
	static void WINAPI service_ctrl(DWORD dwCtrlCode);
	void Control(DWORD dwCtrlCode);
	void Dispatch(LPCTSTR pstrName);

	void Halt(void);

	CString m_strName;

public:
	afx_msg BOOL get_Installed(void);
	afx_msg BOOL get_Running(void);

	afx_msg void Install(void);
	afx_msg void Remove(void);
	afx_msg void Start(void);
	afx_msg void Restart(void);
	afx_msg void Stop(void);
	afx_msg void LogEvent(long nType, LPCTSTR pstrMsg);
	afx_msg void SetTimer(long uElapse);
	afx_msg BSTR get_Icon(void);
	afx_msg void put_Icon(LPCTSTR pstrName);
	afx_msg void DoEvents(void);
	afx_msg void RunService(LPCTSTR pstrName, VARIANT* pVarDisplay, VARIANT* pVarDescription);
	afx_msg void SendCommand(short uCtrl);
	afx_msg void AppendCommand(short uCtrl, VARIANT* pVarName);

	DECLARE_DISPATCH_MAP()

private:
	void ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
	static LRESULT CALLBACK StaticMainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnStart(void);
	void OnStop(void);
	void OnPause(void);
	void OnResume(void);
	void OnTimer(void);
	void OnCommand(long dwCtrlCode);

	CString m_strIcon;
	NOTIFYICONDATA m_tnd;
	SERVICE_STATUS_HANDLE m_sshStatusHandle;
	SERVICE_STATUS m_ssStatus;
	DWORD m_dwError;
	BOOL m_bCheckReg;
	BOOL m_bDesktop;
	BOOL m_bTrayIcon;
	BOOL m_nAutoRestart;
	CString m_strDisplayName;
	CString m_strDescription;
	DWORD m_uElapse;
	HWND m_hShellWnd;
	HMENU m_hMenu;
	BOOL m_bUserMenu;
	long m_bServiceType;
	POINT m_mPos;
	__int64 m_nMenuDelay;
	BOOL m_bInBLCLK;
	__int64 m_nTimer;
	__int64 m_nFreq;
};
