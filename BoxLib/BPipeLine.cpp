#include "StdAfx.h"
#include "BPipeLine.h"

ULONG CBPipeLine::CBPipeDate::Read(void* pv, ULONG cb)
{
	if(!m_semRead.Wait())return 0;

	m_cs.Enter();

	if(!m_pBuffers.GetCount())
	{
		m_cs.Leave();
		m_semRead.Release();
		return 0;
	}

	ULONG ulmaxReadPos = m_pBuffers.GetCount() == 1 ? m_ulWritePos : MAX_PIPE_BLOCK_SIZE;

	if(cb > ulmaxReadPos - m_ulReadPos)
		cb = ulmaxReadPos - m_ulReadPos;

	CopyMemory(pv, m_pBuffers.GetHead() + m_ulReadPos, cb);

	m_ulReadPos += cb;
	if(m_ulReadPos == ulmaxReadPos)
	{
		delete m_pBuffers.RemoveHead();
		m_ulReadPos = 0;

		if(!m_pBuffers.GetCount())
			m_ulWritePos = MAX_PIPE_BLOCK_SIZE;

		m_semWrite.Release();
	}else m_semRead.Release();

	m_cs.Leave();

	return cb;
}

ULONG CBPipeLine::CBPipeDate::Write(void const* pv, ULONG cb)
{
	if(!m_semWrite.Wait())return 0;

	m_cs.Enter();

	if(m_ulWritePos == MAX_PIPE_BLOCK_SIZE)
	{
		if(m_pBuffers.GetCount() >= m_ulMaxBlock)
		{
			m_cs.Leave();
			m_semWrite.Release();
			return 0;
		}

		m_pBuffers.AddTail(new BYTE[MAX_PIPE_BLOCK_SIZE]);

		m_ulWritePos = 0;
		m_semRead.Release();
	}

	if(cb > MAX_PIPE_BLOCK_SIZE - m_ulWritePos)
		cb = MAX_PIPE_BLOCK_SIZE - m_ulWritePos;

	CopyMemory(m_pBuffers.GetTail() + m_ulWritePos, pv, cb);
	m_ulWritePos += cb;

	if(m_ulWritePos < MAX_PIPE_BLOCK_SIZE)
		m_semWrite.Release();

	m_cs.Leave();

	return cb;
}

ULONG CBPipeLine::CBPipeDate::dataAvailable(void)
{
	ULONG nSize = 0;

	m_cs.Enter();
	if(m_pBuffers.GetCount())
		nSize = (m_pBuffers.GetCount() - 1) * MAX_PIPE_BLOCK_SIZE + m_ulWritePos;
	m_cs.Leave();

	return nSize;
}

CBPipeLine::~CBPipeLine(void)
{
	Close();
}

	// IBaseStream
STDMETHODIMP CBPipeLine::Attach(IUnknown *SrcStream)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipeLine::Open(long nSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipeLine::Close(void)
{
	CBComPtr<CBPipeDate> m_pTemp;

	if(m_pTemp = m_pInput)
	{
		m_pTemp->Stop();
		m_pInput.Release();
	}

	if(m_pTemp = m_pOutput)
	{
		m_pTemp->Stop();
		m_pOutput.Release();
	}

	return CBStream::Close();
}

// IPipeLine
HRESULT CBPipeLine::InternalRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	CBComPtr<CBPipeDate> pTemp(m_pInput);

	if(!pTemp)return E_HANDLE;

	ULONG cbRead;

	if(pcbRead == NULL)pcbRead = &cbRead;

	*pcbRead = pTemp->Read(pv, cb);

	if(*pcbRead < cb)return S_FALSE;

	return S_OK;
}

HRESULT CBPipeLine::InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	CBComPtr<CBPipeDate> pTemp(m_pOutput);

	if(!pTemp)return E_HANDLE;

	ULONG cb1, cbWritten = 0;
	LPBYTE pv1 = (LPBYTE)pv;

	while(cb && (cb1 = pTemp->Write(pv1, cb)))
	{
		pv1 += cb1;
		cb -= cb1;
		cbWritten += cb1;
	}

	if(cb)
		return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

	if(pcbWritten)
		*pcbWritten = cbWritten;

	return S_OK;
}

HRESULT CBPipeLine::InternalEOS(VARIANT_BOOL *pEOS)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipeLine::Open(long nSize, IPipeLine** retVal)
{
	m_pInput.Attach(new CBPipeDate(nSize));
	m_pOutput.Attach(new CBPipeDate(nSize));

	CBComPtr<CBPipeLine> pPeer;

	pPeer.CreateInstance();

	pPeer->m_pInput = m_pOutput;
	pPeer->m_pOutput = m_pInput;

	*retVal = pPeer.Detach();

	return S_OK;
}

STDMETHODIMP CBPipeLine::get_dataAvailable(long *pVal)
{
	CBComPtr<CBPipeDate> pTemp(m_pInput);

	if(!pTemp)return E_HANDLE;

	*pVal = pTemp->dataAvailable();

	return S_OK;
}

