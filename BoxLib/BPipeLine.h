#pragma once

#include "BStream.h"

#define MAX_PIPE_BLOCK_SIZE	1024

class __declspec(uuid("94650000-0000-0042-5069-70654C696E65"))
CBPipeLine : public CBDispatch<IPipeLine, CBStream>
{
private:
	class CBPipeDate : public CBObject
	{
	public:
		CBPipeDate(UINT size)
		{
			m_ulMaxBlock = (size + MAX_PIPE_BLOCK_SIZE - 1) / MAX_PIPE_BLOCK_SIZE;

			m_semRead.Create(0, m_ulMaxBlock + 1);
			m_semWrite.Create(m_ulMaxBlock, m_ulMaxBlock + 1);

			m_ulWritePos = MAX_PIPE_BLOCK_SIZE;
			m_ulReadPos = 0;
		}

		~CBPipeDate()
		{
			while(m_pBuffers.GetCount())
				delete m_pBuffers.RemoveHead();
		}

	public:
		ULONG Read(void* pv, ULONG cb);
		ULONG Write(void const* pv, ULONG cb);
		ULONG dataAvailable(void);
		void Stop()
		{
			m_semRead.Release();
			m_semWrite.Release();
		}

	private:
		CAtlList< LPBYTE > m_pBuffers;
		ULONG m_ulWritePos, m_ulReadPos, m_ulMaxBlock;
		CBCriticalSection m_cs;
		CBSemaphore m_semRead, m_semWrite;
	};

	DECLARE_CLASS_EX(CBPipeLine, CBStream)

public:
	~CBPipeLine(void);

public:
	// IBaseStream
	STDMETHOD(Attach)(IUnknown *SrcStream);
	STDMETHOD(Open)(long nSize);
	STDMETHOD(Close)(void);

public:
	// IPipeLine
	STDMETHOD(Open)(long nSize, IPipeLine** retVal);
	STDMETHOD(get_dataAvailable)(long *pVal);

protected:
	virtual HRESULT InternalRead(void *pv, ULONG cb, ULONG *pcbRead);
	virtual HRESULT InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten);
	virtual HRESULT InternalEOS(VARIANT_BOOL *pEOS);

private:
	CBComPtr<CBPipeDate> m_pInput;
	CBComPtr<CBPipeDate> m_pOutput;
};
