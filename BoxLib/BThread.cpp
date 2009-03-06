#include "StdAfx.h"
#include "BThread.h"

static __declspec(thread) CBThread* th_pThread;
static CBTLSBase* s_pTlsLink;
static UINT s_nTlsSize;

CBTLSBase::CBTLSBase(void) : m_pNext(s_pTlsLink), m_nPos(s_nTlsSize)
{
	s_pTlsLink = this;
}

void CBTLSBase::Init(CBThread* pThread)
{
}

void CBTLSBase::Exit(CBThread* pThread)
{
}

void CBTLSBase::Alloc(UINT nSize)
{
	s_nTlsSize += nSize;
}

void* CBTLSBase::GetPtr(CBThread* pThread) const throw()
{
	if(!pThread)pThread = CBThread::GetThread();

	if(!pThread)return NULL;

	return &pThread->m_pTLS[m_nPos];
}

CBThread::CBThread(void)
{
	th_pThread = this;
	m_pTLS.Allocate(m_nSize = s_nTlsSize);
	ZeroMemory(m_pTLS, m_nSize);

	CBTLSBase* m_pLink = s_pTlsLink;

	while(m_pLink)
	{
		m_pLink->Init(this);
		m_pLink = m_pLink->m_pNext;
	}
}

CBThread::~CBThread(void)
{
	th_pThread = NULL;
	CBTLSBase* m_pLink = s_pTlsLink;

	while(m_pLink)
	{
		m_pLink->Exit(this);
		m_pLink = m_pLink->m_pNext;
	}
}

CBThread* CBThread::GetThread(void)
{
	return th_pThread;
}

CBThread** CBThread::GetThreadID(void)
{
	return &th_pThread;
}
