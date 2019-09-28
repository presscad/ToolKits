#pragma once

#include "list.h"
#include "Hashtable.h"
//Hash��ģ����
//CStringKeyHashTable���ƹ���������ΪCHashStrTable wjh-2014.6.24
template <class TYPE> class CHashStrTableEx
{
//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	struct DATA_TYPE
	{
		char key[100];
		TYPE atom;
		DATA_TYPE *next;
	};
	DATA_TYPE* data;// Hash����ָ������ݵ�ָ��
	DWORD m_nHashSize;	// Hash�������洢TYPEԪ�صĸ�������Hash��Ĵ�С��
public:
	CHashStrTableEx(DWORD HashTblSize=0)
	{// ���캯��
		HashFunc=NULL;
		KeyMatchFunc=NULL;
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
	}
    ~CHashStrTableEx(){Empty();}
	bool CreateHashTable(DWORD HashTblSize)
	{
		if(m_nHashSize>0)
			return false;	//����������ٴ���
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
		return true;
	}

//2.���г�Ա��������
public:
	DWORD (*HashFunc)(const char *key, DWORD nHashSize);	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
	bool  (*KeyMatchFunc)(const char* key, const char* szPartialMatchWithKey);	//�ж�key�Ƿ���withKeyƥ�䣬�������withKey����׼������key��withKey����ƥ��
//3.˽�г�Ա��������
private:
	DWORD Hash(const char *key)const	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
	{
		if(HashFunc)
			return HashFunc(key, m_nHashSize);
		ULONG hashkey=0,keylen=(int)strlen(key);
		/*for(int i=0;i<keylen;i++)
			hashkey+=key[i];*/
		//keyΪ��ʱĬ����"CHashStrTable:NULL-KEY"��Ϊ��ֵ,
		//������������ٴ��ÿ��ַ�������ֵ��ѯʱ������Чֵ wht 16-12-05
		char sNullKey[]="CHashStrTable:NULL-KEY";
		const char *sRealKey=keylen>0?key:sNullKey;
		if(keylen<=0)
			keylen=(int)strlen(sRealKey);
		union KEY_VALUE
		{
			BYTE key[4]; //0.A1 1.A2 2.A3 3.A4
			DWORD dwKey; //A4A3A2A1 �Ӹߵ���
		}key_value,value;
		memset(key_value.key,0,sizeof(BYTE)*4);
		for(ULONG i=0;i<keylen;i++)
		{
			ULONG nRemainder=i%3;
			if(nRemainder==0)
				memset(value.key,0,sizeof(BYTE)*4);
			value.key[nRemainder]=sRealKey[i];
			if(nRemainder==2||i==keylen-1)
				key_value.dwKey+=value.dwKey;
		}
		key_value.key[3]=(BYTE)keylen;
		hashkey=key_value.dwKey;
		/* //hashkeyΪULONG �˴�������ָ��� 
		if(hashkey<0)	//keyֵ��������ʱhashkey����Ϊ��ֵ wht 10-09-08
			hashkey*=-1;
		*/
		return hashkey%m_nHashSize;
	}

//4.���г�Ա��������
public:
	DWORD GetHashTableSize()const{return m_nHashSize;}
	DWORD GetMaxDepth()
	{
		int nMaxDepth=0;
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			DATA_TYPE *cursor=&data[i];
			int nTmpDepth=0;
			while(cursor)
			{
				cursor=cursor->next;
				if(cursor)
					nTmpDepth++;
			}
			if(nTmpDepth>nMaxDepth)
				nMaxDepth=nTmpDepth;
		}
		return nMaxDepth;
	}
	bool ContainsKey(const char *key,bool blCompletelyMatch=false)
	{
		if(data==NULL||key==NULL)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if ((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0)
				return true;
			else if (!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key))
				return true;
			else
				cursor=cursor->next;
		}
		return false;
	}
	bool GetValueAt(const char *key,TYPE &val,bool blCompletelyMatch=false)
	{
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)		//i=0?
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if (((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0)||
				(!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key)))
			{
				val=cursor->atom;
				return true;
			}
			else
				cursor=cursor->next;
		}
		return false;
	}
    bool SetValueAt(const char *key,TYPE val)//���������Ż�ȡ�߶�ʵ��
	{
		bool bOk=false;
		if(data==NULL||key==NULL)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		bool blCompletelyMatch=true;	//д��ֵʱӦ�ϸ�ƥ���ֵ wjh-2019.6.17
		DWORD i = Hash(key);
		if(i<0)	//i=0?
			return false;
		if(data[i].key!=NULL&&*data[i].key!=0)//strlen(data[i].key)>0)
		{
			/*if(strcmp(data[i].key,key)==0)
			{
				data[i].atom = val;
				return true;
			}
			else*/
			{
				DATA_TYPE *tail_item=NULL;
				//DATA_TYPE *cursor=data[i].next;
				DATA_TYPE *cursor=&data[i];
				while(cursor)
				{
					tail_item = cursor;
					if (((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0) ||
						(!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key)))
					{
						cursor->atom=val;
						bOk=true;
						break;
					}
					else
						cursor=cursor->next;
				}
				if(!bOk)
				{
					tail_item->next=new DATA_TYPE;
					tail_item->next->next=NULL;
					strncpy(tail_item->next->key,key,99);
					tail_item->next->atom=val;
				}
				return true;
			}
		}
		else
		{
			strncpy(data[i].key,key,99);
			data[i].atom=val;
			return true;
		}
	}
    bool Delete(const char *key)	// ɾ��һ���ڵ�
	{
		bool bOk=false;
		if(data==NULL||key==null)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)
			return false;
		if(strlen(data[i].key)>0)
		{
			DATA_TYPE *cursor=&data[i];
			while(cursor)
			{
				if (((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0) ||
					(!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key)))
				{
					if(cursor->next)
					{
						strncpy(cursor->key,cursor->next->key,99);
						cursor->atom=cursor->next->atom;
						DATA_TYPE *del_item=cursor->next;
						cursor->next=cursor->next->next;
						delete del_item;
					}
					else
						strcpy(cursor->key,"");
					bOk=true;
					break;
				}
				else
					cursor=cursor->next;
			}
			return bOk;
		}
		else
			return false;
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
				while(data[i].next)
				{
					DATA_TYPE *cursor=data[i].next;
					data[i].next=data[i].next->next;
					delete cursor;
				}
			}
		}
		delete[]data;
		m_nHashSize=0;
		data=NULL;
	}
    void EmptyKeys()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
				while(data[i].next)
				{
					DATA_TYPE *cursor=data[i].next;
					data[i].next=data[i].next->next;
					delete cursor;
				}
			}
			strcpy(data[i].key,"");
		}
	}
};
//�ַ�����ϣ����ģ����
template <class TYPE> class CHashStrListEx
{
//1.˽�г�Ա��������
	struct DATA_TYPE
	{
		TYPE atom;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		char key[100];
		bool bDeleted;
		DATA_TYPE(){
			prev=next=NULL;
			memset(key,0,100);
			bDeleted=false;
		}
	};
	CHashStrTableEx<TYPE*> hashTable;
	ULONG m_uHashTableGrowSize;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHashStrListEx()
	{// ���캯��
		NodeNum=0;
		m_uHashTableGrowSize=500;
		cursor = head = tail = NULL;
	}
    ~CHashStrListEx(){Empty();}

//2.���г�Ա��������
public:
	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
	void SetHashFunc(DWORD (*HashFunc)(const char *key, DWORD nHashSize))
	{hashTable.HashFunc=HashFunc;}
	//�ж�key�Ƿ���withKeyƥ�䣬�������withKey����׼������key��withKey����ƥ��
	void SetKeyMatchFunc(bool (*KeyMatchFunc)(const char* key, const char* szPartialMatchWithKey))
	{hashTable.KeyMatchFunc=KeyMatchFunc;}
//3.˽�г�Ա��������
private:
	void RebuildHashtable()
	{
		int hashSize;
		if(m_uHashTableGrowSize>0)
			hashSize=NodeNum-NodeNum%m_uHashTableGrowSize+m_uHashTableGrowSize;
		else if(NodeNum>0)
			hashSize=NodeNum+NodeNum;
		else
			hashSize=100;
		if(hashSize<50)
			hashSize=50;	//��С��ϣ���СΪ50;
		hashTable.Empty();
		hashTable.CreateHashTable(hashSize);
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(!cursor->bDeleted)
				hashTable.SetValueAt(cursor->key,&cursor->atom);
		}
		pop_stack();
	}
//4.���г�Ա��������
public:
	bool push_stack()
	{
		if(cursor)
		{
			POS_STACK.push(cursor);
			return true;
		}
		else
			return false;
	}
	bool pop_stack()
	{
		if(POS_STACK.pop(cursor))
			return true;
		else
		{
			cursor = NULL;
			return false;
		}
	}
	void SetHashTableGrowSize(ULONG growSize,bool bRebuildHashTbl=true){
		m_uHashTableGrowSize=growSize;
		if(bRebuildHashTbl)
			RebuildHashtable();
	}
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
	IXhEnumerator* NewEnumerator()
	{
		CXhInternalTemplEnumerator<void>* pInterEnum=(CXhInternalTemplEnumerator<void>*)IXhEnumerator::NewEnumerator();
		for(TYPE* pTypeObj=GetFirst();pTypeObj;pTypeObj=GetNext())
			pInterEnum->AppendObjPtr(pTypeObj);
		return pInterEnum;
	}
    TYPE* Add(const char *key)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key,true);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			strncpy(cursor->key,key,99);
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,&cursor->atom);
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		strncpy(ptr->key,key,99);
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &ptr->atom;
	}
    TYPE* SetValue(const char *key, TYPE obj)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key,true);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key,pObj);
			return pObj;
		}
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			strncpy(cursor->key,key,99);
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,&cursor->atom);
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->atom=obj;
		strncpy(ptr->key,key,99);
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &ptr->atom;
	}

    TYPE* GetValue(const char *key,bool blCompletelyMatch=false)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj,blCompletelyMatch);
		return pObj;
	}
    TYPE& operator[](const char *key)//���������Ż�ȡ�߶�ʵ��
	{
		return *GetValue(key);
	}
    TYPE* GetNext(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->next;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		if(cursor&&cursor->prev)
		{
	    	cursor = cursor->prev;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
		else
			return NULL;
	}
    TYPE* GetFirst(BOOL bIterDelete=FALSE)
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return NULL;
		}
	}
    TYPE* GetTail(BOOL bIterDelete=FALSE)
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
    char* GetCursorKey() const
	{
		if(cursor)
			return cursor->key;
		else
			return NULL;
	}
	//�������ݵõ���Ӧ��key wht 10-09-07
	bool GetKey(const TYPE *pObj,char *key)
	{
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(&temp->atom==pObj)
			{
				if(key)
					strcpy(key,temp->key);
				return true;
			}
		}
		return false;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(const char *key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&strcmp(cursor->key,key)==0)	//������ɾ���ڵ�
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
    BOOL DeleteCursor(BOOL bClean=FALSE)
	{
		if(cursor==NULL||cursor->bDeleted)
			return FALSE;
		else
		{
			hashTable.RemoveKey(cursor->key);
			if(bClean)
			{
				DATA_TYPE *cursor2=NULL;
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
					cursor2=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
						cursor2=head;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
						cursor2=tail;
					}
				}
				delete cursor;
				cursor=cursor2;
			}
			else 
				cursor->bDeleted=TRUE;
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
			if(NodeNum<0)
				NodeNum=0;
			return TRUE;
		}
	}
	BOOL IsCursorDeleted(){ return cursor->bDeleted;}
	void Clean()
	{
		for(cursor=head;cursor;)
		{
			DATA_TYPE *cursor2=cursor->next;
			if(cursor->bDeleted)
			{	//�ѱ�ɾ��Ԫ��,��λ�����
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
					}
				}
				hashTable.RemoveKey(cursor->key);
				delete cursor;
			}
			cursor=cursor2;
		}
	}
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			delete tail;
			tail = cursor;
		}
		hashTable.EmptyKeys();
		NodeNum=0;
		head = tail = NULL;
		cursor = NULL;
	}
	TYPE* ModifyKeyStr(const char *oldkey,const char *newkey)
	{
		if(strcmp(oldkey,newkey)==0)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
		TYPE* ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(strcmp(cursor->key,oldkey)==0)
			{
				hashTable.RemoveKey(cursor->key);
				strncpy(cursor->key,newkey,99);
				hashTable.SetValueAt(cursor->key,&cursor->atom);
				ret_obj_ptr=&cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	void EmptyStack(){POS_STACK.Empty();}
};
