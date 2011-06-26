#include "StdAfx.h"
#include "BDictionary.h"
#include "BVarType.h"
#include "BMStream.h"
#include "BStream.h"
#include "BListex.h"
#include <atlsafe.h>
#include "msado15.tlh"



CBCriticalSection CBDictionary::m_csContents;

STDMETHODIMP CBDictionary::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hr = CBDispatch<IVariantDictionary>::g_typeinfo.GetIDsOfNames(rgszNames, cNames, rgDispId);
	if(SUCCEEDED(hr))return hr;

	UINT i, j, count;
	CComVariant varEmpty;
	CBVariant varKey;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;

	varKey.vt = VT_BSTR;

	m_cs.Enter();

	for(i = 0; i < cNames; i ++)
	{
		count = (int)m_posArray.GetCount();

		varKey.bstrVal = rgszNames[i];
		pPair = m_mapItems.Lookup(varKey);

		if(pPair != NULL)
		{
			for(j = 0; j < count; j ++)
				if(m_posArray[j] == pPair)
				{
					rgDispId[i] = j | 0x65000000;
					break;
				}
		}else
		{
			m_cs.Leave();

			varKey.vt = VT_EMPTY;

			return DISP_E_MEMBERNOTFOUND;
		}
	}

	m_cs.Leave();

	varKey.vt = VT_EMPTY;

	return S_OK;
}

STDMETHODIMP CBDictionary::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if((dispIdMember & 0xff000000) != 0x65000000)
		return CBDispatch<IVariantDictionary>::g_typeinfo.Invoke((IVariantDictionary*)this, dispIdMember, wFlags,
			pDispParams, pVarResult, pExcepInfo, puArgErr);

	dispIdMember &= 0xffffff;

	CBLock l(&m_cs);

	if(dispIdMember < (int)m_posArray.GetCount())
	{
		if((wFlags & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF)) && pDispParams->cArgs == 1)
		{
			VARIANT* pvar = pDispParams->rgvarg;

			while(pvar->vt == (VT_VARIANT | VT_BYREF))
				pvar = pvar->pvarVal;

			if(!(wFlags & DISPATCH_PROPERTYPUTREF) &&
				(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH))
			{
				l.Unlock();

				if(pvar->punkVal == NULL)
					return DISP_E_UNKNOWNINTERFACE;

				CBDispatchPtr pDisp;
				CComVariant var;

				HRESULT hr = pvar->punkVal->QueryInterface(&pDisp);
				if(FAILED(hr))return hr;

				hr = pDisp.GetProperty(0, &var);
				if(FAILED(hr))return hr;

				l.Lock();
				if(dispIdMember < (int)m_posArray.GetCount())
				{
					var.Detach(&m_posArray[dispIdMember]->m_value);
					return S_OK;
				}else return DISP_E_MEMBERNOTFOUND;
			}else return VariantCopyInd(&m_posArray[dispIdMember]->m_value, pvar);
		}else if(pDispParams->cArgs == 0)
		{
			if(pVarResult != NULL)
				return VariantCopy(pVarResult, &m_posArray[dispIdMember]->m_value);

			return S_OK;
		}

		VARIANT *pvar = &m_posArray[dispIdMember]->m_value;

		if(pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH)
		{
			CBDispatchPtr pDisp;

			HRESULT hr = pvar->punkVal->QueryInterface(&pDisp);
			if(FAILED(hr))
				return DISP_E_BADPARAMCOUNT;

			l.Unlock();
			return pDisp->Invoke(0, riid, lcid, wFlags, pDispParams,
					pVarResult, pExcepInfo, puArgErr);
		}

		return DISP_E_BADPARAMCOUNT;
	}else return DISP_E_MEMBERNOTFOUND;

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Item(VARIANT VarKey, VARIANT *pvar)
{
	if(VarKey.vt & VT_ARRAY)
		return E_INVALIDARG;

	if(VarKey.vt == VT_ERROR)
		return WriteObjectToVariant((IVariantDictionary*)this, pvar);

	m_cs.Enter();
	if(m_bArrayMode && (VarKey.vt == VT_I4 || VarKey.vt == VT_I2))
	{
		long i = varGetNumbar(VarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			VariantCopy(pvar, &m_posArray[i]->m_value);
			m_cs.Leave();
			return S_OK;
		}else
		{
			m_cs.Leave();
			return DISP_E_BADINDEX;
		}
	}

	m_mapItems.Lookup(*(CBVariant*)&VarKey, *(CComVariant*)pvar);
	m_cs.Leave();

	return S_OK;
}

HRESULT CBDictionary::putItem(VARIANT* pvarKey, VARIANT* pvar)
{
	if(pvarKey->vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(pvarKey->vt & VT_ARRAY)return E_INVALIDARG;

	if(pvarKey->vt == VT_BSTR)
	{
		if(pvarKey->bstrVal == NULL)
			return DISP_E_TYPEMISMATCH;

		UINT len = SysStringByteLen(pvarKey->bstrVal);

		if(len & 1)
			return DISP_E_TYPEMISMATCH;

		for(UINT i = 0; i < len / 2; i ++)
			if(pvarKey->bstrVal[i] == 0)
				return DISP_E_TYPEMISMATCH;
	}

	if(pvar->vt == VT_ERROR)
		return DISP_E_PARAMNOTOPTIONAL;

	m_cs.Enter();
	if(m_bArrayMode && (pvarKey->vt == VT_I4 || pvarKey->vt == VT_I2))
	{
		long i = varGetNumbar(*pvarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			VariantCopyInd(&m_posArray[i]->m_value, pvar);
			m_cs.Leave();
			return S_OK;
		}
	}

	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)pvarKey);

	if(pPair == NULL)
	{
		static VARIANT varTemp;

		pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(*(CBVariant*)pvarKey, varTemp);
		m_posArray.Add(pPair);
	}

	VariantCopyInd(&pPair->m_value, pvar);

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::put_Item(VARIANT VarKey, VARIANT pvar)
{
	return putItem(&VarKey, &pvar);
}

STDMETHODIMP CBDictionary::putref_Item(VARIANT VarKey, VARIANT pvar)
{
	return putItem(&VarKey, &pvar);
}

STDMETHODIMP CBDictionary::get_Key(VARIANT VarKey, VARIANT *pvar)
{
	if(VarKey.vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(VarKey.vt & VT_ARRAY)return E_INVALIDARG;

	m_cs.Enter();
	if(m_bArrayMode && (VarKey.vt == VT_I4 || VarKey.vt == VT_I2))
	{
		long i = varGetNumbar(VarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			VariantCopy(pvar, (VARIANT*)&m_posArray[i]->m_key);
			m_cs.Leave();
			return S_OK;
		}else
		{
			m_cs.Leave();
			return DISP_E_BADINDEX;
		}
	}

	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&VarKey);

	if(pPair != NULL)
		VariantCopy(pvar, (VARIANT*)&pPair->m_key);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = (long)m_posArray.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBDictionary::Remove(VARIANT VarKey)
{
	if(VarKey.vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(VarKey.vt & VT_ARRAY)return E_INVALIDARG;

	m_cs.Enter();
	if(m_bArrayMode && (VarKey.vt == VT_I4 || VarKey.vt == VT_I2))
	{
		long i = varGetNumbar(VarKey);

		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			m_mapItems.RemoveAt(m_posArray[i]);
			m_posArray.RemoveAt(i);

			m_cs.Leave();
			return S_OK;
		}else
		{
			m_cs.Leave();
			return DISP_E_BADINDEX;
		}
	}

	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&VarKey);

	if(pPair != NULL)
	{
		for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
			if(m_posArray[i] == pPair)
			{
				m_posArray.RemoveAt(i);
				break;
			}
		m_mapItems.RemoveAt(pPair);
	}
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::RemoveAll(void)
{
	m_cs.Enter();
	m_posArray.RemoveAll();
	m_mapItems.RemoveAll();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_ArrayMode(VARIANT_BOOL *pval)
{
	m_cs.Enter();
	*pval = m_bArrayMode ? VARIANT_TRUE : VARIANT_FALSE;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::put_ArrayMode(VARIANT_BOOL val)
{
	m_cs.Enter();
	m_bArrayMode = (val != VARIANT_FALSE);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Items(VARIANT *pItemsArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;

	m_cs.Enter();

	hr = bstrArray.Create((ULONG)m_posArray.GetCount());
	if(FAILED(hr))
	{
		m_cs.Leave();
		return hr;
	}

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		hr = VariantCopy(&pVar[i], &m_posArray[i]->m_value);
		if(FAILED(hr))
		{
			m_cs.Leave();
			bstrArray.Destroy();
			return hr;
		}
	}

	m_cs.Leave();

	pItemsArray->vt = VT_ARRAY | VT_VARIANT;
	pItemsArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBDictionary::get_Keys(VARIANT *pKeysArray)
{
	CComSafeArray<VARIANT> bstrArray;
	VARIANT* pVar;
	HRESULT hr;

	m_cs.Enter();

	hr = bstrArray.Create((ULONG)m_posArray.GetCount());
	if(FAILED(hr))
	{
		m_cs.Leave();
		return hr;
	}

	pVar = (VARIANT*)bstrArray.m_psa->pvData;

	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		hr = VariantCopy(&pVar[i], (VARIANT*)&m_posArray[i]->m_key);
		if(FAILED(hr))
		{
			m_cs.Leave();
			bstrArray.Destroy();
			return hr;
		}
	}

	m_cs.Leave();

	pKeysArray->vt = VT_ARRAY | VT_VARIANT;
	pKeysArray->parray = bstrArray.Detach();

	return S_OK;
}

STDMETHODIMP CBDictionary::Add(VARIANT VarKey, VARIANT var)
{
	return putItem(&VarKey, &var);
}

STDMETHODIMP CBDictionary::Exists(VARIANT VarKey, VARIANT_BOOL *pExists)
{
	if(VarKey.vt == VT_ERROR)return DISP_E_PARAMNOTOPTIONAL;
	if(VarKey.vt & VT_ARRAY)return E_INVALIDARG;

	m_cs.Enter();
	CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&VarKey);

	*pExists = (pPair != NULL) ? VARIANT_TRUE : VARIANT_FALSE;

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::Sort(VARIANT_BOOL bMode)
{
	POSITION pos;
	UINT apos;

	m_cs.Enter();

	pos = m_mapItems.GetHeadPosition();

	if(!bMode)
	{
		apos = m_posArray.GetCount() - 1;
		while(pos)
			m_posArray[apos --] = m_mapItems.GetNext(pos);
	}else
	{
		apos = 0;
		while(pos)
			m_posArray[apos ++] = m_mapItems.GetNext(pos);
	}

	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDictionary::Load(VARIANT VarSrc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBDictionary* pThis = this;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return LoadObjectFromStream(pStream, CLSID_NULL, (void**)&pThis);
}

STDMETHODIMP CBDictionary::Save(VARIANT VarDesc, short mode)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	SetPersistMode(mode);
	return WriteObjectToStream((IVariantDictionary*)this, pStream);
}

HRESULT CBDictionary::toJsonValue(Json::Value &root, CAtlArray<void*> &arrObjects)
{
	HRESULT hr;
	CBStringA strKey;
	CBStringA strValue;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;

	CBLock l(&m_cs);
	
	arrObjects.Add((void *)this);

	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		pPair = m_posArray[i];

		if(pPair->m_key.vt == VT_BSTR)
			strKey = pPair->m_key.bstrVal;
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, (VARIANTARG*)&pPair->m_key, VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))
				return hr;
			strKey = var.bstrVal;
		}

		if(pPair->m_value.vt == VT_BSTR)
		{
			strValue = pPair->m_value.bstrVal;
			root[(const char *)strKey] = (const char *)strValue;
		}
		else if (pPair->m_value.vt == VT_DISPATCH)
		{
			CComQIPtr<IVariantDictionary, &IID_IVariantDictionary> pDic;
			pDic = pPair->m_value.pdispVal;
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
					return hr;
				pDic.Release();
				root[(const char *)strKey] = value;
			}
			else
			{
				CComQIPtr<IVariantList, &IID_IVariantList> pList;

				pList = pPair->m_value.pdispVal;
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
						return hr;
					pList.Release();
					root[(const char *)strKey] = value;
				}
				else
				{
					//CComQIPtr<_Recordset, &__uuidof(_Recordset)> prs;
					_RecordsetPtr prs = pPair->m_value.pdispVal;
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
						root[(const char *)strKey] = table;
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

			switch(pPair->m_value.vt)
			{
				case VT_NULL:
				case VT_EMPTY:
					root[(const char *)strKey] = Json::nullValue;
					break;
				case VT_BOOL:
					root[(const char *)strKey] = pPair->m_value.boolVal;
					break;
				case VT_INT:
					root[(const char *)strKey] = pPair->m_value.intVal;
					break;
				case VT_I1:
					root[(const char *)strKey] = pPair->m_value.cVal;
					break;
				case VT_I2:
					root[(const char *)strKey] = pPair->m_value.iVal;
					break;
				case VT_I4:
					root[(const char *)strKey] = pPair->m_value.lVal;
					break;
				case VT_I8:
					root[(const char *)strKey] = pPair->m_value.llVal;
					break;
				case VT_UINT:
					root[(const char *)strKey] = pPair->m_value.uintVal;
					break;
				case VT_UI1:
					root[(const char *)strKey] = pPair->m_value.bVal;
					break;
				case VT_UI2:
					root[(const char *)strKey] = pPair->m_value.uiVal;
					break;
				case VT_UI4:
					root[(const char *)strKey] = (unsigned int)pPair->m_value.ulVal;
					break;
				case VT_UI8:
					root[(const char *)strKey] = pPair->m_value.ullVal;
					break;
				case VT_R4:
					root[(const char *)strKey] = (double)pPair->m_value.fltVal;
					break;
				case VT_R8:
					root[(const char *)strKey] = pPair->m_value.dblVal;
					break;
				case VT_CY:
					hr = VariantChangeType(&var, &pPair->m_value, VARIANT_ALPHABOOL, VT_R8);
					root[(const char *)strKey] = var.dblVal;
					break;
				case VT_DECIMAL:
					hr = VariantChangeType(&var, &pPair->m_value, VARIANT_ALPHABOOL, VT_I8);
					root[(const char *)strKey] = var.llVal;
					break;
				case VT_DATE:
					hr = VariantChangeType(&var, &pPair->m_value, VARIANT_ALPHABOOL, VT_BSTR);
					if(FAILED(hr))
						return hr;
					strValue = var.bstrVal;
					root[(const char *)strKey] = (const char *)strValue;
					break;
				case VT_BSTR:
					strValue = pPair->m_value.bstrVal;
					root[(const char *)strKey] = (const char *)strValue;
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

STDMETHODIMP CBDictionary::toJson(int intStyle, BSTR* pvar)
{
	HRESULT hr;
	CBStringA strValue;
	Json::Value root;
	CAtlArray<void*> arrObjects;

	hr = toJsonValue(root, arrObjects);
	if (FAILED(hr)) return hr;

	if (intStyle)
	{
		Json::StyledWriter writer;
		std::string outputConfig = writer.write( root );
		strValue = outputConfig.c_str();
	}
	else
	{
		Json::FastWriter writer;
		std::string outputConfig = writer.write( root );
		strValue = outputConfig.c_str();
	}

	*pvar = strValue.AllocSysString();

	return S_OK;
}

HRESULT CBDictionary::fromJsonValue(Json::Value &root)
{
	if (!root.isObject())
		return E_INVALIDARG;
	
	CComVariant varEmpty;
	CComVariant varKey;
	CBComPtr<CBListEx> pList;
	CBComPtr<CBDictionary> pDic;
	CComDispatchDriver pDisp;
	HRESULT hr;

	Json::Value::Members members(root.getMemberNames());

	CBLock l(&m_cs);
	m_bArrayMode = VARIANT_FALSE;
	for (Json::Value::Members::iterator it = members.begin();it != members.end();++it)
	{
		const std::string &i = *it;
		varKey = i.c_str();

		CRBMap<CBVariant, CComVariant>::CPair* pPair = m_mapItems.Lookup(*(CBVariant*)&varKey);

		if(pPair == NULL)
		{
			pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(*(CBVariant*)&varKey, varEmpty);
			m_posArray.Add(pPair);
		}

		switch(root[i].type())
		{
			case Json::nullValue:
				pPair->m_value.ChangeType(VT_NULL);
				break;
			case Json::intValue:
				if (root[i].asLargestInt()>>32)
					pPair->m_value = root[i].asDouble();
				else
					pPair->m_value = root[i].asInt();
				break;
			case Json::uintValue:
				if (root[i].asLargestInt()>>31)
					pPair->m_value = root[i].asDouble();
				else
					pPair->m_value = (int)root[i].asUInt();
				break;
			case Json::realValue:
				pPair->m_value = root[i].asDouble();
				break;
			case Json::stringValue:
				pPair->m_value = root[i].asCString();
				break;
			case Json::booleanValue:
				pPair->m_value = (bool)root[i].asBool();
				break;
			case Json::arrayValue:
				pList.CreateInstance();
				if (pList==NULL)
					return E_UNEXPECTED;
				pList->fromJsonValue(root[i]);
				hr = pList.QueryInterface(IID_IDispatch, (void **)&pDisp);
				if (FAILED(hr))
					return hr;
				pPair->m_value = pDisp;
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
				pPair->m_value = pDisp;
				pDic.Release();
				pDisp.Release();
				break;
			default:
				return E_UNEXPECTED;
		}
	}
	return S_OK;
}

STDMETHODIMP CBDictionary::fromJson(BSTR bstrJson)
{
	Json::Value root;
	Json::Reader reader;
	CStringA  strJson;
	
	strJson = bstrJson;
	if (!reader.parse(strJson.GetString(), strJson.GetString()+strJson.GetLength(), root))
		return E_INVALIDARG;

	HRESULT hr = fromJsonValue(root);
	if (FAILED(hr))
		return hr;
	
	return S_OK;
}

STDMETHODIMP CBDictionary::Join(BSTR bstrKeyDelimiter, BSTR bstrDelimiter, BSTR* pvar)
{
	HRESULT hr;
	CBString strValue;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;

	CBLock l(&m_cs);
	for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
	{
		if(i > 0)strValue.Append(bstrDelimiter);

		pPair = m_posArray[i];

		if(pPair->m_key.vt == VT_BSTR)
			strValue.Append(pPair->m_key.bstrVal);
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, (VARIANTARG*)&pPair->m_key, VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))
				return hr;
			strValue.Append(var.bstrVal);
		}

		strValue.Append(bstrKeyDelimiter);

		if(pPair->m_value.vt == VT_BSTR)
			strValue.Append(pPair->m_value.bstrVal);
		else
		{
			CComVariant var;

			hr = VariantChangeType(&var, &pPair->m_value, VARIANT_ALPHABOOL, VT_BSTR);
			if(FAILED(hr))
				return hr;
			strValue.Append(var.bstrVal);
		}
	}

	*pvar = strValue.AllocSysString();

	return S_OK;
}

inline BSTR findstr(BSTR bstr, int nCount, LPCWSTR strKey)
{
	BSTR bstrTemp;
	UINT nCountTemp;
	LPCWSTR strKeyTemp;

	while(nCount)
	{
		bstrTemp = bstr;
		nCountTemp = nCount;
		strKeyTemp = strKey;

		while(nCountTemp && *strKeyTemp && *bstrTemp == *strKeyTemp)
		{
			nCountTemp --;
			strKeyTemp ++;
			bstrTemp ++;
		}

		if(!*strKeyTemp)return bstr;

		bstr ++;
		nCount --;
	}

	return bstr;
}

STDMETHODIMP CBDictionary::Split(BSTR bstrExpression, BSTR bstrKeyDelimiter, BSTR bstrDelimiter)
{
	int nCount;
	int strKeyDelimiterLength = ::SysStringLen(bstrKeyDelimiter);
	int strDelimiterLength = ::SysStringLen(bstrDelimiter);
	BSTR pstr, pstr1;
	CBVariant varKey;
	VARIANT varValue;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;
	static VARIANT varEmpty;

	varKey.vt = VT_BSTR;
	VariantInit(&varValue);
	varValue.vt = VT_BSTR;

	nCount = ::SysStringLen(bstrExpression);

	m_cs.Enter();

	while(nCount > 0)
	{
		pstr = findstr(bstrExpression, nCount, bstrDelimiter);
		pstr1 = findstr(bstrExpression, pstr - bstrExpression, bstrKeyDelimiter);

		varKey.bstrVal = ::SysAllocStringLen(bstrExpression, pstr1 - bstrExpression);
		if(pstr1 < pstr)pstr1 += strKeyDelimiterLength;
		varValue.bstrVal = ::SysAllocStringLen(pstr1, pstr - pstr1);

		pPair = m_mapItems.Lookup(varKey);

		if(pPair == NULL)
		{
			pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(varKey, varEmpty);
			m_posArray.Add(pPair);
		}else VariantClear(&pPair->m_value);

		SysFreeString(varKey.bstrVal);

		CopyMemory(&pPair->m_value, &varValue, sizeof(VARIANT));

		nCount -= pstr + strDelimiterLength - bstrExpression;
		bstrExpression = pstr + strDelimiterLength;
	}

	m_cs.Leave();

	varKey.vt = VT_EMPTY;

	return S_OK;
}

STDMETHODIMP CBDictionary::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBDictionary);
	return S_OK;
}

STDMETHODIMP CBDictionary::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBDictionary::Load(IStream *pStm)
{
	HRESULT hr;
	CBVariant var;
	CRBMap<CBVariant, CComVariant>::CPair* pPair;
	CComVariant varEmpty;

	CBLock l(&m_cs);

	m_bArrayMode = FALSE;
	hr = ReadStream(pStm, &m_bArrayMode, 1);
	if(FAILED(hr))return hr;

	while(TRUE)
	{
		hr = var.LoadFromStream(pStm);
		if(FAILED(hr))return hr;

		if(hr == 0xffff)
			break;

		if(var.vt == VT_ERROR || (var.vt & VT_ARRAY))
			return E_INVALIDARG;

		pPair = m_mapItems.Lookup(var);

		if(pPair == NULL)
		{
			pPair = (CRBMap<CBVariant, CComVariant>::CPair*)m_mapItems.SetAt(var, varEmpty);
			m_posArray.Add(pPair);
		}

		hr = ((CBVariant*)&pPair->m_value)->LoadFromStream(pStm);

		if(FAILED(hr))return hr;
	}

	return S_OK;
}

STDMETHODIMP CBDictionary::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	size_t i;

	CBLock l(&m_cs);

	hr = pStm->Write(&m_bArrayMode, 1, NULL);
	if(FAILED(hr))return hr;

	for(i = 0; i < m_posArray.GetCount(); i ++)
	{
		hr = m_posArray[i]->m_key.WriteToStream(pStm);
		if(FAILED(hr))return hr;

		hr = ((CBVariant*)&m_posArray[i]->m_value)->WriteToStream(pStm);
		if(FAILED(hr))return hr;
	}

	i = 0xffff;
	return pStm->Write(&i, sizeof(VARTYPE), NULL);
}

STDMETHODIMP CBDictionary::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBDictionary::InitNew(void)
{
	return RemoveAll();
}

