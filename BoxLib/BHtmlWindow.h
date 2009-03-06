#pragma once

#include "BWindow.h"

class __declspec(uuid("94650000-0000-0000-4874-6D6C466F726D"))
CBHtmlWindow : public CBDispatch<IHTWindow, CBWindow>,
			 public IDispEventSimpleImpl<0, CBHtmlWindow, &DIID_DWebBrowserEvents2>
{
	DECLARE_CLASS_EX(CBHtmlWindow, CBWindow)

public:
	CBHtmlWindow(void);

public:
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_SINK_MAP(CBHtmlWindow)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 102, __StatusTextChange, &StatusTextChangeInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 108, __ProgressChange, &ProgressChangeInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 105, __CommandStateChange, &CommandStateChangeInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 106, __DownloadBegin, &DownloadBeginInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 104, __DownloadComplete, &DownloadBeginInfo)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 113, OnTitleChange, &StatusTextChangeInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 252, __NavigateComplete2, &NavigateComlete2Info)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 250, __BeforeNavigate2, &BeforeNavigate2Info)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 112, __PropertyChange, &StatusTextChangeInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 251, __NewWindow2, &NewWindow2Info)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 259, __DocumentComplete, &NavigateComlete2Info)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 253, __OnQuit, &DownloadBeginInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 254, __OnVisible, &OnVisibleInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 255, __OnToolBar, &OnVisibleInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 256, __OnMenuBar, &OnVisibleInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 257, __OnStatusBar, &OnVisibleInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 258, __OnFullScreen, &OnVisibleInfo)
//		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, 260, __OnTheaterMode, &OnVisibleInfo)
	END_SINK_MAP()

	void __stdcall OnTitleChange(BSTR bstrText)
	{
		if(m_strTitle.IsEmpty())
			SetWindowText(CBStringA(bstrText));
	}

public:
	//	IHtmlForm
	STDMETHOD(get_ContextMenu)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ContextMenu)(VARIANT_BOOL newVal);
	STDMETHOD(get_Argument)(VARIANT *pVal);
	STDMETHOD(get_Browser)(IDispatch** pVal);

	STDMETHOD(Open)(BSTR strURL, VARIANT varArg = VARIANT_ERROR);

	STDMETHOD(OpenModalDialog)(BSTR strUrl, short Width, short Height, VARIANT varArg, VARIANT *pVal);

private:
	CComVariant m_varArgument;
	CComPtr<IWebBrowser2> m_spBrowser;

private:
	static _ATL_FUNC_INFO StatusTextChangeInfo;
	static _ATL_FUNC_INFO DownloadBeginInfo ;
	static _ATL_FUNC_INFO NewWindow2Info; 
	static _ATL_FUNC_INFO CommandStateChangeInfo;
	static _ATL_FUNC_INFO BeforeNavigate2Info;
	static _ATL_FUNC_INFO ProgressChangeInfo;
	static _ATL_FUNC_INFO NavigateComlete2Info;
	static _ATL_FUNC_INFO OnVisibleInfo;
};
