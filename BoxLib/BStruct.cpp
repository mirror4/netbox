#include "StdAfx.h"
#include "BStruct.h"
#include "BMStream.h"
#include "BStream.h"
#include "BListEx.h"
#include "typelib\ti.h"
#include <openssl\md5.h>

#include "msado15.tlh"

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

HRESULT CBStruct::toJsonValue(Json::Value &root, CAtlArray<void*> &arrObjects)
{
	HRESULT hr;
	CBStringA strValue;

	CBLock l(&m_cs);

	arrObjects.Add((void *)this);

	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
	{
		if(m_arrayVariant[i].vt == VT_BSTR)
			strValue = m_arrayVariant[i].bstrVal;
		else if (m_arrayVariant[i].vt == VT_DISPATCH)
		{
			CComQIPtr<IVariantDictionary, &IID_IVariantDictionary> pDic;
			pDic = m_arrayVariant[i].pdispVal;
			if (pDic)
			{
				Json::Value value;

				for(int j=0;j<(int)arrObjects.GetCount();j++)
				{
					if (arrObjects[j] == (void *)(CBDictionary *)pDic.p)
					{
						return CBComObject::SetErrorInfo(L"Duplicate object.");
					}
				}
				hr = ((CBDictionary *)pDic.p)->toJsonValue(value, arrObjects);
				if (FAILED(hr))
				{
					return hr;
				}
				pDic.Release();
				root.append(value);
			}
			else
			{
				CComQIPtr<IVariantList, &IID_IVariantList> pList;

				pList = m_arrayVariant[i].pdispVal;
				if (pList)
				{
					Json::Value value;

					for(int j=0;j<(int)arrObjects.GetCount();j++)
					{
						if (arrObjects[j] == (void *)(CBListEx *)pList.p)
						{
							return CBComObject::SetErrorInfo(L"Duplicate object.");
						}
					}
					hr = ((CBListEx *)pList.p)->toJsonValue(value, arrObjects);
					if (FAILED(hr))
					{
						return hr;
					}
					pList.Release();
					root.append(value);
				}
				else
				{
					//CComQIPtr<_Recordset, &__uuidof(_Recordset)> prs;
					_RecordsetPtr prs = m_arrayVariant[i].pdispVal;
					if (prs)
					{
						Json::Value table, fields;
						CComVariant var;
						LONG nCount;
					    _variant_t Index;
						Index.vt = VT_I2;
						
						nCount = prs->Fields->Count;
						for (int j=0;j<nCount;j++)
						{
							Index.iVal = j;
							_bstr_t bstrname = prs->Fields->GetItem(Index)->Name;
							strValue = bstrname.GetBSTR();
							fields.append((const char *)strValue);
						}
						table.append(fields);
						prs->MoveFirst();
						while (true)
						{
							if (prs->GetEndOfFile()) break;
							Json::Value values;
							for (int j=0;j<nCount;j++)
							{
								Index.iVal = j;
								_variant_t value = prs->Fields->Item[Index]->Value;
								switch (value.vt)
								{
									case VT_NULL:
									case VT_EMPTY:
										values.append(Json::nullValue);
										break;
									case VT_BOOL:
										values.append(value.boolVal);
										break;
									case VT_INT:
										values.append(value.intVal);
										break;
									case VT_I1:
										values.append(value.cVal);
										break;
									case VT_I2:
										values.append(value.iVal);
										break;
									case VT_I4:
										values.append(value.lVal);
										break;
									case VT_I8:
										values.append(value.llVal);
										break;
									case VT_UINT:
										values.append(value.uintVal);
										break;
									case VT_UI1:
										values.append(value.bVal);
										break;
									case VT_UI2:
										values.append(value.uiVal);
										break;
									case VT_UI4:
										values.append((unsigned int)value.ulVal);
										break;
									case VT_UI8:
										values.append(value.ullVal);
										break;
									case VT_R4:
										values.append((double)value.fltVal);
										break;
									case VT_R8:
										values.append(value.dblVal);
										break;
									case VT_CY:
										hr = VariantChangeType(&var, &value, VARIANT_ALPHABOOL, VT_R8);
										values.append(var.dblVal);
										break;
									case VT_DECIMAL:
										hr = VariantChangeType(&var, &value, VARIANT_ALPHABOOL, VT_I8);
										values.append(var.llVal);
										break;
									case VT_DATE:
										hr = VariantChangeType(&var, &value, VARIANT_ALPHABOOL, VT_BSTR);
										if(FAILED(hr))
										{
											return hr;
										}
										strValue = var.bstrVal;
										values.append((const char *)strValue);
										break;
									case VT_BSTR:
										strValue = value.bstrVal;
										values.append((const char *)strValue);
										break;
									default:
										return E_INVALIDARG;
										break;
								}
							}
							table.append(values);
							prs->MoveNext();
						}
						root.append(table);
					}
					else
					{
						return CBComObject::SetErrorInfo(L"Not Internal Object.");
					}
				}
			}
		}
		else
		{
			CComVariant var;

			switch(m_arrayVariant[i].vt)
			{
				case VT_NULL:
				case VT_EMPTY:
					root.append(Json::nullValue);
					break;
				case VT_BOOL:
					root.append(m_arrayVariant[i].boolVal);
					break;
				case VT_INT:
					root.append(m_arrayVariant[i].intVal);
					break;
				case VT_I1:
					root.append(m_arrayVariant[i].cVal);
					break;
				case VT_I2:
					root.append(m_arrayVariant[i].iVal);
					break;
				case VT_I4:
					root.append(m_arrayVariant[i].lVal);
					break;
				case VT_I8:
					root.append(m_arrayVariant[i].llVal);
					break;
				case VT_UINT:
					root.append(m_arrayVariant[i].uintVal);
					break;
				case VT_UI1:
					root.append(m_arrayVariant[i].bVal);
					break;
				case VT_UI2:
					root.append(m_arrayVariant[i].uiVal);
					break;
				case VT_UI4:
					root.append((unsigned int)m_arrayVariant[i].ulVal);
					break;
				case VT_UI8:
					root.append(m_arrayVariant[i].ullVal);
					break;
				case VT_R4:
					root.append((double)m_arrayVariant[i].fltVal);
					break;
				case VT_R8:
					root.append(m_arrayVariant[i].dblVal);
					break;
				case VT_CY:
					hr = VariantChangeType(&var, &m_arrayVariant[i], VARIANT_ALPHABOOL, VT_R8);
					root.append(var.dblVal);
					break;
				case VT_DECIMAL:
					hr = VariantChangeType(&var, &m_arrayVariant[i], VARIANT_ALPHABOOL, VT_I8);
					root.append(var.llVal);
					break;
				case VT_DATE:
					hr = VariantChangeType(&var, &m_arrayVariant[i], VARIANT_ALPHABOOL, VT_BSTR);
					if(FAILED(hr))
					{
						return hr;
					}
					strValue = var.bstrVal;
					root.append((const char *)strValue);
					break;
				case VT_BSTR:
					strValue = m_arrayVariant[i].bstrVal;
					root.append((const char *)strValue);
					break;
				default:
					return E_INVALIDARG;
					break;
			}
		}
	}

	arrObjects.RemoveAt(arrObjects.GetCount()-1);

	return S_OK;
}

HRESULT CBStruct::fromJsonValue(Json::Value &root)
{
	if (!root.isArray())
		return E_INVALIDARG;
	
	CComVariant varEmpty;
	CBComPtr<CBListEx> pList;
	CBComPtr<CBDictionary> pDic;
	CComDispatchDriver pDisp;
	HRESULT hr;

	CBLock l(&m_cs);
	for (size_t i=0;i<root.size();i++)
	{
		size_t n = m_arrayVariant.Add(varEmpty);
		switch(root[i].type())
		{
			case Json::nullValue:
				(*((CComVariant*)&m_arrayVariant[n])).ChangeType(VT_NULL);
				break;
			case Json::intValue:
				if (root[i].asLargestInt()>>32)
					*((CComVariant*)&m_arrayVariant[n]) = root[i].asDouble();
				else
					*((CComVariant*)&m_arrayVariant[n]) = root[i].asInt();
				break;
			case Json::uintValue:
				if (root[i].asLargestInt()>>31)
					*((CComVariant*)&m_arrayVariant[n]) = root[i].asDouble();
				else
					*((CComVariant*)&m_arrayVariant[n]) = (int)root[i].asUInt();
				break;
			case Json::realValue:
				*((CComVariant*)&m_arrayVariant[n]) = root[i].asDouble();
				break;
			case Json::stringValue:
				*((CComVariant*)&m_arrayVariant[n]) = root[i].asCString();
				break;
			case Json::booleanValue:
				*((CComVariant*)&m_arrayVariant[n]) = (bool)root[i].asBool();
				break;
			case Json::arrayValue:
				pList.CreateInstance();
				if (pList==NULL)
					return E_UNEXPECTED;
				pList->fromJsonValue(root[i]);
				hr = pList.QueryInterface(IID_IDispatch, (void **)&pDisp);
				if (FAILED(hr))
					return hr;
				*((CComVariant*)&m_arrayVariant[n]) = pDisp;
				pList.Release();
				pDisp.Release();
				break;
			case Json::objectValue:
				pDic.CreateInstance();
				if (pDic==NULL)
					return E_UNEXPECTED;
				pDic->fromJsonValue(root[i]);
				hr = pDic.QueryInterface(IID_IDispatch, (void **)&pDisp);
				if (FAILED(hr))
					return E_UNEXPECTED;
				*((CComVariant*)&m_arrayVariant[n]) = pDisp;
				pDic.Release();
				pDisp.Release();
				break;
			default:
				return E_UNEXPECTED;
		}

	}
	return S_OK;
}