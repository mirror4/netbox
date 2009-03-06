#pragma once

#include <atlbase.h>

class CBPath
{
private:
	BOOL inline isBackslash(const WCHAR ch) const
	{
		return ch == '\\' || ch == '/';
	}

	char inline chBackslash(void)
	{
		return m_bUrl ? '/' : '\\';
	}

public:
	CBPath() throw() :
		m_bUrl(FALSE),
		m_nRoot(1),
		m_strPath( L"\\" )
	{
	}

	CBPath( const CBPath& path ) :
		m_bUrl(path.m_bUrl),
		m_nRoot(path.m_nRoot),
		m_strPath( path.m_strPath )
	{
	}

	CBPath( PCWSTR pszPath ) :
		m_bUrl(FALSE),
		m_nRoot(1),
		m_strPath( L"\\" )
	{
		Append(pszPath);
	}

	CBPath( PCSTR pszPath ) :
		m_bUrl(FALSE),
		m_nRoot(1),
		m_strPath( L"\\" )
	{
		Append(pszPath);
	}

	operator const CBString& () const throw()
	{
		return m_strPath;
	}

	operator const CBString () const throw()
	{
		return m_strPath;
	}

	operator PCWSTR() const throw()
	{
		return m_strPath;
	}

	void Clear()
	{
		m_bUrl = FALSE;
        m_nRoot = 1;
        m_strPath = L"\\";
	}

	CBPath& operator=( PCWSTR pszPath )
	{
		Clear();
		Append( pszPath );

		return *this;
	}

	CBPath& operator=( PCSTR pszPath )
	{
		Clear();
		Append( pszPath );

		return *this;
	}

	CBPath& operator=( const CBPath& path )
	{
		m_bUrl = path.m_bUrl;
		m_strPath = path.m_strPath;
		return *this;
	}

	CBPath& operator+=( PCWSTR pszMore )
	{
		Append( pszMore );

		return *this;
	}

	CBPath& operator+=( PCSTR pszMore )
	{
		Append( pszMore );

		return *this;
	}

	BSTR AllocSysString() const
	{
		return m_strPath.AllocSysString();
	}

	void AddBackslash()
	{
		size_t iLen = m_strPath.GetLength();

		if (iLen && iLen < MAX_PATH && !isBackslash(((PCWSTR)m_strPath)[iLen - 1]))
			m_strPath.AppendChar(chBackslash());
	}

	void RemoveBackslash()
	{
		int iLen = m_strPath.GetLength();

		if (iLen > m_nRoot && isBackslash(((PCWSTR)m_strPath)[iLen - 1]))
			m_strPath.Delete(iLen - 1);
	}

	void AddExtension( PCWSTR pszExtension )
	{
		m_strPath.Append(pszExtension);
	}

	void Append( PCWSTR pszMore )
	{
		while(*pszMore == ' ')
			pszMore ++;

		size_t iLen = m_strPath.GetLength();

		PWSTR pszBuffer, pszDsc, pszRoot;
		PCWSTR pszSrc;

		pszBuffer = m_strPath.GetBuffer( (int)(iLen + wcslen( pszMore ) + 1) );
		pszDsc = pszBuffer;
		pszRoot = pszBuffer + m_nRoot;

		if(isBackslash(pszMore[0]))
		{
			if(isBackslash(pszMore[1]))
			{
				*pszDsc ++ = chBackslash();
				*pszDsc ++ = chBackslash();
				pszSrc = pszMore + 2;

				while(*pszSrc == ' ' || *pszSrc == '.')
					pszSrc ++;

				while(*pszSrc && !isBackslash(*pszSrc))
					*pszDsc ++ = *pszSrc ++;

				while(*(pszDsc - 1) == ' ' || *(pszDsc - 1) == '.')
					pszDsc --;

				*pszDsc ++ = chBackslash();
				pszRoot = pszDsc;
				m_nRoot = (int)(pszRoot - pszBuffer);

				while(isBackslash(*pszSrc))
					pszSrc ++;
			}else
			{
				pszDsc = pszRoot;
				pszSrc = pszMore + 1;
			}
		}else
		{
			pszSrc = pszMore;
			while(*pszSrc && !isBackslash(*pszSrc) && *pszSrc != ':')
				pszSrc ++;

			if(*pszSrc == ':')
			{
				pszSrc ++;

				CopyMemory(pszDsc, pszMore, (int)(pszSrc - pszMore) * 2);
				pszDsc += pszSrc - pszMore;

				if(pszSrc - pszMore > 2)
				{
					m_bUrl = TRUE;

					if(isBackslash(*pszSrc))
					{
						pszSrc ++;
						*pszDsc ++ = chBackslash();
					}

					if(isBackslash(*pszSrc))
					{
						pszSrc ++;
						*pszDsc ++ = chBackslash();

						while(*pszSrc == ' ' || *pszSrc == '.')
							pszSrc ++;

						while(*pszSrc && !isBackslash(*pszSrc))
							*pszDsc ++ = *pszSrc ++;

						while(*(pszDsc - 1) == ' ' || *(pszDsc - 1) == '.')
							pszDsc --;

						*pszDsc ++ = chBackslash();
					}
				}else
				{
					m_bUrl = FALSE;
					*pszDsc ++ = chBackslash();
				}

				while(isBackslash(*pszSrc))
					pszSrc ++;

				pszRoot = pszDsc;
				m_nRoot = (int)(pszRoot - pszBuffer);
			}else
			{
				pszDsc = pszBuffer + iLen;
				if(!isBackslash(*(pszDsc - 1)))
					*pszDsc ++ = chBackslash();

				pszSrc = pszMore;
			}
		}

		while(*pszSrc)
		{
			if(isBackslash(*pszSrc))
			{
				*pszDsc ++ = chBackslash();
				pszSrc ++;
				while(isBackslash(*pszSrc))
					pszSrc ++;
			}else if(*pszSrc == '.' && (!pszSrc[1] || isBackslash(pszSrc[1])))
			{
				pszSrc ++;
				while(isBackslash(*pszSrc))
					pszSrc ++;
			}else if(*pszSrc == '.' && pszSrc[1] == '.' && (!pszSrc[2] || isBackslash(pszSrc[2])))
			{
				pszSrc += 2;
				while(isBackslash(*pszSrc))
					pszSrc ++;

				if(pszDsc > pszRoot)
				{
					pszDsc --;
					while(!isBackslash(*(pszDsc - 1)))
						pszDsc --;
				}
			}else
			{
				while(*pszSrc == ' ' || *pszSrc == '.' || isBackslash(*pszSrc))
					pszSrc ++;

				while(*pszSrc && !isBackslash(*pszSrc))
					*pszDsc ++ = *pszSrc ++;

				while(*(pszDsc - 1) == ' ' || *(pszDsc - 1) == '.')
					pszDsc --;
			}
		}

		*pszDsc = 0;
		m_strPath.ReleaseBuffer((int)(pszDsc - pszBuffer));
	}

	void Append( PCSTR pszMore )
	{
		Append( CBString(pszMore) );
	}

	PCWSTR GetExtension() const
	{
		PCWSTR pszBuffer = (PCWSTR)m_strPath + m_nRoot;
		PCWSTR pszExtension = NULL;

		while (*pszBuffer)
		{
			if (isBackslash(*pszBuffer) || *pszBuffer == ':')
				pszExtension = NULL;
			else if (*pszBuffer == '.')
				pszExtension = pszBuffer + 1;
			pszBuffer ++;
		}

		return pszExtension;
	}

	PCWSTR GetFileName() const
	{
		PCWSTR pszBuffer = (PCWSTR)m_strPath + m_nRoot;
		PCWSTR pszFileName = NULL;

		while (*pszBuffer)
		{
			if (isBackslash(*pszBuffer) || *pszBuffer ==':')
				pszFileName = NULL;
			else if (!pszFileName)
				pszFileName = pszBuffer;
			pszBuffer ++;
		}

		return pszFileName;
	}

	CBString GetBaseName(void)
	{
		PCWSTR pszBuffer = (PCWSTR)m_strPath + m_nRoot;
		PCWSTR pszFileName = NULL;
		PCWSTR pszExtension = NULL;

		while (*pszBuffer)
		{
			if (isBackslash(*pszBuffer) || *pszBuffer ==':')
				pszFileName = NULL;
			else if (*pszBuffer == '.')
				pszExtension = pszBuffer;
			else if (!pszFileName)
				pszFileName = pszBuffer;
			pszBuffer ++;
		}

		CBString strBaseName;

		if(pszFileName)
		{
			if(pszExtension)
				strBaseName.SetString(pszFileName, (int)(pszExtension - pszFileName));
			else strBaseName = pszFileName;
		}

		return strBaseName;
	}

	void StripPath(BOOL bRemoveBackslash = FALSE)
	{
		PCWSTR pszBuffer = (PCWSTR)m_strPath + m_nRoot;
		PCWSTR pszFileName = NULL;

		while (*pszBuffer)
		{
			if (isBackslash(*pszBuffer) || *pszBuffer ==':')
				pszFileName = NULL;
			else if (!pszFileName)
				pszFileName = pszBuffer;
			pszBuffer ++;
		}

		if(pszFileName)
			m_strPath = m_strPath.Left((int)(pszFileName - (PCWSTR)m_strPath));
		else Clear();

		if(bRemoveBackslash)
			RemoveBackslash();
	}

	BOOL IsDirectory() const
	{
		return ::PathIsDirectoryW( m_strPath );
	}

	BOOL IsRoot() const
	{
		return m_strPath.GetLength() == m_nRoot;
	}

	BOOL IsURL() const
	{
		return m_bUrl;
	}

	BOOL IsUNC() const
	{
		return isBackslash(((PCWSTR)m_strPath)[0]) && isBackslash(((PCWSTR)m_strPath)[1]);
	}

	BOOL IsUNCServer() const
	{
		PCWSTR pszPath = m_strPath;

		if(isBackslash(pszPath[0]) && isBackslash(pszPath[1]))
		{
			pszPath += 2;

			while (*pszPath)
				if (isBackslash(*pszPath ++))
					return FALSE;

			return TRUE;
		}
		return FALSE;
	}

	BOOL IsUNCServerShare() const
	{
		PCWSTR pszPath = m_strPath;

		if(isBackslash(pszPath[0]) && isBackslash(pszPath[1]))
		{
			pszPath += 2;
			BOOL bSeenSlash = FALSE;

			while (*pszPath)
				if (isBackslash(*pszPath ++))
				{
                    if (bSeenSlash)
						return FALSE;
					bSeenSlash = TRUE;
					return FALSE;
				}

			return bSeenSlash;
		}
		return FALSE;
	}

public:
	BOOL m_bUrl;
	int m_nRoot;
	CBString m_strPath;
};
