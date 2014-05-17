#pragma once

#include "BoxLib.h"
#include <time.h>

class CBDate
{
public:
	CBDate() : m_d(0)
	{}

	CBDate(CBDate& d) : m_d(d.m_d)
	{}

	CBDate(DATE d) : m_d(d)
	{}

	CBDate(SYSTEMTIME& d)
	{
		operator=(d);
	}

	CBDate(FILETIME& d)
	{
		operator=(d);
	}

	CBDate(time_t d)
	{
		operator=(d);
	}

/*	CBDate(__time64_t d)
	{
		operator=(d);
	}
*/
	operator DATE() const throw()
	{
		return m_d;
	}

	operator SYSTEMTIME() const throw()
	{
		SYSTEMTIME d;

		VariantTimeToSystemTime(m_d, &d);
		return d;
	}

	operator FILETIME() const throw()
	{
		FILETIME d;
		SYSTEMTIME st;

		st = *this;
		SystemTimeToFileTime(&st, &d);

		return d;
	}

	operator bool() const throw()
	{
		return m_d != 0;
	}

	operator time_t() const throw()
	{
		return (time_t)(m_d * 86400) - 2209161600;
	}

/*	operator __time64_t() const throw()
	{
		return (__time64_t)(m_d * 86400) - 2209161600;
	}
*/
	DATE* operator&() throw()
	{
		return &m_d;
	}

	operator CBString() const throw()
	{
		static WCHAR szMonth[][4] =
		{
			L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
			L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"
		};
		static WCHAR szDays[][4] =
		{
			L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"
		};

		CBString strTime;
		SYSTEMTIME d;

		d = (SYSTEMTIME)*this;
		strTime.Format(L"%s, %02d %s %d %02d:%02d:%02d GMT",
			szDays[d.wDayOfWeek], d.wDay, szMonth[d.wMonth-1], d.wYear,
			d.wHour, d.wMinute, d.wSecond);

		return strTime;
	}

	operator CBStringA() const throw()
	{
		static char szMonth[][4] =
		{
			"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
		};
		static char szDays[][4] =
		{
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
		};

		CBStringA strTime;
		SYSTEMTIME d;

		d = (SYSTEMTIME)*this;
		strTime.Format("%s, %02d %s %d %02d:%02d:%02d GMT",
			szDays[d.wDayOfWeek], d.wDay, szMonth[d.wMonth-1], d.wYear,
			d.wHour, d.wMinute, d.wSecond);

		return strTime;
	}

	DATE operator=(DATE d) throw()
	{
		return m_d = d;
	}

	DATE operator=(CBDate& d) throw()
	{
		return m_d = d.m_d;
	}

	DATE operator=(SYSTEMTIME& d) throw()
	{
		SystemTimeToVariantTime(&d, &m_d);
		return m_d;
	}

	DATE operator=(FILETIME& d) throw()
	{
		SYSTEMTIME st;

		FileTimeToSystemTime(&d, &st);

		return operator=(st);
	}

	DATE operator=(time_t d) throw()
	{
		m_d = (d + 2209161600) / 86400.0;
		return m_d;
	}

/*	DATE operator=(__time64_t d) throw()
	{
		m_d = (d + 2209161600) / 86400.0;
		return m_d;
	}
*/
	DATE operator=(const CBStringA& date) throw()
	{
		return operator=((LPCSTR)date);
	}

	DATE operator=(const CBString& date) throw()
	{
		return operator=((LPCWSTR)date);
	}

	template <typename T>
	DATE operator=(const T* date) throw()
	{
		SYSTEMTIME ds={0};
		int mint, mon;
		const T *monstr, *timstr;
		static const int months[12] =
		{
		('J' << 16) | ('a' << 8) | 'n', ('F' << 16) | ('e' << 8) | 'b',
		('M' << 16) | ('a' << 8) | 'r', ('A' << 16) | ('p' << 8) | 'r',
		('M' << 16) | ('a' << 8) | 'y', ('J' << 16) | ('u' << 8) | 'n',
		('J' << 16) | ('u' << 8) | 'l', ('A' << 16) | ('u' << 8) | 'g',
		('S' << 16) | ('e' << 8) | 'p', ('O' << 16) | ('c' << 8) | 't',
		('N' << 16) | ('o' << 8) | 'v', ('D' << 16) | ('e' << 8) | 'c'};

		if (!date)return m_d;

		while (*date && isspace(*date))
			++date;

		while (*date && !isspace(*date))
			++date;

		if (*date == '\0')return m_d;

		++date;

		if (checkmask(date, "## @$$ #### ##:##:## *"))	/* RFC 1123 format */
		{
			ds.wYear = (WORD)(((date[7] & 0xf) * 10 + (date[8] & 0xf) - 19) * 100 +
					((date[9] & 0xf) * 10) + (date[10] & 0xf));

			ds.wDay = (WORD)(((date[0] & 0xf) * 10) + (date[1] & 0xf));

			monstr = date + 3;
			timstr = date + 12;
		}else if (checkmask(date, "##-@$$-## ##:##:## *"))		/* RFC 850 format  */
		{
			ds.wYear = (WORD)(((date[7] & 0xf) * 10) + (date[8] & 0xf));
			if (ds.wYear < 70)
				ds.wYear += 100;

			ds.wDay = (WORD)(((date[0] & 0xf) * 10) + (date[1] & 0xf));

			monstr = date + 3;
			timstr = date + 10;
		}else if (checkmask(date, "@$$ ~# ##:##:## ####*"))	/* asctime format  */
		{
			ds.wYear = (WORD)(((date[16] & 0xf) * 10 + (date[17] & 0xf) - 19) * 100 +
						((date[18] & 0xf) * 10) + (date[19] & 0xf));

			ds.wDay = (WORD)((date[4] & 0xF) * 10 + (date[5] & 0xf));

			monstr = date;
			timstr = date + 7;
		}else return m_d;
		if (ds.wDay <= 0 || ds.wDay > 31)return m_d;

		ds.wHour = (WORD)(((timstr[0] & 0xf) * 10) + (timstr[1] & 0xf));
		ds.wMinute = (WORD)(((timstr[3] & 0xf) * 10) + (timstr[4] & 0xf));
		ds.wSecond = (WORD)(((timstr[6] & 0xf) * 10) + (timstr[7] & 0xf));

		if ((ds.wHour > 23) || (ds.wMinute > 59) || (ds.wSecond > 61))
			return m_d;

		mint = (monstr[0] << 16) | (monstr[1] << 8) | monstr[2];
		for (mon = 0; mon < 12; mon++)
			if (mint == months[mon])
				break;

		if (mon == 12)return m_d;

		if ((ds.wDay == 31) && (mon == 3 || mon == 5 || mon == 8 || mon == 10))
			return m_d;

		/* February gets special check for leapyear */

		if ((mon == 1) && ((ds.wDay > 29) || ((ds.wDay == 29) && ((ds.wYear & 3) ||
			(((ds.wYear % 100) == 0) && (((ds.wYear % 400) != 100)))))))
			return m_d;

		ds.wMonth = (WORD)(mon + 1);

		ds.wYear += 1900;

		return operator=(ds);
	}

	bool operator==(CBDate& d) const throw()
	{
		return m_d == d.m_d;
	}

	bool operator!=(CBDate& d) const throw()
	{
		return m_d != d.m_d;
	}

	bool operator<(CBDate& d) const throw()
	{
		return m_d < d.m_d;
	}

	bool operator>(CBDate& d) const throw()
	{
		return m_d > d.m_d;
	}

	bool operator<=(CBDate& d) const throw()
	{
		return m_d <= d.m_d;
	}

	bool operator>=(CBDate& d) const throw()
	{
		return m_d >= d.m_d;
	}

	bool operator==(DATE d) const throw()
	{
		return m_d == d;
	}

	bool operator!=(DATE d) const throw()
	{
		return m_d != d;
	}

	bool operator<(DATE d) const throw()
	{
		return m_d < d;
	}

	bool operator>(DATE d) const throw()
	{
		return m_d > d;
	}

	bool operator<=(DATE d) const throw()
	{
		return m_d <= d;
	}

	bool operator>=(DATE d) const throw()
	{
		return m_d >= d;
	}

	template <typename T>
	bool operator==(T d) const throw()
	{
		return m_d == CBDate(d).m_d;
	}

	template <typename T>
	bool operator!=(T d) const throw()
	{
		return m_d != CBDate(d).m_d;
	}

	template <typename T>
	bool operator<(T d) const throw()
	{
		return m_d < CBDate(d).m_d;
	}

	template <typename T>
	bool operator>(T d) const throw()
	{
		return m_d > CBDate(d).m_d;
	}

	template <typename T>
	bool operator<=(T d) const throw()
	{
		return m_d <= CBDate(d).m_d;
	}

	template <typename T>
	bool operator>=(T d) const throw()
	{
		return m_d >= CBDate(d).m_d;
	}

	bool operator==(int d) const throw()
	{
		return m_d == d;
	}

	bool operator!=(int d) const throw()
	{
		return m_d != d;
	}

	void GetTime(void) throw()
	{
		SYSTEMTIME st;

		::GetSystemTime(&st);
		*this = st;
	}

	enum DateInterval
	{
		modeDay,
		modeHour,
		modeMinute,
		modeSecond
	};

	void Add(DateInterval m, int n) throw()
	{
		switch(m)
		{
		case modeDay: m_d += n; break;
		case modeHour: m_d += n / 24.0; break;
		case modeMinute: m_d += n / 1440.0; break;
		case modeSecond: m_d += n / 86400.0; break;
		}
	}

	int Diff(DateInterval m, DATE d) const throw()
	{
		switch(m)
		{
		case modeDay: return int(m_d - d);
		case modeHour: return int((m_d - d) * 24);
		case modeMinute: return int((m_d - d) * 1440);
		case modeSecond: return int((m_d - d) * 86400);
		}
		return 0;
	}

private:
	template <typename T>
	static int checkmask(T data, const char *mask)
	{
		int i, j;
		WCHAR d;

		for (i = j = 0; i < 256; i++)
		{
			d = data[j ++];
			switch (mask[i])
			{
				case '\0':
					return (d == '\0');

				case '*':
					return 1;

				case '@':
					if (!isupper(d))
						return 0;
					break;
				case '$':
					if (!islower(d))
						return 0;
					break;
				case '!':
					if (!isdigit(d))
						j --;
					break;
				case '#':
					if (!isdigit(d))
						return 0;
					break;
				case '&':
					if (!isxdigit(d))
						return 0;
					break;
				case '~':
					if ((d != ' ') && !isdigit(d))
						return 0;
					break;
				default:
					if (mask[i] != d)
						return 0;
					break;
			}
		}
		return 0;
	}

	DATE m_d;
};

class CBDateEx : public CBObject
{
public:
	static void Startup();
	static void Cleanup();
	static void GetDate(CBDateEx** pDate);
	static void GetFastDate(CBDateEx** pDate);

public:
	CBDate	m_date;
	CBString m_strGMT;
};

