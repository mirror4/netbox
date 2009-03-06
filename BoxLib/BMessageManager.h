#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-0000-4D73-674D-616E61676572"))
CBMessageManager : public CBDispatch<IMessageManager>
{
	DECLARE_CLASS(CBMessageManager)

public:
	class CColl : public CBDispatch<IMessageColl>
	{
	public:
		// IMessageManager
		STDMETHOD(GetMessage)(BSTR bstrKey, BSTR *bstrMessage);
		STDMETHOD(FormatMessage)(BSTR bstrKey, BSTR bstr1, BSTR bstr2, BSTR bstr3, BSTR bstr4, BSTR bstr5, BSTR bstr6, BSTR bstr7, BSTR bstr8, BSTR *bstrMessage);
		STDMETHOD(AddMessage)(BSTR bstrKey, BSTR bstrMessage);
		STDMETHOD(RemoveMessage)(BSTR bstrKey);
		STDMETHOD(get_Language)(BSTR *pVal);
		STDMETHOD(get_CodePage)(long *pCodePage);
		STDMETHOD(get_Count)(long *cStrRet);
		STDMETHOD(get_Names)(VARIANT *pItemsArray);
		STDMETHOD(get_Messages)(VARIANT *pItemsArray);

		CBString GetMessage(LPCWSTR bstrKey);
	public:
		CBCriticalSection m_cs;
		CRBMap<CBKeyString, CBString> m_mapMessage;
		CBString m_strLanguage;
		long m_nCodePage;
	};

public:
	template <class T>
	void GetLanguages(CAtlArray<T> &array)
	{
		POSITION pos;

		CBLock l(&m_cs);

		pos = m_mapLang.GetHeadPosition();
		while(pos)
		{
			array.Add(((CRBMap<CBKeyString, CBComPtr<CColl> >::CPair*)pos)->m_key.m_str);
			m_mapLang.GetNext(pos);
		}

		l.Unlock();
	}

	void GetLanguage(LPCWSTR bstrLanguage, CColl** pMessageColl);

public:
	// IMessageManager
	STDMETHOD(GetLanguage)(BSTR bstrLanguage, IMessageColl** pMessageColl);
	STDMETHOD(AddLanguage)(BSTR bstrLanguage, long CodePage, IMessageColl** pMessageColl);
	STDMETHOD(RemoveLanguage)(BSTR bstrLanguage);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get_Languages)(VARIANT *pItemsArray);
	STDMETHOD(Load)(VARIANT VarSrc);
	STDMETHOD(Save)(VARIANT VarDesc);

private:
	CBCriticalSection m_cs;
	CRBMap<CBKeyString, CBComPtr<CColl> > m_mapLang;
};
