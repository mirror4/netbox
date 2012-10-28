#pragma once
#include "BoxTcpServer.h"
#include "BoxDictionary.h"

class CBoxHttpServer : public CBoxTcpServer
{
	DECLARE_DYNCREATE(CBoxHttpServer)
public:
	CBoxHttpServer(void);
	virtual ~CBoxHttpServer(void);

	virtual void OnSleepTimer(__int64 nNowTime);
	virtual CBoxJobWorker* CreateNewWorker(void);

	void SendFile(CBoxSocket* pSocket, CFile* pFile, ULONGLONG ullStart, ULONGLONG ullLen, long retVal);

public:
	afx_msg long Create(LPCTSTR pstrSocketAddress, UINT nSocketPort);
	afx_msg LPDISPATCH AddHost(LPCTSTR pstrName, LPCTSTR pstrRoot);
	afx_msg void AttachHost(LPCTSTR pstrName, LPDISPATCH pdispHost);
	afx_msg void Start(void);
	afx_msg void Close(void);
	afx_msg long get_DownloadWorkers(void);
	afx_msg long get_totalBytes(void);

	DECLARE_DISPATCH_MAP()

public:
	long m_nDownloadWorkers;
	long m_ntotalBytes;
	long m_nMaxRequestEntity;

private:
	__int64 m_nLastCheckTime;

private:
	class CSendFile : public OVERLAPPED
	{
	public:
		~CSendFile(void)
		{
			InterlockedDecrement(&((CBoxHttpServer*)m_pSocket->m_pContext)->m_nDownloadWorkers);
			delete m_pFile;
		}

		CBoxObject<CBoxSocket> m_pSocket;
		CFile *m_pFile;
		ULONGLONG m_ullStart;
		ULONGLONG m_ullLen;
		long m_retVal;

		char *m_pBuf;
		char *buf;
		long sizeofbuf;
		long m_nSize;
		long m_nPos;
	};

	static void CALLBACK SendFile(ULONG_PTR dwParam);
	static VOID CALLBACK IoCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

	static UINT staticSendThread(void* p);
	void SendThread();
	static void CALLBACK StopSendThread(ULONG_PTR dwParam);

	CWinThread *m_pSendThread;
};
