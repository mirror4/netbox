#pragma once

#include <shobjidl.h>
#include <shlguid.h>


class __declspec(uuid("94650000-0000-0053-6865-6C6C4C696E6B"))
CBShellLink : public CBDispatch<IBShellLink>
{
	DECLARE_CLASS(CBShellLink)

public:
	CBShellLink();

public:
	STDMETHOD(get_Arguments)(BSTR *pVal);
	STDMETHOD(put_Arguments)(BSTR Val);
	STDMETHOD(get_Description)(BSTR *pVal);
	STDMETHOD(put_Description)(BSTR Val);
	STDMETHOD(get_Hotkey)(int *pVal);
	STDMETHOD(put_Hotkey)(int Val);
	STDMETHOD(get_IconLocation)(BSTR *pVal);
	STDMETHOD(put_IconLocation)(BSTR Val);
	STDMETHOD(get_IDList)(BSTR *pVal);
	STDMETHOD(put_IDList)(BSTR Val);
	STDMETHOD(get_Path)(BSTR *pVal);
	STDMETHOD(put_Path)(BSTR Val);
	STDMETHOD(get_ShowCmd)(int *pVal);
	STDMETHOD(put_ShowCmd)(int Val);
	STDMETHOD(get_WorkingDirectory)(BSTR *pVal);
	STDMETHOD(put_WorkingDirectory)(BSTR Val);

	STDMETHOD(Load)(BSTR strFileName);
	STDMETHOD(Save)(BSTR strFileName);
	STDMETHOD(Resolve)(int hWnd, int dwFlag);

private:
	CComPtr<IShellLink> m_sl;
};

