#pragma once

#include "BThread.h"

class __declspec(uuid("94650000-0043-4255-4450-536F636B6574"))
CBUDPSocket : public CBDispatch<IUDPSocket>
{
	DECLARE_CLASS(CBUDPSocket)

public:
	CBUDPSocket(void);
	~CBUDPSocket(void);

	class __declspec(uuid("94659465-0000-0055-4450-5061636B6574"))
	CBUDPPacket : public CBDispatch<IUDPPacket>
	{
		DECLARE_CLASS(CBUDPPacket)

	public:
		// IUDPPacket
        STDMETHOD(get_Data)(VARIANT *pVal);
		STDMETHOD(get_LocalAddress)(BSTR *pVal);
		STDMETHOD(get_LocalPort)(LONG *pVal);
		STDMETHOD(get_RemoteAddress)(BSTR *pVal);
		STDMETHOD(get_RemotePort)(LONG *pVal);

	public:
		CComVariant m_varData;
		CBString m_strLocalAddr;
		long m_nLocalPort;
		CBString m_strRemoteAddr;
		long m_nRemotePort;
	};

	// IUDPSocket
	STDMETHOD(Bind)(BSTR strAddr, LONG port);
	STDMETHOD(SendTo)(BSTR strAddr, LONG port, VARIANT varData);
	STDMETHOD(RecvFrom)(IUDPPacket **pVal);
	STDMETHOD(Close)(void);

	STDMETHOD(get_LocalAddress)(BSTR *pVal);
	STDMETHOD(get_LocalPort)(LONG *pVal);
	STDMETHOD(get_PacketSize)(LONG *pVal);
	STDMETHOD(put_PacketSize)(LONG newVal);
	STDMETHOD(get_ReceiveTimeout)(LONG *pVal);
	STDMETHOD(put_ReceiveTimeout)(LONG newVal);
	STDMETHOD(get_BroadCast)(VARIANT_BOOL *pVal);
	STDMETHOD(put_BroadCast)(VARIANT_BOOL newVal);

private:
	void GetLocalInfo(void);
	HRESULT FillAddress(BSTR strAddr, LONG port, SOCKADDR_IN* sockAddr);

	CBCriticalSection m_cs;
	SOCKET m_hSocket;
	CBString m_strAddr;
	long m_nPort;

	CBCriticalSection m_csBuffer;
	long m_nPacketSize;
	CBAutoPtr<char> m_pBuffer;
};
