#pragma once


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

	void SetHandle(HANDLE hProcess);
private:
	HANDLE m_hProcess;
};
