// Logon.cpp : implementation file
//

#include "stdafx.h"
#include <afxinet.h>
#include "NBR.h"
#include "Logon.h"
#include ".\logon.h"
#include <BEncoding.h>

// CLogon dialog

IMPLEMENT_DYNAMIC(CLogon, CDialog)
CLogon::CLogon(CWnd* pParent /*=NULL*/)
	: CDialog(CLogon::IDD, pParent)
{
}

CLogon::~CLogon()
{
}

void CLogon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER, m_wndUser);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
}


BEGIN_MESSAGE_MAP(CLogon, CDialog)
	ON_BN_CLICKED(IDC_REGISTER, OnBnClickedRegister)
END_MESSAGE_MAP()


// CLogon message handlers

BOOL CLogon::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(GetMessage(L"Logon"));

	SetDlgItemText(IDT_USERNAME, GetMessage(L"UserName"));
	SetDlgItemText(IDT_PASSWORD, GetMessage(L"Password"));
	SetDlgItemText(IDOK, GetMessage(L"Logon"));
	SetDlgItemText(IDC_REGISTER, GetMessage(L"Register"));
	SetDlgItemText(IDCANCEL, GetMessage(L"Cancel"));

	m_wndUser.SetFocus();

	if(!theApp.m_strLogon.IsEmpty())
		OnOK();

	return TRUE;
}

void CLogon::OnBnClickedRegister()
{
	ShellExecute(NULL, "open", "http://www.netbox.cn/user/register.asp", NULL, NULL, SW_SHOW);
}

void CLogon::OnOK()
{
	CString strUser, strPassword, strForm, strStatus;

	if(theApp.m_strLogon.IsEmpty())
	{
		m_wndUser.GetWindowText(strUser);
		strUser.Trim();
		m_wndUser.SetWindowText(strUser);
		if(strUser.IsEmpty())
		{
			AfxMessageBox(GetMessage(L"logonEmpty"));
			m_wndUser.SetFocus();
			return;
		}

		m_wndPassword.GetWindowText(strPassword);
		if(strPassword.IsEmpty())
		{
			AfxMessageBox(GetMessage(L"logonEmpty"));
			m_wndPassword.SetFocus();
			return;
		}

		strForm = _T("username=") + CBEncoding::UrlEncode(strUser) + _T("&password=") + CBEncoding::UrlEncode(strPassword);
	}else strForm = theApp.m_strLogon;

    CInternetSession session;
	CHttpConnection* pConnection = session.GetHttpConnection(_T("www.netbox.cn"));
	if(pConnection)
	{
		CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/user/logon.asp"));
		if(pFile)
		{
			pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));
			if(pFile->SendRequest(NULL, NULL, (LPVOID)(LPCTSTR)strForm, strForm.GetLength()))
				pFile->QueryInfo(HTTP_QUERY_STATUS_TEXT, strStatus);
			delete pFile;
		}else
		{
			delete pConnection;
			return;
		}

		if(!strStatus.CompareNoCase(_T("Logon Error")))
		{
			theApp.m_strLogon.Empty();
			AfxMessageBox(GetMessage(L"logErr"));
			m_wndUser.SetFocus();
			return;
		}

		theApp.m_strLogon = strForm;

		pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/user/getlic.asp"));
		if(pFile)
		{
			pFile->AddRequestHeaders(_T("Content-Type: binary"));
			if(pFile->SendRequest(NULL, NULL, theApp.m_mStream.m_pBuffer, theApp.m_mStream.m_dwSize))
			{
				char buffer[512];

				ZeroMemory(buffer, 512);
				if(pFile->Read(buffer, 512) != 512)
					AfxMessageBox(buffer);
				else
				{
					CFile file;

					if(file.Open(theApp.m_strBasePath + _T("nbdw.lic"), CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyNone))
						file.Write(buffer, 512);
				}
			}
			delete pFile;
		}
		delete pConnection;
	}else return;

	CDialog::OnOK();
}

