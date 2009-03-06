#pragma once

#include "BDate.h"
#include "BStream.h"

class __declspec(uuid("94650000-0000-0000-0053-746F72616765"))
CBStorage : public CBDispatch<IBaseStorage>
{
	DECLARE_CLASS(CBStorage)
	APARTMENT_MODE

	class __declspec(uuid("94650000-0053-746F-7261-67654974656D"))
	CBStorageItem : public CBDispatch<IStorageItem>,
					public STATSTG
	{
		DECLARE_CLASS(CBStorageItem)
		APARTMENT_MODE

	public:
		CBStorageItem(void)
		{
			ZeroMemory((STATSTG*)this, sizeof(STATSTG));
		}

		~CBStorageItem(void)
		{
			if(pwcsName)CoTaskMemFree(pwcsName);
		}

	public:
		STDMETHOD(get_Name)(BSTR *pVal)
		{
			*pVal = ::SysAllocString(pwcsName);
			return S_OK;
		}

		STDMETHOD(get_Type)(short *pVal)
		{
			switch(type)
			{
			case STGTY_STORAGE:*pVal = FILE_ATTRIBUTE_ARCHIVE;break;
			case STGTY_STREAM:*pVal = FILE_ATTRIBUTE_DIRECTORY;break;
			};
			return S_OK;			
		}

		STDMETHOD(get_Size)(long *pVal)
		{
			if(cbSize.HighPart)return DISP_E_OVERFLOW;
			*pVal = cbSize.LowPart;
			return S_OK;
		}

		STDMETHOD(get_DateCreated)(DATE *pVal)
		{
			*(CBDate*)pVal = ctime;
			return S_OK;
		}

		STDMETHOD(get_DateLastAccessed)(DATE *pVal)
		{
			*(CBDate*)pVal = atime;
			return S_OK;
		}

		STDMETHOD(get_DateLastModified)(DATE *pVal)
		{
			*(CBDate*)pVal = mtime;
			return S_OK;
		}
	};

	class CBStorageEnum : public CBComObject,
					public IEnumVARIANT
	{
		INTERFACE_BEGIN
			INTERFACE_DEF(IEnumVARIANT)
		INTERFACE_END

		APARTMENT_MODE

	public:
		CBStorageEnum(IEnumSTATSTG* pEnum)
		{
			m_pEnum.Attach(pEnum);
		}

	public:
		// IEnumVARIANT
		STDMETHOD(Next)(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
		{
			CBComPtr<CBStorageItem> pItem;
			HRESULT hr;

			hr = pItem.CreateInstance();
			if(FAILED(hr))return hr;

			hr = m_pEnum->Next(1, pItem, pCeltFetched);
			if(FAILED(hr))return hr;
			if(S_FALSE == hr)return hr;

			rgVar->vt = VT_DISPATCH;
			rgVar->pdispVal = pItem.Detach();

			return S_OK;
		}

		STDMETHOD(Skip)(ULONG celt)
		{
			return m_pEnum->Skip(celt);
		}

		STDMETHOD(Reset)(void)
		{
			return m_pEnum->Reset();
		}

		STDMETHOD(Clone)(IEnumVARIANT **ppEnum)
		{
			IEnumSTATSTG* pEnum;
			HRESULT hr;

			hr = m_pEnum->Clone(&pEnum);
			if(FAILED(hr))return hr;

			*ppEnum = new CBStorageEnum(pEnum);
			return S_OK;
		}

	private:
		CBComPtr<IEnumSTATSTG> m_pEnum;
	};

	class CBStorageStream : public CBStream
	{
	public:
		APARTMENT_MODE

		CBStorageStream(IStream* pStream, CBStorage* pParentStorage) : m_pParentStorage(pParentStorage)
		{
			Attach(pStream);
		}

	private:
		CBComPtr<CBStorage> m_pParentStorage;
	};

public:
	CBStorage(IStorage *pStorage = NULL, CBStorage* pParentStorage = NULL) :
		m_pStorage(pStorage),
		m_pParentStorage(pParentStorage)
	{}

public:
	// IBaseStorage
	STDMETHOD(Create)(BSTR bstrName, VARIANT varOverwrite);
	STDMETHOD(Open)(BSTR bstrName, VARIANT varMode);
	STDMETHOD(Close)();
	STDMETHOD(CreateFile)(BSTR bstrName, VARIANT varOverwrite, IDispatch** ppStream);
	STDMETHOD(OpenFile)(BSTR bstrName, VARIANT varMode, VARIANT varShare, IDispatch** ppStream);
	STDMETHOD(CreateFolder)(BSTR bstrName, VARIANT varOverwrite, IDispatch** ppStorage);
	STDMETHOD(OpenFolder)(BSTR bstrName, VARIANT varMode, IDispatch** ppStorage);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);

private:
	CBComPtr<IStorage> m_pStorage;
	CBComPtr<CBStorage> m_pParentStorage;
};
