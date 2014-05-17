#include "StdAfx.h"
#include ".\burlstream.h"

HRESULT GetBoxStream(BSTR bstrFile, IStream **ppStream);

HRESULT CBUrlStream::Open(BSTR bstrUrl)
{
	Close();

	if(!wcsnicmp(bstrUrl, L"netbox:/", 8))
		return GetBoxStream(bstrUrl + 7, &m_pStream);

	return URLOpenBlockingStream(NULL, CString(bstrUrl), &m_pStream, 0, NULL); 
}

