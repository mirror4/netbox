
#pragma once

#include <objsafe.h>

extern BOOL s_bObjectSafety;
// CBoxSafeObject command target

class CBoxSafeObject : public CCmdTarget
{
	DECLARE_DYNAMIC(CBoxSafeObject)

public:
	CBoxSafeObject(void)
	{
		EnableAutomation();
	}

	BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)
		STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions);
		STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
	END_INTERFACE_PART(ObjectSafety)

protected:
	DECLARE_INTERFACE_MAP()
};


