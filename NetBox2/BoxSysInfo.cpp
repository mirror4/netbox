// BoxSysInfo.cpp : implementation file
//

#include "stdafx.h"
#include "BoxSystem.h"
#include "BoxSysInfo.h"
#include "BoxBinPtr.h"
#include <openssl\md5.h>
#include <iphlpapi.h>
#include <wininet.h>
#include "sice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned char PublicKey[272] = {0x6f, 0x11, 0x43, 0x69, 0xe1, 0x66, 0xfb, 0xb5, 0x5e, 0xed, 0x4c, 0x39, 0x6a, 0x44, 0x86, 0x91, 0x62, 0xf6, 0x07, 0x45, 0xa3, 0xb4, 0xf5, 0x04, 0xc0, 0xd2, 0x58, 0x77, 0x64, 0x49, 0xe9, 0xef, 0xfe, 0x19, 0xfb, 0x44, 0x8a, 0x8d, 0xb2, 0x3f, 0x64, 0x7c, 0x2a, 0x97, 0xad, 0x15, 0x6f, 0x5b, 0x4c, 0x28, 0xb1, 0x41, 0xf5, 0x19, 0xa9, 0x06, 0xdb, 0x18, 0xd3, 0x62, 0x78, 0x81, 0xb5, 0x31, 0xeb, 0xf5, 0xe0, 0xad, 0xfb, 0x3f, 0x5f, 0x58, 0xf4, 0xc8, 0xe0, 0x73, 0xb9, 0xdf, 0x85, 0xd8, 0xa3, 0x0d, 0x09, 0xd1, 0x28, 0x34, 0x21, 0xc3, 0x2b, 0x9c, 0x62, 0xaf, 0x77, 0x0c, 0x38, 0xfc, 0xe4, 0x98, 0x91, 0x2b, 0x3a, 0x32, 0x84, 0x15, 0x01, 0xac, 0x76, 0x33, 0xb5, 0x2a, 0xfc, 0x19, 0x8f, 0x73, 0x40, 0x4a, 0xce, 0xf6, 0x67, 0x49, 0xc0, 0xf0, 0x49, 0xad, 0x02, 0x37, 0x2d, 0x7f, 0x32, 0xe3, 0xca, 0x7f, 0xd0, 0x7e, 0x04, 0xd1, 0xb1, 0x2b, 0x4a, 0xe9, 0xfd, 0x14, 0x45, 0xf0, 0xf4, 0xc7, 0x30, 0x26, 0x25, 0x45, 0x28, 0x5d, 0xdd, 0x36, 0xbc, 0x6c, 0x13, 0x43, 0x02, 0x27, 0x92, 0xd7, 0x94, 0xdd, 0x6d, 0xa3, 0x4d, 0xc7, 0x39, 0x08, 0x87, 0xa4, 0xcb, 0x03, 0xff, 0x1f, 0x19, 0x12, 0x2a, 0x0d, 0x0d, 0x4d, 0x96, 0xdd, 0x3b, 0xb5, 0x8b, 0xc0, 0xb1, 0x2c, 0x8c, 0x7c, 0x89, 0xd4, 0x30, 0x9e, 0x22, 0xcb, 0xba, 0xfd, 0xe9, 0xb6, 0x51, 0x7a, 0xd4, 0x9d, 0x13, 0xc3, 0x64, 0xb0, 0x42, 0xb8, 0xce, 0x62, 0x20, 0x6c, 0x1c, 0xc2, 0x0e, 0x1e, 0x32, 0x4d, 0x77, 0x95, 0xc9, 0x91, 0xa1, 0x1a, 0x7e, 0x55, 0xe3, 0xef, 0x50, 0x36, 0xc1, 0xf1, 0xf3, 0x16, 0x32, 0xe8, 0xeb, 0xfa, 0x73, 0x39, 0xc6, 0x21, 0xc1, 0xda, 0xdc, 0xf7, 0xa3, 0xe2, 0x82, 0x12, 0xfb, 0xb3, 0xab, 0x62, 0x54, 0x74, 0xa4, 0x18, 0x13, 0x86, 0x8b, 0xc1, 0x97, 0xb1, 0xe1, 0x0d, 0x9b, 0x44};
unsigned char DesKey[8] = {0xe0, 0xe2, 0x0e, 0xfa, 0xeb, 0x96, 0x5d, 0xb6};

SNMPAPI SNMP_FUNC_TYPE SnmpUtilOidCpy(AsnObjectIdentifier *DstObjId, AsnObjectIdentifier *SrcObjId)
{
	DstObjId->ids = (UINT *)GlobalAlloc(GMEM_ZEROINIT,SrcObjId->idLength * sizeof(UINT));
	if(!DstObjId->ids)
	{
		SetLastError(1);
		return 0;
	}

	memcpy(DstObjId->ids,SrcObjId->ids,SrcObjId->idLength*sizeof(UINT));
	DstObjId->idLength = SrcObjId->idLength;

	return 1;
}

VOID SNMP_FUNC_TYPE SnmpUtilOidFree(AsnObjectIdentifier *ObjId)
{
	GlobalFree(ObjId->ids);
	ObjId->ids = 0;
	ObjId->idLength = 0;
}

SNMPAPI SNMP_FUNC_TYPE SnmpUtilOidNCmp(AsnObjectIdentifier *ObjIdA, AsnObjectIdentifier *ObjIdB, UINT Len)
{
	UINT CmpLen;
	UINT i;
	int  res;

	CmpLen = Len;
	if(ObjIdA->idLength < CmpLen)
		CmpLen = ObjIdA->idLength;
	if(ObjIdB->idLength < CmpLen)
		CmpLen = ObjIdB->idLength;

	for(i=0;i<CmpLen;i++)
	{
		res = ObjIdA->ids[i] - ObjIdB->ids[i];
		if(res!=0)
			return res;
	}
	return 0;
}

VOID SNMP_FUNC_TYPE SnmpUtilVarBindFree(RFC1157VarBind *VarBind)
{
	BYTE asnType;

	SnmpUtilOidFree(&VarBind->name);

	asnType = VarBind->value.asnType;

	if(asnType==ASN_OBJECTIDENTIFIER)
		SnmpUtilOidFree(&VarBind->value.asnValue.object);
	else if((asnType==ASN_OCTETSTRING) || (asnType==ASN_RFC1155_IPADDRESS) ||
        (asnType==ASN_RFC1155_OPAQUE) || (asnType==ASN_SEQUENCE))
		if(VarBind->value.asnValue.string.dynamic)
			GlobalFree(VarBind->value.asnValue.string.stream);

	VarBind->value.asnType = ASN_NULL;
}

// CBoxSysInfo

IMPLEMENT_DYNAMIC(CBoxSysInfo, CBoxContents)

CBoxSysInfo::CBoxSysInfo(void)
{
	m_pContents->SetReadOnly();

	m_hInst = LoadLibrary(_T("inetmib1.dll"));
	if(m_hInst < (HINSTANCE) HINSTANCE_ERROR)
	{
		m_hInst = NULL;
		return;
	}

	pSnmpExtensionInit pInit = (pSnmpExtensionInit) GetProcAddress(m_hInst,"SnmpExtensionInit");
	m_pQuery = (pSnmpExtensionQuery) GetProcAddress(m_hInst,"SnmpExtensionQuery");

	HANDLE hPollForTrapEvent;
	AsnObjectIdentifier SupportedView;

	if (!pInit(0, &hPollForTrapEvent, &SupportedView))
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
		return;
	}
}

CBoxSysInfo::~CBoxSysInfo(void)
{
	if(m_hInst)
		FreeLibrary(m_hInst);
}

void CBoxSysInfo::determineOSInfo(void)
{
	static TCHAR STR_WINDOWS_95[] = _T("Windows 95");
	static TCHAR STR_WINDOWS_98[] = _T("Windows 98");
	static TCHAR STR_WINDOWS_98_SE[] = _T("Windows 98 Second Edition");
	static TCHAR STR_WINDOWS_ME[] = _T("Windows ME");

	static TCHAR STR_WINDOWS_NT[] = _T("Windows NT");
	static TCHAR STR_WORKSTATION[] = _T("Windows NT Workstation");
	static TCHAR STR_NT_SERVER[] = _T("Windows NT Server");

	static TCHAR STR_WIN2K_PROF[] = _T("Windows 2000 Professional");
	static TCHAR STR_WIN2K_SERVER[] = _T("Windows 2000 Server");
	static TCHAR STR_WIN2K_ADV_SERVER[] = _T("Windows 2000 Advanced Server");
	static TCHAR STR_WIN2K_DATA_CENETER[] = _T("Windows 2000 Datacenter Server");

	static TCHAR STR_WINDOWS_XP_HOME[] = _T("Windows XP Home Edition");
	static TCHAR STR_WINDOWS_XP_PROF[] = _T("Windows XP Professional");

	static TCHAR STR_WHISTLER_SERVER[] = _T("Windows .NET Standard Server");
	static TCHAR STR_WHISTLER_WEB_SERVER[] = _T("Windows .NET Web Server");
	static TCHAR STR_WHISTLER_ADV_SERVER[] = _T("Windows .NET Enterprise Server");
	static TCHAR STR_WHISTLER_DATA_CENETER[] = _T("Windows .NET Datacenter Server");

	OSVERSIONINFOEX  versionInfo;
	BOOL bOsVersionInfoEx = FALSE;

	TCHAR data [4096];
	DWORD dataSize;
	HKEY hKey;
	LONG result;
	int i;

	LPCTSTR pstrPlatform;
	CString strMinorVersion;
	CString strServicePack;
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
		m_pContents->SetValue(_T("OS_Family"), _T("Windows NT"));
		if(bOsVersionInfoEx && versionInfo.dwMajorVersion >= 5)
		{
			switch(versionInfo.wProductType)
			{
				case VER_NT_WORKSTATION:
					if(versionInfo.dwMinorVersion == 1)
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
							pstrPlatform = STR_WHISTLER_WEB_SERVER;
						else if(versionInfo.wSuiteMask & VER_SUITE_DATACENTER)
							pstrPlatform = STR_WHISTLER_DATA_CENETER;
						else if(versionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
							pstrPlatform = STR_WHISTLER_ADV_SERVER;
						else
							pstrPlatform = STR_WHISTLER_SERVER;
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
		m_pContents->SetValue(_T("OS_Family"), _T("Windows"));
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

	strMinorVersion.Format(_T("%d.%d"), versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);

	m_pContents->SetValue(_T("OS_Platform"), pstrPlatform);
	if(!strMinorVersion.IsEmpty())
		m_pContents->SetValue(_T("OS_Version"), strMinorVersion);

	for(i = 0; versionInfo.szCSDVersion[i] == _T(' ') && i < sizeof(versionInfo.szCSDVersion); i ++);
	if(i < sizeof(versionInfo.szCSDVersion) && versionInfo.szCSDVersion[i] != 0)
		m_pContents->SetValue(_T("OS_ServicePack"), versionInfo.szCSDVersion);

	strMinorVersion.Format(_T("%u"), dwBuildNumber);
	m_pContents->SetValue(_T("OS_BuildNumber"), strMinorVersion);

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
		m_pContents->SetValue(_T("IE_Version"), data);

	result = ::RegOpenKeyEx(HKEY_CURRENT_USER,
							_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),
							0, KEY_QUERY_VALUE, &hKey);

	if(result == ERROR_SUCCESS)
	{
		dataSize = sizeof(data);
		result = ::RegQueryValueEx(hKey, _T("User Agent"), NULL, NULL,(LPBYTE) data, &dataSize);

		if(result == ERROR_SUCCESS)
			m_pContents->SetValue(_T("IE_UserAgent"), data);

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
		LPCTSTR ptr, ptr1;

		ptr = data;

		while(*ptr)
		{
			ptr1 = ptr;
			while(*ptr && *ptr != '=')
				ptr ++;

			if(*ptr == 0)
			{
				m_pContents->SetValue(_T("IE_Proxy"), ptr1);
				break;
			}

			strMinorVersion = "IE_Proxy_";
			strMinorVersion.Append(ptr1, (int)(ptr - ptr1));

			ptr ++;

			ptr1 = ptr;
			while(*ptr && *ptr != ';')
				ptr ++;

			strServicePack.SetString(ptr1, (int)(ptr - ptr1));
			m_pContents->SetValue(strMinorVersion, strServicePack);

			if(*ptr)ptr ++;
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
		m_pContents->SetValue(_T("ADO_Version"), data);
}

void CBoxSysInfo::determineRAMInfo(void)
{
	MEMORYSTATUS ms;
	CString str;

	GlobalMemoryStatus(&ms);

	str.Format(_T("%uM"), (ms.dwTotalPhys + 655360) / (1024 * 1024));
	m_pContents->SetValue(_T("RAM"), str);
}

void CBoxSysInfo::determineUSERInfo(void)
{
	TCHAR buf[1024];
	DWORD dwSize = sizeof(buf);

	VerLanguageName(::GetSystemDefaultLangID(), buf, sizeof(buf));
	m_pContents->SetValue(_T("System_Language"), buf);

	GetComputerName(buf, &dwSize);
	m_pContents->SetValue(_T("System_Name"), buf);

	dwSize = sizeof(buf);
	GetUserName(buf, &dwSize);
	m_pContents->SetValue(_T("System_User"), buf);

	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	m_pContents->SetValue(_T("TimeZone_Name"), tzi.StandardName);
	_itot(-tzi.Bias / 60, buf, 10);
	m_pContents->SetValue(_T("TimeZone"), buf);

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
				CString str, str1;

				for(UINT i = 0; i < pAddrs->dwNumEntries; i ++)
				{
					str.Format(_T("%d.%d.%d.%d"),
						pAddrs->table[i].dwAddr & 0xff,
						(pAddrs->table[i].dwAddr >> 8) & 0xff,
						(pAddrs->table[i].dwAddr >> 16) & 0xff,
						(pAddrs->table[i].dwAddr >> 24) & 0xff);

					str1.Format(_T("IP_%d"), i);

					m_pContents->SetValue(str1, str);
				}
			}

			delete pAddrs;
		}
	}
}

void CBoxSysInfo::determineCpuInfo(void)
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
		m_pContents->SetValue(_T("CPU_Family"), szName);

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

		m_pContents->SetValue(_T("CPU_Name"), szName);
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
		m_pContents->SetValue(_T("CPU_Serial"), str);
	}

	SYSTEM_INFO stCpuInfo;

	GetSystemInfo(&stCpuInfo);

	str.Format(_T("%u"), stCpuInfo.dwNumberOfProcessors);
	m_pContents->SetValue(_T("CPU_Number"), str);
}

void CBoxSysInfo::determineScreenInfo(void)
{
	BOOL (WINAPI* pEnumDisplayDevices)(PVOID,DWORD,PVOID,DWORD);
	DISPLAY_DEVICE ddi;

	pEnumDisplayDevices = (BOOL (WINAPI*)(PVOID,DWORD,PVOID,DWORD))GetProcAddress(LoadLibrary("USER32"), "EnumDisplayDevicesA");

	ZeroMemory(&ddi, sizeof(DISPLAY_DEVICE));
	ddi.cb = sizeof(DISPLAY_DEVICE);

	if(pEnumDisplayDevices && pEnumDisplayDevices(NULL, 0, &ddi, 0))
		m_pContents->SetValue(_T("Display_Adapter"), ddi.DeviceString);

	char str[32];

	itoa(GetSystemMetrics(SM_CXSCREEN), str, 10);
	m_pContents->SetValue(_T("Screen_Width"), str);

	itoa(GetSystemMetrics(SM_CYSCREEN), str, 10);
	m_pContents->SetValue(_T("Screen_Height"), str);

	HDC hDC = ::GetDC(NULL);

	itoa(GetDeviceCaps(hDC, BITSPIXEL), str, 10);
	strcat(str, " bit");
	m_pContents->SetValue(_T("Screen_Color"), str);

	::DeleteDC(hDC);
}

void CBoxSysInfo::determineNETInfo(void)
{
	UINT				OID_ifMAC[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 6};
	AsnObjectIdentifier MIB_ifMAC = {sizeof(OID_ifMAC) / sizeof(UINT), OID_ifMAC};

	RFC1157VarBindList	varBindList;
	RFC1157VarBind		varBind;
	AsnInteger			errorStatus;
	AsnInteger			errorIndex;
	int					ret;
	UINT				ifIndex = 0;

	CString strName, strAddr;

	varBindList.list = &varBind;
	SNMP_oidcpy(&varBind.name, &MIB_ifMAC);
	varBindList.len = 1;

	do
	{
		if(m_pQuery(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex))
		{
			ret = SNMP_oidncmp(&varBind.name, &MIB_ifMAC, MIB_ifMAC.idLength);
			if (!ret && varBind.value.asnValue.address.length &&
				!(varBind.value.asnValue.address.stream[0] == 0x44 &&
				varBind.value.asnValue.address.stream[1] == 0x45 &&
				varBind.value.asnValue.address.stream[2] == 0x53 &&
				varBind.value.asnValue.address.stream[3] == 0x54) &&
				!(varBind.value.asnValue.address.stream[0] == 0x00 &&
				varBind.value.asnValue.address.stream[1] == 0x00 &&
				varBind.value.asnValue.address.stream[2] == 0x00 &&
				varBind.value.asnValue.address.stream[3] == 0x00 &&
				varBind.value.asnValue.address.stream[4] == 0x00 &&
				varBind.value.asnValue.address.stream[5] == 0x00))
			{
				strName.Format(_T("NIC_%d"), ifIndex);
				strAddr.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
					varBind.value.asnValue.address.stream[0],
					varBind.value.asnValue.address.stream[1],
					varBind.value.asnValue.address.stream[2],
					varBind.value.asnValue.address.stream[3],
					varBind.value.asnValue.address.stream[4],
					varBind.value.asnValue.address.stream[5]);

				m_pContents->SetValue(strName, strAddr);

				ifIndex++;
			}
		}
	} 
	while (!ret);

	SNMP_FreeVarBind(&varBind);
}

void CBoxSysInfo::determineAPPInfo(void)
{
	static char HEX[] = "0123456789ABCDEF";
	char str[MD5_DIGEST_LENGTH * 2 + 1];
	int i;

	m_pContents->SetValue(_T("APP_Version"), CBoxSystem::getVersion());
	m_pContents->SetValue(_T("APP_Created"), __DATE__ ", " __TIME__);

	for(i = 0; i < MD5_DIGEST_LENGTH; i ++)
	{
		str[i * 2] = HEX[APP_MD5[i] / 16];
		str[i * 2 + 1] = HEX[APP_MD5[i] % 16];
	}
	str[i * 2] = 0;

	m_pContents->SetValue(_T("APP_Info"), str);

	m_pContents->SetValue(_T("PROG_Name"), _T(""));

	if(g_dwAppMark == MK_9465)
	{
		for(i = 0; i < MD5_DIGEST_LENGTH; i ++)
		{
			str[i * 2] = HEX[PROG_MD5[i] / 16];
			str[i * 2 + 1] = HEX[PROG_MD5[i] % 16];
		}
		str[i * 2] = 0;
		m_pContents->SetValue(_T("PROG_Info"), str);
	}
}

void CBoxSysInfo::determineSYSInfo(void)
{
	static BOOL bFirstDetermine = TRUE;
	static __int64 nLastDetermineTime;
	SYSTEMTIME st;
	__int64 nNow;

	GetSystemTime(&st);
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, (FILETIME*)&nNow);

	if(nLastDetermineTime != nNow)
	{
		nLastDetermineTime = nNow;
		if(!SimpleIce())
		{
			determineOSInfo();
			determineUSERInfo();
			determineScreenInfo();
			if(bFirstDetermine)
			{
				determineCpuInfo();
				determineRAMInfo();
				if(m_hInst)
					determineNETInfo();
				determineHDInfo();
				determineAPPInfo();
			}
			bFirstDetermine = FALSE;
		}
	}
}

BEGIN_DISPATCH_MAP(CBoxSysInfo, CBoxContents)
	DISP_FUNCTION(CBoxSysInfo, "SetLicense", SetLicense, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT VTS_BSTR VTS_VARIANT VTS_VARIANT)
	DISP_PROPERTY_PARAM_ID(CBoxSysInfo, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxSysInfo, "Contents", get_Contents, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CBoxSysInfo, "_NewEnum", DISPID_NEWENUM, get__NewEnum, SetNotSupported, VT_UNKNOWN)

	DISP_PROPERTY_EX(CBoxSysInfo, "ProgName", get_ProgName, put_ProgName, VT_BSTR)
END_DISPATCH_MAP()

// CBoxSysInfo message handlers

void CBoxSysInfo::SetLicense(LPCTSTR strNIC, LPCTSTR strHD, LPCTSTR strLicTime, LPCTSTR strExpTime, VARIANT& varLic, LPCTSTR strWkTime, VARIANT& varWkPubKey, VARIANT& varWkLic)
{
	if(!_tcsncmp(strNIC, _T("NIC_"), 4))
		m_strNIC = strNIC;

	if(!_tcsncmp(strHD, _T("IDE_"), 4) || !_tcsncmp(strHD, _T("SCSI_"), 4))
		m_strHD = strHD;

	m_strLicTime = strLicTime;
	m_strExpTime = strExpTime;
	m_aLic = varLic;

	m_strWkTime = strWkTime;
	m_aWkPubKey = varWkPubKey;

	m_aWkLic = varWkLic;
}

LPDISPATCH CBoxSysInfo::get_Contents(void)
{
	determineSYSInfo();
	m_pContents->ExternalAddRef();
	return m_pContents;
}

VARIANT CBoxSysInfo::get_Item(VARIANT& varKey)
{
	determineSYSInfo();
	return m_pContents->get_Item(varKey);
}

LPUNKNOWN CBoxSysInfo::get__NewEnum(void)
{
	determineSYSInfo();
	return m_pContents->get__NewEnum();
}

BSTR CBoxSysInfo::get_ProgName(void)
{
	CComVariant var;

	if(m_pContents->GetValue(_T("PROG_Name"), var) && var.vt == VT_BSTR)
		return ::SysAllocString(var.bstrVal);

	return ::SysAllocString(L"");
}

void CBoxSysInfo::put_ProgName(LPCTSTR pstrName)
{
	CComVariant var;

	if(m_pContents->GetValue(_T("PROG_Name"), var) && var.vt == VT_BSTR && SysStringByteLen(var.bstrVal))
		return;

	m_pContents->SetValue(_T("PROG_Name"), pstrName);
}
