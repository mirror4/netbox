#pragma once

class CBoxPath
{
public:
	CBoxPath(void)
	{
		m_strPath = new TCHAR[MAX_PATH];
		m_strPath[0] = 0;
	}

	~CBoxPath(void)
	{
		delete m_strPath;
	}

	void Combine(LPCTSTR pstrBasePath, LPCTSTR pstrFile = NULL)
	{
		int nCount = 0;

		while(pstrBasePath != NULL || pstrFile != NULL)
		{
			if(pstrBasePath == NULL)
			{
				pstrBasePath = pstrFile;
				pstrFile = NULL;
			}

			while(*pstrBasePath && nCount < MAX_PATH - 1)
			{
				while(*pstrBasePath &&
					(*pstrBasePath == _T('\\') || *pstrBasePath == _T('/') ||
					(*pstrBasePath == _T('.') && (pstrBasePath[1] == _T('\\') || pstrBasePath[1] == _T('/')))))
					pstrBasePath ++;

				if(*pstrBasePath == _T('.') && pstrBasePath[1] == _T('.') &&
					(pstrBasePath[2] == _T('\\') || pstrBasePath[2] == _T('/')))
				{
					while(nCount > 0 && m_strPath[nCount - 1] != _T('\\'))
						nCount --;
					pstrBasePath += 3;
					if(nCount > 0)
						nCount --;
					continue;
				}

				if(*pstrBasePath)
				{
					m_strPath[nCount ++] = _T('\\');
					while(nCount < MAX_PATH - 1 && *pstrBasePath &&
						*pstrBasePath != _T('\\') && *pstrBasePath != _T('/'))
						m_strPath[nCount ++] = *pstrBasePath ++;
				}else if(pstrFile == NULL)
					m_strPath[nCount ++] = _T('\\');
			}

			pstrBasePath = pstrFile;
			pstrFile = NULL;
		}

		m_strPath[nCount] = 0;
	}

	void toLower(void)
	{
		_tcslwr(m_strPath);
	}

	LPTSTR m_strPath;
};
