#include "StdAfx.h"
#include "BHook.h"
#include "BThread.h"

static CBHook * g_pFirstHook;
static LPVOID g_pMaximumApplicationAddress;
static CBCriticalSection g_csHook;
static CBLockResource g_lrHook;

static HMODULE (WINAPI *RealLoadLibraryA)(PCSTR) = LoadLibraryA;
HMODULE WINAPI NewLoadLibraryA(PCSTR pszModulePath)
{
	HMODULE hmod;

	hmod = RealLoadLibraryA(pszModulePath);
	if(hmod)CBHook::DoHook();
	return(hmod);
}
static CBHook m_LoadLibraryA(LoadLibraryA, NewLoadLibraryA);

static HMODULE (WINAPI *RealLoadLibraryW)(PCWSTR) = LoadLibraryW;
static HMODULE WINAPI NewLoadLibraryW(PCWSTR pszModulePath)
{
	HMODULE hmod;

	hmod = RealLoadLibraryW(pszModulePath);
	if(hmod)CBHook::DoHook();
	return(hmod);
}
static CBHook m_LoadLibraryW(LoadLibraryW, NewLoadLibraryW);

static HMODULE (WINAPI *RealLoadLibraryExA)(PCSTR, HANDLE, DWORD) = LoadLibraryExA;
static HMODULE WINAPI NewLoadLibraryExA(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hmod;

	hmod = RealLoadLibraryExA(pszModulePath, hFile, dwFlags);
	if(hmod)CBHook::DoHook();
	return(hmod);
}
static CBHook m_LoadLibraryExA(LoadLibraryExA, NewLoadLibraryExA);

static HMODULE (WINAPI *RealLoadLibraryExW)(PCWSTR, HANDLE, DWORD) = LoadLibraryExW;
static HMODULE WINAPI NewLoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hmod;

	hmod = RealLoadLibraryExW(pszModulePath, hFile, dwFlags);
	if(hmod)CBHook::DoHook();
	return(hmod);
}
static CBHook m_LoadLibraryExW(LoadLibraryExW, NewLoadLibraryExW);

static BOOL (WINAPI *RealFreeLibrary)(HMODULE) = FreeLibrary;
static BOOL WINAPI NewFreeLibrary(HMODULE hLibModule)
{
	BOOL bRet;

	g_csHook.Enter();
	bRet = RealFreeLibrary(hLibModule);
	g_csHook.Leave();

	return bRet;
}
static CBHook m_FreeLibrary(FreeLibrary, NewFreeLibrary);

static FARPROC (WINAPI *RealGetProcAddress)(HMODULE, LPCSTR) = GetProcAddress;
static FARPROC WINAPI NewGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC pfn = RealGetProcAddress(hModule, lpProcName);

	if(pfn == NULL)return pfn;

	CBHook * pHook;

	pHook = g_pFirstHook;
	while(pHook != NULL)
	{
		if(pfn == pHook->m_pfnOrig)
			return pHook->m_pfnNew;

		pHook = pHook->m_pNext;
	}

	return pfn;
}
static CBHook m_GetProcAddress(GetProcAddress, NewGetProcAddress);

void CBHook::DoHook(BOOL bHook)
{
	if(!g_lrHook.Lock())return;
	if(!g_csHook.IsInited())return;
	g_csHook.Enter();

	LPBYTE lp;
	MEMORY_BASIC_INFORMATION mbi;
	char pathName[2];
	PIMAGE_DOS_HEADER pDOSHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	CBHook * pHook;
	DWORD dwOld;
	DWORD i, count, fn;

	for(lp = NULL; lp < g_pMaximumApplicationAddress && (VirtualQuery(lp, &mbi, sizeof(mbi)) == sizeof(mbi));
		lp += mbi.RegionSize)
	{
		if(mbi.State == MEM_COMMIT &&
			mbi.Type != MEM_MAPPED &&
			mbi.AllocationBase != NULL &&
			mbi.AllocationBase == mbi.BaseAddress && 
			::GetModuleFileName((HMODULE)mbi.BaseAddress, pathName, sizeof(pathName)))
		{
			pDOSHeader = (PIMAGE_DOS_HEADER)mbi.BaseAddress;
			if(pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) continue;

			pNTHeader = (PIMAGE_NT_HEADERS)((char*)pDOSHeader + (DWORD)(pDOSHeader->e_lfanew));
			if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) continue;

			pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
				((char*)pDOSHeader + (DWORD)(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));

			count = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size / sizeof(IMAGE_IMPORT_DESCRIPTOR);
			for(i = 0; i < count - 1 && pImportDesc->Name && pImportDesc->FirstThunk; i ++)
			{
				pThunk = (PIMAGE_THUNK_DATA)((char*)mbi.BaseAddress + pImportDesc->FirstThunk);

				while(fn = pThunk->u1.Function)
				{
					pHook = g_pFirstHook;
					while(pHook != NULL)
					{
						if(fn == (DWORD)(bHook ? pHook->m_pfnOrig : pHook->m_pfnNew))
							break;

						pHook = pHook->m_pNext;
					}

					if(pHook != NULL && ::VirtualProtect(&pThunk->u1.Function, sizeof(PROC), PAGE_EXECUTE_READWRITE, &dwOld))
						pThunk->u1.Function = (DWORD)(!bHook ? pHook->m_pfnOrig : pHook->m_pfnNew);

					pThunk ++;
				}

				pImportDesc ++;
			}
		}
	}

	g_lrHook.Unlock();
	g_csHook.Leave();
}

void CBHook::Init()
{
	m_pNext = g_pFirstHook;
	g_pFirstHook = this;
	if(g_pMaximumApplicationAddress == 0)
	{
		SYSTEM_INFO si;

		GetSystemInfo(&si);
		g_pMaximumApplicationAddress = si.lpMaximumApplicationAddress;
	}
}
