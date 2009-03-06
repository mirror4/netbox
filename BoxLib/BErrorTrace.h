#pragma once

#include "BMStream.h"
#include "BDate.h"

class __declspec(uuid("94650000-0000-4572-726F-725472616365"))
CBErrorTrace : public CBDispatch<IErrorTrace>
{
public:
	CBErrorTrace(void);
	~CBErrorTrace(void);

public:
	STDMETHOD(get_Name)(BSTR *pVal);
	STDMETHOD(get_StartTime)(DATE *pVal);
	STDMETHOD(Write)(BSTR strText);
	STDMETHOD(ReadAll)(BSTR *pVal);
	STDMETHOD(Clear)(void);

	void Write(const void* pData, int size);
	void SetName(CBString strName)
	{
		m_strName = strName;
	}

private:
	CBCriticalSection m_cs;
	CBString m_strName;
	CBComPtr<CBTempStream> m_pStream;
	CBDate m_date;
	int m_count;
};

