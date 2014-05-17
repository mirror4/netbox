#pragma once

#include "BoxLink.h"

class CBoxBaseObject
{
public:
	CBoxBaseObject(void) : m_dwRef(1)
	{
	}

	virtual ~CBoxBaseObject(void)
	{
	}

	void ExternalAddRef(void)
	{
		InterlockedIncrement(&m_dwRef);
	}

	void ExternalRelease()
	{
		if (m_dwRef == 0)
			return;

		LONG lResult = InterlockedDecrement(&m_dwRef);
		if (lResult == 0)
			delete this;
	}

private:
	long m_dwRef;
};

template <class T>
class CBoxObject
{
public:
	CBoxObject() : p(NULL)
	{}

	CBoxObject(T* p1) : p(NULL)
	{
		operator=(p1);
	}

	CBoxObject(const CBoxObject<T>& p1) : p(NULL)
	{
		operator=(p1);
	}

	~CBoxObject()
	{
		Release();
	}

	T* Attach(T* p1)
	{
		return (p = p1);
	}

	T* Detach(void)
	{
		T* p1 = p;
		p = NULL;
		return p1;
	}

	T* CreateInstance(CRuntimeClass* pRuntimeClass = NULL)
	{
		Release();

		if(pRuntimeClass == NULL)
			p = new T();
		else
			p = (T*)pRuntimeClass->CreateObject();

		return p;
	}

	void Release(void)
	{
		if(p != NULL)
		{
			p->ExternalRelease();
			p = NULL;
		}
	}

	T* operator=(const CBoxObject<T>& p1)
	{
		Release();

		p = p1.p;
		if(p != NULL)
			p->ExternalAddRef();
		return p;
	}

	T* operator=(CCmdTarget* p1)
	{
		Release();

		p = (T*)p1;
		if(p != NULL)
			p->ExternalAddRef();
		return p;
	}

	T* operator=(T* p1)
	{
		Release();

		p = p1;
		if(p != NULL)
			p->ExternalAddRef();
		return p;
	}

	T* operator=(LPDISPATCH p1)
	{
		return operator=(CCmdTarget::FromIDispatch(p1));
	}

	BOOL operator!=(const T* p1) const
	{
		return p != p1;
	}

	BOOL operator==(const T* p1) const
	{
		return p == p1;
	}

	operator CCmdTarget*() const
	{
		return p;
	}

	operator T*() const
	{
		return p;
	}

	operator T&() const
	{
		return *p;
	}

	operator LPUNKNOWN() const
	{
		if(p == NULL)
			return NULL;

		return p->GetInterface(&IID_IUnknown);
	}

	operator LPDISPATCH() const
	{
		if(p == NULL)
			return NULL;

		return (LPDISPATCH)p->GetInterface(&IID_IDispatch);
	}

	T* operator->() const
	{
		ATLASSERT(p!=NULL);
		return p;
	}

protected:
	T* p;
};
