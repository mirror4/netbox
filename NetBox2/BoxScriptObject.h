#pragma once
#include "BoxScript.h"
#include "BoxSimpleXml.h"
#include <objsafe.h>

class CBoxScriptObject : public CBoxScript
{
	DECLARE_DYNCREATE(CBoxScriptObject)

	BEGIN_INTERFACE_PART(ScriptDisp, IDispatch)
		static void staticRelease(void* dwParam);
		STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
		STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
		STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
		STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	END_INTERFACE_PART(ScriptDisp)

	BEGIN_INTERFACE_PART(ObjectSafety1, IObjectSafety)
		STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions);
		STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
	END_INTERFACE_PART(ObjectSafety1)

public:
	CBoxScriptObject(void);
	virtual ~CBoxScriptObject(void);

protected:
	virtual void InitScript(void);
	virtual int ParseScriptText(LPCSTR pstrText, int nCount, CStringA& strScriptText, int nIncludeFlagIndex);
	virtual int AfterParse(void);
	virtual void ClearScript(void);
	int MapDispId(void);

public:
	CString& GetProgID(void)
	{
		return m_strProgID;
	}

	CLSID& GetClassID(void)
	{
		return m_ClassID;
	}

	BOOL IsPooled(void)
	{
		return m_pHost->m_arrayExtData[1];
	}

public:
	afx_msg LPDISPATCH get_Object(void);
	afx_msg int Init(void);
	afx_msg long Run(void);
	afx_msg LPDISPATCH LoadObject(LPCTSTR pstrFile);
	afx_msg BSTR get_ProgID(void);
	afx_msg BSTR get_ClassID(void);

	DECLARE_DISPATCH_MAP()

public:
	static CComPtr<IGlobalInterfaceTable> m_pGlobalObject;

private:
	void Clear(void);

	DWORD m_dwCookie;
	CCriticalSection m_csCookie;

	CComDispatchDriver m_pScriptDisp;
	CString m_strProgID;
	CLSID	m_ClassID;
	CBoxObject<CBoxSimpleXml> m_pXml;
	DISPID* m_parrayDispIdMap;
	DISPID m_idDefault;

	int m_nScriptLine;
};
