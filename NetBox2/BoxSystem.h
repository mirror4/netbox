
#pragma once

#include <afxtempl.h>
#include "BoxObject.h"
#include <BDictionary.h>
#include <BSimpleDictionary.h>

#include <activscp.h>
#include "activdbg.h"

/*
#include <mscoree.h>
#import "mscorlib.tlb" raw_interfaces_only
*/

// CBoxSystem command target

class CBoxSystem : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxSystem)

public:
	CBoxSystem(void);
	~CBoxSystem(void);

	void AddClass(CRuntimeClass* pRuntimeClass, LPCTSTR pstrNamespace = NULL);
	void AddClass(LPCOLESTR strName, IDispatch *pDisp);

#define AddClassEx(__strName, __className) CBClassRegistry::RegisterClass(__strName, __uuidof(__className), __className::GetClassObject())

	static LPCTSTR getVersion(void);
	static int getVersionLen(void);

	void LoadScriptDebugger();

	CComPtr<IProcessDebugManager> m_pDebugManager;
	CComPtr<IDebugApplication> m_pDebugApp;
	DWORD m_dwCookie;

	void ClearLock(void);

public:
	afx_msg VARIANT get_Item(VARIANT& varKey);
	afx_msg void put_Item(VARIANT& varKey, VARIANT& var);
	afx_msg LPDISPATCH get_Contents(void);

	afx_msg LPUNKNOWN get__NewEnum(void);

	afx_msg LPDISPATCH get_Folders(void);
	afx_msg void AddFolder(LPCTSTR pstrPath, LPCTSTR pstrPathMapped);
	afx_msg void RemoveFolder(LPCTSTR pstrPath);

	afx_msg LPDISPATCH get_Encoding(void);
	afx_msg LPDISPATCH get_SysInfo(void);
	afx_msg LPDISPATCH get_FileSystem(void);

	afx_msg LPDISPATCH get_Scripts(void);

	afx_msg void RegisterClass(LPCTSTR pstrName, VARIANT& varMember);
	afx_msg LPDISPATCH CreateObject(LPCTSTR pstrObject);
	afx_msg LPDISPATCH LoadObject(LPCTSTR pstrFile, VARIANT& varAlone);

	afx_msg void LoadLibrary(LPCTSTR pstrFile, VARIANT& varRoot);

	afx_msg VARIANT get_Config(VARIANT& varSection, VARIANT& varKey);
	afx_msg void put_Config(VARIANT& varSection, VARIANT& varKey, VARIANT& varNewVal);

	afx_msg BSTR get_Version(void);
	afx_msg BSTR get_ApplicationName(void);
	afx_msg BSTR get_ApplicationPath(void);
	afx_msg BSTR get_ScriptName(void);
	afx_msg BSTR MapPath(LPCTSTR pstrPath);

	afx_msg void SetErrorLog(IUnknown* pUnk);

	afx_msg void varLock(VARIANT& varKey);
	afx_msg BOOL varTryLock(VARIANT& varKey);
	afx_msg void varUnlock(VARIANT& varKey);

	BOOL TryLock(CString strKey);
	void Lock(CString strKey);
	void Unlock(CString strKey);

	afx_msg void Sleep(long nTime);

	afx_msg void RegisterTrustedSite(LPCTSTR pstr);
	afx_msg BOOL get_ObjectSafety(void);
	afx_msg void put_ObjectSafety(BOOL b);

	afx_msg VARIANT CallByName(LPDISPATCH pDisp, VARIANT *pName, short sType, VARIANT *pArgs);
	afx_msg long GetIDofName(LPDISPATCH pDisp, VARIANT *pName);

	DECLARE_DISPATCH_MAP()

private:
	static void staticLoadObject(void* dwParam);

	CString m_strConfigFile;

	class CLockNode
	{
	public:
		CLockNode() : m_dwThreadId(0), m_dwCount(0), m_dwUserCount(0)
		{}

		CCriticalSection m_csLock;
		DWORD m_dwThreadId;
		DWORD m_dwCount;
		DWORD m_dwUserCount;
	};

	void _unlock(CString& strKey, CLockNode* pLockNode);
	static BOOL LoadScriptDll(LPCOLESTR pstrName, LPCOLESTR pstrScript);
	BOOL CBoxSystem::LoadVBScriptDllFromPackage();
	BOOL CBoxSystem::LoadVBScriptDllFromFile(LPCSTR lpcsPath);

	CMapStringToPtr m_mapLock;
	CList<CLockNode*> m_FreeLock;
	CCriticalSection m_csLock;

public:
	class CBFolders : public CBSimpleDictionary
	{
	public:
		CBString MapPath(LPCTSTR strKey);
	};

	CBComPtr<IStream> m_pErrorLog;

public:
	CBComPtr<CBDictionary> m_pContents;
	CBComPtr<CBFolders> m_pFolders;
};

