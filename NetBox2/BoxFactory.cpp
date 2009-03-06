#include "StdAfx.h"
#include "NetBox2.h"
#include "BoxObject.h"
#include "atlbase.h"
#include "BoxFactory.h"
#include "BoxScriptObject.h"


STDMETHODIMP CBoxFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
{
	if(ppvObj == NULL)
		return E_POINTER;

	if(pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	if(m_pClass != NULL)
	{
		CCmdTarget* pObject = (CCmdTarget*)m_pClass->CreateObject();

		if(pObject != NULL)
		{
			*ppvObj = pObject->GetIDispatch(FALSE);
			return S_OK;
		}
		return CO_E_CLASSSTRING;
	}

	if(!m_strScriptName.IsEmpty())
	{
		static VARIANT varFalse;
		try
		{
			*ppvObj = theApp.m_pSystem->LoadObject(m_strScriptName, varFalse);
		}catch(CException* e)
		{
			e->Delete();

			return CO_E_CLASSSTRING;
		}

		return S_OK;
	}

	if(m_pDisp != NULL)
		return m_pDisp->QueryInterface(IID_IDispatch, ppvObj);

	return CO_E_CLASSSTRING;
}

STDMETHODIMP CBoxStruct::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxStruct::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxStruct::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxStruct::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxStruct::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
{
	if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
		return E_POINTER;

	*pdwSupportedOptions = 3;
	*pdwEnabledOptions   = 1;

	return S_OK;
}

STDMETHODIMP CBoxStruct::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	return S_OK;
}
