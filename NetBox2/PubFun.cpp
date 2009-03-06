#include "stdafx.h"

void TimeToString(const SYSTEMTIME& st, CString &strTime)
{
	static TCHAR szMonth[][4] =
	{
	    _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"),
		_T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec")
	};

	static TCHAR szDays[][4] =
	{
	    _T("Sun"), _T("Mon"), _T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat")
	};

	strTime.Format(_T("%s, %02d %s %d %02d:%02d:%02d GMT"),
		szDays[st.wDayOfWeek], st.wDay, szMonth[st.wMonth-1], st.wYear,
		st.wHour, st.wMinute, st.wSecond);
}

