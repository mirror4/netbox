#include "StdAfx.h"
#include "BComm.h"
#include "BVarType.h"

	// IBaseStream
STDMETHODIMP CBComm::Attach(IUnknown *SrcStream)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBComm::Open(long nSize)
{
	return E_NOTIMPL;
}

	// ICommStream
HRESULT CBComm::InternalRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(!::ReadFile(m_hFile, pv, cb, pcbRead, NULL))
		return GetErrorResult();

	if(*pcbRead == 0)
		return HRESULT_FROM_WIN32(ERROR_TIMEOUT);

	return S_OK;
}

HRESULT CBComm::InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(!::WriteFile(m_hFile, pv, cb, pcbWritten, NULL))
		return GetErrorResult();

	if(*pcbWritten == 0)
		return HRESULT_FROM_WIN32(ERROR_TIMEOUT);

	return S_OK;
}

HRESULT CBComm::InternalEOS(VARIANT_BOOL *pEOS)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBComm::get_BaudRate(short* pVal)
{
	*pVal = (short)m_dcb.BaudRate;
	return S_OK;
}

STDMETHODIMP CBComm::put_BaudRate(short newVal)
{
	m_dcb.BaudRate = newVal;

	if(!SetCommState(m_hFile, &m_dcb))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::get_DataBits(short* pVal)
{
	*pVal = m_dcb.ByteSize;

	return S_OK;
}

STDMETHODIMP CBComm::put_DataBits(short newVal)
{
	if(newVal < 0 || newVal > 8)
		return E_INVALIDARG;

	m_dcb.ByteSize = (BYTE)newVal;

	if(!SetCommState(m_hFile, &m_dcb))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::get_Handshaking(short* pVal)
{
	*pVal = 0;

	if(m_dcb.fOutX || m_dcb.fInX)
		*pVal |= 1;

	if(m_dcb.fOutxCtsFlow)
		*pVal |= RTS_CONTROL_HANDSHAKE;

	return S_OK;
}

STDMETHODIMP CBComm::put_Handshaking(short newVal)
{
	if(newVal < 0 || newVal > 3)
		return E_INVALIDARG;

	m_dcb.fOutX = m_dcb.fInX = newVal & 1;
	m_dcb.fRtsControl = newVal & RTS_CONTROL_HANDSHAKE;
	m_dcb.fOutxCtsFlow = (newVal & RTS_CONTROL_HANDSHAKE) > 0;

	if(!SetCommState(m_hFile, &m_dcb))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::get_Parity(short* pVal)
{
	*pVal = m_dcb.fParity ? m_dcb.Parity : 0;

	return S_OK;
}

STDMETHODIMP CBComm::put_Parity(short newVal)
{
	if(newVal < NOPARITY || newVal > SPACEPARITY)
		return E_INVALIDARG;

	m_dcb.Parity = (BYTE)newVal;
	m_dcb.fParity = newVal != NOPARITY;

	if(!SetCommState(m_hFile, &m_dcb))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::get_ReadTimeout(long *pVal)
{
	COMMTIMEOUTS ct = {0};

	if(!GetCommTimeouts(m_hFile, &ct))
		return GetErrorResult();

	*pVal = ct.ReadTotalTimeoutConstant;

	return S_OK;
}

STDMETHODIMP CBComm::put_ReadTimeout(long pVal)
{
	COMMTIMEOUTS ct = {0};

	if(!GetCommTimeouts(m_hFile, &ct))
		return GetErrorResult();

	ct.ReadTotalTimeoutConstant = pVal;

	if(!SetCommTimeouts(m_hFile, &ct))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::get_StopBits(short* pVal)
{
	*pVal = m_dcb.StopBits;

	return S_OK;
}

STDMETHODIMP CBComm::put_StopBits(short newVal)
{
	if(newVal < ONESTOPBIT || newVal > TWOSTOPBITS)
		return E_INVALIDARG;

	m_dcb.StopBits = (BYTE)newVal;

	if(!SetCommState(m_hFile, &m_dcb))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::get_WriteTimeout(long *pVal)
{
	COMMTIMEOUTS ct = {0};

	if(!GetCommTimeouts(m_hFile, &ct))
		return GetErrorResult();

	*pVal = ct.WriteTotalTimeoutConstant;

	return S_OK;
}

STDMETHODIMP CBComm::put_WriteTimeout(long pVal)
{
	COMMTIMEOUTS ct = {0};

	if(!GetCommTimeouts(m_hFile, &ct))
		return GetErrorResult();

	ct.WriteTotalTimeoutConstant = pVal;

	if(!SetCommTimeouts(m_hFile, &ct))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBComm::Open(short lPort, VARIANT varMode)
{
	CBStringA strMode;

	Close();

	wsprintf(m_strName, "COM%d", lPort);

	m_hFile.m_h = ::CreateFileA(m_strName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile.m_h = NULL;
		return GetErrorResult();
	}

	COMMTIMEOUTS ct = {0};

	SetCommTimeouts(m_hFile, &ct);

	if(varMode.vt != VT_ERROR)
	{
		HRESULT hr = varGetString(varMode, strMode);
		if(FAILED(hr))return hr;
	}

	if(!GetCommState(m_hFile, &m_dcb))
		return GetErrorResult();

	if(!strMode.IsEmpty())
	{
		if(!BuildCommDCB(strMode, &m_dcb))
			return GetErrorResult();

		if(!SetCommState(m_hFile, &m_dcb))
			return GetErrorResult();
	}

	return S_OK;
}

STDMETHODIMP CBComm::Config(VARIANT varMode)
{
	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	CBStringA strMode;

	if(varMode.vt != VT_ERROR)
	{
		HRESULT hr = varGetString(varMode, strMode);
		if(FAILED(hr))return hr;
	}

	if(!strMode.IsEmpty())
	{
		if(!BuildCommDCB(strMode, &m_dcb))
			return GetErrorResult();

		if(!SetCommState(m_hFile, &m_dcb))
			return GetErrorResult();
	}else
	{
		COMMCONFIG cfg;

		ZeroMemory(&cfg, sizeof(cfg));
		cfg.dwSize = sizeof(cfg);
		cfg.dcb = m_dcb;

		if(CommConfigDialog(m_strName, ::GetForegroundWindow(), &cfg))
		{
			m_dcb = cfg.dcb;

			if(!SetCommState(m_hFile, &m_dcb))
				return GetErrorResult();
		}
	}

	return S_OK;
}

