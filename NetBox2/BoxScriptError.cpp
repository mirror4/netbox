// ScriptError.cpp : implementation file
//

#include "stdafx.h"
#include "BoxScriptError.h"


// CBoxScriptError

IMPLEMENT_DYNAMIC(CBoxScriptError, CBoxSafeObject)


BEGIN_DISPATCH_MAP(CBoxScriptError, CBoxSafeObject)
	DISP_PROPERTY(CBoxScriptError, "ASPCode", m_strASPCode, VT_BSTR)
	DISP_PROPERTY(CBoxScriptError, "Number", m_nNumber, VT_I4)
	DISP_PROPERTY(CBoxScriptError, "Source", m_strSource, VT_BSTR)
	DISP_PROPERTY(CBoxScriptError, "Category", m_strCategory, VT_BSTR)
	DISP_PROPERTY(CBoxScriptError, "File", m_strFile, VT_BSTR)
	DISP_PROPERTY(CBoxScriptError, "Line", m_nLine, VT_I4)
	DISP_PROPERTY(CBoxScriptError, "Column", m_nColumn, VT_I4)
	DISP_PROPERTY(CBoxScriptError, "Description", m_strDescription, VT_BSTR)
	DISP_PROPERTY(CBoxScriptError, "ASPDescription", m_strASPDescription, VT_BSTR)
END_DISPATCH_MAP()

// CBoxScriptError message handlers

