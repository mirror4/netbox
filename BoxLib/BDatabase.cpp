#include "StdAfx.h"
#include "BDatabase.h"
#include "BRecordset.h"
#include "BCodepage.h"
#include "Sqlite\sqlite3.h"

extern "C" __declspec(thread) __int64 th_iCommandTimeout = 0;
static __declspec(thread) CBDatabase* th_NowRunDB;

extern "C" void db_ioCount(BOOL bRead, int bytes)
{
	CBDatabase* pDB = th_NowRunDB;

	if(pDB)
	{
		if(bRead)
		{
			pDB->m_nTotalReadTimes ++;
			pDB->m_nTotalReadBytes += bytes;
		}else
		{
			pDB->m_nTotalWriteTimes ++;
			pDB->m_nTotalWriteBytes += bytes;
		}
	}
}

static __int64 s_nFreq;

CBDatabase::CBDatabase(void) : m_pDB(NULL)
{
	if(s_nFreq == 0)
		QueryPerformanceFrequency((LARGE_INTEGER*)&s_nFreq);

	Close();
}

CBDatabase::~CBDatabase(void)
{
	Close();
}

void CBDatabase::Enter()
{
	m_cs.Enter();
	th_NowRunDB = this;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_nStart);
}

void CBDatabase::Leave()
{
	__int64 n;

	QueryPerformanceCounter((LARGE_INTEGER*)&n);
	m_nTotalExecTime += n - m_nStart;
	th_NowRunDB = NULL;
	m_cs.Leave();
}

STDMETHODIMP CBDatabase::Open(BSTR Path, short Sync)
{
	CBStringA strPath(Path, ::SysStringLen(Path));
	int rc = SQLITE_OK;
	HRESULT hr = S_OK;

	Enter();

	if(m_pDB == NULL)
	{
		rc = sqlite3_open(strPath, (sqlite3**)&m_pDB);

		if( rc!=SQLITE_OK )
			hr = SetErrorInfo(sqlite3_errmsg((sqlite3*)m_pDB));
	}else hr = E_NOTIMPL;

	Leave();

	if(SUCCEEDED(hr) && Sync != 2)
		return put_Synchronous(Sync);

	return hr;
}

STDMETHODIMP CBDatabase::Close(void)
{
	Enter();

	if(m_pDB)sqlite3_close((sqlite3*)m_pDB);
	m_pDB = NULL;

	m_nCmdTimeout = 5000;

	m_nTotalReadTimes = 0;
	m_nTotalReadBytes = 0;
	m_nTotalWriteTimes = 0;
	m_nTotalWriteBytes = 0;
	m_nTotalExecTime = 0;

	Leave();

	return S_OK;
}

VARIANT s_varNULL = {VT_NULL};

HRESULT CBDatabase::Execute(LPCSTR strCommand, long start, long len, IRecordset **ppRS)
{
	if(strCommand == NULL || *strCommand == 0)
		return S_OK;

	CBComPtr<CBRecordset> pRS;
	CBComPtr<CBRecordset> pRSResult;
	int rc = SQLITE_OK;
	const char *zLeftover;
	sqlite3* db = (sqlite3*)m_pDB;
	sqlite3_stmt *pStmt = 0;

	int nRetry = 0;

	long nReadTimes;
	double nReadBytes;
	long nWriteTimes;
	double nWriteBytes;
	__int64 nExecTime;
	long startPos, endPos, RSCount;
	HRESULT hr = S_OK;
	__int64 n;
	int i;

	Enter();

	if(m_pDB == NULL)
	{
		Leave();
		return E_NOTIMPL;
	}

	while(SUCCEEDED(hr) && (rc == SQLITE_OK || (rc == SQLITE_SCHEMA && (++nRetry) < 2)) && strCommand[0])
	{
		int nCol;

		nReadTimes = m_nTotalReadTimes;
		nReadBytes = m_nTotalReadBytes;
		nWriteTimes = m_nTotalWriteTimes;
		nWriteBytes = m_nTotalWriteBytes;
		nExecTime = m_nTotalExecTime;

		th_iCommandTimeout = m_nCmdTimeout;
		pStmt = 0;
		rc = sqlite3_prepare(db, strCommand, -1, &pStmt, &zLeftover);

		if( rc != SQLITE_OK )
		{
			if(pStmt)
				sqlite3_finalize(pStmt);
			continue;
		}

		if(!pStmt)
		{
			strCommand = zLeftover;
			continue;
		}

		rc = sqlite3_step(pStmt);

		if(ppRS != NULL)
		{
			if(pRS == NULL)
			{
				pRS.CreateInstance();
				pRSResult = pRS;
			}else
			{
				pRS->m_pNextRS.CreateInstance();
				pRS = pRS->m_pNextRS;
			}

			if(nCol = sqlite3_column_count(pStmt))
				for(i = 0; i < nCol; i ++)
					pRS->AppendField(CBString((char *)sqlite3_column_name(pStmt, i)));

			QueryPerformanceCounter((LARGE_INTEGER*)&n);
			m_nTotalExecTime += n - m_nStart;
			m_nStart = n;

			startPos = start;
			if(len == -1)endPos = -1;
			else endPos = len + start;
			RSCount = 0;

			while(rc == SQLITE_ROW)
			{
				if(RSCount >= startPos && (endPos == -1 || RSCount < endPos))
					for(i = 0; i < nCol; i ++)
						switch(sqlite3_column_type(pStmt, i))
						{
						case 1:
							pRS->AddValue(sqlite3_column_int(pStmt, i));
							break;
						case 2:
							pRS->AddValue(sqlite3_column_double(pStmt, i));
							break;
						case 3:
							pRS->AddValue((char*)sqlite3_column_text(pStmt, i));
							break;
						case 4:
							hr = pRS->AddBlob(sqlite3_column_blob(pStmt, i), sqlite3_column_bytes(pStmt, i));
							if(FAILED(hr))
							{
								ppRS = NULL;
								rc = SQLITE_OK;
							}
							break;
						case 5:
							pRS->AddValue(s_varNULL);
							break;
						}

				RSCount ++;

				rc = sqlite3_step(pStmt);
			}

			start = 0;
			len = -1;

			if(startPos > RSCount)startPos = RSCount;

			pRS->m_nFirstPosition = startPos;
			pRS->m_nResultCount = RSCount;
			pRS->m_nReadTimes = m_nTotalReadTimes - nReadTimes;
			pRS->m_nReadBytes = m_nTotalReadBytes - nReadBytes;
			pRS->m_nWriteTimes = m_nTotalWriteTimes - nWriteTimes;
			pRS->m_nWriteBytes = m_nTotalWriteBytes - nWriteBytes;
			pRS->m_nExecTime = (m_nTotalExecTime - nExecTime) * 1000 / s_nFreq;
		}

		rc = sqlite3_finalize(pStmt);
		pStmt = 0;

		if(rc != SQLITE_SCHEMA)
		{
			nRetry = 0;
			strCommand = zLeftover;
			while(isspace((unsigned char)strCommand[0]))
				strCommand++;
		}
	}

	if(pStmt)
		sqlite3_finalize(pStmt);

	if(rc != SQLITE_OK && rc == sqlite3_errcode(db))
		hr = SetErrorInfo(sqlite3_errmsg(db));

	Leave();

	if(hr == S_OK && pRSResult != NULL && ppRS != NULL)
		pRSResult.QueryInterface(ppRS);

	return hr;
}

STDMETHODIMP CBDatabase::Execute(BSTR strCommand, long start, long len, IRecordset **ppRS)
{
	if(start < 0 || len < -1)
		return E_INVALIDARG;

	CBCodePage cp(CP_UTF8);
	CBStringA strSql(strCommand, ::SysStringLen(strCommand));

	return Execute(strSql, start, len, ppRS);
}

STDMETHODIMP CBDatabase::GetTable(BSTR strTable, IRecordset **ppRS)
{
	CBCodePage cp(CP_UTF8);
	CBStringA strSql(strTable, ::SysStringLen(strTable));
	HRESULT hr;
	CBComPtr<CBRecordset> pRS;

	if(strSql.Find(']') != -1)
		return E_INVALIDARG;

	hr = Execute("select * from [" + strSql + "] where 1=0", 0, -1, ppRS);
	if(FAILED(hr))return hr;

	pRS = *ppRS;

	if(pRS)
	{
		pRS->m_pDatabase = this;
		pRS->m_strTable.SetString(strTable, ::SysStringLen(strTable));
	}

	return S_OK;
}

STDMETHODIMP CBDatabase::BeginTrans(void)
{
	return Execute("BEGIN");
}

STDMETHODIMP CBDatabase::CommitTrans(void)
{
	return Execute("COMMIT");
}

STDMETHODIMP CBDatabase::RollbackTrans(void)
{
	return Execute("ROLLBACK");
}

STDMETHODIMP CBDatabase::Shrink(void)
{
	return Execute("VACUUM");
}

STDMETHODIMP CBDatabase::get_CommandTimeout(short *pVal)
{
	m_cs.Enter();
	*pVal = m_nCmdTimeout;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDatabase::put_CommandTimeout(short pVal)
{
	m_cs.Enter();
	m_nCmdTimeout = pVal;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDatabase::get_Synchronous(short *pVal)
{
	HRESULT hr;
	CBComPtr<IRecordset> pRS;
	VARIANT var, var1 = {VT_EMPTY};

	hr = Execute("PRAGMA synchronous", 0, -1, &pRS);
	if(FAILED(hr))return hr;

	var.vt = VT_I4;
	var.lVal = 0;

	hr = pRS->get_Value(var, &var1);
	if(FAILED(hr))return hr;

	*pVal = var1.cVal;

	return S_OK;
}

STDMETHODIMP CBDatabase::put_Synchronous(short pVal)
{
	switch(pVal)
	{
	case 0:return Execute("PRAGMA synchronous = OFF");
	case 1:return Execute("PRAGMA synchronous = NORMAL");
	case 2:return Execute("PRAGMA synchronous = FULL");
	}

	return E_INVALIDARG;
}

STDMETHODIMP CBDatabase::get_TotalReadTimes(long *pVal)
{
	m_cs.Enter();
	*pVal = m_nTotalReadTimes;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDatabase::get_TotalReadBytes(double *pVal)
{
	m_cs.Enter();
	*pVal = m_nTotalReadBytes;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDatabase::get_TotalWriteTimes(long *pVal)
{
	m_cs.Enter();
	*pVal = m_nTotalWriteTimes;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDatabase::get_TotalWriteBytes(double *pVal)
{
	m_cs.Enter();
	*pVal = m_nTotalWriteBytes;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDatabase::get_TotalExecTime(double *pVal)
{
	m_cs.Enter();
	*pVal = ((m_nTotalExecTime * 1000) / s_nFreq) / 1000.0;
	m_cs.Leave();

	return S_OK;
}

