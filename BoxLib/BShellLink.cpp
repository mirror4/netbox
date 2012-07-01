#include "StdAfx.h"
#include "BShellLink.h"

CBShellLink::CBShellLink()
{
	m_sl.CoCreateInstance(CLSID_ShellLink);
}

STDMETHODIMP CBShellLink::get_Arguments(BSTR *pVal)
{
	CBStringA str;
	HRESULT hr = m_sl->GetArguments(str.GetBufferSetLength(INFOTIPSIZE), INFOTIPSIZE);
	if (SUCCEEDED(hr))
	{
		str.Truncate(lstrlenA(str));
		*pVal = str.AllocSysString();
	}
	return hr;
}

STDMETHODIMP CBShellLink::put_Arguments(BSTR Val)
{
	CBStringA str(Val);
	return m_sl->SetArguments(str);
}

STDMETHODIMP CBShellLink::get_Description(BSTR *pVal)
{
	CBStringA str;
	HRESULT hr = m_sl->GetDescription(str.GetBufferSetLength(INFOTIPSIZE), INFOTIPSIZE);
	if (SUCCEEDED(hr))
	{
		str.Truncate(lstrlenA(str));
		*pVal = str.AllocSysString();
	}
	return hr;
}

STDMETHODIMP CBShellLink::put_Description(BSTR Val)
{
	CBStringA str(Val);
	return m_sl->SetDescription(str);
}

STDMETHODIMP CBShellLink::get_Hotkey(int *pVal)
{
	WORD wHotKey;
	HRESULT hr = m_sl->GetHotkey(&wHotKey);
	if (SUCCEEDED(hr))
		*pVal = (int)wHotKey;
	return hr;
}

STDMETHODIMP CBShellLink::put_Hotkey(int Val)
{
	WORD wHotKey = Val;
	return m_sl->SetHotkey(wHotKey);
}

STDMETHODIMP CBShellLink::get_IconLocation(BSTR *pVal)
{
	CBStringA str;
	int iIcon;
	HRESULT hr = m_sl->GetIconLocation(str.GetBufferSetLength(MAX_PATH), MAX_PATH, &iIcon);
	if (SUCCEEDED(hr))
	{
		str.Truncate(lstrlenA(str));
		str.AppendFormat(",%d", iIcon);
		*pVal = str.AllocSysString();
	}
	return hr;
}

STDMETHODIMP CBShellLink::put_IconLocation(BSTR Val)
{
	CBStringA str(Val);
	int iIcon = 0, p = str.Find(',');
	if (p >= 0)
	{
		iIcon = atoi(str.Mid(p+1));
		str.Truncate(p);
	}
	return m_sl->SetIconLocation(str, iIcon);
}

STDMETHODIMP CBShellLink::get_IDList(BSTR *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBShellLink::put_IDList(BSTR Val)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBShellLink::get_Path(BSTR *pVal)
{
	CBStringA str;
	HRESULT hr = m_sl->GetPath(str.GetBufferSetLength(MAX_PATH), MAX_PATH, NULL, SLGP_UNCPRIORITY);
	if (SUCCEEDED(hr))
	{
		str.Truncate(lstrlenA(str));
		*pVal = str.AllocSysString();
	}
	return hr;
}

STDMETHODIMP CBShellLink::put_Path(BSTR Val)
{
	CBStringA str(Val);
	return m_sl->SetPath(str);
}

STDMETHODIMP CBShellLink::get_ShowCmd(int *pVal)
{
	return m_sl->GetShowCmd(pVal);
}

STDMETHODIMP CBShellLink::put_ShowCmd(int Val)
{
	return m_sl->SetShowCmd(Val);
}

STDMETHODIMP CBShellLink::get_WorkingDirectory(BSTR *pVal)
{
	CBStringA str;
	HRESULT hr = m_sl->GetWorkingDirectory(str.GetBufferSetLength(MAX_PATH), MAX_PATH);
	if (SUCCEEDED(hr))
	{
		str.Truncate(lstrlenA(str));
		*pVal = str.AllocSysString();
	}
	return hr;
}

STDMETHODIMP CBShellLink::put_WorkingDirectory(BSTR Val)
{
	CBStringA str(Val);
	return m_sl->SetWorkingDirectory(str);
}

STDMETHODIMP CBShellLink::Load(BSTR strFileName)
{
	CComQIPtr<IPersistFile> ppf(m_sl);
	if (ppf)
	{
		HRESULT hr = ppf->Load(strFileName, STGM_READ);
		if (FAILED(hr))
			return hr;
		return hr;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP CBShellLink::Save(BSTR strFileName)
{
	CComQIPtr<IPersistFile> ppf(m_sl);
	if (ppf)
		return ppf->Save(strFileName, TRUE);
	return E_NOINTERFACE;
}

STDMETHODIMP CBShellLink::Resolve(int hWnd, int dwFlag)
{
	return m_sl->Resolve((HWND)hWnd, (DWORD)dwFlag);
}

