#pragma once
#include "BPipe.h"

class __declspec(uuid("94650000-0000-0000-0050-726F63657373"))

CBProcess : public CBDispatch<IProcess>
{
	DECLARE_CLASS(CBProcess)

public:
	CBProcess(void);
	~CBProcess(void);

public:
	STDMETHOD(get_ExitCode)(LONG *newVal);
	STDMETHOD(get_ProcessID)(LONG *newVal);
	STDMETHOD(get_Status)(LONG *newVal);
	STDMETHOD(get_FileName)(BSTR *newVal);

	STDMETHOD(Terminate)();

	STDMETHOD(get_StdIn)(IPipe** ppStream);
	STDMETHOD(get_StdOut)(IPipe** ppStream);
	STDMETHOD(get_StdErr)(IPipe** ppStream);

	void SetHandle(HANDLE hProcess, HANDLE hStdIn = NULL, HANDLE hStdOut = NULL, HANDLE hStdErr = NULL);
private:
	CHandle m_hProcess;
	
	CBComPtr<CBPipe> m_pPipe_StdIn;
	CBComPtr<CBPipe> m_pPipe_StdOut;
	CBComPtr<CBPipe> m_pPipe_StdErr;
};
