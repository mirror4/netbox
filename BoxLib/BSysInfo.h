#pragma once

#include "BCollection.h"

#pragma comment(lib, "iphlpapi.lib")

class __declspec(uuid("94650000-7374-4469-6374-696F6E617279"))
CBSysInfo : public CBDispatch<ISysInfo>,
			public IPersistStreamInit
{
	DECLARE_CLASS(CBSysInfo)

	INTERFACE_BEGIN_EX(CBDispatch<ISysInfo>)
		PERSIST_INTERFACE
	INTERFACE_END

public:
	CBSysInfo(void);

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
	// ISysInfo
	STDMETHOD(get_Item)(VARIANT VarKey, VARIANT *pVariantReturn);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Key)(VARIANT VarKey, VARIANT *pvar);
	STDMETHOD(Add)(BSTR strKey, BSTR strValue);
	STDMETHOD(Exists)(BSTR strKey, VARIANT_BOOL* pExists);
	STDMETHOD(Save)(VARIANT VarDesc);
	STDMETHOD(Update)();

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		CBLock l(&m_cs);
		return m_dict.GetEnum(rgVar, pos, count);
	}

	void AppendUserData(CBString strKey, CBString strValue)
	{
		m_dict[strKey] = strValue;
		m_mapUserItems.Add(strKey);
		m_mapUserItems.Add(strValue);
	}

	void AppendUserData(CBString strKey, CBStringA strValue)
	{
		m_dict[strKey] = strValue;
		m_mapUserItems.Add(strKey);
		m_mapUserItems.Add(CBString(strValue));
	}

	CBString Lookup(CBString strKey)
	{
		CBString str;

		CBString *pString = NULL;

		CBLock l(&m_cs);
		m_dict.Lookup(strKey, &pString);
		if(pString)str = *pString;
		return str;
	}

	size_t GetCount(void)
	{
		CBLock l(&m_cs);
		return m_dict.GetCount();
	}

	template<class T>
	BOOL GetAssoc(UINT i, T& strKey, T& value)
	{
		CBKeyString k;
		CBString v;
		BOOL bRet;

		CBLock l(&m_cs);
		bRet = m_dict.GetAssoc(i, k, v);
		l.Unlock();

		if(!bRet)return bRet;

		strKey = k.m_str;
		value = v;

		return TRUE;
	}

private:
	void Append(CBString strKey, CBString strValue)
	{
		m_dict[strKey] = strValue;
	}

	void Append(CBString strKey, CBStringA strValue)
	{
		m_dict[strKey] = strValue;
	}

private:
	void determineOSInfo(void);
	void determineModuleInfo(void);
	void determineUSERInfo(void);
	void determineScreenInfo(void);
	void determineCpuInfo(void);
	void determineNETInfo(void);
	void determineHDInfo(void);
	void determineVolInfo(void);

private:
	CAtlArray<CBString> m_mapUserItems;
	CBDict<CBString> m_dict;
	CBCriticalSection m_cs;
};

