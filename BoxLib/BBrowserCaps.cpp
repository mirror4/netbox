#include "StdAfx.h"
#include "BBrowserCaps.h"
#include "BFile.h"

static CRBMap< CBStringA, CBComPtr<CBBrowserCaps::_Data> > s_bcDatas;
static CAtlArray< CBComPtr<CBBrowserCaps::_Data> > s_bcDatasExt;

static bool MatchString(const char* szPattern, const char* szInput)
{
	while (*szPattern && *szInput &&
		((*szPattern == *szInput && *szPattern != '#' && *szPattern != '*') ||
		*szPattern == '?' ||
		(*szPattern == '#' && *szInput >= '0' && *szInput <= '9')))
	{
		szPattern++;
		szInput++;
	}

	if (*szPattern == *szInput && *szPattern != '#' && *szPattern != '*')
	{
		return true;
	}

	if (*szPattern == '*')
	{
		szPattern++;
		if (!*szPattern)
		{
			return true;
		}
		while(*szInput)
		{
			if (MatchString(szPattern, szInput))
			{
				return true;
			}

			szInput++;
		}
	}

	return false;
}

static int staticCompare(CBComPtr<CBBrowserCaps::_Data>* pvar1, CBComPtr<CBBrowserCaps::_Data>* pvar2)
{
	return (*pvar2)->UserAgent.GetLength() - (*pvar1)->UserAgent.GetLength();
}

static BOOL s_bInited;
static CBCriticalSection s_csBrowserCaps;

void CBBrowserCaps::Init(void)
{
	if(s_bInited)return;

	s_csBrowserCaps.Enter();
	if(s_bInited)
	{
		s_csBrowserCaps.Leave();
		return;
	}

	CBComPtr<_Data> pData;
	int n;
	BOOL bFind = FALSE;

	pData.CreateInstance();

	CBComPtr<CBFile> pFile;
	CBStringA strLine;

	pFile.CreateInstance();
	if(SUCCEEDED(pFile->Open(L"\\browscap.ini")))
	{
		while(SUCCEEDED(pFile->ReadLine(strLine)))
		{
			strLine.Trim();
			if(!strLine.IsEmpty())
				if(strLine[0] == '[' && strLine[strLine.GetLength() - 1] == ']')
				{
					pData.CreateInstance();
					pData->UserAgent = strLine.Mid(1, strLine.GetLength() - 2);

					if(pData->UserAgent.Find('*') == -1 && pData->UserAgent.Find('?') == -1)
						s_bcDatas.SetAt(pData->UserAgent, pData);
					else s_bcDatasExt.Add(pData);

					if(!pData->UserAgent.Compare("*"))
						bFind = TRUE;
				}else if(strLine[0] != ';')
				{
					n = strLine.Find('=');
					if(n != -1)
					{
						CBStringA strName, strValue;

						strName = strLine.Left(n).Trim();
						if(!strName.IsEmpty())
						{
							strValue = strLine.Mid(n + 1).Trim();
							if(!strName.CompareNoCase("parent"))
							{
								CBComPtr<_Data> pParentData;
								
								if(s_bcDatas.Lookup(strValue, pParentData))
								{
									pData->Name = pParentData->Name;
									pData->Type = pParentData->Type;
									pData->Version = pParentData->Version;
									pData->MajorVersion = pParentData->MajorVersion;
									pData->MinorVersion = pParentData->MinorVersion;
									pData->Platform = pParentData->Platform;
									pData->CSSVersion = pParentData->CSSVersion;
									pData->Frames = pParentData->Frames;
									pData->IFrames = pParentData->IFrames;
									pData->Tables = pParentData->Tables;
									pData->Cookies = pParentData->Cookies;
									pData->BackgroundSounds = pParentData->BackgroundSounds;
									pData->VBScript = pParentData->VBScript;
									pData->JavaScript = pParentData->JavaScript;
									pData->JavaApplets = pParentData->JavaApplets;
									pData->ActiveXControls = pParentData->ActiveXControls;
									pData->CDF = pParentData->CDF;
									pData->AOL = pParentData->AOL;
									pData->Beta = pParentData->Beta;
									pData->Win16 = pParentData->Win16;
									pData->Crawler = pParentData->Crawler;
									pData->WAP = pParentData->WAP;
									pData->NetCLR = pParentData->NetCLR;
								}
							}
							else if(!strName.CompareNoCase("browser"))
							{
								pData->Name = strValue;

								pData->Type = pData->Name;
								if(!pData->MajorVersion.IsEmpty() && pData->MajorVersion.Compare(L"0"))
								{
									pData->Type += ' ';
									pData->Type += pData->MajorVersion;
								}
							}
							else if(!strName.CompareNoCase("Version"))
								pData->Version = strValue;
							else if(!strName.CompareNoCase("MajorVer"))
							{
								pData->MajorVersion = strValue;

								pData->Type = pData->Name;
								if(!pData->MajorVersion.IsEmpty() && pData->MajorVersion.Compare(L"0"))
								{
									pData->Type += ' ';
									pData->Type += pData->MajorVersion;
								}
							}
							else if(!strName.CompareNoCase("MinorVer"))
								pData->MinorVersion = strValue;
							else if(!strName.CompareNoCase("Platform"))
								pData->Platform = strValue;
							else if(!strName.CompareNoCase("CSS"))
								pData->CSSVersion = atoi(strValue);
							else if(!strName.CompareNoCase("Frames"))
								pData->Frames = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("IFrames"))
								pData->IFrames = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("Tables"))
								pData->Tables = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("Cookies"))
								pData->Cookies = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("BackgroundSounds"))
								pData->BackgroundSounds = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("VBScript"))
								pData->VBScript = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("JavaScript"))
								pData->JavaScript = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("JavaApplets"))
								pData->JavaApplets = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("ActiveXControls"))
								pData->ActiveXControls = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("CDF"))
								pData->CDF = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("AOL"))
								pData->AOL = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("Beta"))
								pData->Beta = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("Win16"))
								pData->Win16 = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("Crawler"))
								pData->Crawler = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("WAP"))
								pData->WAP = !strValue.CompareNoCase("true");
							else if(!strName.CompareNoCase("NetCLR"))
								pData->NetCLR = !strValue.CompareNoCase("true");
						}
					}
				}
		}
	}

	if(!bFind)
	{
		pData.CreateInstance();
		pData->UserAgent = "*";
		s_bcDatasExt.Add(pData);
	}

	qsort(s_bcDatasExt.GetData(), s_bcDatasExt.GetCount(), sizeof(CBComPtr<_Data>),
		(int (__cdecl *)(const void *,const void *))staticCompare);

	s_bInited = TRUE;
	s_csBrowserCaps.Leave();
}

void CBBrowserCaps::Clear(void)
{
	s_bcDatas.RemoveAll();
	s_bcDatasExt.RemoveAll();
}

CBBrowserCaps::CBBrowserCaps(LPCSTR pstrUserAgent) : m_strUserAgent(pstrUserAgent)
{
	Init();

	if(s_bcDatas.Lookup(m_strUserAgent, m_pData))
		return;

	size_t i;

	for(i = 0; i < s_bcDatasExt.GetCount(); i ++)
		if (MatchString((LPCSTR)s_bcDatasExt[i]->UserAgent, pstrUserAgent))
		{
			m_pData = s_bcDatasExt[i];
			break;
		}
}

STDMETHODIMP CBBrowserCaps::get_ActiveXControls(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->ActiveXControls;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_AOL(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->AOL;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_BackgroundSounds(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->BackgroundSounds;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Beta(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->Beta;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_CDF(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->CDF;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_NetCLR(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->NetCLR;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Cookies(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->Cookies;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Crawler(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->Crawler;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_CSSVersion(short *pVal)
{
	*pVal = m_pData->CSSVersion;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Frames(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->Frames;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_IFrames(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->IFrames;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_JavaApplets(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->JavaApplets;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_JavaScript(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->JavaScript;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_MajorVersion(BSTR *pVal)
{
	*pVal = m_pData->MajorVersion.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_MinorVersion(BSTR *pVal)
{
	*pVal = m_pData->MinorVersion.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Name(BSTR *pVal)
{
	*pVal = m_pData->Name.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Platform(BSTR *pVal)
{
	*pVal = m_pData->Platform.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Tables(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->Tables;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Type(BSTR *pVal)
{
	*pVal = m_pData->Type.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_VBScript(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->VBScript;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Version(BSTR *pVal)
{
	*pVal = m_pData->Version.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_WAP(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->WAP;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_Win16(VARIANT_BOOL *pVal)
{
	*pVal = m_pData->Win16;
	return S_OK;
}

STDMETHODIMP CBBrowserCaps::get_UserAgent(BSTR *pVal)
{
	*pVal = m_strUserAgent.AllocSysString();
	return S_OK;
}

