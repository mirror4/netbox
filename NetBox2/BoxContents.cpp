// BoxContents.cpp : implementation file
//

#include "stdafx.h"
#include "BoxContents.h"


// CBoxContents

IMPLEMENT_DYNAMIC(CBoxContents, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxContents, CBoxSafeObject)
	DISP_PROPERTY_PARAM_ID(CBoxContents, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxContents, "Contents", get_Contents, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CBoxContents, "_NewEnum", DISPID_NEWENUM, get__NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

// CBoxContents message handlers

LPDISPATCH CBoxContents::get_Contents(void)
{
	m_pContents->ExternalAddRef();
	return m_pContents;
}

VARIANT CBoxContents::get_Item(VARIANT& varKey)
{
	return m_pContents->get_Item(varKey);
}

void CBoxContents::put_Item(VARIANT& varKey, VARIANT& var)
{
	m_pContents->put_Item(varKey, var);
}

LPUNKNOWN CBoxContents::get__NewEnum(void)
{
	return m_pContents->get__NewEnum();
}

