#pragma once

class CBoxLinkNode
{
public:
	CBoxLinkNode(void) :
		m_pNext(NULL),
		m_pPrev(NULL)
	{}

	CBoxLinkNode* m_pNext;
	CBoxLinkNode* m_pPrev;
};

template <class T>
class CBoxLink
{
public:
	CBoxLink(void) :
		m_pHead(NULL),
		m_pTail(NULL)
	{}

	BOOL IsEmpty(void)
	{
		return m_pHead == NULL;
	}

	BOOL InLink(T* pItem)
	{
		ASSERT((m_pHead == NULL && m_pTail == NULL) || (m_pHead != NULL && m_pTail != NULL));

		T* pHead = m_pHead;

		while(pHead != NULL && pHead != pItem)
			pHead = (T*)pHead->m_pNext;

		return pHead == pItem;
	}

	void Append(T* pItem)
	{
		ASSERT(!InLink(pItem));

		pItem->m_pNext = NULL;
		pItem->m_pPrev = m_pTail;
		if (m_pHead != NULL)
			m_pTail->m_pNext = pItem;
		else
			m_pHead = pItem;

		m_pTail = pItem;
	}

	void AddHead(T* pItem)
	{
		ASSERT(!InLink(pItem));

		pItem->m_pPrev= NULL;
		pItem->m_pNext = m_pHead;
		if (m_pTail != NULL)
			m_pHead->m_pPrev = pItem;
		else
			m_pTail = pItem;

		m_pHead = pItem;
	}

	T* Remove(T* pItem)
	{
		ASSERT(InLink(pItem));

		T* pPrev = (T*)pItem->m_pPrev;
		T* pNext = (T*)pItem->m_pNext;

		if (pPrev != NULL)
			pPrev->m_pNext = pNext;
		else
			m_pHead = pNext;

		if (pNext != NULL)
			pNext->m_pPrev = pPrev;
		else
			m_pTail = pPrev;

		return pItem;
	}

	void RemoveAll(void)
	{
		m_pHead = NULL;
		m_pTail = NULL;
	}

	T* GetHead(void)
	{
		return m_pHead;
	}

	T* GetTail(void)
	{
		return m_pTail;
	}

	T* RemoveHead(void)
	{
		T* pHead = m_pHead;
		T* pNext = (T*)m_pHead->m_pNext;

		if (pNext != NULL)
			pNext->m_pPrev = NULL;
		else
			m_pTail = NULL;

		m_pHead = pNext;

		return pHead;
	}

	T* RemoveTail(void)
	{
		T* pTail = m_pTail;
		T* pPrev = (T*)m_pTail->m_pPrev;

		if (pPrev != NULL)
			pPrev->m_pNext = NULL;
		else
			m_pHead = NULL;

		m_pTail = pPrev;

		return pTail;
	}

protected:
	T* m_pHead;
	T* m_pTail;
};
