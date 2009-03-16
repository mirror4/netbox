#include "StdAfx.h"
#include "BSysInfo.h"
#include <iphlpapi.h>
#include <shfolder.h>
#include <Lm.h>
#include "BMStream.h"
#include "BStream.h"
#include "BDate.h"

CBSysInfo::CBSysInfo(void)
{
	Update();
}

STDMETHODIMP CBSysInfo::get_Item(VARIANT VarKey, VARIANT *pVariantReturn)
{
	CBLock l(&m_cs);

	if(VarKey.vt == VT_ERROR)
		return WriteObjectToVariant((ISysInfo*)this, pVariantReturn);

	return m_dict.get_Item(VarKey, pVariantReturn);
}

STDMETHODIMP CBSysInfo::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBSysInfo::get_Count(long *cStrRet)
{
	CBLock l(&m_cs);

	*cStrRet = (long)m_dict.GetCount();
	return S_OK;
}

STDMETHODIMP CBSysInfo::get_Key(VARIANT VarKey, VARIANT *pvar)
{
	CBLock l(&m_cs);

	return m_dict.get_Key(VarKey, pvar);
}

STDMETHODIMP CBSysInfo::Add(BSTR strKey, BSTR strValue)
{
	CBString* pstr;

	CBLock l(&m_cs);

	if(m_dict.Lookup(strKey, &pstr, TRUE) == S_OK)
		return 0x800A01C9;

	*pstr = strValue;

	m_mapUserItems.Add(strKey);
	m_mapUserItems.Add(strValue);

	return S_OK;
}

STDMETHODIMP CBSysInfo::Exists(BSTR strKey, VARIANT_BOOL* pExists)
{
	CBLock l(&m_cs);

	return m_dict.Exists(strKey, pExists);
}

STDMETHODIMP CBSysInfo::Save(VARIANT VarDesc)
{
	HRESULT hr;
	CBComPtr<IStream> pStream;

	hr = CBStream::GetStream(&VarDesc, &pStream, FALSE);
	if(FAILED(hr))return hr;

	return WriteObjectToStream((ISysInfo*)this, pStream);
}

STDMETHODIMP CBSysInfo::Update()
{
	CBLock l(&m_cs);

	m_dict.RemoveAll();

	determineOSInfo();
	determineModuleInfo();
	determineUSERInfo();
	determineScreenInfo();
	determineCpuInfo();
	determineNETInfo();
	determineHDInfo();
	determineVolInfo();

	for(size_t i = 0; i < m_mapUserItems.GetCount(); i += 2)
		Append(m_mapUserItems[i], m_mapUserItems[i + 1]);

	return S_OK;
}

STDMETHODIMP CBSysInfo::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBSysInfo);
	return S_OK;
}

STDMETHODIMP CBSysInfo::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBSysInfo::Load(IStream *pStm)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBSysInfo::Save(IStream *pStm, BOOL fClearDirty)
{
	HRESULT hr;
	static int n = 0;

	hr = pStm->Write(&n, 1, NULL);
	if(FAILED(hr))return hr;

	CBLock l(&m_cs);

	return m_dict.Save(pStm);
}

STDMETHODIMP CBSysInfo::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBSysInfo::InitNew(void)
{
	return E_NOTIMPL;
}

void CBSysInfo::determineOSInfo(void)
{
	static WCHAR STR_WINDOWS_95[] = L"Windows 95";
	static WCHAR STR_WINDOWS_98[] = L"Windows 98";
	static WCHAR STR_WINDOWS_98_SE[] = L"Windows 98 Second Edition";
	static WCHAR STR_WINDOWS_ME[] = L"Windows ME";

	static WCHAR STR_WINDOWS_NT[] = L"Windows NT";
	static WCHAR STR_WORKSTATION[] = L"Windows NT Workstation";
	static WCHAR STR_NT_SERVER[] = L"Windows NT Server";

	static WCHAR STR_WIN2K_PROF[] = L"Windows 2000 Professional";
	static WCHAR STR_WIN2K_SERVER[] = L"Windows 2000 Server";
	static WCHAR STR_WIN2K_ADV_SERVER[] = L"Windows 2000 Advanced Server";
	static WCHAR STR_WIN2K_DATA_CENETER[] = L"Windows 2000 Datacenter Server";

	static WCHAR STR_WINDOWS_XP_HOME[] = L"Windows XP Home Edition";
	static WCHAR STR_WINDOWS_XP_PROF[] = L"Windows XP Professional";

	static WCHAR STR_WINDOWS_LH_HOME[] = L"Windows LongHorn Home Edition";
	static WCHAR STR_WINDOWS_LH_PROF[] = L"Windows LongHorn Professional";

	static WCHAR STR_WIN2003_SERVER[] = L"Windows Server 2003, Standard Edition";
	static WCHAR STR_WIN2003_WEB_SERVER[] = L"Windows Server 2003, Web Edition";
	static WCHAR STR_WIN2003_ADV_SERVER[] = L"Windows Server 2003, Enterprise Edition";
	static WCHAR STR_WIN2003_DATA_CENETER[] = L"Windows Server 2003, Datacenter Edition";

	OSVERSIONINFOEX  versionInfo;
	BOOL bOsVersionInfoEx = FALSE;

	TCHAR data [4096];
	DWORD dataSize;
	HKEY hKey;
	LONG result;
	int i;

	LPCWSTR pstrPlatform;
	CBString strMinorVersion;
	CBString strServicePack;
	DWORD dwBuildNumber = 0;

	::ZeroMemory(&versionInfo, sizeof(OSVERSIONINFOEX));
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = ::GetVersionEx((OSVERSIONINFO *) &versionInfo)))
	{
		versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!::GetVersionEx((OSVERSIONINFO *) &versionInfo))
			return;
	}

	if(versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		Append(L"OS_Family", L"Windows NT");
		if(bOsVersionInfoEx && versionInfo.dwMajorVersion >= 5)
		{
			switch(versionInfo.wProductType)
			{
				case VER_NT_WORKSTATION:
					if(versionInfo.dwMajorVersion == 6)
					{
						pstrPlatform = STR_WINDOWS_LH_PROF;
						if(versionInfo.wSuiteMask & VER_SUITE_PERSONAL)
							pstrPlatform = STR_WINDOWS_LH_HOME;
					}else if(versionInfo.dwMinorVersion == 1)
					{
						pstrPlatform = STR_WINDOWS_XP_PROF;
						if(versionInfo.wSuiteMask & VER_SUITE_PERSONAL)
							pstrPlatform = STR_WINDOWS_XP_HOME;
					}else
						pstrPlatform = STR_WIN2K_PROF;
					break;
				case VER_NT_DOMAIN_CONTROLLER:
				case VER_NT_SERVER:
					if(versionInfo.dwMinorVersion == 2)
					{
						if(versionInfo.wSuiteMask & VER_SUITE_BLADE)
							pstrPlatform = STR_WIN2003_WEB_SERVER;
						else if(versionInfo.wSuiteMask & VER_SUITE_DATACENTER)
							pstrPlatform = STR_WIN2003_DATA_CENETER;
						else if(versionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
							pstrPlatform = STR_WIN2003_ADV_SERVER;
						else
							pstrPlatform = STR_WIN2003_SERVER;
					}else
					{
						if(versionInfo.wSuiteMask & VER_SUITE_DATACENTER)
							pstrPlatform = STR_WIN2K_DATA_CENETER;
						else if(versionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
							pstrPlatform = STR_WIN2K_ADV_SERVER;
						else
							pstrPlatform = STR_WIN2K_SERVER;
					}
					break;
			}
		}
		else
		{
			dataSize = sizeof(data);
			result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
									_T("System\\CurrentControlSet\\Control\\ProductOptions"),
									0, KEY_QUERY_VALUE, &hKey);

			if(result == ERROR_SUCCESS)
			{
				result = ::RegQueryValueEx(hKey, _T("ProductType"), NULL, NULL,(LPBYTE) data, &dataSize);
				RegCloseKey(hKey);
			}

			if(result == ERROR_SUCCESS)
			{
				if(!lstrcmpi(data, _T("WinNT")))
					pstrPlatform = STR_WORKSTATION;
				else
					pstrPlatform = STR_NT_SERVER;
			}else
				pstrPlatform = STR_WINDOWS_NT;
		}

		dwBuildNumber = versionInfo.dwBuildNumber;
	}
	else if(versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		Append(L"OS_Family", L"Windows");
		if((versionInfo.dwMajorVersion > 4) ||((versionInfo.dwMajorVersion == 4) &&
			(versionInfo.dwMinorVersion > 0)))
		{
			if(versionInfo.dwMinorVersion < 90)
				if(versionInfo.szCSDVersion[1] == 'A')
				{
					versionInfo.szCSDVersion[0] = 0;
					pstrPlatform = STR_WINDOWS_98_SE;
				}else
					pstrPlatform = STR_WINDOWS_98;
			else
				pstrPlatform = STR_WINDOWS_ME;
		}else
			pstrPlatform = STR_WINDOWS_95;

		dwBuildNumber =LOWORD(versionInfo.dwBuildNumber);
	}
	else return;

	strMinorVersion.Format(L"%d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);

	Append(L"OS_Platform", pstrPlatform);
	if(!strMinorVersion.IsEmpty())
		Append(L"OS_Version", strMinorVersion);

	for(i = 0; versionInfo.szCSDVersion[i] == _T(' ') && i < sizeof(versionInfo.szCSDVersion); i ++);
	if(i < sizeof(versionInfo.szCSDVersion) && versionInfo.szCSDVersion[i] != 0)
		Append(L"OS_ServicePack", versionInfo.szCSDVersion);

	strMinorVersion.Format(L"%u", dwBuildNumber);
	Append(L"OS_BuildNumber", strMinorVersion);

//------------------Internet Explorer

	dataSize = sizeof(data);
	result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							_T("Software\\Microsoft\\Internet Explorer"),
							0, KEY_QUERY_VALUE, &hKey);

	if(result == ERROR_SUCCESS)
	{
		result = ::RegQueryValueEx(hKey, _T("Version"), NULL, NULL,(LPBYTE) data, &dataSize);

		RegCloseKey(hKey);
	}

	if(result == ERROR_SUCCESS)
		Append(L"IE_Version", data);

	result = ::RegOpenKeyEx(HKEY_CURRENT_USER,
							_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),
							0, KEY_QUERY_VALUE, &hKey);

	if(result == ERROR_SUCCESS)
	{
		dataSize = sizeof(data);
		result = ::RegQueryValueEx(hKey, _T("User Agent"), NULL, NULL,(LPBYTE) data, &dataSize);

		if(result == ERROR_SUCCESS)
			Append(L"IE_UserAgent", data);

		DWORD dwProxyEnable;

		dataSize = sizeof(dwProxyEnable);
		result = ::RegQueryValueEx(hKey, _T("ProxyEnable"), NULL, NULL,(LPBYTE)&dwProxyEnable, &dataSize);

		if(result == ERROR_SUCCESS && dwProxyEnable)
		{
			dataSize = sizeof(data);
			result = ::RegQueryValueEx(hKey, _T("ProxyServer"), NULL, NULL,(LPBYTE) data, &dataSize);
		}else result = -1;

		RegCloseKey(hKey);
	}

//------------------Internet Explorer Proxy

	if(result == ERROR_SUCCESS)
	{
		LPTSTR ptr, ptr1;

		ptr = data;

		while(*ptr)
		{
			ptr1 = ptr;
			while(*ptr && *ptr != '=')
				ptr ++;

			if(*ptr == 0)
			{
				Append(L"IE_Proxy", ptr1);
				break;
			}

			strMinorVersion = L"IE_Proxy_";
			*ptr++ = 0;
			strMinorVersion += ptr1;

			ptr1 = ptr;
			while(*ptr && *ptr != ';')
				ptr ++;

			if(*ptr)
			{
				*ptr++ = 0;
				Append(strMinorVersion, ptr1);
			}else
			{
				Append(strMinorVersion, ptr1);
				break;
			}
		}
	}

//------------------ ADO Version

	dataSize = sizeof(data);
	result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							_T("SOFTWARE\\Microsoft\\DataAccess"),
							0, KEY_QUERY_VALUE, &hKey);

	if(result == ERROR_SUCCESS)
	{
		result = ::RegQueryValueEx(hKey, _T("FullInstallVer"), NULL, NULL,(LPBYTE) data, &dataSize);

		if(result != ERROR_SUCCESS)
			result = ::RegQueryValueEx(hKey, _T("Version"), NULL, NULL,(LPBYTE) data, &dataSize);

		RegCloseKey(hKey);
	}

	if(result == ERROR_SUCCESS)
		Append(L"ADO_Version", data);
}

void CBSysInfo::determineModuleInfo(void)
{
	LPBYTE lp;
	MEMORY_BASIC_INFORMATION mbi;
	char pathName[1024];
	CBString str;
	int i = 0;

	for(lp = NULL; (VirtualQuery(lp, &mbi, sizeof(mbi)) == sizeof(mbi)); lp += mbi.RegionSize)
	{
		if(mbi.State == MEM_COMMIT &&
			mbi.Type != MEM_MAPPED &&
			mbi.AllocationBase != NULL &&
			mbi.AllocationBase == mbi.BaseAddress && 
			::GetModuleFileName((HMODULE)mbi.BaseAddress, pathName, sizeof(pathName)))
		{
			str.Format(L"Module_%d", i ++);
			Append(str, pathName);
		}
	}
}

void CBSysInfo::determineUSERInfo(void)
{
	TCHAR buf[1024];
	DWORD dwSize = sizeof(buf);
	LANGID lid;

	lid = ::GetSystemDefaultLangID();
	_ltot(lid, buf, 10);
	Append(L"System_LanguageID", buf);

	VerLanguageName(lid, buf, sizeof(buf));
	Append(L"System_Language", buf);

	HMODULE hNetapi32 = ::LoadLibrary(_T("Netapi32.dll"));
	if(hNetapi32)
	{
		DWORD (__stdcall *pNetWkstaGetInfo)(LMSTR, DWORD, LPBYTE*);
		DWORD (__stdcall *pNetApiBufferFree)(LPVOID);

		pNetWkstaGetInfo = (DWORD (__stdcall *)(LMSTR, DWORD, LPBYTE*))GetProcAddress(hNetapi32, "NetWkstaGetInfo");
		pNetApiBufferFree = (DWORD (__stdcall *)(LPVOID))GetProcAddress(hNetapi32, "NetApiBufferFree");

		if(pNetWkstaGetInfo && pNetApiBufferFree)
		{
			LPWKSTA_INFO_100 pBuf = NULL;
			NET_API_STATUS nStatus;

			nStatus = pNetWkstaGetInfo(L".", 100, (LPBYTE *)&pBuf);
			if(nStatus == NERR_Success)
				Append(L"System_Domain", pBuf->wki100_langroup);

			if (pBuf)pNetApiBufferFree(pBuf);
			::FreeLibrary(hNetapi32);
			if(nStatus != NERR_Success)hNetapi32 = NULL;
		}else
		{
			::FreeLibrary(hNetapi32);
			hNetapi32 = NULL;
		}
	}

	if(hNetapi32 == NULL)
	{
		HKEY hKey;
		LONG result;
		TCHAR data [4096];
		DWORD dataSize;

		result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
								_T("SYSTEM\\CurrentControlSet\\Services\\MSNP32\\NetworkProvider"),
								0, KEY_QUERY_VALUE, &hKey);

		if(result == ERROR_SUCCESS)
		{
			dataSize = sizeof(data);
			result = ::RegQueryValueEx(hKey, _T("AuthenticatingAgent"), NULL, NULL,(LPBYTE) data, &dataSize);
			RegCloseKey(hKey);
		}

		if(result != ERROR_SUCCESS || data[0] == 0)
		{
			result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
									_T("SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP"),
									0, KEY_QUERY_VALUE, &hKey);

			if(result == ERROR_SUCCESS)
			{
				dataSize = sizeof(data);
				result = ::RegQueryValueEx(hKey, _T("Workgroup"), NULL, NULL,(LPBYTE) data, &dataSize);
				RegCloseKey(hKey);
			}
		}

		if(result == ERROR_SUCCESS)
			Append(L"System_Domain", data);
		else Append(L"System_Domain", L"");
	}

	GetComputerName(buf, &dwSize);
	Append(L"System_Name", buf);

	dwSize = sizeof(buf);
	GetUserName(buf, &dwSize);
	Append(L"System_User", buf);

	::GetWindowsDirectory(buf, sizeof(buf));
	Append(L"Folder_Windows", buf);

	::GetSystemDirectory(buf, sizeof(buf));
	Append(L"Folder_System", buf);

	HMODULE hSHFolder = ::LoadLibrary(_T("SHFolder.dll"));
	if(hSHFolder)
	{
		HRESULT (__stdcall *pSHGetFolderPath)(HWND, int, HANDLE, DWORD, LPSTR);

		pSHGetFolderPath = (HRESULT (__stdcall *)(HWND, int, HANDLE, DWORD, LPSTR))GetProcAddress(hSHFolder, "SHGetFolderPathA");

		if(pSHGetFolderPath)
		{
			if(SUCCEEDED(pSHGetFolderPath(NULL, 0, NULL, 0, buf)))
				Append(L"Folder_Desktop", buf);

			if(SUCCEEDED(pSHGetFolderPath(NULL, 0x0006, NULL, 0, buf)))
				Append(L"Folder_Favorites", buf);

			if(SUCCEEDED(pSHGetFolderPath(NULL, 0x0026, NULL, 0, buf)))
				Append(L"Folder_ProgramFiles", buf);

			if(SUCCEEDED(pSHGetFolderPath(NULL, 0x002b, NULL, 0, buf)))
				Append(L"Folder_ProgramFilesCommon", buf);

			if(SUCCEEDED(pSHGetFolderPath(NULL, 0x0002, NULL, 0, buf)))
				Append(L"Folder_Programs", buf);

			if(SUCCEEDED(pSHGetFolderPath(NULL, 0x000b, NULL, 0, buf)))
				Append(L"Folder_StartMenu", buf);

			if(SUCCEEDED(pSHGetFolderPath(NULL, 0x0005, NULL, 0, buf)))
				Append(L"Folder_MyDocuments", buf);
		}

		::FreeLibrary(hSHFolder);
	}

	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	Append(L"TimeZone_Name", tzi.StandardName);
	_itot(-tzi.Bias / 60, buf, 10);
	Append(L"TimeZone", buf);

//------------------------ IP -----------------------------------------------------------

	MIB_IPADDRTABLE *pAddrs = NULL;
	ULONG uSizeAddrs = 0;

	GetIpAddrTable(pAddrs, &uSizeAddrs, FALSE);
	if(uSizeAddrs)
	{
		pAddrs = (MIB_IPADDRTABLE *)(new char[uSizeAddrs]);
		if(pAddrs)
		{
			if(!GetIpAddrTable(pAddrs, &uSizeAddrs, TRUE))
			{
				CBString str, str1;
				UINT ipCount = 0;

				for(UINT i = 0; i < pAddrs->dwNumEntries; i ++)
					if(pAddrs->table[i].dwAddr && pAddrs->table[i].dwAddr != 0x0100007f)
					{
						str.Format(L"%d.%d.%d.%d",
							pAddrs->table[i].dwAddr & 0xff,
							(pAddrs->table[i].dwAddr >> 8) & 0xff,
							(pAddrs->table[i].dwAddr >> 16) & 0xff,
							(pAddrs->table[i].dwAddr >> 24) & 0xff);

						str1.Format(L"IP_%d", ipCount ++);

						Append(str1, str);
					}
			}

			delete pAddrs;
		}
	}
}

void CBSysInfo::determineScreenInfo(void)
{
	TCHAR data [4096];
	DWORD dataSize;
	HKEY hKey;
	LONG result;
	char str[32];

	_itoa(GetSystemMetrics(SM_CXSCREEN), str, 10);
	Append(L"Screen_Width", str);

	_itoa(GetSystemMetrics(SM_CYSCREEN), str, 10);
	Append(L"Screen_Height", str);

	HDC hDC = ::GetDC(NULL);

	_itoa(GetDeviceCaps(hDC, BITSPIXEL), str, 10);
	strcat(str, " bit");
	Append(L"Screen_Color", str);

	::DeleteDC(hDC);

	dataSize = sizeof(data);
	result = ::RegOpenKeyEx(HKEY_CURRENT_USER,
							_T("Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager"),
							0, KEY_QUERY_VALUE, &hKey);

	if(result == ERROR_SUCCESS)
	{
		result = ::RegQueryValueEx(hKey, _T("ThemeActive"), NULL, NULL,(LPBYTE) data, &dataSize);

		RegCloseKey(hKey);
	}

	if(result == ERROR_SUCCESS)
		Append(L"XPThemeActive", data);
	else Append(L"XPThemeActive", L"0");
}

void CBSysInfo::determineCpuInfo(void)
{
	CString str;
	DWORD dwHighest = 0;
	char szName[64] = {0};

	__asm
	{
		mov		eax, 0
		CPUID
		mov		dwHighest, eax
		mov		DWORD PTR [szName + 0], ebx
		mov		DWORD PTR [szName + 4], edx
		mov		DWORD PTR [szName + 8], ecx
	}

	if(dwHighest != 0)
		Append(L"CPU_Family", szName);

	__asm
	{
		mov		eax, 0x80000000
		CPUID
		mov dwHighest, eax
	}

	if(dwHighest >= 0x80000004)
	{
		__asm
		{
			mov		eax, 0x80000002
			CPUID
			mov		DWORD PTR [szName + 0],eax
			mov		DWORD PTR [szName + 4],ebx
			mov		DWORD PTR [szName + 8],ecx
			mov		DWORD PTR [szName + 12],edx
			mov		eax, 0x80000003
			CPUID
			mov		DWORD PTR [szName + 16],eax
			mov		DWORD PTR [szName + 20],ebx
			mov		DWORD PTR [szName + 24],ecx
			mov		DWORD PTR [szName + 28],edx
			mov		eax, 0x80000004
			CPUID
			mov		DWORD PTR [szName + 32],eax
			mov		DWORD PTR [szName + 36],ebx
			mov		DWORD PTR [szName + 40],ecx
			mov		DWORD PTR [szName + 44],edx
		}

		str = szName;
		str.Trim();
		Append(L"CPU_Name", str);
	}

	DWORD cpueax, cpuebx, cpuecx, cpuedx;

	__asm{
		mov eax, 01h
		cpuid
		mov cpueax, eax;
		mov cpuedx, edx;
	}
	if (cpuedx & 0x20000)
	{
		__asm{
			mov eax, 03h
			cpuid
			mov cpuedx, edx;
			mov cpuecx, ecx;
		}

		str.Format(_T("%08X-%08X-%08X"), cpueax, cpuedx, cpuecx);
		Append(L"CPU_Serial", str);
	}

	SYSTEM_INFO stCpuInfo;

	GetSystemInfo(&stCpuInfo);

	str.Format(_T("%u"), stCpuInfo.dwNumberOfProcessors);
	Append(L"CPU_Number", str);

	MEMORYSTATUS ms;

	GlobalMemoryStatus(&ms);

	str.Format(_T("%uM"), (ms.dwTotalPhys + 655360) / (1024 * 1024));
	Append(L"RAM", str);
}

void CBSysInfo::determineNETInfo(void)
{
	PMIB_IFTABLE pAddrs = NULL;
	ULONG uSizeAddrs = 0;
	ULONG i, ifCount = 0;
	CBString strName, strAddr;

	GetIfTable(pAddrs, &uSizeAddrs, TRUE);
	if(uSizeAddrs)
	{
		pAddrs = (PMIB_IFTABLE)(new char[uSizeAddrs]);
		if(pAddrs)
		{
			if(!GetIfTable(pAddrs, &uSizeAddrs, TRUE))
				for(i = 0; i < pAddrs->dwNumEntries; i ++)
					if(pAddrs->table[i].dwType == MIB_IF_TYPE_ETHERNET && pAddrs->table[i].dwSpeed >= 10000000)
					{
						strName.Format(L"NIC_%d", ifCount ++);
						strAddr.Format(L"%02X:%02X:%02X:%02X:%02X:%02X",
							pAddrs->table[i].bPhysAddr[0],
							pAddrs->table[i].bPhysAddr[1],
							pAddrs->table[i].bPhysAddr[2],
							pAddrs->table[i].bPhysAddr[3],
							pAddrs->table[i].bPhysAddr[4],
							pAddrs->table[i].bPhysAddr[5]);

						Append(strName, strAddr);
					}

			delete pAddrs;
		}
	}

	return;
}

#define DFP_GET_VERSION 0x00074080 
#define DFP_SEND_DRIVE_COMMAND 0x0007c084 
#define DFP_RECEIVE_DRIVE_DATA 0x0007c088 

#pragma pack(4) 
typedef struct _GETVERSIONOUTPARAMS { 
	BYTE bVersion;  // Binary driver version. 
	BYTE bRevision;  // Binary driver revision. 
	BYTE bReserved;  // Not used. 
	BYTE bIDEDeviceMap; // Bit map of IDE devices. 
	DWORD fCapabilities; // Bit mask of driver capabilities. 
	DWORD dwReserved[4]; // For future use. 
}GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS; 

typedef struct _IDEREGS { 
	BYTE bFeaturesReg;  // Used for specifying SMART "commands". 
	BYTE bSectorCountReg; // IDE sector count register 
	BYTE bSectorNumberReg; // IDE sector number register 
	BYTE bCylLowReg;   // IDE low order cylinder value 
	BYTE bCylHighReg;  // IDE high order cylinder value 
	BYTE bDriveHeadReg;  // IDE drive/head register 
	BYTE bCommandReg;  // Actual IDE command. 
	BYTE bReserved;   // reserved for future use.  Must be zero. 
}IDEREGS, *PIDEREGS, *LPIDEREGS; 

typedef struct _SENDCMDINPARAMS { 
	DWORD cBufferSize;  // Buffer size in bytes 
	IDEREGS irDriveRegs;  // Structure with drive register values. 
	BYTE bDriveNumber;  // Physical drive number to send 
       // command to (0,1,2,3). 
	BYTE bReserved[3];  // Reserved for future expansion. 
	DWORD dwReserved[4];  // For future use. 
	//BYTE  bBuffer[1];   // Input buffer. 
}SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS; 

typedef struct _DRIVERSTATUS { 
	BYTE bDriverError;  // Error code from driver, 
       // or 0 if no error. 
	BYTE bIDEStatus;   // Contents of IDE Error register. 
       // Only valid when bDriverError 
       // is SMART_IDE_ERROR. 
	BYTE bReserved[2];  // Reserved for future expansion. 
	DWORD dwReserved[2];  // Reserved for future expansion. 
}DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS; 

typedef struct _SENDCMDOUTPARAMS { 
	DWORD    cBufferSize;  // Size of bBuffer in bytes 
	DRIVERSTATUS DriverStatus;  // Driver status structure. 
	BYTE   bBuffer[512];   // Buffer of arbitrary length 
         // in which to store the data read from the drive. 
}SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS; 

typedef struct _IDSECTOR { 
	USHORT wGenConfig; 
	USHORT wNumCyls; 
	USHORT wReserved; 
	USHORT wNumHeads; 
	USHORT wBytesPerTrack; 
	USHORT wBytesPerSector; 
	USHORT wSectorsPerTrack; 
	USHORT wVendorUnique[3]; 
	CHAR sSerialNumber[20]; 
	USHORT wBufferType; 
	USHORT wBufferSize; 
	USHORT wECCSize; 
	CHAR sFirmwareRev[8]; 
	CHAR sModelNumber[40]; 
	USHORT wMoreVendorUnique; 
	USHORT wDoubleWordIO; 
	USHORT wCapabilities; 
	USHORT wReserved1; 
	USHORT wPIOTiming; 
	USHORT wDMATiming; 
	USHORT wBS; 
	USHORT wNumCurrentCyls; 
	USHORT wNumCurrentHeads; 
	USHORT wNumCurrentSectorsPerTrack; 
	ULONG ulCurrentSectorCapacity; 
	USHORT wMultSectorStuff; 
	ULONG ulTotalAddressableSectors; 
	USHORT wSingleWordDMA; 
	USHORT wMultiWordDMA; 
	BYTE bReserved[128]; 
}IDSECTOR, *PIDSECTOR; 

typedef struct _SCSI_PASS_THROUGH {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    DWORD DataBufferOffset;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
}SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;
#define IOCTL_SCSI_PASS_THROUGH         CTL_CODE(0x00000004, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

static int IDEHDDID(HANDLE hIO, int i, char *szSerial)
{
	GETVERSIONOUTPARAMS vers; 
	SENDCMDINPARAMS in; 
	SENDCMDOUTPARAMS out; 
	PIDSECTOR phdinfo; 

	DWORD dw;
	int j;
	unsigned char c;

	ZeroMemory(&vers, sizeof(vers));
	if (!DeviceIoControl(hIO, DFP_GET_VERSION, 0, 0, &vers, sizeof(vers), &dw, 0))
		return 0;

	if (!(vers.fCapabilities & 1))
		return 0;

//	if (vers.fCapabilities & (16 >> i))
//		return 0;

	ZeroMemory(&in, sizeof(in));
	in.irDriveRegs.bCommandReg=0xec;
	in.bDriveNumber=i;
	in.irDriveRegs.bDriveHeadReg=(i & 1)?0xb0:0xa0;
	in.irDriveRegs.bSectorCountReg=1;
	in.irDriveRegs.bSectorNumberReg=1;
	in.cBufferSize=512;

	ZeroMemory(&out, sizeof(out));
	if (!DeviceIoControl(hIO, DFP_RECEIVE_DRIVE_DATA, &in, sizeof(in), &out, sizeof(out), &dw, 0))
		return 0;

	phdinfo = (PIDSECTOR)out.bBuffer;
	CopyMemory(szSerial, phdinfo->sSerialNumber, 20);
	szSerial[20] = 0;
	for (j=0;j<20;j+=2)
	{
		c = szSerial[j];
		szSerial[j] = szSerial[j+1]; 
		szSerial[j+1] = c; 
	}

	for (j=0;j<20;j++)
		if(szSerial[j] < 32)szSerial[j] = 32;

	return 20;
}

static int SCSIHDDID(HANDLE hIO, int i, char *szSerial)
{
	char buffer[sizeof(SCSI_PASS_THROUGH)+256];
	SCSI_PASS_THROUGH *pspt = (SCSI_PASS_THROUGH *)buffer;

	DWORD dw;
	int j, j1;

	//SCSI
	ZeroMemory(pspt, sizeof(buffer));
	pspt->Length = sizeof(SCSI_PASS_THROUGH);
	pspt->TargetId = 1;
	pspt->CdbLength = 6;
	pspt->SenseInfoLength = 24;
	pspt->DataIn = 1;
	pspt->DataTransferLength = 192;
	pspt->TimeOutValue = 2;
	pspt->DataBufferOffset = pspt->Length + 32;
	pspt->SenseInfoOffset = pspt->Length;
	pspt->Cdb[0] = 0x12;
	pspt->Cdb[1] = 1;
	pspt->Cdb[2] = 0x80;
	pspt->Cdb[4] = 192;
	dw = sizeof(buffer);

	if (!DeviceIoControl(hIO, 0x0004D004, pspt, sizeof(SCSI_PASS_THROUGH), pspt, dw, &dw, 0))
		return 0;

	if (*((BYTE *)pspt+pspt->DataBufferOffset+1)!=0x80)
		return 0;

	j = *((BYTE *)pspt+pspt->DataBufferOffset+3);
	j = j>30?30:j;
	CopyMemory(szSerial,(BYTE *)pspt+pspt->DataBufferOffset+4,j);

	for (j1=0;j1<j;j1++)
		if(szSerial[j1] < 32)szSerial[j1] = 32;

	szSerial[j] = 0;
	return j;
}

static int Rin0Call(void (__cdecl *start_address)(void*), void* arg)
{
	_asm {
		mov		ecx, start_address
		mov		edx, arg

		mov		eax, fs:[0]
		push	eax
		sidt	[esp-02h]
		pop		ebx
		add		ebx, 0x1C

		mov		eax, [ebx]
		shr		eax, 16
		push	ax
		mov     ax, [ebx-4]
		push	ax

		push	ebx

		cli
		lea		eax, l1
		mov		[ebx-4], ax
		shr		eax, 16
		mov		[ebx+2], ax
		sti

		int     3

		cli
		pop		ebx

		pop		ax
		mov		[ebx-4], ax
		pop		ax
		mov		[ebx+2], ax
		sti

		jmp		l2
l1:
		push	edx
		call	ecx
		add		esp, 4
		iretd
l2:
	}
}

struct tagIDE98Arg
{
	int		nDrive;
	char	sSerial[20];
};

#pragma warning (disable:4035) 
inline int inp(short rdx)
{
    _asm xor eax, eax
    _asm mov dx, rdx
    _asm in al, dx
}

inline int inpw(short rdx)
{
    _asm xor eax, eax
    _asm mov dx, rdx
    _asm in  ax, dx
}

inline void outp(short rdx, int ral)
{
    _asm mov dx, rdx
    _asm mov eax, ral
    _asm out dx, al
}

inline int WaitIde(int p)
{
   int   al;
   int	c = 10000;

   while (c > 0 && (al = inp(p)) >= 0x80)
	   c --;

   return c ? al : 0;
}

static void Ring0IDE(void* p)
{
	tagIDE98Arg* pArg = (tagIDE98Arg*)p;
	short	sSerial[256];
	char *szSerial;
	int baseAddress = 0;
	int portValue = 0;
	int waitLoop = 0;
	int index = 0;
	int i, j;

	switch (pArg->nDrive / 2)
	{
		case 0: baseAddress = 0x1f0; break;
		case 1: baseAddress = 0x170; break;
		case 2: baseAddress = 0x1e8; break;
		case 3: baseAddress = 0x168; break;
	}

	WaitIde(baseAddress + 7);

	outp(baseAddress + 6, pArg->nDrive & 1 ? 0xB0 : 0xA0);

	if((WaitIde(baseAddress + 7) & 0x50) != 0x50)
		return;

	outp(baseAddress + 6, pArg->nDrive & 1 ? 0xB0 : 0xA0);
	outp(baseAddress + 7, 0xEC);

	if ((WaitIde(baseAddress + 7) & 0x58) != 0x58)
		return;

	for (index = 0; index < 256; index++)
		sSerial[index] = inpw(baseAddress);

	for(i = 0, j = 0; i < 10; i++)
	{
		pArg->sSerial[j ++] = sSerial[10 + i] >> 8;
		pArg->sSerial[j ++] = sSerial[10 + i] & 0xFF;
	}

	szSerial = (char*)&pArg->sSerial;

	for (i=0;i<j;i++)
		if(szSerial[i] < 32)szSerial[i] = 32;

	pArg->sSerial[j ++] = 0;
}

void CBSysInfo::determineHDInfo(void)
{
	CBString str;
	CString str1;

	if(IS_NT)
	{
		HANDLE hIO; 
		int i, n;
		char szSerial[32];

		for(i = 0, n = 0; i < 40; i ++)
		{
			str1.Format(_T("\\\\.\\PhysicalDrive%d"), i);
			hIO = CreateFile(str1, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
			if (hIO == INVALID_HANDLE_VALUE) continue;

			if (IDEHDDID(hIO, i, szSerial))
			{
				str1 = szSerial;
				str1.Trim();
				if(!str1.IsEmpty())
				{
					str.Format(L"IDE_%d", n ++);
					Append(str, str1);
				}
			}

			CloseHandle(hIO);
		}

		for(i = 0, n = 0; i < 40; i ++)
		{
			str1.Format(_T("\\\\.\\PhysicalDrive%d"), i);
			hIO = CreateFile(str1, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
			if (hIO == INVALID_HANDLE_VALUE) continue;

			ZeroMemory(szSerial, sizeof(szSerial));
			if (SCSIHDDID(hIO, i, szSerial))
			{
				str1 = szSerial;
				str1.Trim();
				if(!str1.IsEmpty())
				{
					str.Format(L"SCSI_%d", n ++);
					Append(str, str1);
				}
			}

			CloseHandle(hIO);
		}
	}else
	{
		int i, n;

		for(i = 0, n = 0; i < 8; i ++)
		{
			tagIDE98Arg a = {i};

			try{Rin0Call(Ring0IDE, &a);}catch(...){};

			str1 = a.sSerial;
			str1.Trim();
			if(!str1.IsEmpty())
			{
				str.Format(L"IDE_%d", n ++);
				Append(str, str1);
			}
		}
	}
}

void CBSysInfo::determineVolInfo(void)
{
	char strpath[] = "c:\\";
	DWORD dwSerial;
	int nCount = 0;
	CBString strName;
	CBString strValue;

	for(int i = 0; i < 26; i ++)
	{
		strpath[0] = 'a' + i;
		if(GetDriveType(strpath) == DRIVE_FIXED)
		{
			dwSerial = 0;
			GetVolumeInformation(strpath, NULL, 0, &dwSerial, NULL, NULL, NULL, NULL);

			strName.Format(L"Volume_%d", nCount ++);
			strValue.Format(L"%04X-%04X", HIWORD(dwSerial), LOWORD(dwSerial));

			Append(strName, strValue);
		}
	}
}
