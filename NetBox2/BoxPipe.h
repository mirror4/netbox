
#pragma once

#include "BoxStream.h"
#include <afxtempl.h>

// CBoxPipe command target

class CBoxPipe : public CBoxStream
{
	DECLARE_DYNCREATE(CBoxPipe)

public:
	CBoxPipe();
	virtual ~CBoxPipe();

public:
	DECLARE_DISPATCH_MAP()

private:
	class CPipeNode
	{
	public:
		int m_nSize;
		char m_buf[1024];
	};

	int m_nPos;
    CList<CPipeNode*> m_listBuffers;
	HANDLE m_hLockRead;
	HANDLE m_hLockWrite;
	CCriticalSection m_csBuffer;
	CCriticalSection m_csLockRead;
};

