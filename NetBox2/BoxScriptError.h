
#pragma once

#include "BoxScript.h"

// CScriptError command target

class CBoxScriptError : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxScriptError)
public:
	void Bind(CBoxScript::CScriptError &error)
	{
		m_strCategory = error.m_strSource;
		m_nNumber = error.m_sCode;
		m_strFile = error.m_strFile;
		m_nLine = error.m_nLine;
		m_nColumn = -1;
		m_strDescription = error.m_strDescription;
	}

public:
	DECLARE_DISPATCH_MAP()

private:
	CString m_strASPCode;
	long m_nNumber;
	CString m_strSource;
	CString m_strCategory;
	CString m_strFile;
	long m_nLine;
	long m_nColumn;
	CString m_strDescription;
	CString m_strASPDescription;
};


