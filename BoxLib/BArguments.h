#pragma once

#include "BList.h"

class __declspec(uuid("94650000-0000-0041-7267-756D656E7473"))
CBArguments : public CBDispatch<IArguments, CBList>
{
	DECLARE_CLASS_EX(CBArguments, CBList)

public:
	STDMETHOD(get_Default)(VARIANT *pVariantReturn);

public:
	STDMETHOD(get_CommandLine)(BSTR* newVal);
	STDMETHOD(put_CommandLine)(BSTR newVal);

private:
	CBString m_strCmdLine;
};

