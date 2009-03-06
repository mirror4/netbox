
#pragma once

#include "BoxContents.h"
#include "BoxArray.h"
#include "BoxTimeCache.h"

#include <snmp.h>

#include <openssl\rsa.h>
#include <openssl\md5.h>
#include <openssl\des.h>

typedef BOOL (WINAPI *pSnmpExtensionInit)(DWORD dwTimeZeroReference, HANDLE *hPollForTrapEvent, AsnObjectIdentifier *supportedView);
typedef BOOL (WINAPI *pSnmpExtensionQuery)(BYTE requestType, RFC1157VarBindList *variableBindings, AsnInteger *errorStatus, AsnInteger *errorIndex);

extern BYTE APP_MD5[MD5_DIGEST_LENGTH];
extern BYTE PROG_MD5[MD5_DIGEST_LENGTH];
#define MK_9465 0x94659465
extern DWORD g_dwAppMark;

// CBoxSysInfo command target

class CBoxSysInfo : public CBoxContents
{
	DECLARE_DYNAMIC(CBoxSysInfo)

public:
	CBoxSysInfo(void);
	~CBoxSysInfo(void);
	void determineSYSInfo(void);

	BOOL inline DoHash(BYTE *data_MD5)
	{
		MD5_CTX ctxMD5;
		CComVariant var;
		CString str;
		int i;

		MD5_Init(&ctxMD5);

		if(m_pContents->GetValue(_T("OS_Platform"), var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(m_pContents->GetValue(_T("CPU_Family"), var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(m_pContents->GetValue(_T("CPU_Number"), var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(!m_strNIC.IsEmpty() && m_pContents->GetValue(m_strNIC, var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(!m_strHD.IsEmpty() && m_pContents->GetValue(m_strHD, var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(m_pContents->GetValue(_T("APP_Info"), var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(m_pContents->GetValue(_T("PROG_Name"), var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));

		if(m_pContents->GetValue(_T("PROG_Info"), var) && var.vt == VT_BSTR)
			MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));
		else if(g_dwAppMark != MK_9465)
			return TRUE;

		i = 0;
		while(1)
		{
			str.Format(_T("LIB_%d"), i);
			if(m_pContents->GetValue(str, var) && var.vt == VT_BSTR)
				MD5_Update(&ctxMD5, var.bstrVal, SysStringByteLen(var.bstrVal));
			else break;

			i ++;
		}

		MD5_Update(&ctxMD5, (LPCWSTR)m_strLicTime, m_strLicTime.GetLength() * 2);
		MD5_Update(&ctxMD5, (LPCWSTR)m_strExpTime, m_strExpTime.GetLength() * 2);

		MD5_Final(data_MD5, &ctxMD5);

		return FALSE;
	}

	__int64 GetTimeFromStr(CStringW &str)
	{
		SYSTEMTIME st;
		__int64 tm;

		if(str.GetLength() != 19)
			return 0;

		ZeroMemory(&st, sizeof(st));

		LPCWSTR pstr = str;

		st.wYear = _wtoi(pstr);
		st.wMonth = _wtoi(pstr + 5);
		st.wDay = _wtoi(pstr + 8);
		st.wHour = _wtoi(pstr + 11);
		st.wMinute = _wtoi(pstr + 14);
		st.wSecond = _wtoi(pstr + 17);

		SystemTimeToFileTime(&st, (FILETIME*)&tm);

		return tm;
	}

	BOOL inline DoCheck(const BYTE *data_MD5, const BYTE *Sign, int nSizeSign, const BYTE *Key, int nSizeKey)
	{
		RSA *m_pRSA = NULL;

		m_pRSA = d2i_RSAPublicKey(&m_pRSA, &Key, nSizeKey);
		if(m_pRSA == NULL)
			return FALSE;

		BYTE WorkLic[272];
		int nSize = RSA_public_decrypt(nSizeSign, Sign, WorkLic, m_pRSA, 1);

		RSA_free(m_pRSA);

		if(nSize != MD5_DIGEST_LENGTH)
			return FALSE;

		for(int i = 0; i < MD5_DIGEST_LENGTH; i ++)
			if(data_MD5[i] != WorkLic[i])
				return FALSE;

		return TRUE;
	}

	BOOL inline DoWorkerCheck(void)
	{
		BYTE RootKey[272];

		extern unsigned char PublicKey[272];
		extern unsigned char DesKey[8];
		des_key_schedule desKey;
		des_cblock IV;

		ZeroMemory(IV, sizeof(IV));
		des_set_key((des_cblock*)&DesKey, desKey);
		des_ncbc_encrypt(PublicKey, RootKey, sizeof(PublicKey), desKey, &IV, DES_DECRYPT);

		BYTE data_MD5[MD5_DIGEST_LENGTH];
		MD5_CTX ctxMD5;

		MD5_Init(&ctxMD5);

		MD5_Update(&ctxMD5, m_aWkPubKey.m_pData, m_aWkPubKey.m_nSize);
		MD5_Update(&ctxMD5, (LPCWSTR)m_strWkTime, m_strWkTime.GetLength() * 2);

		MD5_Final(data_MD5, &ctxMD5);

		if(!DoCheck(data_MD5, m_aWkLic.m_pData, m_aWkLic.m_nSize, RootKey, sizeof(RootKey)))
			return FALSE;

		return TRUE;
	}

	BOOL inline DoVerify(void)
	{
		BYTE data_MD5[MD5_DIGEST_LENGTH];

		determineSYSInfo();

		if(DoHash(data_MD5))
			return TRUE;

		if(!DoWorkerCheck())
			return FALSE;

		if(!DoCheck(data_MD5, m_aLic.m_pData, m_aLic.m_nSize, m_aWkPubKey.m_pData, m_aWkPubKey.m_nSize))
			return FALSE;

		__int64 tmWorker = GetTimeFromStr(m_strWkTime);
		__int64 tmLic = GetTimeFromStr(m_strLicTime);

		if(tmLic < tmWorker - (__int64)600000000 * 60 * 24 || tmLic > tmWorker + (__int64)600000000 * 60 * 24 * 45)
			return FALSE;

		SYSTEMTIME st;
		__int64 ft;

		GetSystemTime(&st);
		SystemTimeToFileTime(&st, (FILETIME*)&ft);

		if(ft < tmLic - (__int64)600000000 * 60 * 24 || ft > tmLic + (__int64)600000000 * 60 * 24 * 455)
			return FALSE;

		if(!m_strExpTime.IsEmpty() && ft > GetTimeFromStr(m_strExpTime) + (__int64)600000000 * 60 * 24)
			return FALSE;

		return TRUE;
	}

public:
	afx_msg void SetLicense(LPCTSTR strNIC, LPCTSTR strHD, LPCTSTR strLicTime, LPCTSTR strExpTime, VARIANT& varLic, LPCTSTR strWkTime, VARIANT& varWkPubKey, VARIANT& varWkLic);

	afx_msg VARIANT get_Item(VARIANT& varKey);
	afx_msg LPDISPATCH get_Contents(void);

	afx_msg LPUNKNOWN get__NewEnum(void);

	afx_msg BSTR get_ProgName(void);
	afx_msg void put_ProgName(LPCTSTR pstrName);

	DECLARE_DISPATCH_MAP()

private:
	void determineOSInfo(void);
	void determineRAMInfo(void);
	void determineUSERInfo(void);
	void determineScreenInfo(void);
	void determineCpuInfo(void);
	void determineNETInfo(void);
	void determineAPPInfo(void);
	void determineHDInfo(void);

	CString m_strNIC;
	CString m_strHD;

	CStringW m_strLicTime;
	CStringW m_strExpTime;
	CBoxArray m_aLic;

	CStringW m_strWkTime;
	CBoxArray m_aWkPubKey;

	CBoxArray m_aWkLic;

	HINSTANCE				m_hInst;
	pSnmpExtensionQuery		m_pQuery;
};
