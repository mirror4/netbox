// BoxScript.cpp : implementation file
//

#include "stdafx.h"
#include "BoxScript.h"
#include "BoxFile.h"
#include "BoxPath.h"
#include "BoxScriptError.h"

// CBoxScript

IMPLEMENT_DYNCREATE(CBoxScript, CBoxSafeObject)

CBoxScript::CBoxScript() :
	m_bEnableDebug(FALSE),
	m_bStepDebug(FALSE),
	m_nTransaction(0),
	m_nDiskFileCount(0),
	m_nCacheFileCount(0),
	m_nScriptLine(0)
{
	m_pHost = new CScriptHost();
	m_strLanguage = _T("VBScript");
	m_dwThreadId = GetCurrentThreadId();
}

CBoxScript::~CBoxScript()
{
	ClearScript();
}

BEGIN_DISPATCH_MAP(CBoxScript, CBoxSafeObject)
	DISP_PROPERTY_EX(CBoxScript, "Source", GetNotSupported, put_Source, VT_BSTR)
	DISP_PROPERTY(CBoxScript, "Language", m_strLanguage, VT_BSTR)
	DISP_PROPERTY(CBoxScript, "EnableDebug", m_bEnableDebug, VT_BOOL)
	DISP_PROPERTY(CBoxScript, "StepDebug", m_bStepDebug, VT_BOOL)
	DISP_PROPERTY(CBoxScript, "Transaction", m_nTransaction, VT_I4)
	DISP_PROPERTY_EX(CBoxScript, "Timeout", get_Timeout, put_Timeout, VT_I4)

	DISP_FUNCTION(CBoxScript, "Load", Load, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CBoxScript, "AddNameItem", AddNameItem, VT_EMPTY, VTS_BSTR VTS_DISPATCH)
	DISP_FUNCTION(CBoxScript, "Run", Run, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxScript, "Close", Close, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxScript, "SetGlobalObjects", SetGlobalObjects, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION(CBoxScript, "GetLastError", GetLastErrorDISP, VT_DISPATCH, VTS_NONE)
END_DISPATCH_MAP()

void CBoxScript::clearCache(void)
{
//	m_csCacheScript.Lock();
//	g_cacheScript.RemoveAll();
//	m_csCacheScript.Unlock();
}

// CBoxScript message handlers

int CBoxScript::ParseScriptText(LPCSTR pstrText, int nCount, CStringA& strScriptText, int nIncludeFlagIndex)
{
	CStringA strTempText;
	if(nCount >= 2 && (BYTE)pstrText[0] == 0xff && (BYTE)pstrText[1] == 0xfe)
	{
		int _nTempCount = WideCharToMultiByte(_AtlGetConversionACP(), 0, LPWSTR(pstrText + 2), (nCount - 2) / 2, NULL, 0, NULL, NULL);
		LPSTR _pstr = strTempText.GetBuffer(_nTempCount);

		WideCharToMultiByte(_AtlGetConversionACP(), 0, LPWSTR(pstrText + 2), (nCount - 2) / 2, _pstr, _nTempCount, NULL, NULL);
		strTempText.ReleaseBuffer(_nTempCount);

		pstrText = strTempText;
		nCount = strTempText.GetLength();
	}

	static struct
	{
		char *pstrName;
		int nSize;
	}CmdName[] =
	{
		{"#include", 8},
		{"#language", 9},
		{"#debug", 6},
		{"#timeout", 8},
		{"#transaction", 12}
	};
	#define CMD_COUNT (sizeof(CmdName) / sizeof(CmdName[0]))
	int i;
	LPCSTR pstrTemp, pstrTemp1;
	int nTempCount;
	int nLineCount = 1;

	while(nCount > 0 && IsBlank(pstrText[0]))
	{
		if(pstrText[0] == '\n')
			nLineCount ++;

		pstrText ++;
		nCount --;
	}

	while(nCount > 0 && pstrText[0] == '#')
	{
		for(i = 0; i < CMD_COUNT; i ++)
			if(nCount > CmdName[i].nSize &&
				!_strnicmp(pstrText, CmdName[i].pstrName, CmdName[i].nSize) &&
				IsBlankChar(pstrText[CmdName[i].nSize]))
				break;

		if(i == CMD_COUNT)
			break;

		pstrText += CmdName[i].nSize;
		nCount -= CmdName[i].nSize;

		while(nCount > 0 && IsBlankChar(pstrText[0]))
		{
			pstrText ++;
			nCount --;
		}

		if(nCount > 0 && pstrText[0] == '\"')
		{
			pstrText ++;
			nCount --;
		}

		pstrTemp = pstrText;
		nTempCount = nCount;
		while(nTempCount > 0 && !IsLineChar(pstrTemp[0]))
		{
			pstrTemp ++;
			nTempCount --;
		}

		pstrTemp1 = pstrTemp;
		while(pstrTemp1 > pstrText && IsBlankChar(pstrTemp1[0]))
			pstrTemp1 --;

		if(pstrTemp1 > pstrText && pstrTemp1[-1] == '\"')
			pstrTemp1 --;

		CStringA strValue;

		strValue.SetString(pstrText, (int)(pstrTemp1 - pstrText));

		pstrText = pstrTemp;
		nCount = nTempCount;

		switch(i)
		{
		case 0:
			strValue.MakeLower();
			if(LoadScriptFile(BOX_CA2CT(strValue), strScriptText, nLineCount))
				return 500;
			break;
		case 1:
			m_strLanguage = strValue;
			break;
		case 2:
			m_bEnableDebug = !strValue.CompareNoCase("on") || !strValue.CompareNoCase("true");
			if(!strValue.CompareNoCase("step"))
				m_bStepDebug = TRUE;
			break;
		case 3:
			m_pHost->m_nTimeout = atoi(strValue);
			break;
		case 4:
			if(!strValue.CompareNoCase("Required"))
                m_nTransaction = 3;
			else if(!strValue.CompareNoCase("Requires_New"))
				m_nTransaction = 2;
			else if(!strValue.CompareNoCase("Supported"))
				m_nTransaction = 1;
			else if(!strValue.CompareNoCase("Not_Supported"))
				m_nTransaction = 0;
		}

		while(nCount > 0 && IsBlank(pstrText[0]))
		{
			if(pstrText[0] == '\n')
				nLineCount ++;

			pstrText ++;
			nCount --;
		}
	}

	AddLineMap(nIncludeFlagIndex, nLineCount);

	strScriptText.Append(pstrText, nCount);
	strScriptText += _T("\r\n");

	m_nScriptLine ++;

	pstrTemp = pstrText;
	nTempCount = nCount;
	while(nTempCount > 0)
	{
		if(pstrTemp[0] == '\n')
			m_nScriptLine ++;

		pstrTemp ++;
		nTempCount --;
	}

	return 0;
}

void CBoxScript::put_Timeout(long nTimeout)
{
	m_pHost->m_nTimeout = nTimeout;
}

long CBoxScript::get_Timeout()
{
	return m_pHost->m_nTimeout;
}

int CBoxScript::LoadScriptFile(LPCTSTR pstrFile, CStringA& strScriptText, int nLineNo)
{
	CFile* pFile;
	CString strOldPath;
	CBoxPath path;
	int nIncludeFlagIndex;

	if(!m_strBasePath.IsEmpty() && pstrFile[0] != _T('\\'))
		path.Combine(m_strBasePath.Left(m_strBasePath.ReverseFind(_T('\\')) + 1), pstrFile);
	else path.Combine(pstrFile);

	for(nIncludeFlagIndex = 0; nIncludeFlagIndex < m_pHost->m_arrayFile.GetCount(); nIncludeFlagIndex ++)
	{
		if(!m_pHost->m_arrayFile[nIncludeFlagIndex].CompareNoCase(path.m_strPath))
		{
			if(m_arrayIncludeFlags[nIncludeFlagIndex] != 0)
			{
				SetParserError(nLineNo, m_strBasePath, CString("Cyclic Include \"") + path.m_strPath + _T("\""));
				return 500;
			}

			m_arrayIncludeFlags[nIncludeFlagIndex] = 1;
			break;
		}
	}

	if(nIncludeFlagIndex == m_pHost->m_arrayFile.GetCount())
	{
		m_pHost->m_arrayFile.Add(path.m_strPath);
		m_arrayIncludeFlags.Add(1);
	}

	strOldPath = m_strBasePath;
	m_strBasePath = path.m_strPath;

	if((pFile = g_pFile->Open(m_strBasePath)) && pFile != BOX_FOLDER)
	{
		int nSize;
		char* strBuffer;
		int iResult = 0;

		nSize = (int)pFile->GetLength();

		if(pFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
		{
			m_nCacheFileCount ++;
			if(nSize)
			{
				char *bufptr, *bufptr1;

				pFile->GetBufferPtr(CFile::bufferRead, 0, (void**)&bufptr, (void**)&bufptr1);

				iResult = ParseScriptText(bufptr, nSize, strScriptText, nIncludeFlagIndex);
			}
		}else
		{
			m_nDiskFileCount ++;
			if(nSize)
			{
				strBuffer = new char[nSize];
				if(strBuffer == NULL)
				{
					delete pFile;
					m_arrayIncludeFlags[nIncludeFlagIndex] = 0;
					m_strBasePath = strOldPath;
					return 500;
				}

				pFile->Read(strBuffer, nSize);
				iResult = ParseScriptText(strBuffer, nSize, strScriptText, nIncludeFlagIndex);
				delete strBuffer;
			}
		}
		delete pFile;

		if(iResult != 0)
		{
			m_arrayIncludeFlags[nIncludeFlagIndex] = 0;
			m_strBasePath = strOldPath;
			return iResult;
		}

		m_pHost->m_strScriptName = m_strBasePath;

		m_arrayIncludeFlags[nIncludeFlagIndex] = 0;
		m_strBasePath = strOldPath;
		return 0;
	}

	if(!strOldPath.IsEmpty())
		SetParserError(nLineNo, strOldPath, CString("Include File \"") + m_strBasePath + _T("\" not found."));

	m_arrayIncludeFlags[nIncludeFlagIndex] = 0;
	m_strBasePath = strOldPath;
	return 404;
}

void CBoxScript::GetScriptErrorInfo(void)
{
	m_error.m_strSource = m_pHost->m_strErrorSource;
	m_error.m_sCode = m_pHost->m_scode;
	m_error.m_strDescription = m_pHost->m_strErrorMessage;
	m_error.m_nLine = m_pHost->m_nErrorLine;
	m_error.m_strFile = m_pHost->m_strErrorFile;
}

void CBoxScript::InitScript(void)
{
}

int CBoxScript::AfterParse(void)
{
	return 0;
}

long CBoxScript::Load(LPCTSTR pstrFile)
{
	m_error.Clear();

	m_strBasePath.Empty();
	m_arrayIncludeFlags.RemoveAll();

	m_nDiskFileCount = 0;
	m_nCacheFileCount = 0;
	m_nScriptLine = 0;

	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		m_strBasePath = pNowScript->m_strScriptName;

	CBoxPath path;

	if(!m_strBasePath.IsEmpty() && pstrFile[0] != _T('\\'))
		path.Combine(m_strBasePath.Left(m_strBasePath.ReverseFind(_T('\\')) + 1), pstrFile);
	else path.Combine(pstrFile);

	InitScript();

	CStringA strScriptText;
	int iResult = LoadScriptFile(path.m_strPath, strScriptText);
	m_strBasePath.Empty();

	if(iResult != 0)
		return iResult;

	if(m_nCacheFileCount)
	{
		m_bStepDebug = FALSE;
		m_bEnableDebug = FALSE;
	}

	m_pHost->SetFileName(path.m_strPath);

	if(!m_pHost->Create(m_strLanguage, m_bEnableDebug, m_bStepDebug, m_nTransaction))
	{
		GetScriptErrorInfo();
		return 500;
	}

	m_pHost->SetScriptSite();

	if(m_pHost->Load(BOX_CA2CT(strScriptText)) != S_OK)
	{
		GetScriptErrorInfo();
		return 500;
	}

	if(AfterParse() != 0)
		return 500;

	return 0;
}

long CBoxScript::put_Source(LPCTSTR pstrScript)
{
	m_error.Clear();

	m_strBasePath.Empty();
	m_arrayIncludeFlags.RemoveAll();

	m_nDiskFileCount = 0;
	m_nCacheFileCount = 0;
	m_nScriptLine = 0;

	if(!m_pHost->Create(m_strLanguage, m_bEnableDebug, m_bStepDebug, m_nTransaction))
		return 500;

	m_pHost->SetScriptSite();

	if(m_pHost->Load(pstrScript) != S_OK)
	{
		GetScriptErrorInfo();
		return 500;
	}

	return 0;
}

void CBoxScript::AddNameItem(LPCTSTR pstrName, LPDISPATCH pUnk)
{
	m_pHost->AddNameItem(pstrName, pUnk);
}

long CBoxScript::Run(void)
{
	int iResult = m_pHost->Run();
	if(iResult == 0)
		iResult = m_pHost->Stop(0);
	else m_pHost->Stop(0);

	if(iResult != 0)
		GetScriptErrorInfo();

	return iResult;
}

void CBoxScript::ClearScript(void)
{
	m_pHost->Close();

	m_pHost.Release();

	m_error.Clear();

	m_strBasePath.Empty();
	m_arrayIncludeFlags.RemoveAll();

	m_bEnableDebug = FALSE;
	m_bStepDebug = FALSE;
	m_nTransaction = 0;
	m_nDiskFileCount = 0;
	m_nCacheFileCount = 0;
	m_nScriptLine = 0;
}

void CBoxScript::Close(void)
{
	ClearScript();
	m_pHost = new CScriptHost();
}

void CBoxScript::SetGlobalObjects(void)
{
	m_pHost->CloneParentNameItem();
}

LPDISPATCH CBoxScript::GetLastErrorDISP(void)
{
	CBoxObject<CBoxScriptError> pScriptError;

	pScriptError.CreateInstance();

	int nOldLine = m_error.m_nLine;

	if(nOldLine == 0)
		GetScriptErrorInfo();

	pScriptError->Bind(m_error);

	if(nOldLine == 0)
		m_error.Clear();

	pScriptError->ExternalAddRef();
	return pScriptError;
}

