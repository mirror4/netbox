#pragma once

HRESULT WriteObjectToStream(IUnknown* pUnk, IStream* pStream, BOOL bWriteClassID = FALSE);
HRESULT LoadObjectFromStream(IStream* pStream, REFIID riid, void ** ppvObj);
HRESULT WriteObjectToVariant(IDispatch* pObject, VARIANT* pvar);
void SetPersistMode(short mode);

HRESULT inline ReadStream(IStream* pStream, void *pv, ULONG cb)
{
	char *pBuffer = (char*)pv;
	HRESULT hr;
	ULONG cbRead;

	while(cb)
	{
		hr = pStream->Read(pBuffer, cb, &cbRead);
		if(FAILED(hr))return hr;
		if(cbRead == 0)
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		cb -= cbRead;
		pBuffer += cbRead;
	}

	return S_OK;
}

class CBVariant : public tagVARIANT
{
public:
	CBVariant() throw()
	{
		::VariantInit(this);
	}

	CBVariant(const CBVariant& src) throw()
	{
		vt = VT_EMPTY;
		HRESULT hr = ::VariantCopy(this, (VARIANT*)&src);
		if (FAILED(hr))vt = VT_ERROR;
	}

	~CBVariant() throw()
	{
		::VariantClear(this);
	}

private:
	int GetClass(void) const throw()
	{
		switch(vt)
		{
		case VT_UNKNOWN:
		case VT_DISPATCH:
			return VT_UNKNOWN;
		case VT_BSTR:
			return VT_BSTR;
		case VT_DATE:
			return VT_DATE;
		case VT_BOOL:
			return VT_BOOL;
		case VT_R4:
		case VT_R8:
			return VT_R8;
		case VT_I1:
		case VT_UI1:
		case VT_I2:
		case VT_UI2:
		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_I8:
		case VT_UI8:
			return VT_UI8;
		default:
			return 0;
		}
	}

	template <typename T>
	static int _check_value(const T v)
	{
		if(v < 0)return -1;
		if(v > 0)return 1;
		return 0;
	}

	double _get_float(void) const throw()
	{
		if(vt == VT_R4)
			return fltVal;
		if(vt == VT_R8)
			return dblVal;

		return 0;
	}

	__int64 _get_int(void) const throw()
	{
		switch(vt)
		{
		case VT_I1:
		case VT_UI1:
			return bVal;
		case VT_I2:
		case VT_UI2:
			return iVal;
		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
			return lVal;
		case VT_I8:
		case VT_UI8:
			return llVal;
		}

		return 0;
	}

	static IUnknown* _getobjaddr(IUnknown* p)
	{
		IUnknown* p1 = NULL;

		if(p)
		{
			p->QueryInterface(&p1);
			p1->Release();
		}
		return p1;
	}

public:
	int Compare(const CBVariant* varSrc) const throw()
	{
		int vc, vc1;

		vc = GetClass();
		vc1 = varSrc->GetClass();

		if(vc != vc1 || vc == 0)
			return vc - vc1;

		switch(vc)
		{
		case VT_UNKNOWN:return _check_value(_getobjaddr(punkVal) - _getobjaddr(varSrc->punkVal));
		case VT_BSTR:	return wcsicmp(bstrVal, varSrc->bstrVal);
		case VT_DATE:	return _check_value(date - varSrc->date);
		case VT_BOOL:	return _check_value(boolVal - varSrc->boolVal);
		case VT_R8:		return _check_value(_get_float() - varSrc->_get_float());
		case VT_UI8:	return _check_value(_get_int() - varSrc->_get_int());
		}

		return 0;
	}

	int Compare(const VARIANT* varSrc) const throw()
	{
		return Compare((const CBVariant*)varSrc);
	}

	bool operator==(const VARIANT& varSrc) const throw()
	{
		return Compare(&varSrc) == 0;
	}

	bool operator<(const VARIANT& varSrc) const throw()
	{
		return Compare(&varSrc) < 0;
	}

	bool operator>(const VARIANT& varSrc) const throw()
	{
		return Compare(&varSrc) > 0;
	}

	static int GetSize(VARTYPE vtType)
	{
		int cbWrite = 0;
		switch (vtType)
		{
		case VT_UI1:
		case VT_I1:
			cbWrite = sizeof(BYTE);
			break;
		case VT_I2:
		case VT_UI2:
		case VT_ERROR:
		case VT_BOOL:
			cbWrite = sizeof(short);
			break;
		case VT_I4:
		case VT_UI4:
		case VT_R4:
		case VT_INT:
		case VT_UINT:
		case VT_HRESULT:
			cbWrite = sizeof(long);
			break;
		case VT_I8:
		case VT_UI8:
		case VT_R8:
		case VT_CY:
		case VT_DATE:
			cbWrite = sizeof(LONGLONG);
			break;
		case VT_DECIMAL:
			cbWrite = sizeof(DECIMAL) - 2;
			break;
		default:
			break;
		}

		return cbWrite;
	}

	HRESULT LoadFromStream(IStream* pStream)
	{
		HRESULT hr;
		VARTYPE vtRead = VT_EMPTY;
		ULONG cbSize;

		hr = VariantClear(this);
		if (FAILED(hr))return hr;

		hr = ReadStream(pStream, &vtRead, sizeof(VARTYPE));
		if (FAILED(hr))return hr;

		if(vtRead == VT_EMPTY || vtRead == VT_NULL || vtRead == VT_VOID)
		{
			vt = vtRead;
			return S_OK;
		}

		if(vtRead == 0xffff)return 0xffff;

		punkVal = NULL;

		if(vtRead == VT_UNKNOWN)
		{
			hr = LoadObjectFromStream(pStream, IID_IUnknown, (void**)&punkVal);
			if(FAILED(hr))return hr;
			vt = vtRead;
			return hr;
		}

		if(vtRead == VT_DISPATCH)
		{
			hr = LoadObjectFromStream(pStream, IID_IDispatch, (void**)&punkVal);
			if(FAILED(hr))return hr;
			vt = vtRead;
			return hr;
		}

		if(vtRead == VT_BSTR)
		{
			bstrVal = NULL;
			hr = ReadStream(pStream, (void*) &cbSize, sizeof(cbSize));
			if(FAILED(hr))return hr;

			bstrVal = SysAllocStringByteLen(NULL, cbSize-sizeof(OLECHAR));
			if (bstrVal == NULL)return E_OUTOFMEMORY;

			hr = ReadStream(pStream, (void*) bstrVal, cbSize);
			if(FAILED(hr))
			{
				SysFreeString(bstrVal);
				return hr;
			}
			vt = vtRead;
			return hr;
		}

		if(vtRead & VT_ARRAY)
		{
			LPSAFEARRAY pArray;
			SAFEARRAYBOUND *pArrayBound, abTemp;
			VARTYPE vtElem = vtRead & VT_TYPEMASK;
			USHORT cDims;
			ULONG i, nCount = 1;

			hr = ReadStream(pStream, &cDims, sizeof(USHORT));
			if (FAILED(hr))return hr;

			pArrayBound = new SAFEARRAYBOUND[cDims];
			if(pArrayBound == NULL)return E_OUTOFMEMORY;

			hr = ReadStream(pStream, pArrayBound, sizeof(SAFEARRAYBOUND) * cDims);
			if (FAILED(hr))
			{
				delete pArrayBound;
				return hr;
			}

			for(i = 0; i < (ULONG)cDims / 2; i ++)
			{
				abTemp = pArrayBound[i];
				pArrayBound[i] = pArrayBound[cDims - i - 1];
				pArrayBound[cDims - i - 1] = abTemp;
			}

			pArray = SafeArrayCreate(vtElem, cDims, pArrayBound);
			delete pArrayBound;
			if(pArray == NULL)return E_OUTOFMEMORY;

			for(i = 0; i < pArray->cDims; i ++)
				nCount *= pArray->rgsabound[i].cElements;

			if(vtElem == VT_VARIANT)
			{
				CBVariant* pData = (CBVariant*)pArray->pvData;
				for(i = 0; i < nCount; i ++)
				{
					hr = pData[i].LoadFromStream(pStream);
					if (FAILED(hr))
					{
						SafeArrayDestroy(pArray);
						return hr;
					}
				}
			}else
			{
				cbSize = GetSize(vtElem);
				if(cbSize == 0)
				{
					SafeArrayDestroy(pArray);
					return DISP_E_BADVARTYPE;
				}

				hr = ReadStream(pStream, pArray->pvData, nCount * cbSize);
				if (FAILED(hr))
				{
					SafeArrayDestroy(pArray);
					return hr;
				}
			}

			vt = vtElem | VT_ARRAY;
			parray = pArray;

			return S_OK;
		}

		cbSize = GetSize(vtRead);

		if (cbSize == 0)return DISP_E_BADVARTYPE;

		if(cbSize == 14)
			hr = ReadStream(pStream, (void*) &decVal.signscale, cbSize);
		else
			hr = ReadStream(pStream, (void*) &bVal, cbSize);
		if(FAILED(hr))return hr;
		vt = vtRead;
		return hr;
	}

	HRESULT WriteToStream(IStream* pStream) const
	{
		const VARIANT * pVar = this;

		HRESULT hr;

		if(pVar->vt & VT_ARRAY)
		{
			LPSAFEARRAY pArray = pVar->vt & VT_BYREF ? *pVar->pparray : pVar->parray;
			int cbWrite = 0;
			ULONG i, nCount;

			SafeArrayLock(pArray);

			if(pArray->cDims < 1)return DISP_E_BADVARTYPE;

			VARTYPE vtElem, vtVar;

			vtElem = pVar->vt & VT_TYPEMASK;

			vtVar = vtElem | VT_ARRAY;
			hr = pStream->Write(&vtVar, sizeof(VARTYPE), NULL);
			if (FAILED(hr))
			{
				SafeArrayUnlock(pArray);
				return hr;
			}

			hr = pStream->Write(&pArray->cDims, sizeof(pArray->cDims), NULL);
			if (FAILED(hr))
			{
				SafeArrayUnlock(pArray);
				return hr;
			}

			hr = pStream->Write(&pArray->rgsabound, sizeof(SAFEARRAYBOUND) * pArray->cDims, NULL);
			if (FAILED(hr))
			{
				SafeArrayUnlock(pArray);
				return hr;
			}

			nCount = 1;
			for(i = 0; i < pArray->cDims; i ++)
				nCount *= pArray->rgsabound[i].cElements;

			if(vtElem == VT_VARIANT)
			{
				CBVariant* pData = (CBVariant*)pArray->pvData;
				for(i = 0; i < nCount; i ++)
				{
					hr = pData[i].WriteToStream(pStream);
					if (FAILED(hr))
					{
						SafeArrayUnlock(pArray);
						return hr;
					}
				}
			}else
			{
				cbWrite = GetSize(vtElem);
				if(cbWrite == 0)
				{
					SafeArrayUnlock(pArray);
					return DISP_E_BADVARTYPE;
				}

				hr = pStream->Write(pArray->pvData, nCount * cbWrite, NULL);
			}

			SafeArrayUnlock(pArray);

			return hr;
		}

		hr = pStream->Write(&pVar->vt, sizeof(VARTYPE), NULL);
		if (FAILED(hr))return hr;

		if(pVar->vt == VT_UNKNOWN || pVar->vt == VT_DISPATCH)
			return WriteObjectToStream(pVar->punkVal, pStream, TRUE);

		if(pVar->vt == VT_EMPTY || pVar->vt == VT_NULL || pVar->vt == VT_VOID)
			return S_OK;

		ULONG cbWrite;

		if (pVar->vt == VT_BSTR)
		{
			cbWrite = pVar->bstrVal ? SysStringByteLen(pVar->bstrVal)+sizeof(OLECHAR) : 0;

			hr = pStream->Write((void*) &cbWrite, sizeof(cbWrite), NULL);
			if(FAILED(hr) || !cbWrite)return hr;

			return pStream->Write((void*) pVar->bstrVal, cbWrite, NULL);
		}

		cbWrite = GetSize(vt);

		if (cbWrite != 0)
		{
			if(cbWrite == 14)
				hr = pStream->Write((void*) &decVal.signscale, cbWrite, NULL);
			else
				hr = pStream->Write((void*) &pVar->bVal, cbWrite, NULL);

			return hr;
		}

		return DISP_E_BADVARTYPE;
	}
};
