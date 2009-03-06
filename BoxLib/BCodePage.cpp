#include "StdAfx.h"
#include "BCodepage.h"

static __declspec(thread) UINT th_nCodePage;

static class cInitCodepage
{
public:
	cInitCodepage(void)
	{
		g_pfnGetThreadACP = staticGetThreadACP;
	}

	static UINT WINAPI staticGetThreadACP() throw()
	{
		return th_nCodePage;
	}
}___cInitCodepage;

CBCodePage::CBCodePage(UINT uiNowCP) : m_nCodePage(th_nCodePage)
{
	if(uiNowCP)th_nCodePage = uiNowCP;
}

CBCodePage::~CBCodePage(void)
{
	th_nCodePage = m_nCodePage;
}

BOOL CBCodePage::IsDBCSLeadByte(BYTE TestChar)
{
	return IsDBCSLeadByteEx(th_nCodePage, TestChar);
}

void CBCodePage::setCodePage(UINT nCodePage)
{
	if(nCodePage)th_nCodePage = nCodePage;
}

static int cpcompare(char **arg1, char **arg2)
{
	return _strcmpi(*arg1, *arg2);
}

UINT CBCodePage::CodePageFromCharSet(LPCSTR strCharSet)
{
	static struct
	{
		LPCSTR	strName;
		UINT	uiCodePage;
		UINT	uiCodePageEx;
	}s_cps[] =
	{
		{"_autodetect", 0xc6f4, 0xc6f4},
		{"_autodetect_all", 0xc351, 0xc351},
		{"_autodetect_kr", 0xc705, 0xc705},
		{"_iso-2022-jp$ESC", 0x3a4, 0xc42d},
		{"_iso-2022-jp$SIO", 0x3a4, 0xc42e},
		{"ANSI_X3.4-1968", 0x4e4, 0x4e4},
		{"ANSI_X3.4-1986", 0x4e4, 0x4e4},
		{"arabic", 0x4e8, 0x6fb4},
		{"ascii", 0x4e4, 0x4e4},
		{"ASMO-708", 0x4e8, 0x2c4},
		{"Big5", 0x3b6, 0x3b6},
		{"chinese", 0x3a8, 0x3a8},
		{"CN-GB", 0x3a8, 0x3a8},
		{"cp1256", 0x4e8, 0x4e8},
		{"cp367", 0x4e4, 0x4e4},
		{"cp819", 0x4e4, 0x4e4},
		{"cp852", 0x354, 0x354},
		{"cp866", 0x362, 0x362},
		{"csASCII", 0x4e4, 0x4e4},
		{"csbig5", 0x3b6, 0x3b6},
		{"csEUCKR", 0x3b5, 0x3b5},
		{"csEUCPkdFmtJapanese", 0x3a4, 0xcadc},
		{"csGB2312", 0x3a8, 0x3a8},
		{"csISO2022JP", 0x3a4, 0xc42d},
		{"csISO2022KR", 0x3b5, 0xc431},
		{"csISO58GB231280", 0x3a8, 0x3a8},
		{"csISOLatin1", 0x4e4, 0x4e4},
		{"csISOLatin2", 0x4e2, 0x6fb0},
		{"csISOLatin4", 0x4e9, 0x6fb2},
		{"csISOLatin5", 0x4e6, 0x4e6},
		{"csISOLatinArabic", 0x4e8, 0x6fb4},
		{"csISOLatinCyrillic", 0x4e3, 0x6fb3},
		{"csISOLatinGreek", 0x4e5, 0x6fb5},
		{"csISOLatinHebrew", 0x4e7, 0x6fb6},
		{"csKOI8R", 0x4e3, 0x5182},
		{"csKSC56011987", 0x3b5, 0x3b5},
		{"csShiftJIS", 0x3a4, 0x3a4},
		{"csUnicode11UTF7", 0x4b0, 0xfde8},
		{"csWindows31J", 0x3a4, 0x3a4},
		{"cyrillic", 0x4e3, 0x6fb3},
		{"DOS-720", 0x4e8, 0x2d0},
		{"DOS-862", 0x4e7, 0x35e},
		{"DOS-874", 0x36a, 0x36a},
		{"ECMA-114", 0x4e8, 0x6fb4},
		{"ECMA-118", 0x4e5, 0x6fb5},
		{"ELOT_928", 0x4e5, 0x6fb5},
		{"euc-jp", 0x3a4, 0xcadc},
		{"euc-kr", 0x3b5, 0x3b5},
		{"Extended_UNIX_Code_Packed_Format_for_Japanese", 0x3a4, 0xcadc},
		{"GB2312", 0x3a8, 0x3a8},
		{"GB_2312-80", 0x3a8, 0x3a8},
		{"GBK", 0x3a8, 0x3a8},
		{"greek", 0x4e5, 0x6fb5},
		{"greek8", 0x4e5, 0x6fb5},
		{"hebrew", 0x4e7, 0x6fb6},
		{"hz-gb-2312", 0x3a8, 0xcec8},
		{"IBM367", 0x4e4, 0x4e4},
		{"ibm819", 0x4e4, 0x4e4},
		{"ibm852", 0x354, 0x354},
		{"ibm866", 0x362, 0x362},
		{"iso-2022-jp", 0x3a4, 0xc42c},
		{"iso-2022-kr", 0x3b5, 0xc431},
		{"iso-8859-1", 0x4e4, 0x4e4},
		{"iso-8859-11", 0x36a, 0x36a},
		{"iso-8859-2", 0x4e2, 0x6fb0},
		{"iso-8859-3", 0x4e6, 0x6fb1},
		{"iso-8859-4", 0x4e9, 0x6fb2},
		{"iso-8859-5", 0x4e3, 0x6fb3},
		{"iso-8859-6", 0x4e8, 0x6fb4},
		{"iso-8859-7", 0x4e5, 0x6fb5},
		{"iso-8859-8", 0x4e7, 0x6fb6},
		{"ISO-8859-8 Visual", 0x4e7, 0x6fb6},
		{"iso-8859-8-i", 0x4e7, 0x96c6},
		{"iso-8859-9", 0x4e6, 0x4e6},
		{"iso-ir-100", 0x4e4, 0x4e4},
		{"iso-ir-101", 0x4e2, 0x6fb0},
		{"iso-ir-110", 0x4e9, 0x6fb2},
		{"iso-ir-111", 0x4e9, 0x6fb2},
		{"iso-ir-126", 0x4e5, 0x6fb5},
		{"iso-ir-127", 0x4e8, 0x6fb4},
		{"iso-ir-138", 0x4e7, 0x6fb6},
		{"iso-ir-144", 0x4e3, 0x6fb3},
		{"iso-ir-148", 0x4e6, 0x4e6},
		{"iso-ir-149", 0x3b5, 0x3b5},
		{"iso-ir-58", 0x3a8, 0x3a8},
		{"iso-ir-6", 0x4e4, 0x4e4},
		{"ISO646-US", 0x4e4, 0x4e4},
		{"iso8859-1", 0x4e4, 0x4e4},
		{"iso8859-2", 0x4e2, 0x6fb0},
		{"ISO_646.irv:1991", 0x4e4, 0x4e4},
		{"iso_8859-1", 0x4e4, 0x4e4},
		{"iso_8859-1:1987", 0x4e4, 0x4e4},
		{"iso_8859-2", 0x4e2, 0x6fb0},
		{"iso_8859-2:1987", 0x4e2, 0x6fb0},
		{"ISO_8859-4", 0x4e9, 0x6fb2},
		{"ISO_8859-4:1988", 0x4e9, 0x6fb2},
		{"ISO_8859-5", 0x4e3, 0x6fb3},
		{"ISO_8859-5:1988", 0x4e3, 0x6fb3},
		{"ISO_8859-6", 0x4e8, 0x6fb4},
		{"ISO_8859-6:1987", 0x4e8, 0x6fb4},
		{"ISO_8859-7", 0x4e5, 0x6fb5},
		{"ISO_8859-7:1987", 0x4e5, 0x6fb5},
		{"ISO_8859-8", 0x4e7, 0x6fb6},
		{"ISO_8859-8:1988", 0x4e7, 0x6fb6},
		{"ISO_8859-9", 0x4e6, 0x4e6},
		{"ISO_8859-9:1989", 0x4e6, 0x4e6},
		{"koi", 0x4e3, 0x5182},
		{"koi8-r", 0x4e3, 0x5182},
		{"koi8-ru", 0x4e3, 0x556a},
		{"korean", 0x3b5, 0x3b5},
		{"ks_c_5601", 0x3b5, 0x3b5},
		{"ks_c_5601-1987", 0x3b5, 0x3b5},
		{"ks_c_5601-1989", 0x3b5, 0x3b5},
		{"KSC5601", 0x3b5, 0x3b5},
		{"KSC_5601", 0x3b5, 0x3b5},
		{"l1", 0x4e4, 0x4e4},
		{"l2", 0x4e2, 0x6fb0},
		{"l4", 0x4e9, 0x6fb2},
		{"l5", 0x4e6, 0x4e6},
		{"latin1", 0x4e4, 0x4e4},
		{"latin2", 0x4e2, 0x6fb0},
		{"latin4", 0x4e9, 0x6fb2},
		{"latin5", 0x4e6, 0x4e6},
		{"logical", 0x4e7, 0x4e7},
		{"ms_Kanji", 0x3a4, 0x3a4},
		{"shift-jis", 0x3a4, 0x3a4},
		{"shift_jis", 0x3a4, 0x3a4},
		{"unicode", 0x4b0, 0x4b0},
		{"unicode-1-1-utf-7", 0x4b0, 0xfde8},
		{"unicode-1-1-utf-8", 0x4b0, 0xfde9},
		{"unicode-2-0-utf-8", 0x4b0, 0xfde9},
		{"unicodeFFFE", 0x4b0, 0x4b1},
		{"us", 0x4e4, 0x4e4},
		{"us-ascii", 0x4e4, 0x4e4},
		{"utf-7", 0x4b0, 0xfde8},
		{"utf-8", 0x4b0, 0xfde9},
		{"visual", 0x4e7, 0x6fb6},
		{"windows-1250", 0x4e2, 0x4e2},
		{"windows-1251", 0x4e3, 0x4e3},
		{"windows-1252", 0x4e4, 0x4e4},
		{"windows-1253", 0x4e5, 0x4e5},
		{"Windows-1254", 0x4e6, 0x4e6},
		{"windows-1255", 0x4e7, 0x4e7},
		{"windows-1256", 0x4e8, 0x4e8},
		{"windows-1257", 0x4e9, 0x4e9},
		{"windows-1258", 0x4ea, 0x4ea},
		{"windows-874", 0x36a, 0x36a},
		{"x-ansi", 0x4e4, 0x4e4},
		{"x-cp1250", 0x4e2, 0x4e2},
		{"x-cp1251", 0x4e3, 0x4e3},
		{"x-euc", 0x3a4, 0xcadc},
		{"x-euc-jp", 0x3a4, 0xcadc},
		{"x-ms-cp932", 0x3a4, 0x3a4},
		{"x-sjis", 0x3a4, 0x3a4},
		{"x-unicode-2-0-utf-7", 0x4b0, 0xfde8},
		{"x-unicode-2-0-utf-8", 0x4b0, 0xfde9},
		{"x-user-defined", 0xc350, 0xc350},
		{"x-x-big5", 0x3b6, 0x3b6}
	};

	char ** result = (char **)bsearch((const void *)&strCharSet, (const void *)&s_cps, sizeof(s_cps) / sizeof(s_cps[0]),
						sizeof(s_cps[0]), (int (*)(const void*, const void*))cpcompare );

	if(result == NULL)
		return 0;

	if(result[1] != result[2])
	{
		CPINFO cpi;

		if(GetCPInfo((UINT)result[2], &cpi))
			result[1] = result[2];
		else result[2] = result[1];
	}

	return (UINT)result[1];
}

