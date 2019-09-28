#ifndef __XH_PTR_LIST_H_
#define __XH_PTR_LIST_H_
#include "f_ent_list.h"

template <class TYPE> class CXhPtrList : public ATOM_LIST<TYPE*>
{
public:
	void (*DeleteObjectFunc)(TYPE* pObj);
	CXhPtrList(){DeleteObjectFunc=NULL;}
	~CXhPtrList(){Empty();}
public:
	TYPE* append(TYPE* pType)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE **ppTypeObj = ATOM_LIST<TYPE*>::append(pType);
		return *ppTypeObj;
	}
	TYPE* append()//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE* pTypeObj=new TYPE();
		TYPE **ppTypeObj = ATOM_LIST<TYPE*>::append(pTypeObj);
		return *ppTypeObj;
	}
    TYPE* GetNext(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetNext(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetPrev(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetFirst(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetFirst(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetTail(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetTail(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	BOOL DeleteCursor()
	{
		TYPE* pObj=GetCursor();
		if(pObj)
		{
			if(DeleteObjectFunc)
				DeleteObjectFunc(pObj);
			else 
				delete pObj;
			return ATOM_LIST<TYPE*>::DeleteCursor(TRUE);
		}
		return FALSE;
	}
	BOOL DeleteNode(TYPE* pType)
	{
		BOOL bRetCode=FALSE;
		int nPush=push_stack();
		for(TYPE* pObj=GetFirst(TRUE);pObj;pObj=GetNext(TRUE))
		{
			if(pObj==pType)
			{
				bRetCode=DeleteCursor();
				break;
			}
		}
		pop_stack(nPush);
		return bRetCode;
	}
    TYPE* GetCursor()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetCursor();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	void Empty(){
		if(DeleteObjectFunc!=NULL)
		{
			for(TYPE* pObj=GetFirst(TRUE);pObj;pObj=GetNext(TRUE))
				DeleteObjectFunc(pObj);
		}
		else
		{
			for(TYPE* pObj=GetFirst(TRUE);pObj;pObj=GetNext(TRUE))
				delete pObj;
		}
		ATOM_LIST<TYPE*>::Empty(); 
	}
};

//����ģ����������ָ�뼯�ϣ����漰�����������ڣ������ATOM_LIST<TYPE*>������ֱֵ�Ӽ�ΪTYPE* wht 14-08-21
template <class TYPE> class CXhPtrSet : public ATOM_LIST<TYPE*>
{
public:
	CXhPtrSet(){;}
	~CXhPtrSet(){;}
	CXhPtrSet(const CXhPtrSet<TYPE> &srcSet)
	{
		Clone(srcSet);
	}
	CXhPtrSet& operator =(const CXhPtrSet<TYPE> &srcSet)
	{
		Clone(srcSet);
		return *this;
	}
	void Clone(const CXhPtrSet<TYPE> &srcSet)
	{
		Empty();
		for(TYPE* pAtom=(const_cast<CXhPtrSet<TYPE>*>(&srcSet))->GetFirst();pAtom;pAtom=(const_cast<CXhPtrSet<TYPE>*>(&srcSet))->GetNext())
			append(pAtom);
	}
public:
	void append(const CXhPtrSet<TYPE> &srcSet)
	{
		for(TYPE* pAtom=(const_cast<CXhPtrSet<TYPE>*>(&srcSet))->GetFirst();pAtom;pAtom=(const_cast<CXhPtrSet<TYPE>*>(&srcSet))->GetNext())
			append(pAtom);
	}
	TYPE* append(TYPE* pType)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE **ppTypeObj = ATOM_LIST<TYPE*>::append(pType);
		return *ppTypeObj;
	}
	TYPE* GetNext()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetNext();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	TYPE* GetPrev()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetPrev();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	TYPE* GetFirst()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetFirst();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	TYPE* GetTail()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetTail();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	TYPE* GetCursor()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetCursor();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	TYPE* SetCursorValue(TYPE* pType)//�ڽڵ�����ĵ�ǰλ�ø��½ڵ�ֵ
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetCursor();
		if(ppType)
			return *ppType=pType;
		else
			return NULL;
	}
	BOOL DeleteAt(long ii)
	{
		return ATOM_LIST<TYPE*>::DeleteAt(ii);
	}
	void Empty(){
		ATOM_LIST<TYPE*>::Empty(); 
	}
};
#endif