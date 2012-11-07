#pragma once
#include "BoxFile.h"

class CBoxZipFile : public CBoxFile
{
public:
	CBoxZipFile(void);

public:
	virtual CFile* Open(LPCTSTR pstrName);
	virtual SYSTEMTIME GetFileTime(CFile* pFile);
	int ExtractFile(LPCTSTR pstrName, LPCTSTR pstrDest);

private:
	CCriticalSection m_cs;
	CMapStringToPtr m_mapIndex;
	CBoxSafeFile m_fileMe;
	SYSTEMTIME m_tm;
};

