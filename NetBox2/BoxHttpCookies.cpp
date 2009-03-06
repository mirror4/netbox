#include "StdAfx.h"
#include "BoxObject.h"
#include "BoxHttpCookies.h"
#include "BoxHttpCookiesItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CBoxHttpCookies::CBoxHttpCookies(void)
{
	SetAutoAdd();
}

void CBoxHttpCookies::InitNewValue(CComVariant& var)
{
	CBoxObject<CBoxHttpCookiesItem> pItem;

	pItem.CreateInstance();
	var = (LPDISPATCH)pItem;
}

BEGIN_DISPATCH_MAP(CBoxHttpCookies, CBoxDictionary)
	DISP_PROPERTY_PARAM_ID(CBoxHttpCookies, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)
END_DISPATCH_MAP()

void CBoxHttpCookies::put_Item(VARIANT& varKey, VARIANT& var)
{
	CBoxObject<CBoxHttpCookiesItem> pItem;
	VARIANT varItem;

	varItem = get_Item(varKey);
	pItem = varItem.pdispVal;
	VariantClear(&varItem);
	if(pItem != NULL)
	{
		pItem->RemoveAll();
		pItem->SetDefaultValue(var);
	}
}
