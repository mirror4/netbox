#include "StdAfx.h"
#include "NetBox2.h"
#include "BoxObject.h"
#include "BoxSystem.h"
#include "BoxCommand.h"
#include "BoxScript.h"
#include "BoxFile.h"
#include <BArguments.h>

static CBoxScript* s_pNowScript = NULL;

BOOL WINAPI CBoxCommand::CtrlHandler(DWORD fdwCtrlType)
{
	if(fdwCtrlType == CTRL_C_EVENT)
	{
		s_pNowScript->Interrupt(_T("The script was canceled by the user."));
		return TRUE;
	}

	return FALSE;
} 

void CBoxCommand::Start(void)
{
	CBComPtr<CBArguments> pArgs;
	CString str;

	pArgs.Attach(theApp.get_Arguments());
	m_pConsole.Attach(theApp.get_Console());
	m_strBasePath = _T("\\");

	m_pConsole->WriteLine(CBoxSystem::getVersion());
	m_pConsole->WriteLine("");

	while(TRUE)
	{
		m_pConsole->WriteText(_T("#"));

		pArgs->put_CommandLine((BSTR)(LPCWSTR)CBString(m_pConsole->ReadLine()));

		if(pArgs->GetCount())
		{
			str = pArgs->GetString(0);

			if(!str.CompareNoCase(_T("exit")))
				return;
			else
			{
				CBoxObject<CBoxScript> pScript;
				int nError;

				pScript.CreateInstance();

				if(!IsBoxFile(str.Right(4)))
					str += ".box";

				nError = pScript->Load(m_strBasePath + str);
				if(nError == 404)
					m_pConsole->WriteLine("Bad Command");

				if(nError == 0)
				{
					g_pFile->m_strAppName = str;

					pScript->AddNameItem(_T("Shell"), (LPDISPATCH)theApp.GetInterface(&IID_IDispatch));
					pScript->AddNameItem(_T("NetBox"), theApp.m_pSystem);

					s_pNowScript = pScript;
					SetConsoleCtrlHandler(CtrlHandler, TRUE);
					nError = pScript->Run();
					SetConsoleCtrlHandler(CtrlHandler, FALSE);
					s_pNowScript = NULL;
				}

				if(nError != 0 && nError != 404)
				{
					CBoxScript::CScriptError &error = pScript->GetErrorMessage();

					str = error.m_strSource;
					str += _T('\n');

					if(error.m_sCode != 0)
						str.AppendFormat(_T("Error Number: %08X\nFile: "), error.m_sCode);
					else str.Append(_T("File: "), 6);
					str += error.m_strFile;
					str.AppendFormat(_T("\nLine: %d\nDescription: "), error.m_nLine);
					str += error.m_strDescription;

					m_pConsole->WriteLine("");
					m_pConsole->WriteLine(str);
					m_pConsole->WriteLine("");
				}
			}
		}
	}
}
