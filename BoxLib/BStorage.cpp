#include "StdAfx.h"
#include "BStorage.h"
#include "BVarType.h"

STDMETHODIMP CBStorage::Create(BSTR bstrName, VARIANT varOverwrite)
{
	Close();
	return StgCreateDocfile(bstrName, 
		(varGetNumbar(varOverwrite, VARIANT_TRUE) != VARIANT_FALSE ? STGM_CREATE : 0) | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &m_pStorage);
}

STDMETHODIMP CBStorage::Open(BSTR bstrName, VARIANT varMode)
{
	Close();

	DWORD dwMode;

	dwMode = varGetNumbar(varMode, 1) - 1;
	if(dwMode > 2)return E_INVALIDARG;

	return StgOpenStorage(bstrName, NULL, dwMode | STGM_SHARE_EXCLUSIVE, 0, 0, &m_pStorage);
}

STDMETHODIMP CBStorage::Close()
{
	m_pStorage.Release();
	m_pParentStorage.Release();
	return S_OK;
}

STDMETHODIMP CBStorage::CreateFile(BSTR bstrName, VARIANT varOverwrite, IDispatch** ppStream)
{
	if(m_pStorage == NULL)return E_HANDLE;

	CBComPtr<IStream> pStm;

	HRESULT hr = m_pStorage->CreateStream(bstrName,
		(varGetNumbar(varOverwrite, VARIANT_TRUE) != VARIANT_FALSE ? STGM_CREATE : 0) | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, 0, &pStm);
	if(FAILED(hr))return hr;

	*ppStream = new CBStorageStream(pStm, this);

	return S_OK;
}

STDMETHODIMP CBStorage::OpenFile(BSTR bstrName, VARIANT varMode, VARIANT varShare, IDispatch** ppStream)
{
	DWORD dwMode;

	if(m_pStorage == NULL)return E_HANDLE;

	dwMode = varGetNumbar(varMode, 1) - 1;
	if(dwMode > 2)return E_INVALIDARG;

	CBComPtr<IStream> pStm;

	HRESULT hr = m_pStorage->OpenStream(bstrName, 0, dwMode | STGM_SHARE_EXCLUSIVE, 0, &pStm);
	if(FAILED(hr))return hr;

	*ppStream = new CBStorageStream(pStm, this);

	return S_OK;
}

STDMETHODIMP CBStorage::CreateFolder(BSTR bstrName, VARIANT varOverwrite, IDispatch** ppStorage)
{
	if(m_pStorage == NULL)return E_HANDLE;

	CBComPtr<IStorage> pStg;

	HRESULT hr = m_pStorage->CreateStorage(bstrName,
		(varGetNumbar(varOverwrite, VARIANT_TRUE) != VARIANT_FALSE ? STGM_CREATE : 0) | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, 0, &pStg);
	if(FAILED(hr))return hr;

	*ppStorage = new CBStorage(pStg, this);

	return S_OK;
}

STDMETHODIMP CBStorage::OpenFolder(BSTR bstrName, VARIANT varMode, IDispatch** ppStorage)
{
	DWORD dwMode;

	if(m_pStorage == NULL)return E_HANDLE;

	dwMode = varGetNumbar(varMode, 1) - 1;
	if(dwMode > 2)return E_INVALIDARG;

	CBComPtr<IStorage> pStg;

	HRESULT hr = m_pStorage->OpenStorage(bstrName, 0, dwMode | STGM_SHARE_EXCLUSIVE, 0, 0, &pStg);
	if(FAILED(hr))return hr;

	*ppStorage = new CBStorage(pStg, this);

	return S_OK;
}

STDMETHODIMP CBStorage::get__NewEnum(IUnknown **ppEnumReturn)
{
	if(m_pStorage == NULL)return E_HANDLE;
	IEnumSTATSTG* pEnum;
	HRESULT hr;

	hr = m_pStorage->EnumElements(0, 0, 0, &pEnum);
	if(FAILED(hr))return hr;

	*ppEnumReturn = (IObjectSafety*)new CBStorageEnum(pEnum);

	return S_OK;
}

