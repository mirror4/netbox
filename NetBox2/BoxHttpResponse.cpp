// BoxHttpResponse.cpp : implementation file
//

#include "stdafx.h"
#include "BoxString.h"
#include "BoxHttpResponse.h"
#include "ScriptHost.h"
#include <atlutil.h>
#include "BoxBinPtr.h"

// CBoxHttpResponse

IMPLEMENT_DYNAMIC(CBoxHttpResponse, CBoxStream)

HRESULT CBoxHttpResponse::StreamWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(m_pAccept->WriteToClient((const char*)pv, cb) != cb)
		return E_ABORT;

	if(pcbWritten != NULL)
		*pcbWritten = cb;
	return S_OK;
}

BEGIN_DISPATCH_MAP(CBoxHttpResponse, CBoxStream)
	DISP_FUNCTION(CBoxHttpResponse, "Write", Write, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxHttpResponse, "WriteBlock", WriteBlock, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CBoxHttpResponse, "Redirect", Redirect, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpResponse, "MapToFile", MapToFile, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpResponse, "AddHeader", AddHeader, VT_EMPTY, VTS_BSTR VTS_BSTR)

	DISP_PROPERTY_EX(CBoxHttpResponse, "Cookies", get_Cookies, SetNotSupported, VT_DISPATCH)

	DISP_PROPERTY_EX(CBoxHttpResponse, "Buffer", get_Buffer, put_Buffer, VT_BOOL)

	DISP_PROPERTY_EX(CBoxHttpResponse, "CacheControl", GetNotSupported, put_CacheControl, VT_BSTR)
	DISP_PROPERTY_EX(CBoxHttpResponse, "ContentType", GetNotSupported, put_ContentType, VT_BSTR)
	DISP_PROPERTY_EX(CBoxHttpResponse, "ContentEncoding", GetNotSupported, put_ContentEncoding, VT_BSTR)
	DISP_PROPERTY_EX(CBoxHttpResponse, "Expires", GetNotSupported, put_Expires, VT_I4)
	DISP_PROPERTY_EX(CBoxHttpResponse, "ExpiresAbsolute", GetNotSupported, put_ExpiresAbsolute, VT_DATE)
	DISP_PROPERTY_EX(CBoxHttpResponse, "Status", GetNotSupported, put_Status, VT_BSTR)

	DISP_FUNCTION(CBoxHttpResponse, "AppendToLog", AppendToLog, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxHttpResponse, "BinaryWrite", BinaryWrite, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxHttpResponse, "Clear", Clear, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxHttpResponse, "End", End, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxHttpResponse, "Flush", Flush, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxHttpResponse, "PICS", PICS, VT_EMPTY, VTS_BSTR)

	DISP_FUNCTION(CBoxHttpResponse, "IsClientConnected", IsClientConnected, VT_BOOL, VTS_NONE)

	DISP_PROPERTY_EX(CBoxHttpResponse, "Socket", get_Socket, SetNotSupported, VT_DISPATCH)
END_DISPATCH_MAP()

// CBoxHttpResponse message handlers

LPDISPATCH CBoxHttpResponse::get_Cookies(void)
{
	if(m_pCookies == NULL)
	{
		m_pCookies.CreateInstance();
		m_pCookies->m_pReadCookies = m_pAccept->GetCookie();
	}
	m_pCookies->AddRef();
	return m_pCookies;
}

BOOL CBoxHttpResponse::get_Buffer(void)
{
	return m_pAccept->get_Buffer();
}

void CBoxHttpResponse::put_Buffer(BOOL bBuffer)
{
	m_pAccept->put_Buffer(bBuffer);
}

void CBoxHttpResponse::put_CacheControl(LPCTSTR pstrCacheControl)
{
	m_pAccept->SetHeader(_T("Cache-control"), pstrCacheControl);
}

void CBoxHttpResponse::put_ContentType(LPCTSTR pstrContentType)
{
	m_pAccept->SetHeader(_T("Content-Type"), pstrContentType);
}

void CBoxHttpResponse::put_Expires(long nTime)
{
	FILETIME ft;
	SYSTEMTIME st;
	CString strTime;
	CBoxObject<CBoxTimeCache::CTime> pTime;

	m_pAccept->m_time.GetTime(pTime);
	*(__int64*)&ft = pTime->m_time + (__int64)600000000 * nTime;
	pTime.Release();

	FileTimeToSystemTime(&ft, &st);
	TimeToString(st, strTime);

	m_pAccept->SetHeader(_T("Expires"), strTime);
}

void CBoxHttpResponse::put_ExpiresAbsolute(DATE dateExpires)
{
	SYSTEMTIME st;
	CString strTime;

	VariantTimeToSystemTime(dateExpires, &st);
	TimeToString(st, strTime);

	m_pAccept->SetHeader(_T("Expires"), strTime);
}

void CBoxHttpResponse::put_Status(LPCTSTR pstrStatus)
{
	m_pAccept->SetStatus(pstrStatus);
}

void CBoxHttpResponse::AddHeader(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	m_pAccept->AddHeader(pstrName, pstrValue);
}

void CBoxHttpResponse::AppendToLog(LPCTSTR pstrLog)
{
}

void CBoxHttpResponse::BinaryWrite(VARIANT& varData)
{
	CBoxBinPtr varPtr(varData);

	m_pAccept->WriteToClient(varPtr, varPtr.m_nSize);
}

void CBoxHttpResponse::Clear(void)
{
	m_pAccept->Clear();
}

void CBoxHttpResponse::End(void)
{
	CScriptHost* pNowScript = CScriptHost::GetCurrentScript();

	if(pNowScript != NULL)
		pNowScript->Stop(0);

	m_bEnd = TRUE;
}

void CBoxHttpResponse::Flush(void)
{
	m_pAccept->Flush();
}

void CBoxHttpResponse::PICS(LPCTSTR pstrPICSLabel)
{
	m_pAccept->SetHeader(_T("PICS-label"), pstrPICSLabel);
}

void CBoxHttpResponse::Redirect(LPCTSTR pstrUrl)
{
	put_Status(_T("302 Object moved"));
	m_pAccept->AddHeader(_T("Location"), pstrUrl);
	Clear();
	End();
}

void CBoxHttpResponse::MapToFile(LPCTSTR pstrFile)
{
	m_pAccept->MapToFile(pstrFile);

	Clear();
	End();
}

void CBoxHttpResponse::Write(VARIANT& var)
{
	while(var.vt == (VT_VARIANT | VT_BYREF))
		var = *var.pvarVal;

	if(var.vt == VT_BSTR)
	{
		CStringA str(var.bstrVal);

		m_pAccept->WriteToClient(str, str.GetLength());
	}else if(var.vt & VT_ARRAY)
		m_pAccept->WriteToClient("[Array]", 7);
	else if(var.vt == VT_UNKNOWN)
		m_pAccept->WriteToClient("[Object]", 8);
	else
	{
		CComVariant varStr;

		::VariantChangeType(&varStr, &var, VARIANT_ALPHABOOL, VT_BSTR);

		if(varStr.vt == VT_BSTR)
		{
			CStringA str(varStr.bstrVal);

			m_pAccept->WriteToClient(str, str.GetLength());
		}
	}
}

void CBoxHttpResponse::WriteBlock(long id)
{
	char* pBlock;
	int nSize;

	pBlock = (char *)m_pScript->GetBlock(id);
	if(pBlock != NULL)
	{
		nSize = *(int*)pBlock;
		pBlock += sizeof(int);

		m_pAccept->WriteToClient(pBlock, nSize);
	}
}

BOOL CBoxHttpResponse::IsClientConnected(void)
{
	return m_pAccept->IsClientConnected();
}

void CBoxHttpResponse::put_ContentEncoding(LPCTSTR pstrContentEncoding)
{
	if(_tcsicmp(pstrContentEncoding, _T("gzip")) && _tcsicmp(pstrContentEncoding, _T("deflate")))
		AfxThrowOleDispatchException(0x4005, _T("Not Support"), 0);
	m_pAccept->SetEncoding(pstrContentEncoding);
}

LPDISPATCH CBoxHttpResponse::get_Socket(void)
{
	m_pAccept->m_pSocket->ExternalAddRef();
	return m_pAccept->m_pSocket;
}
