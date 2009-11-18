// BoxHttpAccept.cpp : implementation file
//

#include "stdafx.h"
#include "BoxString.h"
#include "NetBox2.h"
#include "BoxScript.h"
#include "BoxHttpScript.h"
#include "BoxHttpAccept.h"
#include "BoxHttpSession.h"
#include "BoxHttpContext.h"
#include "BoxFile.h"
#include "BoxPath.h"
#include "BoxMemFile.h"
#include <BPath.h>
#include <BEncoding.h>
#include <BCodePage.h>

static const char * const status_lines[] =
{
	"100 Continue",
	"101 Switching Protocols",
	"102 Processing",
#define LEVEL_200  3
	"200 OK",
	"201 Created",
	"202 Accepted",
	"203 Non-Authoritative Information",
	"204 No Content",
	"205 Reset Content",
	"206 Partial Content",
	"207 Multi-Status",
#define LEVEL_300 11
	"300 Multiple Choices",
	"301 Moved Permanently",
	"302 Object moved",
	"303 See Other",
	"304 Not Modified",
	"305 Use Proxy",
	"306 unused",
	"307 Temporary Redirect",
#define LEVEL_400 19
	"400 Bad Request",
	"401 Authorization Required",
	"402 Payment Required",
	"403 Forbidden",
	"404 File Not Found",
	"405 Method Not Allowed",
	"406 Not Acceptable",
	"407 Proxy Authentication Required",
	"408 Request Time-out",
	"409 Conflict",
	"410 Gone",
	"411 Length Required",
	"412 Precondition Failed",
	"413 Request Entity Too Large",
	"414 Request-URI Too Large",
	"415 Unsupported Media Type",
	"416 Requested Range Not Satisfiable",
	"417 Expectation Failed",
	"418 Host Not Found",
	"419 unused",
	"420 unused",
	"421 unused",
	"422 Unprocessable Entity",
	"423 Locked",
	"424 Failed Dependency",
#define LEVEL_500 44
	"500 Internal Server Error",
	"501 Method Not Implemented",
	"502 Bad Gateway",
	"503 Service Temporarily Unavailable",
	"504 Gateway Time-out",
	"505 HTTP Version Not Supported",
	"506 Variant Also Negotiates",
	"507 Insufficient Storage",
	"508 unused",
	"509 unused",
	"510 Not Extended"
#define RESPONSE_CODES 55
};

//extern int s_AppDevID, s_BadDevID;

inline int checkmask(const char *data, const char *mask)
{	int i;
	char d;

	for (i = 0; i < 256; i++)
	{
		d = data[i];
		switch (mask[i])
		{
			case '\0':
				return (d == '\0');

			case '*':
				return 1;

			case '@':
				if (!isupper(d))
					return 0;
				break;
			case '$':
				if (!islower(d))
					return 0;
					break;
			case '#':
				if (!isdigit(d))
					return 0;
				break;
			case '&':
				if (!isxdigit(d))
					return 0;
				break;
			case '~':
				if ((d != ' ') && !isdigit(d))
					return 0;
				break;
			default:
				if (mask[i] != d)
					return 0;
				break;
		}
	}
	return 0;
}

static SYSTEMTIME StringToTime(const char *date)
{	SYSTEMTIME ds={0};
	int mint, mon;
	const char *monstr, *timstr;
	static const int months[12] =
	{
	('J' << 16) | ('a' << 8) | 'n', ('F' << 16) | ('e' << 8) | 'b',
	('M' << 16) | ('a' << 8) | 'r', ('A' << 16) | ('p' << 8) | 'r',
	('M' << 16) | ('a' << 8) | 'y', ('J' << 16) | ('u' << 8) | 'n',
	('J' << 16) | ('u' << 8) | 'l', ('A' << 16) | ('u' << 8) | 'g',
	('S' << 16) | ('e' << 8) | 'p', ('O' << 16) | ('c' << 8) | 't',
	('N' << 16) | ('o' << 8) | 'v', ('D' << 16) | ('e' << 8) | 'c'};

	if (!date)
		return ds;

	while (*date && isspace(*date))
		++date;

	if (*date == '\0')
		return ds;

	if ((date = strchr(date, ' ')) == NULL)
		return ds;

	++date;

	if (checkmask(date, "## @$$ #### ##:##:## *"))	/* RFC 1123 format */
	{
		ds.wYear = (WORD)(((date[7] & 0xf) * 10 + (date[8] & 0xf) - 19) * 100 +
				((date[9] & 0xf) * 10) + (date[10] & 0xf));

		ds.wDay = (WORD)(((date[0] & 0xf) * 10) + (date[1] & 0xf));

		monstr = date + 3;
		timstr = date + 12;
	}else if (checkmask(date, "##-@$$-## ##:##:## *"))		/* RFC 850 format  */
	{
		ds.wYear = (WORD)(((date[7] & 0xf) * 10) + (date[8] & 0xf));
		if (ds.wYear < 70)
			ds.wYear += 100;

		ds.wDay = (WORD)(((date[0] & 0xf) * 10) + (date[1] & 0xf));

		monstr = date + 3;
		timstr = date + 10;
	}else if (checkmask(date, "@$$ ~# ##:##:## ####*"))	/* asctime format  */
	{
		ds.wYear = (WORD)(((date[16] & 0xf) * 10 + (date[17] & 0xf) - 19) * 100 +
					((date[18] & 0xf) * 10) + (date[19] & 0xf));

		ds.wDay = (WORD)((date[4] & 0xF) * 10 + (date[5] & 0xf));

		monstr = date;
		timstr = date + 7;
	}else
		return ds;

	if (ds.wDay <= 0 || ds.wDay > 31)
		return ds;

	ds.wHour = (WORD)(((timstr[0] & 0xf) * 10) + (timstr[1] & 0xf));
	ds.wMinute = (WORD)(((timstr[3] & 0xf) * 10) + (timstr[4] & 0xf));
	ds.wSecond = (WORD)(((timstr[6] & 0xf) * 10) + (timstr[7] & 0xf));

	if ((ds.wHour > 23) || (ds.wMinute > 59) || (ds.wSecond > 61))
		return ds;

	mint = (monstr[0] << 16) | (monstr[1] << 8) | monstr[2];
	for (mon = 0; mon < 12; mon++)
	if (mint == months[mon])
		break;
	if (mon == 12)
		return ds;

	if ((ds.wDay == 31) && (mon == 3 || mon == 5 || mon == 8 || mon == 10))
		return ds;

	/* February gets special check for leapyear */

	if ((mon == 1) &&
	((ds.wDay > 29)
	 || ((ds.wDay == 29)
		 && ((ds.wYear & 3)
		 || (((ds.wYear % 100) == 0)
			 && (((ds.wYear % 400) != 100)))))))
		return ds;

	ds.wMonth = (WORD)(mon + 1);

	ds.wYear += 1900;

	return ds;
}

// CBoxHttpAccept

CBoxHttpAccept::CBoxHttpAccept() :
	m_nRangeBegin(0),
	m_nRangeEnd(0),
	m_nContentLength(0),
	m_nContentPosition(0),
	m_bKeepAlive(FALSE),
	m_bDepth(FALSE),
	m_fileResponse(NULL),
	m_nStatusCode(200),
	m_nResponseContentLength(0),
	m_bBuffer(TRUE),
	m_zipFile(NULL),
	m_nMethod(HTTP_GET),
	m_nLocalPort(80),
	m_bSendHeader(FALSE)
{
	m_astrRequestHeaderName.SetSize(0, 1024);
	m_astrRequestHeaderValue.SetSize(0, 1024);
}

// CBoxHttpAccept message handlers

long CBoxHttpAccept::OnAccept()
{
	m_pSocket = m_varJob.pdispVal;
	int nError;

	nError = ReadRequest();
	if(nError == 100)
	{
		m_pSocket.Release();

		return TRUE;
	}

	if(nError != -1)
	{
		if(nError)
		{
			m_nStatusCode = nError;
			BuildInfomation();
			AddHeader(_T("Content-Type"), _T("text/html"));
		}else
		{
			ReadHeader();

			BuildInfomation();

/*			if(m_nMethod == HTTP_PRODINFO)
				nError = ProdInfo();
			else
*/				if(m_nMethod >= HTTP_PUT)
			{
				if(MapRequestPath(m_nAgentVersion == 1))
					nError = BuildResponse();
				else nError = 404;
			}else if(MapRequestPath())
			{
				if(m_nMethod == HTTP_OPTIONS)
					nError = BuildOptions();
				else nError = BuildResponse();

				if(nError == 404)
					if(MapRequestPath(TRUE))
						nError = BuildResponse();
					else nError = 404;
			}else nError = 418;

			if(!m_bSendHeader && nError != 0)
				m_nStatusCode = nError;
		}

		SendResponse();

		ClearAll();
	}

	m_pSocket.Release();

	return m_bKeepAlive;
}

int CBoxHttpAccept::BuildResponse(void)
{
	switch(m_nMethod)
	{
	case HTTP_UNKNOWN:
		return 400;
	case HTTP_DELETE:
		return DeleteContent();
	case HTTP_PUT:
		return PutContent();
	case HTTP_MOVE:
		return MoveContent();
	case HTTP_COPY:
		return MoveContent(TRUE);
	case HTTP_MKCOL:
		return CreateFolder();
	case HTTP_PROPFIND:
		return FindProperty();
	default:
		return m_bIsDirectory ? BuildDirectory() : BuildFile();
	}
}

int CBoxHttpAccept::ReadRequest(void)
{
	CString str;
	LPCTSTR pstr, pstr1, pstr2;

	m_bKeepAlive = FALSE;
	m_nAgentVersion = 0;

	while(TRUE)
	{
		if(m_pSocket->ReadString(str) == SOCKET_ERROR)
			return -1;

		if(str.IsEmpty())
		{
			if(m_pSocket->IsEmpty())
				return 100;
		}else break;
	}

	pstr = pstr1 = str;

	while(*pstr1 && *pstr1 != _T(' '))
		pstr1 ++;

	m_strMethod.SetString(pstr, (int)(pstr1 - pstr));
	while(*pstr1 && *pstr1 == _T(' '))
		pstr1 ++;

	pstr = pstr1;

	if(!m_strMethod.CompareNoCase(_T("GET")))m_nMethod = HTTP_GET;
	else if(!m_strMethod.CompareNoCase(_T("POST")))m_nMethod = HTTP_POST;
	else if(!m_strMethod.CompareNoCase(_T("HEAD")))m_nMethod = HTTP_HEAD;
	else if(!m_strMethod.CompareNoCase(_T("OPTIONS")))m_nMethod = HTTP_OPTIONS;
	else if(!m_strMethod.CompareNoCase(_T("PUT")))m_nMethod = HTTP_PUT;
	else if(!m_strMethod.CompareNoCase(_T("DELETE")))m_nMethod = HTTP_DELETE;
	else if(!m_strMethod.CompareNoCase(_T("MOVE")))m_nMethod = HTTP_MOVE;
	else if(!m_strMethod.CompareNoCase(_T("COPY")))m_nMethod = HTTP_COPY;
	else if(!m_strMethod.CompareNoCase(_T("MKCOL")))m_nMethod = HTTP_MKCOL;
	else if(!m_strMethod.CompareNoCase(_T("PROPFIND")))m_nMethod = HTTP_PROPFIND;
//	else if(!m_strMethod.CompareNoCase(_T("PRODINFO")))m_nMethod = HTTP_PRODINFO;
	else m_nMethod = HTTP_UNKNOWN;

	while(*pstr1 && *pstr1 != _T(' ') && *pstr1 != _T('?'))
		pstr1 ++;

	if(pstr1 == pstr)
	{
		m_strMethod.Empty();
		return 400;
	}

	m_strUrl.SetString(pstr, (int)(pstr1 - pstr));

	if(*pstr1 == _T('?'))
	{
		pstr1 ++;
		pstr2 = pstr1;

		while(*pstr1 && *pstr1 != _T(' '))
			pstr1 ++;

		m_strQueryString.SetString(pstr2, (int)(pstr1 - pstr2));
	}

	while(*pstr1 && *pstr1 == _T(' '))
		pstr1 ++;

	pstr = pstr1;

	m_strProtocol = pstr;

	if(!m_strProtocol.IsEmpty() &&
		(m_strProtocol.GetLength() != 8 || m_strProtocol[6] != _T('.') ||
		_tcsnicmp(m_strProtocol, _T("HTTP/"), 5)))
		return 400;

	return 0;
}

void CBoxHttpAccept::ReadHeader(void)
{
	if(!m_strProtocol.IsEmpty())
	{
		CString str, str1;
		LPCTSTR pstr, pstr1;

		str.Preallocate(1024);

		if(m_strProtocol[7] > _T('0'))
			m_bKeepAlive = TRUE;

		while(m_pSocket->ReadString(str) != SOCKET_ERROR && !str.IsEmpty())
		{
			pstr = pstr1 = str;

			while(*pstr1 && *pstr1 != _T(':'))
				pstr1 ++;

			str1.SetString(pstr, (int)(pstr1 - pstr));
			if(*pstr1 == _T(':')) pstr1 ++;
			while(*pstr1 && *pstr1 == _T(' '))
				pstr1 ++;

			if(!str1.IsEmpty() && *pstr1)
			{
				m_astrRequestHeaderName.Add(str1);
				m_astrRequestHeaderValue.Add(pstr1);

				if(!str1.CompareNoCase(_T("Content-Length")))
					m_nContentLength = _tstoi(pstr1);
				else if(!str1.CompareNoCase(_T("Range")))
				{
					if(!_tcsnicmp(pstr1, _T("bytes="), 6))
					{
						pstr1 += 6;
						if(pstr = _tcschr(pstr1, _T('-')))
						{
							pstr ++;

							m_nRangeBegin = _tstoi(pstr1);
							m_nRangeEnd = _tstoi(pstr);
						}
					}
				}else if(!str1.CompareNoCase(_T("Content-Type")))
					m_strContentType = pstr1;
				else if(!str1.CompareNoCase(_T("Cookie")))
					m_strCookie = pstr1;
				else if(!str1.CompareNoCase(_T("Accept-Encoding")))
				{
					m_strAcceptEncoding = pstr1;
					m_strAcceptEncoding.MakeLower();
				}else if(!str1.CompareNoCase(_T("Connection")))
					m_bKeepAlive = !_tcsicmp(pstr1, _T("Keep-Alive"));
				else if(!str1.CompareNoCase(_T("Host")))
				{
					pstr = pstr1;
					while(*pstr1 && *pstr1 != _T(':'))
						pstr1 ++;
					m_strHostName.SetString(pstr, (int)(pstr1 - pstr));
					if(*pstr1 == _T(':'))
						m_nLocalPort = atoi(pstr1 + 1);
				}
				else if(!str1.CompareNoCase(_T("If-Modified-Since")))
					m_tmLastModify = StringToTime(BOX_CT2CA(pstr1));
				else if(!str1.CompareNoCase(_T("User-Agent")))
					m_strUserAgent = pstr1;

				if(m_nMethod >= HTTP_OPTIONS)
				{
					if(!str1.CompareNoCase(_T("User-Agent")))
					{
						if(!_tcsnicmp(pstr1, _T("Microsoft-WebDAV-MiniRedir"), 26))
							m_nMethod = HTTP_UNKNOWN;
					}else if(!str1.CompareNoCase(_T("Content-Language")))
						m_nAgentVersion = 1;
					else if(!str1.CompareNoCase(_T("Depth")))
						m_bDepth = _tcsicmp(pstr1, _T("0"));
				}
			}
		}
	}
}

BOOL CBoxHttpAccept::MapRequestPath(BOOL bUTF8)
{
	CBPath path;
	CString str;
	int p;

	if(bUTF8)
	{
		CBString strTemp;

		{
			CBCodePage cp(CP_UTF8);
			strTemp = CBEncoding::UrlDecode(m_strUrl, m_strUrl.GetLength());
		}

		m_strPathInfo = strTemp;
		//str = m_strPathInfo;
	}else
	{
		m_strPathInfo = CBEncoding::UrlDecode(m_strUrl, m_strUrl.GetLength());
		//str = m_strPathInfo;
	}

	//path = str;

	CComVariant var;

	str = m_strHostName;

	do
	{
		m_pServerInfo->GetValue(str, var);
		if(str.IsEmpty())
			break;

		p = str.Find(_T('.'));
		if(p == -1)str.Empty();
		else str = str.Mid(p + 1);
	}while(var.vt != VT_DISPATCH);

	if(var.vt != VT_DISPATCH)
		return FALSE;

	m_pHost = var.pdispVal;
	if(m_pHost == NULL)
		return FALSE;

	m_pApplicationHost = m_pHost;

	CString strQueryString;

	str.Empty();
	if (m_pHost->URLRewrite(m_strPathInfo, str, strQueryString))
	{
		if (m_strQueryString.GetLength())
			m_strQueryString.Append('&');
		m_strQueryString.Append(strQueryString);
	}
	m_strPathInfo = str;
	path = m_strPathInfo;

	if(m_pHost->m_pContents->get_Count())
	{
		m_strBaseUrl = path.m_strPath;
		str = m_strBaseUrl;
		var.Clear();

		do
		{
			m_pHost->m_pContents->GetValue(m_strBaseUrl, var);
			if(m_strBaseUrl.IsEmpty() || var.vt == VT_DISPATCH)
				break;

			p = m_strBaseUrl.ReverseFind(_T('\\'));
			if(p == -1)m_strBaseUrl.Empty();
			else m_strBaseUrl = m_strBaseUrl.Left(p);
		}while(var.vt != VT_DISPATCH);

		if(var.vt == VT_DISPATCH)
		{
			m_pHost = var.pdispVal;
			if(m_pHost == NULL)return FALSE;

			if(!m_pHost->m_bGlobalApplication)
				m_pApplicationHost = m_pHost;

			m_strMapedPath = (LPCTSTR)str + m_strBaseUrl.GetLength();
		}else m_strMapedPath = str;
	}else m_strMapedPath = path.m_strPath;

	m_bIsDirectory = (m_strMapedPath[m_strMapedPath.GetLength() - 1] == _T('\\'));

	m_strMapedPath = m_pHost->m_strRoot + m_strMapedPath;

	return TRUE;
}

void CBoxHttpAccept::FillServerVariables(CBRequestDictionary* pServerVariables)
{
	CString strAllHttp;
	CString strAllRaw;
	WCHAR strTemp[16];
	CComBSTR bstr;

	int count, i;

	count = (int)m_astrRequestHeaderName.GetCount();
	for(i = 0; i < count; i ++)
	{
		strAllRaw.Append(m_astrRequestHeaderName[i] + _T(": ") + m_astrRequestHeaderValue[i] + _T("\r\n"));

		m_astrRequestHeaderName[i].MakeUpper();
		m_astrRequestHeaderName[i].Replace(_T('-'), _T('_'));
		m_astrRequestHeaderName[i] = _T("HTTP_") + m_astrRequestHeaderName[i];

		strAllHttp.Append(m_astrRequestHeaderName[i] + _T(": ") + m_astrRequestHeaderValue[i] + _T("\n"));
	}

	pServerVariables->AddValue(L"ALL_HTTP", strAllHttp);
	pServerVariables->AddValue(L"ALL_RAW", strAllRaw);

	pServerVariables->AddValue(L"APPL_PHYSICAL_PATH", m_pHost->m_strRoot);

	_ltow(m_nContentLength, strTemp, 10);
	pServerVariables->AddValue(L"CONTENT_LENGTH", strTemp);
	pServerVariables->AddValue(L"CONTENT_TYPE", m_strContentType);
	pServerVariables->AddValue(L"GATEWAY_INTERFACE", "CGI/1.1");

	pServerVariables->AddValue(L"PATH_INFO", m_strPathInfo);
	pServerVariables->AddValue(L"PATH_TRANSLATED", m_strMapedPath);
	pServerVariables->AddValue(L"QUERY_STRING", m_strQueryString);
	pServerVariables->AddValue(L"REQUEST_METHOD", m_strMethod);

	bstr.Attach(m_pSocket->get_RemoteAddress());
	pServerVariables->AddValue(L"REMOTE_ADDR", bstr);
	_ltow(m_pSocket->get_RemotePort(), strTemp, 10);
	pServerVariables->AddValue(L"REMOTE_PORT", strTemp);
	pServerVariables->AddValue(L"REMOTE_HOST", bstr);

	bstr.Attach(m_pSocket->get_LocalAddress());
	pServerVariables->AddValue(L"LOCAL_ADDR", bstr);
	_ltow(m_pSocket->get_LocalPort(), strTemp, 10);
	pServerVariables->AddValue(L"LOCAL_PORT", strTemp);

	pServerVariables->AddValue(L"SCRIPT_NAME", m_strPathInfo);
	pServerVariables->AddValue(L"SERVER_NAME", m_strHostName);

	_ltow(m_nLocalPort, strTemp, 10);
	pServerVariables->AddValue(L"SERVER_PORT", strTemp);

	pServerVariables->AddValue(L"SERVER_PROTOCOL", m_strProtocol);
	pServerVariables->AddValue(L"SERVER_SOFTWARE", CBoxSystem::getVersion());
	pServerVariables->AddValue(L"URL", m_strPathInfo);

	//pServerVariables->AddValue(L"URL_ORIGINAL", m_strUrl);

	for(i = 0; i < count; i ++)
		pServerVariables->AddValue(CBString(m_astrRequestHeaderName[i]), m_astrRequestHeaderValue[i]);
}

void CBoxHttpAccept::FillForm(CBRequestDictionary* pForm)
{
	if(((CBoxHttpServer*)m_pServer)->m_nMaxRequestEntity &&
		m_nContentLength > ((CBoxHttpServer*)m_pServer)->m_nMaxRequestEntity)
		return;

	if(!m_strContentType.CompareNoCase(_T("application/x-www-form-urlencoded")))
	{
		CAutoVectorPtr <char> pBuffer;
		int nSize;

		pBuffer.Allocate(m_nContentLength);

		nSize = ReadFromClient(pBuffer, m_nContentLength);

		if(nSize == m_nContentLength)
			pForm->ParseUrlEncodeString(pBuffer, m_nContentLength);
	}else
	{
		int pos;

		pos = m_strContentType.Find(_T(';'));
		if(pos != -1)
		{
			CString str;

			str.SetString(m_strContentType, pos);

			if(str.CompareNoCase(_T("multipart/form-data")))return;
		}else if(m_strContentType.CompareNoCase(_T("multipart/form-data")))return;

		CAutoVectorPtr <char> pBuffer;
		int nSize;

		pBuffer.Allocate(m_nContentLength);

		nSize = ReadFromClient(pBuffer, m_nContentLength);

		if(nSize == m_nContentLength)
			pForm->ParseUploadString(pBuffer, m_nContentLength);
	}
}

void CBoxHttpAccept::BuildInfomation(void)
{
	AddHeader(_T("Server"), CBoxSystem::getVersion());

	CBoxObject<CBoxTimeCache::CTime> pTime;

	m_time.GetTime(pTime);
	AddHeader(_T("Date"), pTime->m_strTime);
	pTime.Release();

	AddHeader(_T("Connection"), m_bKeepAlive ? _T("Keep-Alive") : _T("Close"));
}

int CBoxHttpAccept::BuildOptions(void)
{
	CString str;

	str = "OPTIONS, GET, HEAD";
	if(m_pHost->m_bEnableBrowse)
		str.Append(_T(", PROPFIND"));
	if(m_pHost->m_bEnableWrite)
		str.Append(_T(", DELETE, PUT, POST, MOVE, MKCOL, PROPPATCH"));

	AddHeader(_T("DAV"), _T("1, 2"));
	AddHeader(_T("Public"), _T("OPTIONS, GET, HEAD, DELETE, PUT, POST, MOVE, MKCOL, PROPFIND, PROPPATCH"));
	AddHeader(_T("Allow"), str);
	AddHeader(_T("Cache-Control"), _T("private"));

	return 0;
}

/*int CBoxHttpAccept::ProdInfo(void)
{
	m_fileResponse = new CBoxMemFile();
	SetHeader(_T("Cache-control"), _T("private"));

	m_fileResponse->Write(theApp.m_DevIDS, theApp.m_nDevCount * sizeof(int));
	m_fileResponse->SeekToBegin();
	m_nResponseContentLength = (int)m_fileResponse->GetLength();

	return 0;
}*/

__declspec(thread) CBoxHttpAccept* t_pHttpAccept = NULL;

int CBoxHttpAccept::BuildFile(void)
{
/*	if(s_BadDevID)
	{
		CString str;

		str.Format(_T("http://www.netbox.cn/user/info.asp?devID=%d&err=1"), s_BadDevID);
		AddHeader(_T("Location"), str);
		return 302;
	}
*/
	LPCTSTR pstrExt = NULL;

	if(m_strMapedPath.GetLength() > 4)
		pstrExt = m_strMapedPath + m_strMapedPath.GetLength() - 4;

	if(m_pHost->m_bEnableScript)
	{
		CBoxObject<CBoxScript> pScript;
		CBoxObject<CBoxHttpScript> pHttpScript;
		CString str;

		if(IsBoxFile(pstrExt))
			pScript.CreateInstance();
		else if(IsAspFile(pstrExt))
		{
			pHttpScript.CreateInstance();
			pHttpScript->SetBasePath(m_pHost->m_strRoot);
			pHttpScript->SetSessionState(m_pHost->m_bEnableSession);
			pScript = (CBoxScript*)pHttpScript;
		}else if(IsWscFile(pstrExt))
			return 403;

		if(pScript != NULL)
		{
			long nError;

			pScript->EnableDebug(m_pHost->m_bEnableDebug);
			if(!m_pHost->m_strLanguage.IsEmpty())
				pScript->SetLanguage(m_pHost->m_strLanguage);

			pScript->put_Timeout(m_pHost->m_nScriptTimeout);

			nError = pScript->Load(m_strMapedPath);
			if(nError == 404)
				return 404;

			m_fileResponse = new CBoxMemFile();
			SetHeader(_T("Content-Type"), _T("text/html"));
			SetHeader(_T("Cache-control"), _T("private"));

			if(nError == 0)
			{
				CBoxObject<CBoxHttpContext> pContext;

				pContext.CreateInstance();

				pContext->Bind(this, pScript);

				t_pHttpAccept = this;

				if(pHttpScript != NULL ? pHttpScript->GetSessionState() : m_pHost->m_bEnableSession)
				{
					CBoxObject<CBoxHttpSession> pSession;
					CString strSessionCookie;
					int pos, pos1, pos2;

					if(!m_strCookie.IsEmpty() && (pos = m_strCookie.Find(m_pApplicationHost->m_strHostKey)) != -1)
					{
						pos2 = pos + m_pApplicationHost->m_strHostKey.GetLength();

						if(m_strCookie[pos2] == _T('='))
						{
							pos2 ++;
							pos1 = m_strCookie.Find(_T(';'), pos2);
							if(pos1 == -1)
								pos1 = m_strCookie.GetLength();

							strSessionCookie = m_strCookie.Mid(pos2, pos1 - pos2);
							m_strCookie.Delete(pos, pos1 - pos + 1);
						}
					}

					if(!m_pApplicationHost->GetSession(strSessionCookie, pSession, pContext))
						if(m_pApplicationHost->m_strSessionDomain.IsEmpty())
							AddHeader(_T("Set-Cookie"), m_pApplicationHost->m_strHostKey + _T('=') + strSessionCookie + _T("; path=/"));
						else AddHeader(_T("Set-Cookie"), m_pApplicationHost->m_strHostKey + _T('=') + strSessionCookie + _T("; domain=") + m_pApplicationHost->m_strSessionDomain + _T("; path=/"));

					pScript->AddNameItem(_T("Session"), pSession);
				}

				if(!pContext->m_pResponse->m_bEnd)
					nError = pScript->Run();

				t_pHttpAccept = NULL;

				if(!m_bSendHeader && m_pCookies != NULL)
					AddHeader(m_pCookies->RanderCookie());
			}

			try
			{
				if(nError != 0)
					if(m_pHost->m_strErrorMessage.IsEmpty())
					{
						CBoxScript::CScriptError &error = pScript->GetErrorMessage();

						CStringA str("<font face=\"Arial\" size=2><p>");

						str += error.m_strSource;

						if(error.m_sCode != 0)
							str.AppendFormat(" \'%08x\'\r\n", error.m_sCode);
						str.AppendFormat("<p>%s\r\n<p>%s, line %d</font><p>", error.m_strDescription, error.m_strFile, error.m_nLine);

						WriteToClient(str, str.GetLength());

						WriteToClient(_T("<hr>\r\n<small>Host by <a href=\"http://www.netbox.cn\" target=\"_blank\">"), 68);
						WriteToClient(CBoxSystem::getVersion(), CBoxSystem::getVersionLen());
						WriteToClient(_T("</a></small>\r\n</body></html>"), 28);
					}else WriteToClient(m_pHost->m_strErrorMessage, m_pHost->m_strErrorMessage.GetLength());
			}catch(CException* e)
			{
				e->Delete();

				m_nRangeBegin = 0;
				m_nRangeEnd = 0;

				return FALSE;
			}

			pScript.Release();
			pHttpScript.Release();

			theApp.m_pSystem->ClearLock();

			if(m_strMapToFile.IsEmpty())
			{
				m_nRangeBegin = 0;
				m_nRangeEnd = 0;

				if(m_zipFile)m_zipFile->Close();
				m_fileResponse->SeekToBegin();
				m_nResponseContentLength = (int)m_fileResponse->GetLength();

				return nError;
			}

			SetHeader(_T("Content-Type"), _T(""));
			SetHeader(_T("Cache-control"), _T(""));

			delete m_fileResponse;
			m_fileResponse = NULL;
			m_strMapedPath = m_strMapToFile;

			pstrExt = NULL;

			if(m_strMapedPath.GetLength() > 4)
				pstrExt = m_strMapedPath + m_strMapedPath.GetLength() - 4;
		}
	}

	m_fileResponse = g_pFile->Open(m_strMapedPath);
	if(m_fileResponse == NULL)
		return 404;

	if(m_fileResponse == BOX_FOLDER)
	{
		m_fileResponse = NULL;
		if(!m_strMapToFile.IsEmpty())
			return 403;
		m_strPathInfo = CBEncoding::UrlEncode(m_strPathInfo);
		m_strPathInfo += _T('/');
		AddHeader(_T("Location"), m_strPathInfo);
		return 302;
	}

	if(m_fileResponse->IsKindOf(RUNTIME_CLASS(CMemFile)) && IsExeFile(pstrExt))
		return 403;

	m_nResponseContentLength = (int)m_fileResponse->GetLength();
	g_pFile->GetFileTimeString(m_fileResponse, m_mapResponseHeader[_T("Last-Modified")]);

	SYSTEMTIME tm = g_pFile->GetFileTime(m_fileResponse);
	if(!memcmp(&tm, &m_tmLastModify, sizeof(tm)))
	{
		delete m_fileResponse;
		m_fileResponse = NULL;

		return 304;
	}

	AddHeader(_T("Content-Type"), theApp.GetContentTypeFromFileName(m_strMapedPath));

	if(m_nResponseContentLength > 1 && (m_nRangeBegin != 0 || m_nRangeEnd != 0))
	{
		if(m_nRangeEnd == 0)
			m_nRangeEnd = m_nResponseContentLength - 1;

		if(m_nRangeBegin == -m_nRangeEnd)
		{
			m_nRangeEnd = m_nResponseContentLength - 1;
			m_nRangeBegin += m_nResponseContentLength;
		}

		CString str;

		str.Format(_T("bytes %d-%d/%d"), m_nRangeBegin, m_nRangeEnd, m_nResponseContentLength);
		AddHeader(_T("Content-Range"), str);

		m_nResponseContentLength = m_nRangeEnd - m_nRangeBegin + 1;

		m_nStatusCode = 206;
	}

	return 0;
}

static int BrowseFolderCompare(const CString * pstr1, const CString * pstr2)
{
	int ret = (*pstr2)[41] - (*pstr1)[41];

	if(ret)return ret;

	return _tcsicmp((LPCTSTR)(*pstr1) + 41, (LPCTSTR)(*pstr2) + 41);
}

int CBoxHttpAccept::BuildDirectory(void)
{
	CString strDirectory;

	int nError = 404;
	strDirectory = m_strMapedPath;

	int ac = (int)m_pHost->m_astrDefault.GetCount();

	for(int i = 0; i < ac; i ++)
	{
		m_strMapedPath = strDirectory + m_pHost->m_astrDefault[i];
		nError = BuildFile();
		if(nError != 404)
			return nError;
	}

	m_strMapedPath = strDirectory;

	SYSTEMTIME st;
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	CString str, str1, strBaseUrl;
	CStringW wstr;

	str = g_pFile->m_strBasePath + ((LPCTSTR)m_strMapedPath + 1);
	str += _T("*.*");

	hFind = FindFirstFile(str, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return 404;

	if(!m_pHost->m_bEnableBrowse)
	{
		FindClose(hFind);
		return 403;
	}

	m_fileResponse = new CBoxMemFile();
	SetEncoding(_T("gzip"));

	AddHeader(_T("Cache-control"), _T("private"));
	AddHeader(_T("Content-Type"), _T("text/html"));

	WriteToClient(_T("<head>\r\n<title>"), 15);
	WriteToClient(m_strHostName, m_strHostName.GetLength());
	WriteToClient(_T(" - "), 3);
	WriteToClient(m_strPathInfo, m_strPathInfo.GetLength());
	WriteToClient(_T("</title>\r\n</head>\r\n<body>\r\n"), 27);
	if(m_pHost->m_bAutoWebDAV)
	{
		WriteToClient(_T("<a folder=\"http://"), 18);
		WriteToClient(m_strHostName, m_strHostName.GetLength());
		WriteToClient(m_strPathInfo, m_strPathInfo.GetLength());
		WriteToClient(_T("\" id=o style=\"behavior:url(#default#AnchorClick)\"></a><script>o.click()</script>"), 80);
	}

	WriteToClient(m_pHost->m_strBrowseHeader, m_pHost->m_strBrowseHeader.GetLength());
	WriteToClient(_T("<H1>"), 4);
	WriteToClient(m_strHostName, m_strHostName.GetLength());
	WriteToClient(_T(" - "), 3);
	WriteToClient(m_strPathInfo, m_strPathInfo.GetLength());
	WriteToClient(_T("</H1><hr>\r\n<pre>"), 16);

	if(m_strPathInfo.Compare(_T("/")))
		WriteToClient(_T("<a href=\"../\">[To Parent Directory]</a>\r\n"), 41);

	CStringArray astrAll;

	do
	{
		str1 = fd.cFileName;
		if(str1.Compare(_T(".")) && str1.Compare(_T("..")) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{
			FileTimeToSystemTime(&fd.ftLastWriteTime, &st);
			str.Format(_T("        %04d-%02d-%02d %02d:%02d:%02d"),
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				str += _T("         &lt;dir&gt; <a href=\"");
			else
				str.AppendFormat(_T("%14I64d <a href=\""), ((__int64)fd.nFileSizeHigh << 32) + fd.nFileSizeLow);

			str += CBEncoding::UrlEncode(str1);

			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				str += _T("/\">");
			else str += _T("\">");

			str1 = fd.cFileName;
			CBoxEncode::HtmlEncode(str1);
			str += str1;

			str += _T("</a>\r\n");

			astrAll.Add(str);
		}
	}while(FindNextFile(hFind, &fd));

	FindClose(hFind);

	int count = (int)astrAll.GetCount();

	if(count)
	{
		qsort(astrAll.GetData(), astrAll.GetSize(), sizeof(CString), (int (__cdecl *)(const void *, const void *))BrowseFolderCompare);

		for(int i = 0; i < count; i ++)
			WriteToClient(astrAll[i], astrAll[i].GetLength());
	}

	WriteToClient(_T("</pre><hr>"), 10);
	WriteToClient(m_pHost->m_strBrowseFooter, m_pHost->m_strBrowseFooter.GetLength());
	WriteToClient(_T("</body>"), 7);

	if(m_zipFile)m_zipFile->Close();
	m_fileResponse->SeekToBegin();
	m_nResponseContentLength = (int)m_fileResponse->GetLength();

	return 0;
}

int CBoxHttpAccept::FindProperty(void)
{
	if(!m_pHost->m_bEnableBrowse)
		return 403;

	SYSTEMTIME st;
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	CString str, str1, strBaseUrl;
	CStringW wstr;

	str = g_pFile->m_strBasePath + ((LPCTSTR)m_strMapedPath + 1);
	if(m_bDepth)
	{
		if(m_bIsDirectory)str += _T("*.*");
		else str += _T("\\*.*");
	}else if(m_bIsDirectory)str = str.Left(str.GetLength() - 1);

	hFind = FindFirstFile(str, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return 404;

	m_fileResponse = new CBoxMemFile();
	AddHeader(_T("Cache-control"), _T("private"));
	AddHeader(_T("Content-Type"), _T("text/xml"));

	WriteToClient("<?xml version=\"1.0\"?><a:multistatus xmlns:b=\"urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/\" xmlns:c=\"xml:\" xmlns:a=\"DAV:\">", 123);

	// XP Explorer can't access chinese path with other port
	int i = m_nLocalPort;
	if(i != 80)wstr.Format(L":%d", i);
	wstr += m_strPathInfo;

	int nTempCount = WideCharToMultiByte(CP_UTF8, 0, wstr, wstr.GetLength(), NULL, 0, 0, 0);
	LPSTR pstr = strBaseUrl.GetBuffer(nTempCount);

	WideCharToMultiByte(CP_UTF8, 0, wstr, wstr.GetLength(), pstr, nTempCount, 0, 0);
	strBaseUrl.ReleaseBuffer(nTempCount);
	strBaseUrl = CBEncoding::UrlEncode(strBaseUrl, TRUE);

	strBaseUrl = CString(_T("http://")) + m_strHostName + strBaseUrl;
	if(!m_bIsDirectory)strBaseUrl.Append("/", 1);

	AddHeader(_T("Content-Location"), strBaseUrl);

	do
	{
		str1 = fd.cFileName;
		if(str1.Compare(_T("..")) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{
			WriteToClient("<a:response><a:href>", 20);
			WriteToClient(strBaseUrl, strBaseUrl.GetLength());

			if(str1.Compare(_T(".")))
			{
				wstr = str1;

				int nTempCount = WideCharToMultiByte(CP_UTF8, 0, wstr, wstr.GetLength(), NULL, 0, 0, 0);
				pstr = str1.GetBuffer(nTempCount);

				WideCharToMultiByte(CP_UTF8, 0, wstr, wstr.GetLength(), pstr, nTempCount, 0, 0);
				str1.ReleaseBuffer(nTempCount);
				str1 = CBEncoding::UrlEncode(str1, TRUE);

				WriteToClient(str1, str1.GetLength());
			}

			WriteToClient("</a:href><a:propstat><a:status>HTTP/1.1 200 OK</a:status><a:prop>", 65);

			FileTimeToSystemTime(&fd.ftLastWriteTime, &st);
			str.Format(_T("<a:getlastmodified b:dt=\"dateTime.tz\">%04d-%02d-%02dT%02d:%02d:%02d.%03dZ</a:getlastmodified>"),
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			WriteToClient(str, str.GetLength());

			FileTimeToSystemTime(&fd.ftCreationTime, &st);
			str.Format(_T("<a:creationdate b:dt=\"dateTime.tz\">%04d-%02d-%02dT%02d:%02d:%02d.%03dZ</a:creationdate>"),
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			WriteToClient(str, str.GetLength());

			FileTimeToSystemTime(&fd.ftLastAccessTime, &st);
			str.Format(_T("<a:lastaccessed b:dt=\"dateTime.tz\">%04d-%02d-%02dT%02d:%02d:%02d.%03dZ</a:lastaccessed>"),
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			WriteToClient(str, str.GetLength());

			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				WriteToClient("<a:iscollection b:dt=\"boolean\">1</a:iscollection></a:prop></a:propstat></a:response>", 84);
			else
			{
				str.Format(_T("<a:getcontentlength b:dt=\"int\">%I64d</a:getcontentlength></a:prop></a:propstat></a:response>"), ((__int64)fd.nFileSizeHigh << 32) + fd.nFileSizeLow);
				WriteToClient(str, str.GetLength());
			}
		}
	}while(m_bDepth && FindNextFile(hFind, &fd));

	FindClose(hFind);

	WriteToClient("</a:multistatus>", 16);

	m_fileResponse->SeekToBegin();
	m_nResponseContentLength = (int)m_fileResponse->GetLength();

	return 207;
}

int CBoxHttpAccept::DeleteContent(void)
{
	if(!m_pHost->m_bEnableWrite)
		return 403;

	CString str;

	str = g_pFile->m_strBasePath + ((LPCTSTR)m_strMapedPath + 1);

	if(::PathIsDirectory(str))
	{
		if(::RemoveDirectory(str))return 204;
	}else
	{
		if(::DeleteFile(str))return 204;
	}
	return 423;
}

int CBoxHttpAccept::PutContent(void)
{
	if(!m_pHost->m_bEnableWrite)
		return 403;

	CString str;

	str = g_pFile->m_strBasePath + ((LPCTSTR)m_strMapedPath + 1);

	CBoxSafeFile file;

	if(!file.Open(str, CFile::modeCreate | CFile::modeWrite))
		return 501;

	char buf[1024];
	int nSize;

	while(m_nContentPosition < m_nContentLength)
	{
		nSize = m_nContentLength - m_nContentPosition;
		if(nSize > 1024)
			nSize = 1024;

		if(m_pSocket->ReadBlock(buf, nSize) != nSize)
			break;

		m_nContentPosition += nSize;

		file.Write(buf, nSize);
	}

	return 201;
}

int CBoxHttpAccept::CreateFolder(void)
{
	if(!m_pHost->m_bEnableWrite)
		return 403;

	CString str;

	str = g_pFile->m_strBasePath + ((LPCTSTR)m_strMapedPath + 1);

	if(::CreateDirectory(str, NULL))
		return 201;

	return 409;
}

int CBoxHttpAccept::MoveContent(BOOL bCopy)
{
	if(!m_pHost->m_bEnableWrite)
		return 403;

	CString str;

	str = g_pFile->m_strBasePath + ((LPCTSTR)m_strMapedPath + 1);

	CString strDesc;

	strDesc = GetRequestHeader(_T("Destination"));
	if(!_tcsnicmp(strDesc, _T("http://"), 7))
		strDesc = strDesc.Mid(strDesc.Find(_T('/'), 7));

	strDesc = CBEncoding::UrlDecode(strDesc);

	if(m_nAgentVersion == 1)
	{
		CStringW wstr;

		int nTempCount = MultiByteToWideChar(CP_UTF8, 0, strDesc, strDesc.GetLength(), NULL, 0);
		LPWSTR pwstr = wstr.GetBuffer(nTempCount);

		MultiByteToWideChar(CP_UTF8, 0, strDesc, strDesc.GetLength(), pwstr, nTempCount);
		wstr.ReleaseBuffer(nTempCount);

		strDesc = wstr;
	}

	CBoxPath path;

	path.Combine(strDesc);
	strDesc = path.m_strPath;

	if(_tcsnicmp(m_strBaseUrl, strDesc, m_strBaseUrl.GetLength()))
		return 409;

	path.Combine(m_pHost->m_strRoot, (LPCTSTR)strDesc + m_strBaseUrl.GetLength());
	strDesc = g_pFile->m_strBasePath + (path.m_strPath + 1);

	if(bCopy)
	{
		if(::CopyFile(str, strDesc, FALSE))
	        return 201;
	}else
		if(::MoveFile(str, strDesc))
	        return 201;

	return 404;
}

void CBoxHttpAccept::SendHeader()
{
	static int shortcut[6] = {0, LEVEL_200, LEVEL_300, LEVEL_400, LEVEL_500, RESPONSE_CODES};
	int n, pos;
	char strContentLength[16];

	if(m_strStatus.IsEmpty())
	{
		if (m_nStatusCode < 100 || m_nStatusCode >= 600)
			m_nStatusCode = 500;

		n = m_nStatusCode / 100;
		pos = shortcut[n - 1] + m_nStatusCode % 100;
		if(pos >= shortcut[n])
			pos = LEVEL_500;

		m_strStatus = status_lines[pos];
	}else m_nStatusCode = -1;

	CStringA str;

	str.Preallocate(4096);

	str.Append("HTTP/1.1 ", 9);
	str.Append(m_strStatus);
	str.Append("\r\n", 2);

	str.Append(m_strAddHeader, m_strAddHeader.GetLength());

	POSITION mappos = m_mapResponseHeader.GetStartPosition();
	CString strHead, strValue;

	while(mappos != NULL)
	{
		m_mapResponseHeader.GetNextAssoc(mappos, strHead, strValue);
		if(!strHead.IsEmpty() && !strValue.IsEmpty())
		{
			str.Append(strHead);
			str.Append(": ", 2);
			str.Append(strValue);
			str.Append("\r\n", 2);
		}
	}

	if(m_pHost != NULL)
		str.Append(m_pHost->m_strAddHeader);

	if(m_fileResponse == NULL)
	{
		if(m_nStatusCode != 304 && m_nStatusCode != 200)
		{
			if(m_nMethod != HTTP_HEAD)
			{
				CStringA strMsg;

				strMsg.SetString("<html><head><title>", 19);
				strMsg.Append(m_strStatus, m_strStatus.GetLength());
				strMsg.Append("</title></head>\r\n<body><h1>", 27);
				strMsg.Append(m_strStatus, m_strStatus.GetLength());
				strMsg.Append("</h1><hr>\r\n<small>Host by <a href=\"http://www.netbox.cn\" target=\"_blank\">", 73);
				strMsg.Append(CBoxSystem::getVersion(), CBoxSystem::getVersionLen());
				strMsg.Append("</a></small>\r\n</body></html>", 28);

				str.AppendFormat("Content-Length: %d\r\nContent-Type: text/html\r\n\r\n", strMsg.GetLength());
				str.Append(strMsg);
			}else str.Append("Content-Length: 100\r\nContent-Type: text/html\r\n\r\n");
		}else
			str.Append("Content-Length: 0\r\n\r\n", 21);
	}else
	{
		if(m_bBuffer)
		{
			str.Append("Content-Length: ", 16);
			_ltoa(m_nResponseContentLength, strContentLength, 10);
			str.Append(strContentLength);
			str.Append("\r\n\r\n", 4);
		}else str.Append("\r\n", 2);
	}

	m_pSocket->WriteBlock(str, str.GetLength());

	m_bSendHeader = TRUE;
}

BOOL CBoxHttpAccept::SendResponse(void)
{
	char buf[4096];
	int nSize;

	while(m_nContentPosition < m_nContentLength)
	{
		nSize = m_nContentLength - m_nContentPosition;
		if(nSize > sizeof(buf))
			nSize = sizeof(buf);

		if(m_pSocket->ReadBlock(buf, nSize) != nSize)
			break;

		m_nContentPosition += nSize;
	}

	if(!m_bSendHeader && !m_strProtocol.IsEmpty())
		SendHeader();

	if(m_fileResponse != NULL && m_nResponseContentLength != 0 && m_nMethod != HTTP_HEAD)
	{
		InterlockedIncrement(&((CBoxHttpServer*)m_pServer)->m_nDownloadWorkers);

		if(!IS_WINNT || m_pSocket->IsSSL())
		{
			if(m_fileResponse->IsKindOf(RUNTIME_CLASS(CMemFile)))
			{
				char *bufptr, *bufptr1;

				m_fileResponse->GetBufferPtr(CFile::bufferRead, 0, (void**)&bufptr, (void**)&bufptr1);
				m_pSocket->WriteBlock(bufptr + m_nRangeBegin, m_nResponseContentLength);

				m_nResponseContentLength = 0;
			}else
			{
				int n;

				m_fileResponse->Seek(m_nRangeBegin, CFile::begin);
				while(m_nResponseContentLength)
				{
					n = m_fileResponse->Read(buf, sizeof(buf));

					if(m_pSocket->WriteBlock(buf, n) != n)
						break;

					m_nResponseContentLength -= n;
					InterlockedExchangeAdd(&((CBoxHttpServer*)m_pServer)->m_ntotalBytes, n);
				}
			}

			InterlockedDecrement(&((CBoxHttpServer*)m_pServer)->m_nDownloadWorkers);
		}else
		{
			((CBoxHttpServer*)m_pServer)->SendFile(m_pSocket, m_fileResponse, m_nRangeBegin, m_nResponseContentLength, m_bKeepAlive);
			m_fileResponse = NULL;
			m_bKeepAlive = FALSE;
		}
	}

	return TRUE;
}

void CBoxHttpAccept::SetEncoding(LPCTSTR pstrContentEncoding)
{
	if(m_strAcceptEncoding.Find(CString(pstrContentEncoding).MakeLower()) == -1)return;

	static BYTE gz_magic[] = {0x1f, 0x8b, 8, 0, 0, 0, 0, 0, 0, 0x0F};

	if(!m_bBuffer)AfxThrowOleDispatchException(0x4005, _T("Buffering Off"), 0);
	if(m_fileResponse->GetLength())AfxThrowOleDispatchException(0x4005, _T("Buffer Not Empty"), 0);
	SetHeader(_T("Content-Encoding"), pstrContentEncoding);

	m_zipFile = new CZipFile;

	if(!_tcsicmp(pstrContentEncoding, _T("gzip")))
		m_fileResponse->Write(gz_magic, sizeof(gz_magic));
	m_zipFile->Open(m_fileResponse, CFile::modeWrite);
}

void CBoxHttpAccept::ClearAll(void)
{
	m_pHost.Release();
	m_pApplicationHost.Release();
	m_pCookies.Release();

	m_strUrl.Empty();
	m_strMethod.Empty();
	m_strMapedPath.Empty();
	m_strQueryString.Empty();
	m_strProtocol.Empty();
	m_strHostName.Empty();
	m_strUserAgent.Empty();
	m_nLocalPort = 80;
	m_strCookie.Empty();
	m_strBaseUrl.Empty();
	m_strMapToFile.Empty();
	ZeroMemory(&m_tmLastModify, sizeof(m_tmLastModify));

	m_nRangeBegin = 0,
	m_nRangeEnd = 0,

	m_bBuffer = TRUE;
	m_strAcceptEncoding.Empty();
	m_bSendHeader = FALSE;
	m_bDepth = FALSE;
	m_nMethod = HTTP_GET;

	if(m_zipFile)
	{
		delete m_zipFile;
		m_zipFile = NULL;
	}

	m_nContentLength = 0;
	m_nContentPosition = 0;
	m_strContentType.Empty();

	m_astrRequestHeaderName.SetSize(0, 1024);
	m_astrRequestHeaderValue.SetSize(0, 1024);

	if(m_fileResponse != NULL)
	{
		if(m_fileResponse != BOX_FOLDER)
			delete m_fileResponse;

		m_fileResponse = NULL;
	}

	m_mapResponseHeader.RemoveAll();
	m_strAddHeader.Empty();
	m_nStatusCode = 200;
	m_strStatus.Empty();
	m_nResponseContentLength = 0;
}
