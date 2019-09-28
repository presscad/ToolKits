#ifndef __ARRAYPTR_LIST_H_
#define __ARRAYPTR_LIST_H_
#include "f_ent_list.h"
//�̶�����Ԫ��ָ���ַ�Ķ�̬����ģ����
template <class TYPE> class ARRAYPTR_LIST : public ATOM_LIST<TYPE>
{
public:
	typedef TYPE* TYPEPTR;
    ARRAYPTR_LIST()
	{// ���캯��
		indexPtr=0;
		m_nSize=0;
		m_nMaxSize=0;
		m_nGrowBy=1;
		m_pIndexTbl=NULL;
	}
    ~ARRAYPTR_LIST(){Empty();}

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	long indexPtr;
	long m_nSize;
	long m_nMaxSize;
	long m_nGrowBy;
//2.���г�Ա��������
public:	//��������ʱ��Ҫʹ��m_pIndexTbl���ʱ���Ϊpublic���� wjh-2014.4.1
	TYPEPTR* m_pIndexTbl;   // the actual array of data
//3.˽�г�Ա��������
private:
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    bool InternalDeleteCursor()
	{
		TYPE* pCurrent=GetCursor();
		if(indexPtr<m_nSize)
		{
			BOOL deleted=false;
			for(TYPE* pType=ATOM_LIST<TYPE>::GetFirst();pType;pType=ATOM_LIST<TYPE>::GetNext())
			{
				if(pType==pCurrent)
				{
					deleted=ATOM_LIST<TYPE>::DeleteCursor(TRUE);
					break;
				}
			}
			if(deleted)
			{
				memmove(m_pIndexTbl+indexPtr,m_pIndexTbl+indexPtr+1,sizeof(TYPEPTR)*(m_nSize-indexPtr-1));
				m_nSize--;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
//4.���г�Ա��������
public:
    TYPE* append()//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pTypeObj = ATOM_LIST<TYPE>::append();
		if(m_nSize>=m_nMaxSize)
		{
			TYPEPTR* pData=new TYPEPTR[m_nMaxSize+m_nGrowBy];
			memcpy(pData,m_pIndexTbl,sizeof(TYPEPTR)*m_nMaxSize);
			delete[]m_pIndexTbl;
			m_pIndexTbl=pData;
			m_nMaxSize+=m_nGrowBy;
		}
		m_nSize++;
		indexPtr=m_nSize-1;
		m_pIndexTbl[indexPtr]=pTypeObj;
		return m_pIndexTbl[m_nSize-1];
	}
    TYPE* append(const TYPE &atom)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pAtom = append();
		*pAtom = atom;
		return pAtom;
	}
	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������indexPtr��ָ��ڵ�ǰ���
    TYPE* insert(const TYPE &atom, long ii=-1)
	{
		TYPE *pTypeObj = ATOM_LIST<TYPE>::insert(atom,ii);
		if(m_nSize>=m_nMaxSize)
		{
			TYPEPTR* pData=new TYPEPTR[m_nMaxSize+m_nGrowBy];
			memcpy(pData,m_pIndexTbl,sizeof(TYPEPTR)*m_nMaxSize);
			delete[]m_pIndexTbl;
			m_pIndexTbl=pData;
			m_nMaxSize+=m_nGrowBy;
		}
		if(ii>=0&&ii<m_nSize)
		{	//��ָ��λ�ò���ڵ�
			indexPtr=ii;
			memmove(m_pIndexTbl+ii+1,m_pIndexTbl+ii,sizeof(TYPEPTR)*(m_nSize-ii));
		}
		else 
			indexPtr=m_nSize;
		m_pIndexTbl[indexPtr]=pTypeObj;
		m_nSize++;
		return m_pIndexTbl[indexPtr];
	}
	long GetCurrentIndex()const//��ȡ��ǰ�ڵ��������
	{
		return indexPtr;
	}

    TYPE* GetByIndex(long ii)
	{
		if(ii<0||ii>=m_nSize)
    		return NULL;
		return m_pIndexTbl[ii];
	}
    TYPE& operator[](long ii)//���������Ż�ȡ�߶�ʵ��
	{
		if(ii<0||ii>=m_nSize)
    		throw "�����ַԽ�磡";
		return *m_pIndexTbl[ii];
	}
    TYPE* GetNext()
	{
		ATOM_LIST<TYPE>::GetNext();
		while(indexPtr<m_nSize-1)
		{
			indexPtr++;
			return m_pIndexTbl[indexPtr];
		}
		return NULL;
	}
    TYPE* GetPrev()
	{
		ATOM_LIST<TYPE>::GetPrev();
		while(indexPtr>0)
		{
			indexPtr--;
			return m_pIndexTbl[indexPtr];
		}
		return NULL;
	}
    TYPE* GetFirst()
	{
		ATOM_LIST<TYPE>::GetFirst();
		indexPtr=0;
		if(m_nSize>0)
			return m_pIndexTbl[indexPtr];
		else
			return NULL;
	}
    TYPE* GetTail()
	{
		ATOM_LIST<TYPE>::GetTail();
		indexPtr=m_nSize-1;
		if(indexPtr>=0)
			return m_pIndexTbl[indexPtr];
		else
		{
			indexPtr=0;
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(indexPtr<m_nSize)
			return m_pIndexTbl[indexPtr];
		else
			return NULL;
	}
    BOOL DeleteCursor(BOOL bClean=FALSE)
	{
		TYPE* pType=GetCursor();
		if(pType==NULL||IsCursorDeleted())
			return FALSE;
		for(int i=0;i<m_nSize;i++)
		{
			if(m_pIndexTbl[i]==pType)
			{
				m_nSize--;
				if(m_nSize>i);
					memmove(&m_pIndexTbl[i],m_pIndexTbl[i+1],(m_nSize-i)*4);
				return ATOM_LIST<TYPE>::DeleteCursor(bClean);
			}
		}
		return FALSE;
	}
    BOOL DeleteAt(long ii)	// ɾ��һ���ڵ�
	{
		if(ii<0||ii>=m_nSize)
			return FALSE;
		TYPE* pType=m_pIndexTbl[ii];
		for(TYPE* pTmpType=GetFirst();pTmpType;pTmpType=GetNext())
		{
			if(pTmpType==pType)
			{
				m_nSize--;
				if(m_nSize>ii)
					memmove(&m_pIndexTbl[ii],&m_pIndexTbl[ii+1],(m_nSize-ii)*4);
				return ATOM_LIST<TYPE>::DeleteCursor(TRUE);
			}
		}
		return FALSE;
	}
	void SetSize(long size,long growBy=1)
	{
		if(size>0)
			m_nSize=size;
		else
			m_nSize=0;
		if(growBy>0)
			m_nGrowBy=growBy;
		if(size>m_nMaxSize)
		{
			m_nMaxSize=size;
			if(m_pIndexTbl)
				delete	[]m_pIndexTbl;
			m_pIndexTbl=new TYPEPTR[size];
		}
		else if(m_nMaxSize==0)
			m_pIndexTbl=NULL;
	}
	long GetSize()const{return m_nSize;}
    bool RemoveAt(long ii)	// ɾ��һ���ڵ�
	{
		indexPtr=ii;
		return InternalDeleteCursor();
	}
    void Destroy()		// ����������ǰ��ʵ��
	{
		~ARRAY_LIST();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		ATOM_LIST<TYPE>::Empty();
		delete []m_pIndexTbl;
		m_pIndexTbl=NULL;
		m_nSize=m_nMaxSize=indexPtr=0;
	}
	//����ָ��ָ���Ӧ������ֵ wht 12-03-27
	int GetIndex(const TYPE *pObj)
	{
		for(int i=0;i<m_nSize;i++)
		{
			if(m_pIndexTbl[i]==pObj)
				return i;
		}
		return -1;
	}
};
#endif