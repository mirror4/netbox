#include "StdAfx.h"
#include "BoxHttpScript.h"
#include "BoxPath.h"

IMPLEMENT_DYNCREATE(CBoxHttpScript, CBoxScript)

void CBoxHttpScript::InitScript(void)
{
	m_pHost->m_arrayExtData[0] = m_bSessionState;
}

class _checkLine
{
public:
	_checkLine(void) : m_chLineBreak(0)
	{}

	BOOL IsBreak(WCHAR ch)
	{
		if(m_chLineBreak)
			return (m_chLineBreak == ch);

		if(ch == '\r' || ch == '\n')
		{
			m_chLineBreak = ch;
			return TRUE;
		}

		return FALSE;
	}

private:
	int	m_chLineBreak;
};

int CBoxHttpScript::ParseScriptText(LPCSTR pstrText, int nCount, CStringA& strScriptText, int nIncludeFlagIndex)
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
	}

	LPCSTR pstrTemp, pstrTemp1, pstrTemp2, pstrPath;
	int nTempCount;
	int index;
	int mInclude;
	int nLineCount = 1, nTempLineCount = 0;
	BOOL bCountBlankLine;
	BOOL bIsSpace = TRUE;
	_checkLine cl;

	pstrTemp = pstrText;
	nTempCount = nCount;

	while(nTempCount > 0)
		if(pstrTemp[0] == '<' && nTempCount > 1)
		{
			if(pstrTemp[1] == '%')
			{
				if(nCount - nTempCount > 0 && !bIsSpace)
				{
					bIsSpace = TRUE;
					index = AddBlock(pstrText, nCount - nTempCount);
					if(index != -1)
					{
						strScriptText.AppendFormat("Response.WriteBlock(%d)\r\n", index);
						m_nScriptLine ++;
					}
				}

				pstrTemp += 2;
				nTempCount -= 2;

				while(nTempCount > 0 && IsBlank(pstrTemp[0]))
				{
					if(cl.IsBreak(pstrTemp[0]))
						nLineCount ++;

					pstrTemp ++;
					nTempCount --;
				}

				while(nTempCount > 0 && pstrTemp[0] == '@')
				{
					while(nTempCount > 1 && !IsLineChar(pstrTemp[0]) &&
						(pstrTemp[0] != '%' || pstrTemp[1] != '>'))
					{
						CStringA strTemp, strTemp1;

						pstrTemp ++;
						nTempCount --;

						while(nTempCount > 0 && IsBlankChar(pstrTemp[0]))
						{
							pstrTemp ++;
							nTempCount --;
						}

						if(!nTempCount)
							break;

						pstrTemp2 = pstrTemp;
						while(nTempCount > 1 && pstrTemp[0] != '=' && !IsBlank(pstrTemp[0]) &&
							(pstrTemp[0] != '%' || pstrTemp[1] != '>'))
						{
							if(cl.IsBreak(pstrTemp[0]))
								nLineCount ++;

							pstrTemp ++;
							nTempCount --;
						}

						if(!nTempCount || IsLineChar(pstrTemp[0]))
							break;

						strTemp.SetString(pstrTemp2, (int)(pstrTemp - pstrTemp2));

						while(nTempCount > 0 && IsBlankChar(pstrTemp[0]))
						{
							pstrTemp ++;
							nTempCount --;
						}

						if(!nTempCount || pstrTemp[0] != '=')
							break;

						pstrTemp ++;
						nTempCount --;

						while(nTempCount > 0 && IsBlankChar(pstrTemp[0]))
						{
							pstrTemp ++;
							nTempCount --;
						}

						if(!nTempCount)
							break;

						if(pstrTemp[0] == '\"' || pstrTemp[0] == '\'')
						{
							while(nTempCount > 0 && (pstrTemp[0] == '\"' || pstrTemp[0] == '\''))
							{
								pstrTemp ++;
								nTempCount --;
							}

							pstrTemp2 = pstrTemp;
							while(nTempCount > 1 && pstrTemp[0] != '\"' && !IsLineChar(pstrTemp[0]) &&
								(pstrTemp[0] != '%' || pstrTemp[1] != '>'))
							{
								pstrTemp ++;
								nTempCount --;
							}

							strTemp1.SetString(pstrTemp2, (int)(pstrTemp - pstrTemp2));

							while(nTempCount > 0 && (pstrTemp[0] == '\"' || pstrTemp[0] == '\''))
							{
								pstrTemp ++;
								nTempCount --;
							}
						}else
						{
							pstrTemp2 = pstrTemp;
							while(nTempCount > 1 && !IsBlank(pstrTemp[0]) &&
								(pstrTemp[0] != '%' || pstrTemp[1] != '>'))
							{
								if(cl.IsBreak(pstrTemp[0]))
									nLineCount ++;

								pstrTemp ++;
								nTempCount --;
							}

							strTemp1.SetString(pstrTemp2, (int)(pstrTemp - pstrTemp2));
						}

						if(!strTemp.CompareNoCase("LANGUAGE"))
							m_strLanguage = strTemp1;
						else if(!strTemp.CompareNoCase("ENABLESESSIONSTATE"))
							m_pHost->m_arrayExtData[0] = !strTemp1.CompareNoCase("True");
						else if(!strTemp.CompareNoCase("DEBUG"))
						{
							m_bEnableDebug =  !strTemp1.CompareNoCase("on") || !strTemp1.CompareNoCase("true");
							m_bStepDebug = !strTemp1.CompareNoCase("step");
						}else if(!strTemp.CompareNoCase("TIMEOUT"))
						{
							m_pHost->m_nTimeout = atoi(strTemp1);
						}else if(!strTemp.CompareNoCase("TRANSACTION"))
						{
							if(!strTemp1.CompareNoCase("Required"))
								m_nTransaction = 3;
							else if(!strTemp1.CompareNoCase("Requires_New"))
								m_nTransaction = 2;
							else if(!strTemp1.CompareNoCase("Supported"))
								m_nTransaction = 1;
							else if(!strTemp1.CompareNoCase("Not_Supported"))
								m_nTransaction = 0;
						}
					}

					while(nTempCount > 0 && IsBlank(pstrTemp[0]))
					{
						if(cl.IsBreak(pstrTemp[0]))
							nLineCount ++;

						pstrTemp ++;
						nTempCount --;
					}
				}

				if(nTempCount > 0 && pstrTemp[0] == '=')
				{
					pstrTemp ++;
					nTempCount --;

					while(nTempCount > 0 && IsBlank(pstrTemp[0]))
					{
						if(cl.IsBreak(pstrTemp[0]))
							nLineCount ++;

						pstrTemp ++;
						nTempCount --;
					}

					pstrTemp1 = pstrTemp2 = pstrTemp;
					nTempLineCount = 0;
					while(nTempCount > 1 && (pstrTemp1[0] != '%' || pstrTemp1[1] != '>'))
					{
						if(cl.IsBreak(pstrTemp1[0]))
							nTempLineCount ++;
						else if(!IsBlank(pstrTemp1[0]))
							pstrTemp2 = pstrTemp1;

						pstrTemp1 ++;
						nTempCount --;
					}

					if(nTempCount < 2)
					{
						SetParserError(nLineCount, m_strBasePath, CString("The Script block lacks the close of script tag (%>)."));
						return 500;
					}

					if(pstrTemp2 >= pstrTemp)
					{
						AddLineMap(nIncludeFlagIndex, nLineCount);
						strScriptText += "Response.Write(";
						strScriptText.Append(pstrTemp, (int)(pstrTemp2 - pstrTemp) + 1);
						strScriptText += ")\r\n";
						m_nScriptLine ++;
					}

					pstrTemp = pstrTemp1;
					nLineCount += nTempLineCount;
				}else while(nTempCount > 1 && (pstrTemp[0] != '%' || pstrTemp[1] != '>'))
				{
					bCountBlankLine = FALSE;

					pstrTemp1 = pstrTemp2 = pstrTemp;
					nTempLineCount = 0;
					while(nTempCount > 1 && (pstrTemp1[0] != '%' || pstrTemp1[1] != '>'))
					{
						if(cl.IsBreak(pstrTemp1[0]))
						{
							if(bCountBlankLine)
								break;

							bCountBlankLine = TRUE;
							nTempLineCount ++;
						}else if(!IsBlank(pstrTemp1[0]))
						{
							bCountBlankLine = FALSE;
							pstrTemp2 = pstrTemp1;
						}

						pstrTemp1 ++;
						nTempCount --;
					}

					AddLineMap(nIncludeFlagIndex, nLineCount);
					strScriptText.Append(pstrTemp, (int)(pstrTemp2 - pstrTemp) + 1);
					m_nScriptLine += nTempLineCount;
					strScriptText.Append("\r\n");
					if(!bCountBlankLine)m_nScriptLine ++;

					nLineCount += nTempLineCount;
					pstrTemp = pstrTemp1;

					while(nTempCount > 0 && IsBlank(pstrTemp[0]))
					{
						if(cl.IsBreak(pstrTemp[0]))
							nLineCount ++;

						pstrTemp ++;
						nTempCount --;
					}

					if(nTempCount < 2)
					{
						SetParserError(nLineCount, m_strBasePath, CString("The Script block lacks the close of script tag (%>)."));
						return 500;
					}
				}

				if(nTempCount > 0)
				{
					pstrTemp += 2;
					nTempCount -= 2;
				}

				pstrText = pstrTemp;
				nCount = nTempCount;
			}else if(nTempCount > 3 && pstrTemp[1] == '!' && pstrTemp[2] == '-' && pstrTemp[3] == '-')
			{
				pstrTemp += 4;
				nTempCount -= 4;
				mInclude = 0;
				pstrPath = 0;

				pstrTemp1 = pstrTemp;
				while(nTempCount > 0 && IsBlankChar(pstrTemp1[0]))
				{
					pstrTemp1 ++;
					nTempCount --;
				}

				if(nTempCount > 0 && pstrTemp1[0] == '#')
				{
					pstrTemp1 ++;
					nTempCount --;

					while(nTempCount > 0 && IsBlankChar(pstrTemp1[0]))
					{
						pstrTemp1 ++;
						nTempCount --;
					}

					if(nTempCount > 7 && !strnicmp(pstrTemp1, "include", 7))
					{
						pstrTemp1 += 7;
						nTempCount -= 7;

						while(nTempCount > 0 && IsBlankChar(pstrTemp1[0]))
						{
							pstrTemp1 ++;
							nTempCount --;
						}

						if(nTempCount > 7 && !strnicmp(pstrTemp1, "virtual", 7))
						{
							pstrTemp1 += 7;
							nTempCount -= 7;
							mInclude = 1;
						}else if(nTempCount > 4 && !strnicmp(pstrTemp1, "file", 4))
						{
							pstrTemp1 += 4;
							nTempCount -= 4;
							mInclude = 2;
						}

						if(mInclude != 0)
						{
							while(nTempCount > 0 && IsBlankChar(pstrTemp1[0]))
							{
								pstrTemp1 ++;
								nTempCount --;
							}

							if(nTempCount > 0 && pstrTemp1[0] == '=')
							{
								pstrTemp1 ++;
								nTempCount --;

								while(nTempCount > 0 && IsBlankChar(pstrTemp1[0]))
								{
									pstrTemp1 ++;
									nTempCount --;
								}

								pstrPath = pstrTemp1;
							}
						}
					}

					while(nTempCount > 0 && pstrTemp1[0] != '>')
					{
						pstrTemp1 ++;
						nTempCount --;
					}

					if(pstrPath != NULL)
					{
						if(pstrText != pstrTemp - 4 && !bIsSpace)
						{
							bIsSpace = TRUE;
							index = AddBlock(pstrText, (int)(pstrTemp - pstrText - 4));
							if(index != -1)
							{
								strScriptText.AppendFormat("Response.WriteBlock(%d)\r\n", index);
								m_nScriptLine ++;
							}
						}

						pstrTemp = pstrTemp1;
						if(nTempCount > 0)
						{
							pstrTemp1 ++;
							nTempCount --;
						}

                        pstrText = pstrTemp1;
						nCount = nTempCount;

						while(pstrTemp > pstrPath && (pstrTemp[-1] == '-' || IsBlankChar(pstrTemp[-1])))
							pstrTemp --;

						while(pstrTemp > pstrPath && (pstrTemp[-1] == '\"' || pstrTemp[-1] == '\''))
							pstrTemp --;
						while(pstrTemp > pstrPath && (pstrPath[0] == '\"' || pstrPath[0] == '\''))
							pstrPath ++;

						if(pstrTemp > pstrPath)
						{
							CStringA strTemp;

							strTemp.SetString(pstrPath, (int)(pstrTemp - pstrPath));

							if(strTemp.GetLength() > 4)
							{
								LPCSTR pstrTempExt = (LPCSTR)strTemp + (strTemp.GetLength() - 4);

								if(IsBoxFile(pstrTempExt) || IsWscFile(pstrTempExt))
								{
									SetParserError(nLineCount, m_strBasePath, CString("Cannot include file \"") + strTemp + _T("\""));
									return 500;
								}
							}

							if(mInclude == 1)
							{
								CBoxPath path;

								path.Combine(m_strHttpBasePath, BOX_CA2CT(strTemp));
								if(LoadScriptFile(path.m_strPath, strScriptText, nLineCount))
									return 500;
							}else
								if(LoadScriptFile(BOX_CA2CT(strTemp), strScriptText, nLineCount))
									return 500;
						}
					}
				}

				pstrTemp = pstrTemp1;
			}else
			{
				pstrTemp ++;
				nTempCount --;
			}
		}else
		{
			if(!isspace((BYTE)pstrTemp[0]))
				bIsSpace = FALSE;

			if(cl.IsBreak(pstrTemp[0]))
				nLineCount ++;

			pstrTemp ++;
			nTempCount --;
		}

	if(nCount - nTempCount > 0 && !bIsSpace)
	{
		index = AddBlock(pstrText, nCount - nTempCount);
		if(index != -1)
		{
			strScriptText.AppendFormat("Response.WriteBlock(%d)\r\n", index);
			m_nScriptLine ++;
		}
	}

	return 0;
}
