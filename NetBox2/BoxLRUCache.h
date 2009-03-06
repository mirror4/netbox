#pragma once

#include <afxtempl.h>
#include "BoxLink.h"
#include "BoxObject.h"

class CBoxLRUCacheNode : public CBoxLinkNode
{
public:
	CString m_strNodeKey;
};

template<class T>
class CBoxLRUCache
{
public:
	CBoxLRUCache(void)
	{
		m_mapNode.InitHashTable(3049);
	}

	BOOL IsEmpty(void)
	{
		return m_link.IsEmpty();
	}

	void Update(T* pItem)
	{
		m_link.Remove(pItem);
		m_link.AddHead(pItem);
	}

	BOOL Exists(LPCTSTR pstrKey)
	{
		if(m_link.IsEmpty())
			return FALSE;

		CString strTemp(pstrKey);

		strTemp.MakeLower();

		return m_mapNode.PLookup(strTemp) != NULL;
	}

	BOOL Lookup(LPCTSTR pstrKey, CBoxObject<T>& pItem)
	{
		if(m_link.IsEmpty())
			return FALSE;

		CString strTemp(pstrKey);

		strTemp.MakeLower();

		if(m_mapNode.Lookup(strTemp, pItem))
		{
			Update(pItem);

			return TRUE;
		}

		return FALSE;
	}

	T* GetLast(void)
	{
		return m_link.GetTail();
	}

	void Remove(T* pItem)
	{
		if(!pItem->m_strNodeKey.IsEmpty())
		{
			if(m_mapNode.RemoveKey(pItem->m_strNodeKey))
				m_link.Remove(pItem);
			pItem->m_strNodeKey.Empty();
		}
	}

	void Add(LPCTSTR pstrKey, T* pItem)
	{
		CString strTemp(pstrKey);

		strTemp.MakeLower();

		CBoxObject<T> &p = m_mapNode[strTemp];

		if(p != NULL)
			m_link.Remove(p);

		pItem->m_strNodeKey = strTemp;
		p = pItem;
		m_link.AddHead(pItem);
	}

	void RemoveAll(void)
	{
		m_link.RemoveAll();
		m_mapNode.RemoveAll();
	}

private:
	CMap<CString, LPCTSTR, CBoxObject<T>, T*> m_mapNode;
	CBoxLink<T> m_link;
};
