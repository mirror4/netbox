#include "StdAfx.h"
#include "NetBox2.h"
#include "BoxScriptObject.h"

CComPtr<IGlobalInterfaceTable> CBoxScriptObject::m_pGlobalObject;

IMPLEMENT_DYNCREATE(CBoxScriptObject, CBoxScript)

CBoxScriptObject::CBoxScriptObject(void) :
	m_parrayDispIdMap(NULL),
	m_idDefault(-1),
	m_nScriptLine(0)
{
	ZeroMemory(&m_ClassID, sizeof(m_ClassID));
}

CBoxScriptObject::~CBoxScriptObject(void)
{
	Clear();
}

void CBoxScriptObject::InitScript(void)
{
	m_pHost->m_arrayExtData[0] = 0;
	m_pHost->m_arrayExtData[1] = 0;
}

void CBoxScriptObject::ClearScript(void)
{
	Clear();
	CBoxScript::ClearScript();
}

void CBoxScriptObject::Clear(void)
{
	if(m_pScriptDisp != NULL)
	{
		if(m_dwThreadId == theApp.m_nThreadID)
			m_pGlobalObject->RevokeInterfaceFromGlobal(m_dwCookie);

		m_pScriptDisp.Release();
	}

	m_strProgID.Empty();
	ZeroMemory(&m_ClassID, sizeof(m_ClassID));

	if(m_parrayDispIdMap != NULL)
	{
		delete m_parrayDispIdMap;
		m_parrayDispIdMap = NULL;
	}
}

int CBoxScriptObject::ParseScriptText(LPCSTR pstrText, int nCount, CStringA& strScriptText, int nIncludeFlagIndex)
{
	m_pXml.CreateInstance();
	if(m_pXml == NULL)return 500;
	if(m_pXml->Parse(pstrText, nCount))
	{
		SetParserError(m_pXml->m_nErrorLine, m_strBasePath, m_pXml->m_strDescription);
		return 500;
	}

	CBoxObject<CBoxSimpleXml> pNodeComponent;

	if(!m_pXml->LookupNode(_T("component"), pNodeComponent))return 500;

	CBoxObject<CBoxSimpleXml> pNode;

	if(pNodeComponent->LookupProcessing(_T("component"), pNode))
	{
		CString str;

		if(pNode->LookupAttribute(_T("debug"), str))
		{
			m_bEnableDebug =  !str.CompareNoCase(_T("true")) || !str.CompareNoCase(_T("on"));
			m_bStepDebug = !str.CompareNoCase(_T("step"));
		}

		if(pNode->LookupAttribute(_T("timeout"), str))
			m_pHost->m_nTimeout = atoi(str);

		if(pNode->LookupAttribute(_T("pool"), str))
			m_pHost->m_arrayExtData[1] = !str.CompareNoCase(_T("true")) || !str.CompareNoCase(_T("on"));
	}

	if(pNodeComponent->LookupNode(_T("registration"), pNode))
	{
		CString str;

		pNode->m_mapAttributes.Lookup(_T("progid"), m_strProgID);
		pNode->m_mapAttributes.Lookup(_T("classid"), str);
		::CLSIDFromString((LPWSTR)(LPCWSTR)CStringW(str), &m_ClassID);
	}

	if(!pNodeComponent->LookupNode(_T("script"), pNode))return 500;
	if(!pNode->m_mapAttributes.Lookup(_T("language"), m_strLanguage))return 500;

	m_nScriptLine = pNode->m_nCDataLine;
	AddLineMap(nIncludeFlagIndex, pNode->m_nCDataLine);
	strScriptText = pNode->m_strCData;

	return 0;
}

int CBoxScriptObject::AfterParse(void)
{
	if(m_pXml == NULL)return 500;

	CString str;
	CStringW wstr, wstrName, wstrinternalName, wstrGet, wstrPut;
	WORD wFlags;
	LPWSTR* pDispMapStrings;
	LPWSTR pString;
	int nParamCount;

	CBoxObject<CBoxSimpleXml> pNodeComponent;
	if(!m_pXml->LookupNode(_T("component"), pNodeComponent))return 500;

	CBoxObject<CBoxSimpleXml> pNode;

	if(!pNodeComponent->LookupNode(_T("public"), pNode))return 500;

	CBoxSimpleXml* pItem;
	CBoxSimpleXml* pItem1;

	POSITION pos = pNode->m_listNodes.GetHeadPosition();
	POSITION pos1;

	while(pos)
	{
		pItem = pNode->m_listNodes.GetNext(pos);
		if(!pItem->m_strName.Compare(_T("property")))
			wFlags = DISPATCH_PROPERTYGET;
		else if(!pItem->m_strName.Compare(_T("method")))
			wFlags = DISPATCH_METHOD;
		else continue;

		if(wFlags != 0)
		{
			wstrName.Empty();
			wstrinternalName.Empty();
			wstrGet.Empty();
			wstrPut.Empty();

			if(!pItem->LookupAttribute(_T("name"), wstrName))
				continue;

			pItem->LookupAttribute(_T("internalName"), wstrinternalName);

			nParamCount = -1;

			if(wFlags == DISPATCH_PROPERTYGET)
			{
				pItem->LookupAttribute(_T("get"), wstrGet);
				pItem->LookupAttribute(_T("put"), wstrPut);

				pos1 = pItem->m_listNodes.GetHeadPosition();

				while(pos1)
				{
					pItem1 = pItem->m_listNodes.GetNext(pos1);

					if(!pItem1->m_strName.Compare(_T("get")))
					{
						if(!pItem1->LookupAttribute(_T("internalName"), wstrGet))
						{
							wstrGet = L"get_";
							wstrGet += wstrName;
						}
					}else if(!pItem1->m_strName.Compare(_T("put")))
					{
						if(!pItem1->LookupAttribute(_T("internalName"), wstrPut))
						{
							wstrPut = L"put_";
							wstrPut += wstrName;
						}
					}else if(!pItem1->m_strName.Compare(_T("parameter")))
						if(nParamCount == -1)nParamCount = 1;
						else nParamCount ++;
				}
			}else
			{
				pos1 = pItem->m_listNodes.GetHeadPosition();
				while(pos1)
				{
					pItem1 = pItem->m_listNodes.GetNext(pos1);

					if(!pItem1->m_strName.Compare(_T("parameter")))
						if(nParamCount == -1)nParamCount = 1;
						else nParamCount ++;
				}
			}

			if(!wstrGet.IsEmpty() || !wstrPut.IsEmpty())
				wstrinternalName.Empty();

			pDispMapStrings = (LPWSTR*)(new char[sizeof(LPWSTR) * 5 + 8 +
				wstrName.GetLength() * 2 +
				wstrinternalName.GetLength() * 2 +
				wstrGet.GetLength() * 2 +
				wstrPut.GetLength() * 2]);

			pString = (LPWSTR)&pDispMapStrings[5];

			wcsncpy(pString, wstrName, wstrName.GetLength() + 1);
			pDispMapStrings[0] = pString;
			pString += wstrName.GetLength() + 1;

			if(!wstrinternalName.IsEmpty())
			{
				wcsncpy(pString, wstrinternalName, wstrinternalName.GetLength() + 1);
				pDispMapStrings[1] = pString;
				pString += wstrinternalName.GetLength() + 1;
			}else pDispMapStrings[1] = NULL;

			if(!wstrGet.IsEmpty())
			{
				wcsncpy(pString, wstrGet, wstrGet.GetLength() + 1);
				pDispMapStrings[2] = pString;
				pString += wstrGet.GetLength() + 1;
			}else pDispMapStrings[2] = NULL;

			if(!wstrPut.IsEmpty())
			{
				wcsncpy(pString, wstrPut, wstrPut.GetLength() + 1);
				pDispMapStrings[3] = pString;
				pString += wstrPut.GetLength() + 1;
			}else pDispMapStrings[3] = NULL;

			pDispMapStrings[4] = (LPWSTR)nParamCount;

			m_pHost->m_arrayBlock.Add(pDispMapStrings);

			if(pItem->LookupAttribute(_T("dispid"), str) && !_tstoi(str))
					m_pHost->m_arrayExtData[0] = (int)m_pHost->m_arrayBlock.GetCount();
		}
	}

	m_pXml.Release();

	return 0;
}

int CBoxScriptObject::MapDispId(void)
{
	int i, j, nCount = (int)m_pHost->m_arrayBlock.GetCount();

	if(nCount == 0)
		return 500;

	m_parrayDispIdMap = new DISPID[nCount * 3];

	for(i = 0; i < nCount; i ++)
	{
		LPWSTR* pDispMapStrings = (LPWSTR*)m_pHost->m_arrayBlock[i];
		int n = 0;

		for(j = 0; j < 3; j ++)
			if(pDispMapStrings[j + 1] != NULL)
			{
				m_pScriptDisp.GetIDOfName(pDispMapStrings[j + 1], &m_parrayDispIdMap[i * 3 + j]);
				n ++;
			}else m_parrayDispIdMap[i * 3 + j] = -1;

		if(n == 0)
			m_pScriptDisp.GetIDOfName(pDispMapStrings[0], &m_parrayDispIdMap[i * 3]);
	}

	return 0;
}

BEGIN_DISPATCH_MAP(CBoxScriptObject, CBoxScript)
	DISP_PROPERTY_EX(CBoxScriptObject, "ProgID", get_ProgID, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxScriptObject, "ClassID", get_ClassID, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxScriptObject, "Object", get_Object, SetNotSupported, VT_DISPATCH)

	DISP_FUNCTION(CBoxScript, "Init", Init, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxScript, "Run", Run, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxScript, "LoadObject", LoadObject, VT_DISPATCH, VTS_BSTR)
END_DISPATCH_MAP()


BSTR CBoxScriptObject::get_ProgID(void)
{
	return m_strProgID.AllocSysString();
}

BSTR CBoxScriptObject::get_ClassID(void)
{
	LPOLESTR ppsz = NULL;
	BSTR bstr = NULL;

	::StringFromCLSID(m_ClassID, &ppsz);

	if(ppsz)
	{
		bstr = ::SysAllocString(ppsz);
		CoTaskMemFree(ppsz);
	}

	return bstr;
}

int CBoxScriptObject::Init()
{
	if(m_pHost->m_arrayExtData[1])
		AddNameItem(_T("NetBox"), theApp.m_pSystem);
	else
		SetGlobalObjects();

	if(Run() != 0)
		return 500;

	m_pScriptDisp = m_pHost->GetScriptDispatch();
	if(m_pScriptDisp == NULL)
		return 500;

	if(MapDispId() != 0)
		return 500;

	if(m_dwThreadId == theApp.m_nThreadID)
		m_pGlobalObject->RegisterInterfaceInGlobal(m_pScriptDisp, IID_IDispatch, &m_dwCookie);

	return 0;
}

long CBoxScriptObject::Run(void)
{
	int iResult = m_pHost->Run();
	if(iResult != 0)
		GetScriptErrorInfo();

	return iResult;
}

LPDISPATCH CBoxScriptObject::get_Object(void)
{
	ExternalAddRef();
	return &m_xScriptDisp;
}

LPDISPATCH CBoxScriptObject::LoadObject(LPCTSTR pstrFile)
{
	if(Load(pstrFile) != 0)
		return NULL;

	if(Init() != 0)
		return NULL;

	ExternalAddRef();
	return &m_xScriptDisp;
}

STDMETHODIMP_(ULONG) CBoxScriptObject::XScriptDisp::AddRef()
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ScriptDisp)
	return pThis->ExternalAddRef();
}

void CBoxScriptObject::XScriptDisp::staticRelease(void* dwParam)
{
	ULONG* pParam = (ULONG*)dwParam;
	pParam[1] = ((CBoxScriptObject*)pParam[0])->ExternalRelease();
}

STDMETHODIMP_(ULONG) CBoxScriptObject::XScriptDisp::Release()
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ScriptDisp)
	if(pThis->m_dwThreadId == theApp.m_nThreadID && pThis->m_dwThreadId != GetCurrentThreadId())
	{
		ULONG param[2];
		param[0] = (ULONG)pThis;
		theApp.CallProc(staticRelease, param);
		return param[1];
	}
	return pThis->ExternalRelease();
}

STDMETHODIMP CBoxScriptObject::XScriptDisp::QueryInterface(REFIID iid, LPVOID far* ppvObj)
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ScriptDisp)

	*ppvObj = NULL;

	if (IsEqualIID(iid, IID_IUnknown))
		*ppvObj = (IUnknown*)this;
	else if (IsEqualIID(iid, IID_IDispatch))
		*ppvObj = (IDispatch*)this;
	else if (IsEqualIID(iid, IID_IObjectSafety))
		*ppvObj = &pThis->m_xObjectSafety1;

	if(*ppvObj)
	{
		AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}

STDMETHODIMP CBoxScriptObject::XScriptDisp::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxScriptObject::XScriptDisp::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxScriptObject::XScriptDisp::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ScriptDisp)

	if(pThis->m_pScriptDisp != NULL)
	{
		int nCount = (int)pThis->m_pHost->m_arrayBlock.GetCount();

		for(UINT i = 0; i < cNames; i ++)
			for(int j = 0; j < nCount; j ++)
				if(!_wcsicmp(rgszNames[i], ((LPWSTR*)pThis->m_pHost->m_arrayBlock[j])[0]))
				{
					rgDispId[i] = j + 1;
					break;
				}
		return S_OK;
	}

	return E_NOTIMPL;
}

STDMETHODIMP CBoxScriptObject::XScriptDisp::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    HRESULT hr = E_NOTIMPL;
	DWORD dwThreadId;
	CComDispatchDriver pMainObject;

	METHOD_PROLOGUE_EX_(CBoxScriptObject, ScriptDisp)

	dwThreadId = GetCurrentThreadId();

	if(pThis->m_dwThreadId != dwThreadId)
	{
		if(pThis->m_dwThreadId == theApp.m_nThreadID)
		{
			pThis->m_csCookie.Lock();
			m_pGlobalObject->GetInterfaceFromGlobal(pThis->m_dwCookie, IID_IDispatch, (void**)&pMainObject);
			pThis->m_csCookie.Unlock();
		}

		if(pMainObject == NULL)
		{
			if(pExcepInfo)
			{
				pExcepInfo->scode = 0x800A000D;
				pExcepInfo->bstrSource = SysAllocString(L"NetBox ScriptObject");
				pExcepInfo->bstrDescription = SysAllocString(L"Cannot use an instance of a script object across the different thread.");

				return DISP_E_EXCEPTION;
			}else return DISP_E_BADCALLEE;
		}
	}

	if(dispIdMember == 0)
		dispIdMember = pThis->m_pHost->m_arrayExtData[0];

	if(dispIdMember > 0)
	{
		dispIdMember --;

		if(pThis->m_pScriptDisp != NULL && dispIdMember >= 0 && dispIdMember < pThis->m_pHost->m_arrayBlock.GetCount())
		{
			int nParamCount = ((int*)pThis->m_pHost->m_arrayBlock[dispIdMember])[4];
			int nNamedArgs = 0;

			if(pThis->m_parrayDispIdMap[dispIdMember * 3] != -1)
				dispIdMember = pThis->m_parrayDispIdMap[dispIdMember * 3];
			else if(wFlags & (DISPATCH_PROPERTYGET | DISPATCH_METHOD))
			{
				dispIdMember = pThis->m_parrayDispIdMap[dispIdMember * 3 + 1];
				wFlags = DISPATCH_METHOD;
			}else
			{
				nNamedArgs = 1;
				nParamCount += nNamedArgs;
				pDispParams->cNamedArgs = 0;
				dispIdMember = pThis->m_parrayDispIdMap[dispIdMember * 3 + 2];
				wFlags = DISPATCH_METHOD;
			}

			DISPPARAMS NewParams;

			if((wFlags & DISPATCH_METHOD) && (int)pDispParams->cArgs < nParamCount)
			{
				ZeroMemory(&NewParams, sizeof(NewParams));

				NewParams.rgvarg = new VARIANTARG[nParamCount];
				ZeroMemory(NewParams.rgvarg, sizeof(VARIANTARG) * nParamCount);
				if(nNamedArgs)
					NewParams.rgvarg[0] = pDispParams->rgvarg[0];
				if(nNamedArgs < (int)pDispParams->cArgs)
					CopyMemory(NewParams.rgvarg + nParamCount - pDispParams->cArgs + nNamedArgs, pDispParams->rgvarg + nNamedArgs, (pDispParams->cArgs - nNamedArgs) * sizeof(VARIANTARG));
				NewParams.cArgs = nParamCount;

				for(int i = nNamedArgs; i < (int)(nParamCount - pDispParams->cArgs + nNamedArgs); i ++)
					NewParams.rgvarg[i].vt = VT_ERROR;

				pDispParams = &NewParams;
			}

			if(pMainObject != NULL)
				hr = pMainObject->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
			else hr = pThis->m_pScriptDisp->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

			if(pDispParams == &NewParams)delete NewParams.rgvarg;

			if(FAILED(hr) && pThis->m_pHost->m_scode)
			{
				if(pExcepInfo)
				{
					pExcepInfo->scode = pThis->m_pHost->m_scode;
					pExcepInfo->bstrSource = pThis->m_pHost->m_strErrorSource.AllocSysString();
					pExcepInfo->bstrDescription = pThis->m_pHost->m_strErrorMessage.AllocSysString();
				}
				return DISP_E_EXCEPTION;
			}
		}
	}

	return hr;
}

STDMETHODIMP_(ULONG) CBoxScriptObject::XObjectSafety1::AddRef()
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ObjectSafety1)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBoxScriptObject::XObjectSafety1::Release()
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ObjectSafety1)
	return pThis->ExternalRelease();
}

STDMETHODIMP CBoxScriptObject::XObjectSafety1::QueryInterface(REFIID iid, LPVOID far* ppvObj)
{
	METHOD_PROLOGUE_EX_(CBoxScriptObject, ObjectSafety1)

	return pThis->m_xScriptDisp.QueryInterface(iid, ppvObj);
}

STDMETHODIMP CBoxScriptObject::XObjectSafety1::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
{
	if (!s_bObjectSafety)
		return E_NOINTERFACE;

	if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
		return E_POINTER;

	*pdwSupportedOptions = 3;
	*pdwEnabledOptions   = 1;

	return S_OK;
}

STDMETHODIMP CBoxScriptObject::XObjectSafety1::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	if (!s_bObjectSafety)
		return E_NOINTERFACE;

	return S_OK;
}
