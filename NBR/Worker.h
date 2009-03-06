#pragma once

#include "NBRDlg.h"
#include <openssl\md5.h>
#include <openssl\rand.h>

class CLogDialog;

// CWorker

class CWorker : public CWinThread
{
public:
	CWorker();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void StartWorker(CLogDialog * pLog);

private:
	BOOL StartBuild();
	BOOL PackFile();
	BOOL PackOneFile(CString strFileName);
	BOOL EndBuild();
	BOOL DoBuild();
	BOOL LogLastError(LPCTSTR pstrFile);

	CStringW m_strRootPath;
	CString m_strLogString;
	int m_logCount;

private:
	CLogDialog * m_pwndLog;
	CNBRDlg * m_pwndMain;

	CFile m_file;
	CMemFile m_IndexFile;
	DWORD m_dwPackInfo[3];
	BYTE m_MD5Pass[MD5_DIGEST_LENGTH];
};


