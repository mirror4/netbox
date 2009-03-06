#pragma once

class CBoxCommand
{
public:
	void Start(void);

private:
	static BOOL WINAPI CBoxCommand::CtrlHandler(DWORD fdwCtrlType);
	CBComPtr<CBConsole> m_pConsole;
	CString m_strBasePath;
};
