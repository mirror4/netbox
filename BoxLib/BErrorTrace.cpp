#include "StdAfx.h"
#include "BErrorTrace.h"

CBErrorTrace::CBErrorTrace(void)
{
	m_pStream.CreateInstance();
	m_date.GetTime();
}

CBErrorTrace::~CBErrorTrace(void)
{
}

STDMETHODIMP CBErrorTrace::get_Name(BSTR *pVal)
{
	*pVal = m_strName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBErrorTrace::get_StartTime(DATE *pVal)
{
	*pVal = m_date;
	return S_OK;
}

void CBErrorTrace::Write(const void* pData, int size)
{
	CBString strDate;
	SYSTEMTIME st;

	GetSystemTime(&st);

	strDate.Format(L"[%02d-%02d %02d:%02d:%02d]", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	m_cs.Enter();
	m_pStream->Write((LPCWSTR)strDate, strDate.GetLength() * 2);
	m_pStream->Write(pData, size);
	m_pStream->Write(L"\r\n", 4);
	m_cs.Leave();
}

STDMETHODIMP CBErrorTrace::Write(BSTR strText)
{
	Write(strText, ::SysStringByteLen(strText));
	return S_OK;
}

STDMETHODIMP CBErrorTrace::ReadAll(BSTR *pVal)
{
	ULONG l;

	m_cs.Enter();

	*pVal = ::SysAllocStringByteLen(NULL, l = (ULONG)m_pStream->GetLength());

	m_pStream->SeekToBegin();
	m_pStream->Read(*pVal, l);

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBErrorTrace::Clear(void)
{
	m_cs.Enter();
	m_pStream.CreateInstance();
	Write(L"The log was cleared.", 40);
	m_cs.Leave();

	return S_OK;
}
