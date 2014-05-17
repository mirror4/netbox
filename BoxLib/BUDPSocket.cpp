#include "StdAfx.h"
#include "BVarType.h"
#include "BUDPSocket.h"

HRESULT CBUDPSocket::CBUDPPacket::get_Data(VARIANT *pVal)
{
	*(CComVariant*)pVal = m_varData;
	return S_OK;
}

HRESULT CBUDPSocket::CBUDPPacket::get_LocalAddress(BSTR *pVal)
{
	*pVal = m_strLocalAddr.AllocSysString();
	return S_OK;
}

HRESULT CBUDPSocket::CBUDPPacket::get_LocalPort(LONG *pVal)
{
	*pVal = m_nLocalPort;
	return S_OK;
}

HRESULT CBUDPSocket::CBUDPPacket::get_RemoteAddress(BSTR *pVal)
{
	*pVal = m_strRemoteAddr.AllocSysString();
	return S_OK;
}

HRESULT CBUDPSocket::CBUDPPacket::get_RemotePort(LONG *pVal)
{
	*pVal = m_nRemotePort;
	return S_OK;
}

CBUDPSocket::CBUDPSocket(void) : m_nPort(0)
{
	m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);

	m_nPacketSize = 4096;
	m_pBuffer.Allocate(m_nPacketSize);
}

CBUDPSocket::~CBUDPSocket(void)
{
	closesocket(m_hSocket);
}

void CBUDPSocket::GetLocalInfo(void)
{
	SOCKADDR_IN sockAddr;
	int SockLen;

	SockLen = sizeof(SOCKADDR);
	getsockname(m_hSocket, (SOCKADDR*)&sockAddr, &SockLen);

	m_cs.Enter();
	m_strAddr.Format(L"%d.%d.%d.%d",
		sockAddr.sin_addr.S_un.S_un_b.s_b1,
		sockAddr.sin_addr.S_un.S_un_b.s_b2,
		sockAddr.sin_addr.S_un.S_un_b.s_b3,
		sockAddr.sin_addr.S_un.S_un_b.s_b4);
	m_nPort = ntohs(sockAddr.sin_port);
	m_cs.Leave();
}

HRESULT CBUDPSocket::FillAddress(BSTR strAddr, LONG port, SOCKADDR_IN* psockAddr)
{
	ZeroMemory(psockAddr, sizeof(SOCKADDR_IN));

	CStringA str(strAddr);
	str.Trim();

	psockAddr->sin_family = AF_INET;

	if(str.GetLength())
	{
		psockAddr->sin_addr.s_addr = inet_addr(str);

		if (psockAddr->sin_addr.s_addr == INADDR_NONE)
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(str);
			if (lphost == NULL)return GetErrorResult();

			psockAddr->sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
	}

	psockAddr->sin_port = htons((u_short)port);

	return S_OK;
}

STDMETHODIMP CBUDPSocket::Bind(BSTR strAddr, LONG port)
{
	SOCKADDR_IN sockAddr;
	HRESULT hr = FillAddress(strAddr, port, &sockAddr);
	if(FAILED(hr))return hr;

	if(bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)))
		return GetErrorResult();

	GetLocalInfo();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::SendTo(BSTR strAddr, LONG port, VARIANT varData)
{
	SOCKADDR_IN sockAddr;
	HRESULT hr = FillAddress(strAddr, port, &sockAddr);
	if(FAILED(hr))return hr;

	CBVarPtr varPtr;

	hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	if(sendto(m_hSocket, (const char *)varPtr.m_pData, varPtr.m_nSize, 0, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
		return GetErrorResult();

	if(!m_nPort)GetLocalInfo();
	return S_OK;
}

STDMETHODIMP CBUDPSocket::RecvFrom(IUDPPacket **pVal)
{
	CBComPtr<CBUDPPacket> pPacket;
	CBVarPtr varPtr;
	SOCKADDR_IN sockAddr;
	HRESULT hr;
	int len, sockLen;

	ZeroMemory(&sockAddr, sizeof(sockAddr));

	CBLock l(&m_csBuffer);

	sockLen = sizeof(sockAddr);
	len = recvfrom(m_hSocket, m_pBuffer, m_nPacketSize, 0, (SOCKADDR*)&sockAddr, &sockLen);
	if(len == SOCKET_ERROR)return GetErrorResult();

	hr = varPtr.Create(len);
	if(FAILED(hr))return hr;

	CopyMemory(varPtr.m_pData, m_pBuffer, len);

	l.Unlock();

	pPacket.CreateInstance();

	hr = varPtr.GetVariant(pPacket->m_varData);
	if(FAILED(hr))return hr;

	m_cs.Enter();
	pPacket->m_strLocalAddr = m_strAddr;
	pPacket->m_nLocalPort = m_nPort;
	m_cs.Leave();

	pPacket->m_strRemoteAddr.Format(L"%d.%d.%d.%d",
		sockAddr.sin_addr.S_un.S_un_b.s_b1,
		sockAddr.sin_addr.S_un.S_un_b.s_b2,
		sockAddr.sin_addr.S_un.S_un_b.s_b3,
		sockAddr.sin_addr.S_un.S_un_b.s_b4);
	pPacket->m_nRemotePort = ntohs(sockAddr.sin_port);

	*pVal = pPacket.Detach();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::Close(void)
{
	closesocket(m_hSocket);
	m_hSocket = SOCKET_ERROR;

	return S_OK;
}

STDMETHODIMP CBUDPSocket::get_LocalAddress(BSTR *pVal)
{
	m_cs.Enter();
	*pVal = m_strAddr.AllocSysString();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::get_LocalPort(LONG *pVal)
{
	m_cs.Enter();
	*pVal = m_nPort;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::get_PacketSize(LONG *pVal)
{
	m_csBuffer.Enter();
	*pVal = m_nPacketSize;
	m_csBuffer.Leave();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::put_PacketSize(LONG newVal)
{
	if(newVal <= 0 || newVal > 65520)
		return E_INVALIDARG;

	m_csBuffer.Enter();
	m_nPacketSize = newVal;
	m_pBuffer.Allocate(m_nPacketSize);
	m_csBuffer.Leave();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::get_ReceiveTimeout(LONG *pVal)
{
	int len = sizeof(LONG);

	if(getsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)pVal, &len) == SOCKET_ERROR)
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::put_ReceiveTimeout(LONG newVal)
{
	if(setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&newVal, sizeof(LONG)) == SOCKET_ERROR)
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBUDPSocket::get_BroadCast(VARIANT_BOOL *pVal)
{
	int len = sizeof(BOOL);
	BOOL bRet;

	if(getsockopt(m_hSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bRet, &len) == SOCKET_ERROR)
		return GetErrorResult();

	*pVal = bRet ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBUDPSocket::put_BroadCast(VARIANT_BOOL newVal)
{
	BOOL bVal = (newVal != VARIANT_FALSE);

	if(setsockopt(m_hSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bVal, sizeof(BOOL)) == SOCKET_ERROR)
		return GetErrorResult();

	return S_OK;
}

