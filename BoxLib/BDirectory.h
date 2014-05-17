#pragma once

#include "BDictionary.h"

class CBPath
{
public:
	CBPath(void)
	{
		m_strPath = new WCHAR[MAX_PATH];
		m_strPath[0] = 0;
	}

	~CBPath(void)
	{
		delete m_strPath;
	}

	void Combine(LPCWSTR pstrBasePath, LPCWSTR pstrFile = NULL)
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

	LPWSTR m_strPath;
};

class __declspec(uuid("94650000-0000-0044-6972-6563746F7279"))
CBDirectory : public CBDispatchEx<IVariantDirectory, CBDictionary>
{
	DECLARE_CLASS_EX(CBDirectory, CBDictionary)

public:
	// IVariantDirectory
	STDMETHOD(get_AutoCreateDirectory)(VARIANT_BOOL *pbACD);
	STDMETHOD(put_AutoCreateDirectory)(VARIANT_BOOL bACD);

	STDMETHOD(CreateDirectory)(BSTR strPath);
	STDMETHOD(CopyTo)(BSTR strFrom, BSTR strTo);
	STDMETHOD(MoveTo)(BSTR strFrom, BSTR strTo);
	STDMETHOD(Link)(BSTR strFrom, BSTR strTo);

private:
	HRESULT getItem(BSTR strPath, CRBMap<CBVariant, CComVariant>::CPair** ppPair, BOOL bAutoCreate);
	BOOL m_bAutoCreateDirectory;
};

