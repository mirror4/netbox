#pragma once
#include "afx.h"

class CBoxMemFile : public CMemFile
{
public:
	virtual void GrowFile(SIZE_T dwNewLen)
	{
		m_nGrowBytes = m_nBufferSize / 5;
		if(m_nGrowBytes < 1024)m_nGrowBytes = 1024;

		CMemFile::GrowFile(dwNewLen);
	}
};
