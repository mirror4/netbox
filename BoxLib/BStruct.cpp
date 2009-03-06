#include "StdAfx.h"
#include "BStruct.h"
#include "BMStream.h"
#include "BStream.h"
#include "typelib\ti.h"
#include <openssl\md5.h>

static ELEMDESC s_mElemDesc = {{{NULL}, VT_VARIANT}, {NULL, 0x1}};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Item", {0x00000000, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Item", {0x00000000, NULL, &s_mElemDesc, FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Item", {0x00000000, NULL, &s_mElemDesc, FUNC_DISPATCH, INVOKE_PROPERTYPUTREF, CC_STDCALL, 1, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

class CBStructExt : public CBStruct
{
public:
	CBStructExt(CBTypeInfo* pTypeInfo) : m_pTypeInfo(pTypeInfo)
	{}

public:
	// IDispatch

	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		if(ppTInfo == NULL)
			return E_POINTER;

		if(iTInfo == 0)
			return m_pTypeInfo->QueryInterface(ppTInfo);

		return E_NOTIMPL;
	}

	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		return m_pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
	}

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		return m_pTypeInfo->Invoke(this, dispIdMember, wFlags, pDispParams,
			pVarResult, pExcepInfo, puArgErr);
	}

	STDMETHOD(GetClassID)(CLSID *pClassID)
	{
		return m_pTypeInfo->GetClassID(pClassID);
	}

public:
	CBComPtr<CBTypeInfo> m_pTypeInfo;
};

static int staticCompareString(CBString* pstr1, CBString* pstr2)
{
	return pstr1->Compare(*pstr2);
}

CLSID CBStruct::CBStructFactory::CBStructTypeInfo::Init(LPCWSTR pstrName, LPCWSTR pstrInit)
{
	m_pTypeInfo = &CBDispatch<IVariantStruct>::g_typeinfo;

	CBString str, strTemp;
	int curPos= 0;
	MD5_CTX ctxMD5;
	CLSID clsid;

	str = pstrInit;
	while(str.Find(L' ') != -1)
		str.Replace(L" ", L"");

	str.MakeLower();

	strTemp = str.Tokenize(L",", curPos);
	while(strTemp != L"")
	{
		if(!strTemp.IsEmpty())m_arrarName.Add(strTemp);
		strTemp = str.Tokenize(L",", curPos);
	}

	qsort(m_arrarName.GetData(), m_arrarName.GetCount(), sizeof(CBString),
		(int (__cdecl *)(const void *,const void *))staticCompareString);

	MD5_Init(&ctxMD5);
	for(UINT n = 0; n < m_arrarName.GetCount(); n ++)
	{
		m_arrayMethod.Add(s_mData[0]);
		m_arrayMethod[n * 3].szName = m_arrarName[n];
		m_arrayMethod[n * 3].Function.memid = 0x65000000 + n;
		m_arrayMethod[n * 3].Function.oVft += n * 12;
		m_arrayMethod.Add(s_mData[1]);
		m_arrayMethod[n * 3 + 1].szName = m_arrarName[n];
		m_arrayMethod[n * 3 + 1].Function.memid = 0x65000000 + n;
		m_arrayMethod[n * 3 + 1].Function.oVft += n * 12;
		m_arrayMethod.Add(s_mData[2]);
		m_arrayMethod[n * 3 + 2].szName = m_arrarName[n];
		m_arrayMethod[n * 3 + 2].Function.memid = 0x65000000 + n;
		m_arrayMethod[n * 3 + 2].Function.oVft += n * 12;

		MD5_Update(&ctxMD5, (LPCWSTR)m_arrarName[n], m_arrarName[n].GetLength() * 2 + 2);
	}
	MD5_Final((unsigned char *)&clsid, &ctxMD5);
	clsid.Data1 = 0x94659465;

	CBTypeInfo::Init(clsid, pstrName, m_arrayMethod.GetData(), m_arrayMethod.GetCount());

	return clsid;
}

STDMETHODIMP CBStruct::CBStructFactory::CBStructTypeInfo::Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, 
			VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	CBComPtr<IVariantStruct> pObject;
	HRESULT hr;

	hr = ((IUnknown*)pvInstance)->QueryInterface(&pObject);
	if(FAILED(hr))return hr;

	if((memid & 0xff000000) != 0x65000000)
		return CBDispatch<IVariantStruct>::g_typeinfo.Invoke(pObject, memid, wFlags, pDispParams, 
				pVarResult, pExcepInfo, puArgErr);
	else if((int)(memid & 0xffffff) < (int)m_arrayMethod.GetCount() * 3)
	{
		UINT cArgs = pDispParams->cArgs;
		UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

		if((wFlags & DISPATCH_PROPERTYPUTREF) && cArgs1 == 0)
		{
			INVOKE_PARAM_NAMED(VT_VARIANT, 0)

			return pObject->putref_Item(memid & 0xffffff, v0);
		}

		if((wFlags & DISPATCH_PROPERTYPUT) && cArgs1 == 0)
		{
			INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 0)

			return pObject->put_Item(memid & 0xffffff, v0);
		}

		INVOKE_PARAM_RET(VT_VARIANT, 0)
		hr = pObject->get_Item(memid & 0xffffff, v0);

		INVOKE_DISP;
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBStruct::CBStructFactory::CBStructFactory(LPCWSTR pstrName, LPCWSTR pstrInit)
{
	m_pTypeInfo.CreateInstance();
	m_clsid = m_pTypeInfo->Init(pstrName, pstrInit);
}

STDMETHODIMP CBStruct::CBStructFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
{
	if(ppvObj == NULL)
		return E_POINTER;

	if(pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	CBStructExt* p;

	p = new CBStructExt(m_pTypeInfo);
	if(p == NULL)
		return E_OUTOFMEMORY;

	p->setCount(m_pTypeInfo->m_arrarName.GetCount());
	HRESULT hr = p->QueryInterface(riid, ppvObj);
	p->InternalRelease();

	return hr;
}

CBStruct::~CBStruct(void)
{
	ClearAll();
}

void CBStruct::ClearAll(void)
{
	m_cs.Enter();

	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
		VariantClear(&m_arrayVariant[i]);

	m_cs.Leave();
}

STDMETHODIMP CBStruct::get_Item(long id, VARIANT *pvar)
{
	if(id == -1)
		return WriteObjectToVariant((IVariantList*)this, pvar);

	if(id < 0)return DISP_E_BADINDEX;

	m_cs.Enter();
	if(id < (int)m_arrayVariant.GetCount())
	{
		VariantCopy(pvar, &m_arrayVariant[id]);
		m_cs.Leave();

		return S_OK;
	}
	m_cs.Leave();

	return DISP_E_BADINDEX;
}

STDMETHODIMP CBStruct::put_Item(long id, VARIANT pvar)
{
	if(id < 0)return DISP_E_BADINDEX;

	m_cs.Enter();
	if(id < (int)m_arrayVariant.GetCount())
	{
		VariantCopyInd(&m_arrayVariant[id], &pvar);
		m_cs.Leave();
		return S_OK;
	}
	m_cs.Leave();

	return DISP_E_BADINDEX;
}

STDMETHODIMP CBStruct::putref_Item(long id, VARIANT pvar)
{
	return put_Item(id, pvar);
}

STDMETHODIMP CBStruct::Load(VARIANT VarSrc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBStruct* pThis = this;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return LoadObjectFromStream(pStream, CLSID_NULL, (void**)&pThis);
}

STDMETHODIMP CBStruct::Save(VARIANT VarDesc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return WriteObjectToStream((IQueue*)this, pStream);
}

STDMETHODIMP CBStruct::GetClassID(CLSID *pClassID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBStruct::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBStruct::Load(IStream *pStm)
{
	HRESULT hr;
	size_t i;
 
	CBLock l(&m_cs);

	for(i = 0; i < m_arrayVariant.GetCount(); i ++)
	{
		hr = ((CBVariant*)&m_arrayVariant[i])->LoadFromStream(pStm);
		if(FAILED(hr))return hr;
	}

	return S_OK;
}

STDMETHODIMP CBStruct::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	size_t i;

	CBLock l(&m_cs);

	for(i = 0; i < m_arrayVariant.GetCount(); i ++)
	{
		hr = ((CBVariant*)&m_arrayVariant[i])->WriteToStream(pStm);
		if(FAILED(hr))return hr;
	}

	return S_OK;
}

STDMETHODIMP CBStruct::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBStruct::InitNew(void)
{
	ClearAll();
	return S_OK;
}
