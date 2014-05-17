#pragma once
#include "BoxDictionary.h"

class CBoxHttpCookies : public CBoxDictionary
{
public:
	CBoxHttpCookies(void);

public:
	virtual void InitNewValue(CComVariant& var);

public:
	afx_msg void put_Item(VARIANT& varKey, VARIANT& var);

	DECLARE_DISPATCH_MAP()
};
