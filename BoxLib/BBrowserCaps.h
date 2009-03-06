#pragma once

struct tagBrowserCapsData
{
	LPCSTR UserAgent;
	LPCWSTR Name;
	LPCWSTR Type;
	LPCWSTR Version;
	LPCWSTR MajorVersion;
	LPCWSTR MinorVersion;
	LPCWSTR Platform;
	short CSSVersion;
	VARIANT_BOOL Frames;
	VARIANT_BOOL IFrames;
	VARIANT_BOOL Tables;
	VARIANT_BOOL Cookies;
	VARIANT_BOOL BackgroundSounds;
	VARIANT_BOOL VBScript;
	VARIANT_BOOL JavaScript;
	VARIANT_BOOL JavaApplets;
	VARIANT_BOOL ActiveXControls;
	VARIANT_BOOL CDF;
	VARIANT_BOOL AOL;
	VARIANT_BOOL Beta;
	VARIANT_BOOL Win16;
	VARIANT_BOOL Crawler;
	VARIANT_BOOL WAP;
	VARIANT_BOOL NetCLR;
};

class CBBrowserCaps : public CBDispatch<IBrowserCap>
{
public:
	CBBrowserCaps(LPCSTR pstrUserAgent);

public:
	class _Data : public CBObject
	{
	public:
		_Data(void) :
			Name(L"Unknown Browser"),
			Type(L"Unknown Browser"),
			Platform(L"Unknown"),
			CSSVersion(0),
			Frames(0),
			IFrames(0),
			Tables(VARIANT_TRUE),
			Cookies(0),
			BackgroundSounds(0),
			VBScript(0),
			JavaScript(0),
			JavaApplets(0),
			ActiveXControls(0),
			CDF(0),
			AOL(0),
			Beta(0),
			Win16(0),
			Crawler(0),
			WAP(0),
			NetCLR(0)
		{}

	public:
		CBStringA UserAgent;
		CBString Name;
		CBString Type;
		CBString Version;
		CBString MajorVersion;
		CBString MinorVersion;
		CBString Platform;
		short CSSVersion;
		VARIANT_BOOL Frames;
		VARIANT_BOOL IFrames;
		VARIANT_BOOL Tables;
		VARIANT_BOOL Cookies;
		VARIANT_BOOL BackgroundSounds;
		VARIANT_BOOL VBScript;
		VARIANT_BOOL JavaScript;
		VARIANT_BOOL JavaApplets;
		VARIANT_BOOL ActiveXControls;
		VARIANT_BOOL CDF;
		VARIANT_BOOL AOL;
		VARIANT_BOOL Beta;
		VARIANT_BOOL Win16;
		VARIANT_BOOL Crawler;
		VARIANT_BOOL WAP;
		VARIANT_BOOL NetCLR;
	};

public:
	static void Init(void);
	static void Clear(void);

public:
	STDMETHOD(get_ActiveXControls)(VARIANT_BOOL *pVal);
	STDMETHOD(get_AOL)(VARIANT_BOOL *pVal);
	STDMETHOD(get_BackgroundSounds)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Beta)(VARIANT_BOOL *pVal);
	STDMETHOD(get_CDF)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Cookies)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Crawler)(VARIANT_BOOL *pVal);
	STDMETHOD(get_CSSVersion)(short *pVal);
	STDMETHOD(get_Frames)(VARIANT_BOOL *pVal);
	STDMETHOD(get_IFrames)(VARIANT_BOOL *pVal);
	STDMETHOD(get_JavaApplets)(VARIANT_BOOL *pVal);
	STDMETHOD(get_JavaScript)(VARIANT_BOOL *pVal);
	STDMETHOD(get_MajorVersion)(BSTR *pVal);
	STDMETHOD(get_MinorVersion)(BSTR *pVal);
	STDMETHOD(get_Name)(BSTR *pVal);
	STDMETHOD(get_NetCLR)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Platform)(BSTR *pVal);
	STDMETHOD(get_Tables)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Type)(BSTR *pVal);
	STDMETHOD(get_VBScript)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Version)(BSTR *pVal);
	STDMETHOD(get_WAP)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Win16)(VARIANT_BOOL *pVal);
	STDMETHOD(get_UserAgent)(BSTR *pVal);

private:
	CBStringA m_strUserAgent;
	CBComPtr<_Data> m_pData;
};
