#include "StdAfx.h"
#include "BRecordset.h"
#include "BStream.h"
#include "BVarType.h"
#include "BDate.h"
#include "BCodepage.h"

CBRecordset::~CBRecordset(void)
{
	for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
		VariantClear(&m_arrayVariant[i]);
}

STDMETHODIMP CBRecordset::get_Value(VARIANT VarKey, VARIANT *pvar)
{
	if(VarKey.vt & VT_ARRAY)
		return E_INVALIDARG;

	if(VarKey.vt == VT_ERROR)
		return WriteObjectToVariant((IRecordset*)this, pvar);

	int i;

	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	if(VarKey.vt == VT_I4 || VarKey.vt == VT_I2)
	{
		i = varGetNumber(VarKey);
		
		if(i < 0 || i >= GetFieldCount())
			return DISP_E_BADINDEX;
	}else
	{
		CBString strKey;
		HRESULT hr;

		hr = varGetString(VarKey, strKey);
		if(FAILED(hr))return hr;

		for(i = 0; i < GetFieldCount() && strKey.CompareNoCase(m_arrayFields[i]); i ++);

		if(i == GetFieldCount())return E_INVALIDARG;
	}

	i += m_nPos;

	if(i >=0 && i < (int)m_arrayVariant.GetCount())
	{
		VariantCopy(pvar, &m_arrayVariant[i]);
		return S_OK;
	}else return DISP_E_BADINDEX;

	return S_OK;
}

STDMETHODIMP CBRecordset::put_Value(VARIANT VarKey, VARIANT pvar)
{
	if(VarKey.vt & VT_ARRAY)
		return E_INVALIDARG;

	if(VarKey.vt == VT_UNKNOWN || VarKey.vt == VT_DISPATCH)
		return DISP_E_BADVARTYPE;

	int i;
	HRESULT hr;

	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase == NULL)return E_NOTIMPL;

	if(m_arrayVariant.IsEmpty())
		AddNew();

	if(VarKey.vt == VT_I4 || VarKey.vt == VT_I2)
	{
		i = varGetNumber(VarKey);
		
		if(i < 0 || i >= GetFieldCount())
			return DISP_E_BADINDEX;
	}else
	{
		CBString strKey;

		hr = varGetString(VarKey, strKey);
		if(FAILED(hr))return hr;

		for(i = 0; i < GetFieldCount() && strKey.CompareNoCase(m_arrayFields[i]); i ++);

		if(i == GetFieldCount())return E_INVALIDARG;
	}

	i += m_nPos;

	if(i >=0 && i < (int)m_arrayVariant.GetCount())
	{
		VariantCopy(&m_arrayVariant[i], &pvar);
		return S_OK;
	}else return DISP_E_BADINDEX;

	return S_OK;
}

STDMETHODIMP CBRecordset::Move(long pos)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	pos = (pos - m_nFirstPosition) * GetFieldCount();

	if(pos < -GetFieldCount())pos = -GetFieldCount();
	else if(pos > (int)m_arrayVariant.GetCount())
		pos = (int)m_arrayVariant.GetCount();

	m_nPos = pos;

	return S_OK;
}

STDMETHODIMP CBRecordset::MoveFirst(void)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	m_nPos = 0;

	return S_OK;
}

STDMETHODIMP CBRecordset::MoveLast(void)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	m_nPos = m_arrayVariant.GetCount();
	if(m_nPos > 0)
		m_nPos -= GetFieldCount();

	return S_OK;
}

STDMETHODIMP CBRecordset::MoveNext(void)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	if(m_nPos < (int)m_arrayVariant.GetCount())
		m_nPos += GetFieldCount();

	return S_OK;
}

STDMETHODIMP CBRecordset::MovePrevious(void)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	if(m_nPos >= 0)
		m_nPos -= GetFieldCount();

	return S_OK;
}

STDMETHODIMP CBRecordset::Load(VARIANT VarSrc)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;
	CBRecordset* pThis = this;

	hr = CBStream::GetStream(&VarSrc, &pStream);
	if(FAILED(hr))return hr;

	return LoadObjectFromStream(pStream, CLSID_NULL, (void**)&pThis);
}

STDMETHODIMP CBRecordset::Save(VARIANT VarDesc)
{
	if(m_pFields == NULL)return E_NOTIMPL;

	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	return WriteObjectToStream((IRecordset*)this, pStream);
}

STDMETHODIMP CBRecordset::AddNew(void)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase == NULL)return E_NOTIMPL;

	if(m_arrayVariant.IsEmpty())
	{
		m_arrayVariant.SetCount(GetFieldCount());
		ZeroMemory(&m_arrayVariant[0], GetFieldCount() * sizeof(VARIANT));
	}else for(int i = 0; i < GetFieldCount(); i ++)
		VariantClear(&m_arrayVariant[i]);

	return S_OK;
}

inline WCHAR* iToStr(int n, WCHAR* pstr, int len)
{
	int len1 = len;

	while(len)
	{
		len --;
		pstr[len] = n % 10 + '0';
		n /= 10;
	}

	return pstr + len1;
}

STDMETHODIMP CBRecordset::Update(void)
{
	CBString strSQL(L"INSERT INTO [");
	CBString strValue;
	BOOL bFirst = TRUE;
	HRESULT hr;
	VARIANT varTemp = {VT_EMPTY};
	int i;

	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase == NULL)return E_NOTIMPL;

	strSQL.Append(m_strTable);
	strSQL.Append(L"](", 2);

	for(i = 0; i < GetFieldCount(); i ++)
	{
		VARIANT* pvar = &m_arrayVariant[i];

		if(pvar->vt != VT_NULL && pvar->vt != VT_EMPTY)
		{
			if(bFirst)
			{
				strSQL.AppendChar(L'[');
				bFirst = FALSE;
			}else
			{
				strSQL.Append(L",[", 2);
				strValue.AppendChar(L',');
			}

			strSQL.Append(m_arrayFields[i]);
			strSQL.AppendChar(L']');

			if(pvar->vt == VT_DATE)
			{
				WCHAR wstr[22]; // "9999-99-99 99:99:99"
				WCHAR *pstr = wstr;
				CBDate d(pvar->date);
				SYSTEMTIME st;

				st = d;

				*pstr ++ = '\'';
				if(st.wYear != 1899 || st.wMonth != 12 || st.wDay != 30)
				{
					pstr = iToStr(st.wYear, pstr, 4);
					*pstr ++ = '-';

					pstr = iToStr(st.wMonth, pstr, 2);
					*pstr ++ = '-';

					pstr = iToStr(st.wDay, pstr, 2);
				}

				if(st.wHour || st.wMinute || st.wSecond)
				{
					if(pstr != wstr)
						*pstr ++ = ' ';

					pstr = iToStr(st.wHour, pstr, 2);
					*pstr ++ = ':';

					pstr = iToStr(st.wMinute, pstr, 2);
					*pstr ++ = ':';

					pstr = iToStr(st.wSecond, pstr, 2);
				}

				*pstr ++ = '\'';

				strValue.Append(wstr, pstr - wstr);
			}else if(pvar->vt == VT_BSTR)
			{
				BSTR TextString = pvar->bstrVal;

				CBAutoPtr<WCHAR> pszEncoded;
				UINT i, len = SysStringLen(TextString);
				UINT nPos = 0;
				WCHAR ch;

				pszEncoded.Allocate(len * 2 + 2);

				pszEncoded[nPos ++] = '\'';
				for(i = 0; i < len; i ++)
				{
					ch = TextString[i];

					if(ch == '\'')
						pszEncoded[nPos ++] = '\'';
					pszEncoded[nPos ++] = ch;
				}
				pszEncoded[nPos ++] = '\'';

				strValue.Append(pszEncoded, nPos);
			}else if(pvar->vt & VT_ARRAY)
			{
				CBVarPtr varPtr;
				CBAutoPtr<WCHAR> pszEncoded;
				static char HexChar[] = "0123456789ABCDEF";
				UINT i;

				hr = varPtr.Attach(pvar);
				if(FAILED(hr))return hr;

				pszEncoded.Allocate(varPtr.m_nSize * 2 + 3);

				pszEncoded[0] = L'x';
				pszEncoded[1] = L'\'';

				for(i = 0; i < varPtr.m_nSize; i ++)
				{
					pszEncoded[i * 2 + 2] = HexChar[varPtr.m_pData[i] >> 4];
					pszEncoded[i * 2 + 3] = HexChar[varPtr.m_pData[i] & 0xf];
				}

				pszEncoded[i * 2 + 2] = L'\'';

				strValue.Append(pszEncoded, i * 2 + 3);
			}else
			{
				hr = VariantChangeType(&varTemp, pvar, 0, VT_BSTR);
				if(FAILED(hr))return hr;

				if(varTemp.bstrVal && varTemp.bstrVal[0])
					strValue.Append(varTemp.bstrVal, SysStringLen(varTemp.bstrVal));
				else strValue.Append(L"\'\'", 2);

				VariantClear(&varTemp);
			}
		}
	}

	strSQL.Append(L") VALUES(", 9);
	strSQL.Append(strValue);
	strSQL.AppendChar(L')');

	CBCodePage cp(CP_UTF8);
	CBStringA strSql(strSQL);

	return m_pDatabase->Execute(strSql);
}

STDMETHODIMP CBRecordset::get_BOF(VARIANT_BOOL *pvar)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	*pvar = (m_arrayVariant.GetCount() == 0 || m_nPos <= -GetFieldCount()) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_EOF(VARIANT_BOOL *pvar)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	*pvar = (m_arrayVariant.GetCount() == 0 || m_nPos >= (int)m_arrayVariant.GetCount()) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_AbsolutePosition(long *pvar)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	*pvar = m_nPos / GetFieldCount() + m_nFirstPosition;

	return S_OK;
}

STDMETHODIMP CBRecordset::put_AbsolutePosition(long pvar)
{
	return Move(pvar);
}

STDMETHODIMP CBRecordset::get_ResultCount(long *pvar)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	*pvar = m_nResultCount;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_RecordCount(long *pvar)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	*pvar = m_arrayVariant.GetCount() / GetFieldCount();
	return S_OK;
}

STDMETHODIMP CBRecordset::get_FirstPosition(long *pvar)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	*pvar = m_nFirstPosition;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_Fields(IStringList **ppFields)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	return m_pFields.QueryInterface(ppFields);
}

STDMETHODIMP CBRecordset::get_NextRecordset(IRecordset** ppRS)
{
	CBLock l(&m_cs);

	if(m_pFields == NULL)return E_NOTIMPL;

	return m_pNextRS.QueryInterface(ppRS);
}

STDMETHODIMP CBRecordset::get_ReadTimes(long *pVal)
{
	*pVal = m_nReadTimes;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_ReadBytes(double *pVal)
{
	*pVal = m_nReadBytes;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_WriteTimes(long *pVal)
{
	*pVal = m_nWriteTimes;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_WriteBytes(double *pVal)
{
	*pVal = m_nWriteBytes;
	return S_OK;
}

STDMETHODIMP CBRecordset::get_ExecTime(double *pVal)
{
	*pVal = m_nExecTime / 1000.0;
	return S_OK;
}

STDMETHODIMP CBRecordset::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBRecordset);
	return S_OK;
}

STDMETHODIMP CBRecordset::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBRecordset::Load(IStream *pStm)
{
	HRESULT hr;
	long n, nFieldCount;
	int i;

	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	hr = ReadStream(pStm, &nFieldCount, sizeof(nFieldCount));
	if(FAILED(hr))return hr;

	for(i = 0; i < nFieldCount; i ++)
	{
		CBString str;

		hr = ReadStream(pStm, &n, sizeof(n));
		if(FAILED(hr))return hr;

		hr = ReadStream(pStm, str.GetBuffer(n / 2 - 1), n);
		if(FAILED(hr))return hr;

		str.ReleaseBuffer(n / 2 - 1);

		AppendField(str);
	}

	hr = ReadStream(pStm, &m_nResultCount, sizeof(m_nResultCount));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &m_nFirstPosition, sizeof(m_nFirstPosition));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &n, sizeof(n));
	if(FAILED(hr))return hr;

	for(i = 0; i < n; i ++)
	{
		VARIANT var = {VT_EMPTY};

		hr = ((CBVariant*)&var)->LoadFromStream(pStm);
		if(FAILED(hr))return hr;

		m_arrayVariant.Add(var);
	}

	hr = ReadStream(pStm, &m_nPos, sizeof(m_nPos));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &m_nReadTimes, sizeof(m_nReadTimes));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &m_nReadBytes, sizeof(m_nReadBytes));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &m_nWriteTimes, sizeof(m_nWriteTimes));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &m_nWriteBytes, sizeof(m_nWriteBytes));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &m_nExecTime, sizeof(m_nExecTime));
	if(FAILED(hr))return hr;

	return S_OK;
}

STDMETHODIMP CBRecordset::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	long n;
	int i;

	CBLock l(&m_cs);

	if(m_pFields == NULL || m_pDatabase != NULL)return E_NOTIMPL;

	n = GetFieldCount();
	hr = pStm->Write(&n, sizeof(n), NULL);
	if(FAILED(hr))return hr;

	for(i = 0; i < GetFieldCount(); i ++)
	{
		n = m_arrayFields[i].GetLength() * 2 + 2;

		hr = pStm->Write(&n, sizeof(n), NULL);
		if(FAILED(hr))return hr;

		hr = pStm->Write((LPCWSTR)m_arrayFields[i], n, NULL);
		if(FAILED(hr))return hr;
	}

	hr = pStm->Write(&m_nResultCount, sizeof(m_nResultCount), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&m_nFirstPosition, sizeof(m_nFirstPosition), NULL);
	if(FAILED(hr))return hr;

	n = m_arrayVariant.GetCount();
	hr = pStm->Write(&n, sizeof(n), NULL);
	if(FAILED(hr))return hr;

	for(i = 0; i < n; i ++)
	{
		hr = ((CBVariant*)&m_arrayVariant[i])->WriteToStream(pStm);
		if(FAILED(hr))return hr;
	}

	hr = pStm->Write(&m_nPos, sizeof(m_nPos), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&m_nReadTimes, sizeof(m_nReadTimes), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&m_nReadBytes, sizeof(m_nReadBytes), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&m_nWriteTimes, sizeof(m_nWriteTimes), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&m_nWriteBytes, sizeof(m_nWriteBytes), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&m_nExecTime, sizeof(m_nExecTime), NULL);
	if(FAILED(hr))return hr;

	return S_OK;
}

STDMETHODIMP CBRecordset::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBRecordset::InitNew(void)
{
	return E_NOTIMPL;
}

