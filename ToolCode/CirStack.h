#ifndef __CIRCULAR_STACK_H_
#define __CIRCULAR_STACK_H_
template <class TYPE> class CCirStack
{
	struct DATA_NODE
	{
		TYPE data;
		DATA_NODE *pPrev;
		DATA_NODE *pNext;
	};
	DATA_NODE *pHead;
	DATA_NODE *pCursor;
	TYPE *work;
	long m_nCount;
public:
	CCirStack();
	CCirStack(long nCount);
	~CCirStack(){Empty(); delete pHead;}
	long SetSize(long nCount)
	{
		if(nCount<1)
			return m_nCount;
		else
			m_nCount = nCount;
		return m_nCount;
	}
	BOOL IsEmpty()
	{
		if(pCursor==pHead)
			return TRUE;
		else
			return FALSE;
	}
	long GetNodeCount()
	{
		long i=0;
		DATA_NODE *pTemp = pHead;
		while(pTemp!=pCursor)
		{
			pTemp = pTemp->pNext;
			i++;
		}
		return i;
	}
	long GetRemnants()
	{
		return m_nCount-GetNodeCount();
	}
	void Empty()
	{
		while(pHead!=pHead->pNext)
		{
			DATA_NODE *pTemp = pHead->pNext;
			pHead->pNext = pTemp->pNext;
			pTemp->pNext->pPrev = pHead;
			delete pTemp;
		}
		pCursor = pHead;
		pCursor->pNext = pHead;
		pCursor->pPrev = pHead;
	}
	TYPE* TopAtom()		//����ջ��Ԫ��
	{
		return &pCursor->data;
	}
	void push(const TYPE &val)
	{
		if(pCursor->pNext!=pHead)	//ѭ��ջδ��
		{
			pCursor = pCursor->pNext;
			pCursor->data = val;
		}
		else	//ѭ��ջ����
		{
			if(GetNodeCount()<m_nCount)//�����������Լ��������ڴ�
			{
				DATA_NODE *pTemp = new DATA_NODE;
				pTemp->data = val;
				pTemp->pPrev = pCursor;
				pTemp->pNext = pCursor->pNext;
				pTemp->pNext->pPrev = pTemp;
				pCursor->pNext = pTemp;
				pCursor = pTemp;
			}
			else	//�����������ټ��������ڴ棬ֻ�ܸ�����ǰ�Ľڵ�
			{
				pCursor = pHead;
				pCursor->data = val;
				pHead = pHead->pNext;
			}
		}
	}
	BOOL pop(TYPE& res)
	{
		if(pCursor==pHead)	//ջΪ��
			return false;
		else
		{
			res = pCursor->data;
			pCursor = pCursor->pPrev;
			return true;
		}
	}
};

template <class TYPE> 
CCirStack<TYPE>::CCirStack()
{
	m_nCount = 1024;
	pHead = new DATA_NODE;
	pCursor = pHead;
	pCursor->pNext = pHead;
	pCursor->pPrev = pHead;
}

template <class TYPE> 
CCirStack<TYPE>::CCirStack(long nCount)
{
	m_nCount = nCount;
	pHead = new DATA_NODE;
	pCursor = pHead;
	pCursor->pNext = pHead;
	pCursor->pPrev = pHead;
}
#endif
