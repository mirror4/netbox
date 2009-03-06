#pragma once

#include "BList.h"
#include "BVarType.h"
#include "BDatabase.h"

class __declspec(uuid("94659465-0000-0052-6563-6F7264736574"))
CBRecordset : public CBDispatch<IRecordset>,
			  public IPersistStreamInit
{
	DECLARE_CLASS(CBRecordset)

	INTERFACE_BEGIN_EX(CBDispatch<IRecordset>)
		PERSIST_INTERFACE
	INTERFACE_END

public:
	CBRecordset(void) :
		m_nPos(0),
		m_nResultCount(0),
		m_nFirstPosition(0),
		m_nReadTimes(0),
		m_nReadBytes(0),
		m_nWriteTimes(0),
		m_nWriteBytes(0),
		m_nExecTime(0)
	{}

	~CBRecordset(void);

public:
	// IPersist
	STDMETHOD(GetClassID)(CLSID *pClassID);

public:
	// IPersistStream
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

public:
	// IPersistStreamInit
	STDMETHOD(InitNew)(void);

public:
	// IRecordset
	STDMETHOD(get_Value)(VARIANT VarKey, VARIANT *pvar);
	STDMETHOD(put_Value)(VARIANT VarKey, VARIANT pvar);
	STDMETHOD(Move)(long pos);
	STDMETHOD(MoveFirst)(void);
	STDMETHOD(MoveLast)(void);
	STDMETHOD(MoveNext)(void);
	STDMETHOD(MovePrevious)(void);
	STDMETHOD(Load)(VARIANT VarSrc);
	STDMETHOD(Save)(VARIANT VarDesc);
	STDMETHOD(AddNew)(void);
	STDMETHOD(Update)(void);
	STDMETHOD(get_BOF)(VARIANT_BOOL *pvar);
	STDMETHOD(get_EOF)(VARIANT_BOOL *pvar);
	STDMETHOD(get_AbsolutePosition)(long *pvar);
	STDMETHOD(put_AbsolutePosition)(long pvar);
	STDMETHOD(get_ResultCount)(long *pvar);
	STDMETHOD(get_RecordCount)(long *pvar);
	STDMETHOD(get_FirstPosition)(long *pvar);
	STDMETHOD(get_Fields)(IStringList **ppFields);
	STDMETHOD(get_NextRecordset)(IRecordset** ppRS);
	STDMETHOD(get_ReadTimes)(long *pVal);
	STDMETHOD(get_ReadBytes)(double *pVal);
	STDMETHOD(get_WriteTimes)(long *pVal);
	STDMETHOD(get_WriteBytes)(double *pVal);
	STDMETHOD(get_ExecTime)(double *pVal);

public:
	void AppendField(CBString strName)
	{
		if(m_pFields == NULL)m_pFields.CreateInstance();

		m_arrayFields.Add(strName);
		m_pFields->AddValue(strName);
	}

	template<class TYPE>
	void AddValue(const TYPE& value)
	{
		VARIANT var = {VT_EMPTY};

		*(CComVariant*)&var = value;
		m_arrayVariant.Add(var);
	}

	HRESULT AddBlob(const void * pData, int size)
	{
		VARIANT var = {VT_EMPTY};
		CBVarPtr varPtr;
		HRESULT hr;

		hr = varPtr.Create(size);
		if(FAILED(hr))return hr;

		CopyMemory(varPtr.m_pData, pData, size);

		hr = varPtr.GetVariant(var);
		if(FAILED(hr))return hr;

		m_arrayVariant.Add(var);

		return S_OK;
	}

	int GetFieldCount()
	{
		return m_arrayFields.GetCount();
	}

	CBComPtr<CBRecordset> m_pNextRS;

private:
	CBCriticalSection m_cs;
	CBComPtr<CBList> m_pFields;
	CAtlArray<CBString> m_arrayFields;
	CAtlArray<VARIANT> m_arrayVariant;
	long m_nPos;

public:
	CBComPtr<CBDatabase> m_pDatabase;
	CBString m_strTable;

public:
	long m_nResultCount;
	long m_nFirstPosition;
	long m_nReadTimes;
	double m_nReadBytes;
	long m_nWriteTimes;
	double m_nWriteBytes;
	__int64 m_nExecTime;
};

