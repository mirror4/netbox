#pragma once

template <int nStringSize>
class CBoxFixString
{
public:
	CBoxFixString(void)
	{
		Empty();
	}

	operator TCHAR*(void) const
	{
		return (TCHAR*)m_strBuffer;
	}

	void operator=(LPCTSTR pszSrc)
	{
		SetString(pszSrc);
	}

	void operator=(LPCWSTR pszSrc)
	{
		SetString(pszSrc);
	}

	void operator+=(LPCTSTR pszSrc)
	{
		Append(pszSrc);
	}

	void operator+=(TCHAR chSrc)
	{
		Append(chSrc);
	}

	void Empty(void)
	{
		m_nLength = 0;
		m_strBuffer[0] = 0;
	}

	int GetLength(void)
	{
		return m_nLength;
	}

	BOOL IsEmpty(void)
	{
		return m_nLength == 0;
	}

	void SetString(LPCTSTR pszSrc)
	{
		SetString( pszSrc, (int)_tcslen(pszSrc));
	}

	void SetString(LPCTSTR pszSrc, int nLength)
	{
		if(nLength > nStringSize)
			nLength = nStringSize;

		memcpy(m_strBuffer, pszSrc, nLength * sizeof(TCHAR));

		m_nLength = nLength;
		m_strBuffer[m_nLength] = 0;
	}

	void SetString(LPCWSTR pszSrc)
	{
		SetString( pszSrc, (int)wcslen(pszSrc));
	}

	void SetString(LPCWSTR pszSrc, int nLength)
	{
		m_nLength = WideCharToMultiByte(_AtlGetConversionACP(), 0, pszSrc, nLength, m_strBuffer, nStringSize, NULL, NULL);
		m_strBuffer[m_nLength] = 0;
	}

	void Append(TCHAR chSrc)
	{
		if(m_nLength < nStringSize)
		{
			m_strBuffer[m_nLength] = chSrc;
			m_nLength ++;
			m_strBuffer[m_nLength] = 0;
		}
	}

	void Append(LPCTSTR pszSrc)
	{
		Append( pszSrc, (int)_tcslen(pszSrc));
	}

	void Append(LPCTSTR pszSrc, int nLength)
	{
		if(m_nLength + nLength > nStringSize)
			nLength = nStringSize - m_nLength;

		memcpy(m_strBuffer + m_nLength, pszSrc, nLength * sizeof(TCHAR));
		m_nLength += nLength;
		m_strBuffer[m_nLength] = 0;
	}

	int CompareNoCase( LPCTSTR psz )
	{
		return _tcsicmp(m_strBuffer, psz);
	}

	int Compare( LPCTSTR psz )
	{
		return _tcscmp(m_strBuffer, psz);
	}

	void MakeLower(void)
	{
		_tcslwr(m_strBuffer);
	}

	int Find( LPCTSTR psz )
	{
		psz = _tcsstr(m_strBuffer, psz);
		if(psz == NULL)return -1;
		return (int)(psz - m_strBuffer);
	}

	int Find( TCHAR c )
	{
		LPCTSTR psz = _tcschr(m_strBuffer, c);
		if(psz == NULL)return -1;
		return (int)(psz - m_strBuffer);
	}

	BSTR AllocSysString(void)
	{
		int nLen = ::MultiByteToWideChar( _AtlGetConversionACP(), 0, m_strBuffer, m_nLength, NULL, NULL);
		BSTR bstr = ::SysAllocStringLen(NULL, nLen);
		if(bstr != NULL)
			::MultiByteToWideChar(_AtlGetConversionACP(), 0, m_strBuffer, m_nLength, bstr, nLen);
		return(bstr);
	}

private:
	int m_nLength;
	TCHAR m_strBuffer[nStringSize + 1];
};
