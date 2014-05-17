#pragma once

class CBHook
{
public:
	PROC m_pfnOrig;
	PROC m_pfnNew;
	CBHook * m_pNext;

	void Init();

public:
	static void DoHook(BOOL bHook = TRUE);
	template <typename F>CBHook(F pfnOrig, F pfnNew) :
		m_pfnOrig((PROC)pfnOrig), m_pfnNew((PROC)pfnNew)
	{	Init();}

	template <typename F>CBHook(LPCSTR strModule, LPCSTR strFunction, F pfnNew, F* ppfnOrig = NULL) :
		m_pfnOrig(NULL), m_pfnNew((PROC)pfnNew)
	{
		HMODULE h;

		h = GetModuleHandleA(strModule);

		if(!h)
			h = LoadLibraryA(strModule);

		if(h != NULL)
		{
			m_pfnOrig = GetProcAddress(h, strFunction);
			if(ppfnOrig)*(PROC*)ppfnOrig = m_pfnOrig;
		}

		Init();
	}
};

HMODULE WINAPI NewLoadLibraryA(PCSTR);
FARPROC WINAPI NewGetProcAddress(HMODULE hModule, LPCSTR lpProcName);
