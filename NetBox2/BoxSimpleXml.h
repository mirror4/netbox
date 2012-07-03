#pragma once

#include <afxtempl.h>
#include "BoxObject.h"

class CBoxSimpleXml : public CBoxBaseObject
{
public:
	CBoxSimpleXml(void) :
		m_pParent(NULL),
		m_bProcessing(FALSE),
		m_nTextLine(0),
		m_nCDataLine(0),
		m_uiCodePage(0)
	{}

	int Parse(LPCSTR pstrText, int nCount, BOOL bUTF8 = TRUE);

private:
	BOOL _LookupNode(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode);
	BOOL _LookupProcessing(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode);

	inline CString wstr2str(LPCWSTR pstrName)
	{
		CString strName;

		int nCount = lstrlenW(pstrName);
		int _nTempCount = WideCharToMultiByte(m_uiCodePage, 0, pstrName, nCount, NULL, 0, NULL, NULL);
		LPSTR _pstr = strName.GetBuffer(_nTempCount);

		WideCharToMultiByte(m_uiCodePage, 0, pstrName, nCount, _pstr, _nTempCount, NULL, NULL);
		strName.ReleaseBuffer(_nTempCount);

		return strName;
	}

	inline CStringW str2wstr(LPCTSTR pstrName)
	{
		CStringW wstrTemp;

		int nCount = lstrlenA(pstrName);
		int _nTempCount = MultiByteToWideChar(m_uiCodePage, 0, pstrName, nCount, NULL, 0);
		LPWSTR _pstr = wstrTemp.GetBuffer(_nTempCount);

		MultiByteToWideChar(m_uiCodePage, 0, pstrName, nCount, _pstr, _nTempCount);
		wstrTemp.ReleaseBuffer(_nTempCount);

		return wstrTemp;
	}

public:
	BOOL LookupNode(LPCWSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode)
	{
		return _LookupNode(wstr2str(pstrName), pNode);
	}

	BOOL LookupNode(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode)
	{
		if (m_uiCodePage && _AtlGetConversionACP() != m_uiCodePage)
		{
			CString strTemp = wstr2str(CStringW(pstrName));
			return _LookupNode(strTemp, pNode);
		}
		return _LookupNode(pstrName, pNode);
	}

	BOOL LookupProcessing(LPCWSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode)
	{
		return _LookupProcessing(wstr2str(pstrName), pNode);
	}

	BOOL LookupProcessing(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode)
	{
		if (m_uiCodePage && _AtlGetConversionACP() != m_uiCodePage)
		{
			CString strTemp = wstr2str(CStringW(pstrName));
			return _LookupProcessing(strTemp, pNode);
		}
		return _LookupProcessing(pstrName, pNode);
	}

	template<class TYPE>
	BOOL _LookupAttribute(LPCTSTR pstrName, TYPE& str)
	{
		CString strTemp;

		if(m_mapAttributes.Lookup(pstrName, strTemp))
		{
			if (m_uiCodePage)
			{
				str = str2wstr(strTemp);
				return TRUE;
			}
			
			str = strTemp;
			return TRUE;
		}

		return FALSE;
	}

	template<class TYPE>
	BOOL LookupAttribute(LPCWSTR pstrName, TYPE& str)
	{
		return _LookupAttribute(wstr2str(pstrName), str);
	}

	template<class TYPE>
	BOOL LookupAttribute(LPCTSTR pstrName, TYPE& str)
	{
		if (m_uiCodePage && _AtlGetConversionACP() != m_uiCodePage)
		{
			CString strTemp = wstr2str(CStringW(pstrName));
			return _LookupAttribute(strTemp, str);
		}
		return _LookupAttribute(pstrName, str);
	}

	CStringW get_CData()
	{
		return str2wstr(m_strCData);
	}

	CStringW get_Name()
	{
		return str2wstr(m_strName);
	}

public:
	BOOL m_bProcessing;
	CBoxSimpleXml* m_pParent;
	CString m_strName;
	CString m_strText;
	int m_nTextLine;
	CString m_strCData;
	int m_nCDataLine;
	CMapStringToString m_mapAttributes;
	CList< CBoxObject<CBoxSimpleXml> > m_listProcessings;
	CList< CBoxObject<CBoxSimpleXml> > m_listNodes;

	int m_nErrorLine;
	CString m_strDescription;

	UINT m_uiCodePage;
};

