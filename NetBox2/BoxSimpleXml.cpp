#include "StdAfx.h"
#include "BoxSimpleXml.h"

#define IsTagChar(c) (((c) == '<') || ((c) == '>') || ((c) == '?') || ((c) == '/') || ((c) == '='))

BOOL CBoxSimpleXml::LookupNode(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode)
{
	CBoxSimpleXml* pItem;
	POSITION pos = m_listNodes.GetHeadPosition();

	while(pos)
	{
		pItem = m_listNodes.GetNext(pos);
		if(!pItem->m_strName.Compare(pstrName))
		{
			pNode = pItem;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBoxSimpleXml::LookupProcessing(LPCTSTR pstrName, CBoxObject<CBoxSimpleXml>& pNode)
{
	CBoxSimpleXml* pItem;
	POSITION pos = m_listProcessings.GetHeadPosition();

	while(pos)
	{
		pItem = m_listProcessings.GetNext(pos);
		if(!pItem->m_strName.Compare(pstrName))
		{
			pNode = pItem;
			return TRUE;
		}
	}

	return FALSE;
}

int CBoxSimpleXml::Parse(LPCSTR pstrText, int nCount, BOOL bUTF8)
{
	CStringA strTempText;
	if(nCount >= 2 && (BYTE)pstrText[0] == 0xff && (BYTE)pstrText[1] == 0xfe)
	{
		int _nTempCount = WideCharToMultiByte(_AtlGetConversionACP(), 0, LPWSTR(pstrText + 2), (nCount - 2) / 2, NULL, 0, NULL, NULL);
		LPSTR _pstr = strTempText.GetBuffer(_nTempCount);

		WideCharToMultiByte(_AtlGetConversionACP(), 0, LPWSTR(pstrText + 2), (nCount - 2) / 2, _pstr, _nTempCount, NULL, NULL);
		strTempText.ReleaseBuffer(_nTempCount);

		pstrText = strTempText;
		nCount = strTempText.GetLength();

		bUTF8 = FALSE;
	}

	CBoxObject<CBoxSimpleXml> pNode(this);
	CBoxSimpleXml* pParent = NULL;
	LPCSTR pstrTemp;
	int nTempCount;
	char strMark;
	CStringA strTemp, strTemp1;
	CString str;

	m_nErrorLine = 1;

	while(nCount > 0)
	{
		while(nCount > 0 && IsBlank(pstrText[0]))
		{
			if(pstrText[0] == '\n')
				m_nErrorLine ++;
			pstrText ++;
			nCount --;
		}

		if(nCount > 1 && pstrText[0] == '<')
		{
			pstrText ++;
			nCount --;

			if(bUTF8 && (nCount < 4 || pstrText[0] != '?' || pstrText[1] != 'x' ||
				pstrText[2] != 'm' || pstrText[3] != 'l' || !IsBlank(pstrText[4])))
			{
				CStringW wstr;

				nTempCount = MultiByteToWideChar(CP_UTF8, 0, pstrText, nCount, NULL, 0);
				LPWSTR pwstr = wstr.GetBuffer(nTempCount);

				MultiByteToWideChar(CP_UTF8, 0, pstrText, nCount, pwstr, nTempCount);
				wstr.ReleaseBuffer(nTempCount);

				strTempText = wstr;
				pstrText = strTempText;
				nCount = strTempText.GetLength();

				bUTF8 = FALSE;
			}

			if(pstrText[0] == '!')
			{
				pstrText ++;
				nCount --;

				if(nCount > 1 && pstrText[0] == '-' && pstrText[1] == '-')
				{
					pstrText += 2;
					nCount -= 2;

					while(nCount > 2 && (pstrText[0] != '-' || pstrText[1] != '-' || pstrText[2] != '>'))
					{
						if(pstrText[0] == '\n')
							m_nErrorLine ++;
						pstrText ++;
						nCount --;
					}

					if(nCount > 2 && pstrText[0] == '-' && pstrText[1] == '-' && pstrText[2] == '>')
					{
						pstrText += 3;
						nCount -= 3;
					}else
					{
						m_strDescription = _T("Unterminated comment - matching \'-->\' not found");
						return -1;
					}
				}else if(nCount > 6 && !strncmp(pstrText, "[CDATA[", 7))
				{
					pstrText += 7;
					nCount -= 7;

					pstrTemp = pstrText;
					nTempCount = nCount;

					pNode->m_nCDataLine = m_nErrorLine;

					while(nTempCount > 2 && (pstrTemp[0] != ']' || pstrTemp[1] != ']' || pstrTemp[2] != '>'))
					{
						if(pstrTemp[0] == '\n')
							m_nErrorLine ++;
						pstrTemp ++;
						nTempCount --;
					}

					pNode->m_strCData = CStringA(pstrText, (int)(pstrTemp - pstrText));

					pstrText = pstrTemp;
					nCount = nTempCount;

					if(nCount > 2 && pstrText[0] == ']' && pstrText[1] == ']' && pstrText[2] == '>')
					{
						pstrText += 3;
						nCount -= 3;
					}else
					{
						m_strDescription = _T("Unterminated CDATA section - matching \']]>\' not found");
						return -1;
					}
				}else
				{
					m_strDescription = _T("Unrecognized markup");
					return -1;
				}
			}else if(pstrText[0] == '/')
			{
				pstrText ++;
				nCount --;

				pstrTemp = pstrText;
				nTempCount = nCount;

				while(nTempCount > 0 && pstrTemp[0] != '>')
				{
					if(pstrTemp[0] == '\n')
						m_nErrorLine ++;
					pstrTemp ++;
					nTempCount --;
				}

				if(pstrTemp != pstrText)
					strTemp.SetString(pstrText, (int)(pstrTemp - pstrText));
				else
				{
					m_strDescription = _T("Expecting a valid name");
					return -1;
				}

				pstrText = pstrTemp;
				nCount = nTempCount;

				if(pNode->m_strName.Compare(BOX_CA2CT(strTemp)))
				{
					m_strDescription = _T("The end tag does not match the start tag : ");
					m_strDescription += pNode->m_strName;
					return -1;
				}

				if(nCount > 0 && pstrText[0] == '>')
				{
					pstrText ++;
					nCount --;
				}else
				{
					m_strDescription = _T("Unterminated end tag - matching \'>\' not found");
					return -1;
				}

				pNode = pParent;
				pParent = pNode->m_pParent;
			}else
			{
				pParent = pNode;
				pNode.CreateInstance();
				pNode->m_pParent = pParent;

				if(pstrText[0] == '?')
				{
					pNode->m_bProcessing = TRUE;
					pstrText ++;
					nCount --;
				}

				pstrTemp = pstrText;
				nTempCount = nCount;

				while(nTempCount > 0 && !IsTagChar(pstrTemp[0]) && !IsBlank(pstrTemp[0]))
				{
					pstrTemp ++;
					nTempCount --;
				}

				if(pstrTemp != pstrText)
					pNode->m_strName = CStringA(pstrText, (int)(pstrTemp - pstrText));
				else
				{
					m_strDescription = _T("Expecting a valid name");
					return -1;
				}

				pstrText = pstrTemp;
				nCount = nTempCount;

				if(pNode->m_bProcessing)
					pParent->m_listProcessings.AddTail(pNode);
				else pParent->m_listNodes.AddTail(pNode);

				while(nCount > 0 && IsBlank(pstrText[0]))
				{
					if(pstrText[0] == '\n')
						m_nErrorLine ++;
					pstrText ++;
					nCount --;
				}

				while(nCount > 0 && !IsTagChar(pstrText[0]))
				{
					pstrTemp = pstrText;
					nTempCount = nCount;

					while(nTempCount > 0 && !IsTagChar(pstrTemp[0]) && !IsBlank(pstrTemp[0]))
					{
						pstrTemp ++;
						nTempCount --;
					}

					if(pstrTemp != pstrText)
						strTemp.SetString(pstrText, (int)(pstrTemp - pstrText));
					else
					{
						m_strDescription = _T("Expecting a valid name");
						return -1;
					}

					pstrText = pstrTemp;
					nCount = nTempCount;

					while(nCount > 0 && IsBlank(pstrText[0]))
					{
						if(pstrText[0] == '\n')
							m_nErrorLine ++;
						pstrText ++;
						nCount --;
					}

					if(nCount > 0 && pstrText[0] == '=')
					{
						pstrText ++;
						nCount --;
					}else
					{
						m_strDescription = _T("Expecting a valid name");
						return -1;
					}

					while(nCount > 0 && IsBlank(pstrText[0]))
					{
						if(pstrText[0] == '\n')
							m_nErrorLine ++;
						pstrText ++;
						nCount --;
					}

					if(nCount > 0 && (pstrText[0] == '\"' || pstrText[0] == '\''))
					{
						strMark = pstrText[0];
						pstrText ++;
						nCount --;
					}else
					{
						m_strDescription = _T("Expecting an attribute value enclosed in quotes");
						return -1;
					}

					pstrTemp = pstrText;
					nTempCount = nCount;

					while(nTempCount > 0 && pstrTemp[0] != strMark && !IsLineChar(pstrTemp[0]))
					{
						pstrTemp ++;
						nTempCount --;
					}

					strTemp1.SetString(pstrText, (int)(pstrTemp - pstrText));

					pstrText = pstrTemp;
					nCount = nTempCount;

					if(nCount > 0 && pstrText[0] == strMark)
					{
						pstrText ++;
						nCount --;
					}else
					{
						m_strDescription = _T("Expecting a valid name");
						return -1;
					}

					pNode->m_mapAttributes.SetAt(BOX_CA2CT(strTemp), BOX_CA2CT(strTemp1));

					while(nCount > 0 && IsBlank(pstrText[0]))
					{
						if(pstrText[0] == '\n')
							m_nErrorLine ++;
						pstrText ++;
						nCount --;
					}
				}

				if(pNode->m_bProcessing)
					if(nCount > 0 && pstrText[0] == '?')
					{
						pstrText ++;
						nCount --;

						if(bUTF8 && (!pNode->m_mapAttributes.Lookup(_T("encoding"), str) || !str.CompareNoCase(_T("utf-8"))))
						{
							CStringW wstr;

							nTempCount = MultiByteToWideChar(CP_UTF8, 0, pstrText, nCount, NULL, 0);
							LPWSTR pwstr = wstr.GetBuffer(nTempCount);

							MultiByteToWideChar(CP_UTF8, 0, pstrText, nCount, pwstr, nTempCount);
							wstr.ReleaseBuffer(nTempCount);

							strTempText = wstr;
							pstrText = strTempText;
							nCount = strTempText.GetLength();
						}

						bUTF8 = FALSE;

						pNode = pParent;
						pParent = pNode->m_pParent;
					}else
					{
						m_strDescription = _T("Unterminated processing instruction - matching \'?>\' not found");
						return -1;
					}
				else if(nCount > 0 && pstrText[0] == '/')
				{
					pstrText ++;
					nCount --;

					pNode = pParent;
					pParent = pNode->m_pParent;
				}

				if(nCount > 0 && pstrText[0] == '>')
				{
					pstrText ++;
					nCount --;
				}else
				{
					m_strDescription = _T("Expecting a valid name");
					return -1;
				}
			}
		}else
		{
			pstrTemp = pstrText;
			nTempCount = nCount;

			pNode->m_nTextLine = m_nErrorLine;

			while(nTempCount > 0 && pstrTemp[0] != '<')
			{
				if(pstrTemp[0] == '\n')
					m_nErrorLine ++;
				pstrTemp ++;
				nTempCount --;
			}

			pNode->m_strText = CStringA(pstrText, (int)(pstrTemp - pstrText));

			pstrText = pstrTemp;
			nCount = nTempCount;
		}
	}

	return 0;
}
