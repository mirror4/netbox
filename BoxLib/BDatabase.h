#pragma once

#include "BThread.h"

class __declspec(uuid("94650000-0000-0000-4461-746162617365"))
CBDatabase : public CBDispatch<IDatabase>
{
	DECLARE_CLASS(CBDatabase)

public:
	CBDatabase(void);
	~CBDatabase(void);

public:
	// IDatabase
	STDMETHOD(Open)(BSTR Path, short Sync = 2);
	STDMETHOD(Close)(void);
	STDMETHOD(Execute)(BSTR strCommand, long start, long len, IRecordset **ppRS);
	STDMETHOD(GetTable)(BSTR strTable, IRecordset **ppRS);
	STDMETHOD(BeginTrans)(void);
	STDMETHOD(CommitTrans)(void);
	STDMETHOD(RollbackTrans)(void);
	STDMETHOD(Shrink)(void);
	STDMETHOD(get_CommandTimeout)(short *pVal);
	STDMETHOD(put_CommandTimeout)(short pVal);
	STDMETHOD(get_Synchronous)(short *pVal);
	STDMETHOD(put_Synchronous)(short pVal);
	STDMETHOD(get_TotalReadTimes)(long *pVal);
	STDMETHOD(get_TotalReadBytes)(double *pVal);
	STDMETHOD(get_TotalWriteTimes)(long *pVal);
	STDMETHOD(get_TotalWriteBytes)(double *pVal);
	STDMETHOD(get_TotalExecTime)(double *pVal);

public:
	HRESULT Execute(LPCSTR strCommand, long start = 0, long len = -1, IRecordset **ppRS = NULL);

private:
	CBCriticalSection m_cs;
	void* m_pDB;
	short m_nCmdTimeout;
	__int64 m_nStart;

	void Enter();
	void Leave();

public:
	long m_nTotalReadTimes;
	double m_nTotalReadBytes;
	long m_nTotalWriteTimes;
	double m_nTotalWriteBytes;
	__int64 m_nTotalExecTime;
};

extern VARIANT s_varNULL;

