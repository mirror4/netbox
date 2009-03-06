#pragma once
#include "BoxScript.h"

class CBoxHttpScript : public CBoxScript
{
	DECLARE_DYNCREATE(CBoxHttpScript)
public:
	CBoxHttpScript(void)
	{}

	void SetBasePath(LPCTSTR pstrPath)
	{
		m_strHttpBasePath = pstrPath;
	}

	BOOL GetSessionState(void)
	{
		return m_pHost->m_arrayExtData[0];
	}

	void SetSessionState(BOOL bSessionState)
	{
		m_bSessionState = bSessionState;
	}

protected:
	virtual void InitScript(void);
	virtual int ParseScriptText(LPCSTR pstrText, int nCount, CStringA& strScriptText, int nIncludeFlagIndex);

	int AddBlock(LPCSTR pBuf, int nSize)
	{
		char* pBuffer = new char[nSize + sizeof(int)];

		if(pBuffer != NULL)
		{
			*(int*)pBuffer = nSize;
			memcpy(pBuffer + sizeof(int), pBuf, nSize);
			return (int)m_pHost->m_arrayBlock.Add(pBuffer);
		}

		return -1;
	}

private:
	CString m_strHttpBasePath;
	BOOL m_bSessionState;
};
