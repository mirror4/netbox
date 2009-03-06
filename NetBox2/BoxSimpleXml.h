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
		m_nCDataLine(0)
	{}

	int Parse(LPCSTR pstrText, int nCount, BOOL bUTF8 = TRUE);
	BOOL LookupNode(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode);
	BOOL LookupProcessing(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode);

	template<class TYPE>
	BOOL LookupAttribute(LPCTSTR pstrName, TYPE& str)
	{
		CString strTemp;

		if(m_mapAttributes.Lookup(pstrName, strTemp))
		{
			str = strTemp;
			return TRUE;
		}

		return FALSE;
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
};

