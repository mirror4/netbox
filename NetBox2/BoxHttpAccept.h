#pragma once

#include "BoxHttpHost.h"
#include "BoxHttpServer.h"
#include "BoxFixString.h"
#include "ZipFile.h"
#include <BRequestDictionary.h>

// CBoxHttpAccept command target

class CBoxHttpAccept : public CBoxJobWorker
{
public:
	CBoxHttpAccept();

	int WriteToClient(const char *pbuf, int len)
	{
		if(len > 0)
			if(m_bBuffer)
				if(m_zipFile)m_zipFile->Write(pbuf, len);
				else m_fileResponse->Write(pbuf, len);
			else
			{
				if(!m_bSendHeader && !m_strProtocol.IsEmpty())
					SendHeader();

				return m_pSocket->WriteBlock(pbuf, len);
			}

		return len;
	}

	void Clear(void)
	{
		if(!m_bBuffer)
			AfxThrowOleDispatchException(0x4005, _T("Buffering Off"), 0);

		if(m_zipFile)
		{
			m_zipFile->Close();
			delete m_zipFile;
			m_zipFile = NULL;
			m_mapResponseHeader.RemoveKey(_T("Content-Encoding"));
		}

		m_fileResponse->SetLength(0);
	}

	void Flush(void)
	{
		if(!m_bBuffer)
			AfxThrowOleDispatchException(0x4005, _T("Buffering Off"), 0);
	}

	CString GetRequestHeader(LPCTSTR pstrName)
	{
		CString str;

		for(int i = 0; i < m_astrRequestHeaderName.GetCount(); i ++)
			if(!m_astrRequestHeaderName[i].CompareNoCase(pstrName))
				return m_astrRequestHeaderValue[i];

		return str;
	}

	void SetHeader(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if(m_bSendHeader)
			AfxThrowOleDispatchException(0x4005, _T("Header Error"), 0);

		if(*pstrValue)
			m_mapResponseHeader[pstrName] = pstrValue;
		else m_mapResponseHeader.RemoveKey(pstrName);
	}

	void AddHeader(LPCTSTR pstrHeader)
	{
		if(m_bSendHeader)
			AfxThrowOleDispatchException(0x4005, _T("Header Error"), 0);

		m_strAddHeader.Append(pstrHeader);
	}

	void AddHeader(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if(m_bSendHeader)
			AfxThrowOleDispatchException(0x4005, _T("Header Error"), 0);

		m_strAddHeader.Append(pstrName);
		m_strAddHeader.Append(_T(": "), 2);
		m_strAddHeader.Append(pstrValue);
		m_strAddHeader.Append(_T("\r\n"), 2);
	}

	void SetStatus(LPCTSTR pstrStatus)
	{
		if(m_bSendHeader)
			AfxThrowOleDispatchException(0x4005, _T("Header Error"), 0);
		m_strStatus = pstrStatus;
	}

	BOOL get_Buffer(void)
	{
		return m_bBuffer;
	}

	void put_Buffer(BOOL bBuffer)
	{
		if(!bBuffer && m_fileResponse->GetLength())
			AfxThrowOleDispatchException(0x4005, _T("Buffering On"), 0);

		if(bBuffer && m_bSendHeader)
			AfxThrowOleDispatchException(0x4005, _T("Header Error"), 0);

		m_bBuffer = bBuffer;
		if(!bBuffer)m_bKeepAlive = FALSE;
	}

	void SetEncoding(LPCTSTR pstrContentEncoding);

	long GetContentLength(void)
	{
		return m_nContentLength;
	}

	BSTR GetContentType(void)
	{
		return m_strContentType.AllocSysString();
	}

	long ReadFromClient(char *pbuf, int len)
	{
		if(len > 0)
		{
			if(len > m_nContentLength - m_nContentPosition)
				len = m_nContentLength - m_nContentPosition;

			len = m_pSocket->ReadBlock(pbuf, len);
			if(len != SOCKET_ERROR)
				m_nContentPosition += len;
			else len = 0;
		}

		return len;
	}

	BOOL IsClientConnected()
	{
		return !m_pSocket->get_Eof();
	}

	CString& GetBasePath(void)
	{
		return m_pHost->m_strRoot;
	}

	LPCTSTR GetMapedPath(void)
	{
		return m_strMapedPath;
	}

	LPCTSTR GetUserAgent(void)
	{
		return m_strUserAgent;
	}

	void MapToFile(LPCTSTR pstrFile)
	{
		m_strMapToFile = pstrFile;
	}

	CBoxObject<CBoxApplication>& GetApplication(void)
	{
		return m_pApplicationHost->m_pApplication;
	}

	CBRequestDictionary* GetCookie()
	{
		if(m_pCookies == NULL)
		{
			m_pCookies.CreateInstance();
			m_pCookies->ParseCookieString(m_strCookie, m_strCookie.GetLength());
		}

		return m_pCookies;
	}
	void FillForm(CBRequestDictionary* pForm);
	void FillQueryString(CBRequestDictionary* pQueryString)
	{
		pQueryString->ParseUrlEncodeString(m_strQueryString, m_strQueryString.GetLength());
	}

	void FillServerVariables(CBRequestDictionary* pServerVariables);

	CBoxTimeCache::CCache m_time;
	CBoxObject<CBoxSocket> m_pSocket;

protected:
	virtual long OnAccept(void);

private:
	int ReadRequest(void);
	void ReadHeader(void);
	BOOL MapRequestPath(BOOL bUTF8 = FALSE);
	void BuildInfomation(void);
	int BuildOptions(void);
	int BuildFile(void);
	int BuildDirectory(void);
	int FindProperty(void);
	int DeleteContent(void);
	int PutContent(void);
	int MoveContent(BOOL bCopy = FALSE);
	int CreateFolder(void);
//	int ProdInfo(void);
	int BuildResponse(void);
	void SendHeader();
	BOOL SendResponse(void);
	void ClearAll(void);

private:
	CBoxObject<CBoxHttpHost> m_pHost;
	CBoxObject<CBoxHttpHost> m_pApplicationHost;

	CBoxFixString<16> m_strProtocol;
	CBoxFixString<128> m_strHostName;
	CBoxFixString<128> m_strUserAgent;
	int m_nLocalPort;
	CBoxFixString<4096> m_strUrl;
	CString m_strPathInfo;
	CBoxFixString<4096> m_strMapedPath;
	CBoxFixString<4096> m_strQueryString;
	CString m_strCookie;
	CString m_strBaseUrl;
	CString m_strMapToFile;
	SYSTEMTIME m_tmLastModify;
	CBComPtr<CBRequestDictionary> m_pCookies;

	enum HttpMethod
	{
		HTTP_GET = 0,
		HTTP_POST,
		HTTP_HEAD,
		HTTP_OPTIONS,
		HTTP_PROPFIND,
		HTTP_DELETE,
		HTTP_PUT,
		HTTP_MOVE,
		HTTP_COPY,
		HTTP_MKCOL,
//		HTTP_PRODINFO,
		HTTP_UNKNOWN
	};

	HttpMethod m_nMethod;
	CBoxFixString<16> m_strMethod;
	int m_nAgentVersion;

	BOOL m_bIsDirectory;
	int m_nRangeBegin;
	int m_nRangeEnd;
	int m_nContentLength;
	int m_nContentPosition;
	CBoxFixString<128> m_strContentType;
	BOOL m_bKeepAlive;
	BOOL m_bSendHeader;
	BOOL m_bBuffer;
	BOOL m_bDepth;

	CBoxFixString<32> m_strAcceptEncoding;
	CZipFile *m_zipFile;

	CStringArray m_astrRequestHeaderName;
	CStringArray m_astrRequestHeaderValue;

	int m_nStatusCode;
	CBoxFixString<128> m_strStatus;
	int m_nResponseContentLength;
	CFile* m_fileResponse;
	CMapStringToString m_mapResponseHeader;
	CBoxFixString<4096> m_strAddHeader;
};
