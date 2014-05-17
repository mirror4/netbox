#pragma once

#include "BoxLRUCache.h"

class CBoxCacheNode : public CBoxBaseObject,
					public CBoxLRUCacheNode
{
public:
	CBoxCacheNode() :
		m_pBuffer(NULL),
		m_nSize(0)
	{
	}

	~CBoxCacheNode(void)
	{
		if(m_pBuffer != NULL)
			delete m_pBuffer;
	}

public:
	BYTE* m_pBuffer;
	int m_nSize;
	SYSTEMTIME m_tm;
	CString m_strTime;
};

class CBoxCacheFile : public CMemFile
{
public:
	CBoxCacheFile(CBoxCacheNode* pNode)
	{
		m_pNode = pNode;

		Attach(m_pNode->m_pBuffer, m_pNode->m_nSize);
	}

	SYSTEMTIME GetFileTime(void)
	{
		return m_pNode->m_tm;
	}

	void GetFileTimeString(CString& strTime)
	{
		strTime = m_pNode->m_strTime;
	}

private:
	CBoxObject<CBoxCacheNode> m_pNode;
};
