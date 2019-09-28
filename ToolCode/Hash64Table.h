#pragma once

typedef unsigned __int64    XHUINT64;
union DUALKEY{
	struct{DWORD lowId,highId;};
	XHUINT64 i64DualKey;
	DUALKEY(DWORD low,DWORD high,bool sortkey=false)
	{
		lowId =sortkey?max(high,low):low;
		highId=sortkey?min(high,low):high;
	}
	DUALKEY(XHUINT64 id64=0){i64DualKey=id64;}
	XHUINT64 operator =(XHUINT64 id64){return i64DualKey=id64;}
	operator XHUINT64(){return i64DualKey;}
};
//Hash��ģ����
template <class TYPE> class CHash64Table
{
//1.˽�г�Ա��������
	struct DATA_TYPE
	{
		XHUINT64 key;
		TYPE atom;
		DATA_TYPE *next;
	};
public:
    CHash64Table(DWORD HashTblSize=0)
	{// ���캯��
		HashFunc=NULL;
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
	}
    ~CHash64Table(){Empty();}
	bool CreateHashTable(DWORD HashTblSize)
	{
		if(m_nHashSize>0)
			Empty();	//����������ٴ���
		m_nHashSize=HashTblSize;
		if(data!=NULL)
			Empty();
		if(m_nHashSize>0)
		{
			if((data = new DATA_TYPE[m_nHashSize])==NULL)
				return false;
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
		return true;
	}
    DWORD m_nHashSize;	// Hash�������洢TYPEԪ�صĸ�������Hash��Ĵ�С��
    DATA_TYPE* data;// Hash����ָ������ݵ�ָ��

//2.���г�Ա��������
public:
	DWORD (*HashFunc)(XHUINT64 key,DWORD nHashSize);
//3.˽�г�Ա��������
private:
	DWORD Hash(XHUINT64 key)const	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
	{
		if(HashFunc)
			return HashFunc(key,m_nHashSize);
		else
		{
			if(m_nHashSize>0)
			{
				if(key<m_nHashSize)
					return (DWORD)key;
				else
					return (DWORD)(key%m_nHashSize);
			}
			else
				return 0;
		}
	}

//4.���г�Ա��������
public:
	DWORD GetHashTableSize()const{return m_nHashSize;}
	DWORD GetMaxDepth(DWORD* idle_count=NULL,DWORD* extra_count=NULL)
	{
		int nMaxDepth=0;
		DWORD count1=0,count2=0;
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			DATA_TYPE *cursor=&data[i];
			if(cursor->key==0)
				count1++;
			int nTmpDepth=0;
			while(cursor)
			{
				cursor=cursor->next;
				if(cursor)
					nTmpDepth++;
			}
			count2+=nTmpDepth;
			if(nTmpDepth>nMaxDepth)
				nMaxDepth=nTmpDepth;
		}
		if(idle_count)
			*idle_count=count1;
		if(extra_count)
			*extra_count=count2;
		return nMaxDepth;
	}
	bool ContainsKey(XHUINT64 key)
	{
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
				return true;
			else
				cursor=cursor->next;
		}
		return false;
	}
	bool GetValueAt(XHUINT64 key,TYPE &val)
	{
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
			{
				val=cursor->atom;
				return true;
			}
			else
				cursor=cursor->next;
		}
		return false;
	}
    bool SetValueAt(XHUINT64 key,TYPE val)//���������Ż�ȡ�߶�ʵ��
	{
		bool bOk=false;
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)		//������Ĺ�ϣֵ����ϣ��δ��ʼ��ʱ������0
			return false;
		if(data[i].key!=0)
		{
			if(data[i].key==key)
			{
				data[i].atom = val;
				return true;
			}
			else
			{
				DATA_TYPE *tail_item=&data[i];
				DATA_TYPE *cursor=data[i].next;
				while(cursor)
				{
					tail_item=cursor;
					if(cursor->key==key)
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
					tail_item->next->key=key;
					tail_item->next->atom=val;
				}
				return true;
			}
		}
		else
		{
			data[i].key=key;
			data[i].atom=val;
			return true;
		}
	}
	bool RemoveKey(XHUINT64 key)
	{
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
			return false;
		DATA_TYPE *prev=NULL,*cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
			{
				if(prev==NULL)
				{
					if(data[i].next)
					{	//��ǰ�ڵ�֮�������������ʱ��ֱ����յ�ǰ�ڵ㽫�������ݶ�ʧ wht 12-08-30
						data[i].key=data[i].next->key;
						data[i].atom=data[i].next->atom;
						DATA_TYPE *del_item=data[i].next;
						data[i].next=data[i].next->next;
						delete del_item;
					}
					else
					{
						data[i].key=0;
						memset(cursor,0,sizeof(DATA_TYPE));
					}
					return true;
				}
				else
				{
					prev->next=cursor->next;
					delete cursor;
					return true;
				}
			}
			else
			{
				prev=cursor;
				cursor=cursor->next;
			}
		}
		return false;
	}
    bool Delete(XHUINT64 key)	// ɾ��һ���ڵ�
	{
		bool bOk=false;
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)
			return false;
		if(data[i].key!=0)
		{
			if(data[i].key==key)
			{
				if(data[i].next)
				{
					data[i].key=data[i].next->key;
					data[i].atom=data[i].next->atom;
					DATA_TYPE *del_item=data[i].next;
					data[i].next=data[i].next->next;
					delete del_item;
				}
				else
					data[i].key=0;
				return true;
			}
			else
			{
				DATA_TYPE *cursor=data[i].next;
				while(cursor)
				{
					if(cursor->key==key)
					{
						if(cursor->next)
						{
							cursor->key=cursor->next->key;
							cursor->atom=cursor->next->atom;
							DATA_TYPE *del_item=cursor->next;
							cursor->next=cursor->next->next;
							delete del_item;
						}
						else
							cursor->key=0;
						bOk=true;
						break;
					}
					else
						cursor=cursor->next;
				}
				return bOk;
			}
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
			data[i].key=0;
		}
	}
};
//����ģ����
template <class TYPE> class CHash64List
{
protected:
	XHUINT64 unique_id;
	XHUINT64 m_dwMinmalId;
	typedef struct tagDATA_TYPE
	{
		TYPE atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		XHUINT64 key;
		BOOL bDeleted;
		tagDATA_TYPE(){key=0;bDeleted=FALSE;}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHash64Table<TYPE*> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHash64List()
	{// ���캯��
		unique_id=0;
		m_dwMinmalId=1;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
		AllocateUniqueIdFunc=NULL;
	}
    ~CHash64List(){Empty();}

//2.���г�Ա��������
public:
	void SetHashFunc(DWORD (*hash_func)(XHUINT64 key,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	XHUINT64 (*AllocateUniqueIdFunc)(void* pContext);						//��ѡ���趨�ص�����
	void (*LoadDefaultObjectInfo)(TYPE *pObj,XHUINT64 key);
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
			unique_id=max(unique_id,cursor->key);
			if(!cursor->bDeleted)
				hashTable.SetValueAt(cursor->key,&cursor->atom);
		}
		pop_stack();
	}
	//��ȡһ���µľֲ�Ψһ��ʶ���ڵ�ǰ��ϣ�����ڣ�
	XHUINT64 GetLocalUniqueId()
	{
		XHUINT64 testid=m_dwMinmalId;
		DATA_TYPE* indicator=tail;
		while(testid<10000000){	//��10000000��Ϊ�˷�ֹ��һ����ʱ������ѭ��
			if(hashTable.GetHashTableSize()>0&&hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//��ǰtestid����Ψһ��ʶ
				break;
		}
		return testid;
	}
//4.���г�Ա��������
public:
	void SetMinmalId(XHUINT64 minId){m_dwMinmalId=minId;}
	XHUINT64 AllocateUniqueId()
	{
		XHUINT64 id=0;
		if(AllocateUniqueIdFunc==NULL)
			id=GetLocalUniqueId();
		if(id>0)
			return id;
		return AllocateUniqueIdFunc(this);
	}
	//��ʱ��GetNewId��������������Ŀ����Ϊ�˱�������дʱ������ʾ��GetNext�������ţ������ú�����������ܱ�AllocateUniqueIdȡ��
	XHUINT64 NewUniqueId(){
		unique_id++;
		return unique_id;
	}
	TYPE* ModifyKeyId(XHUINT64 oldkey, XHUINT64 newkey)
	{
		if(newkey==oldkey)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
		TYPE* ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(cursor->key==oldkey)
			{
				hashTable.RemoveKey(cursor->key);
				cursor->key=newkey;
				hashTable.SetValueAt(cursor->key,&cursor->atom);
				ret_obj_ptr=&cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	int push_stack()
	{
		if(cursor)
			return POS_STACK.push(cursor);
		else
			return FALSE;  //0
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
 	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ��ӣ��ú��������ڰ������������ wjh-2014.8.25
    TYPE* insert(const TYPE &obj,long ii=-1,XHUINT64 key=0)
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			return pObj;
		}

		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;
		ptr->atom = obj;

		if(NodeNum<=0)//������
		{
    		cursor = head = tail = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
		}
		else if(ii < 0)//�Ե�ǰ�ڵ�λ��Ϊ�½ڵ����λ��
		{
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//ԭ��ǰ�ڵ㴦��
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else	//cursorԭΪ�׽ڵ�
				head = ptr;
			cursor->prev = ptr;
		}
		else if(ii>=0&&ii<(long)NodeNum)//ָ�������λ��
		{
			int i=0;
			for(cursor=head;cursor;cursor=cursor->next,i++)
			{
				if(cursor->bDeleted)
				{
					i--;
					continue;
				}
				else if(i==ii)
					break;
			}
        		//��������ڵ���������ԭ�������ӹ�ϵ
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//����λ�ô�ԭ�ڵ㴦��
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else if(cursor->prev==NULL)//ָ�������ԭΪ�����׽ڵ�
				head = ptr;
			cursor->prev = ptr;
		}
		else //(ii>=NodeNum)
		{
  			//GetTail();//cursor is equal to tail now.   --00.05.25
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
		}
		cursor=ptr;
		NodeNum++;
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&ptr->atom,key);
		unique_id=max(unique_id,ptr->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&ptr->atom);
		return &ptr->atom;
	}
	TYPE* Append(TYPE obj,XHUINT64 key=0)//�ڽڵ������ĩβ��ӽڵ�
	{
		if(key>0)
			return SetValue(key,obj);
		else
			return SetValue(key,obj,true);
	}
    TYPE* Add(XHUINT64 key)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&cursor->atom,key);
		unique_id=max(unique_id,cursor->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &cursor->atom;
	}
	//allocZeroKey=trueʱ�����key=0ϵͳ���Զ�����һ��Ψһ��ʶkey��allocZeroKey=falseʱ����ֱ�ӷ��ؿ�
    TYPE* SetValue(XHUINT64 key, TYPE obj,bool allocZeroKey=false)//�ڽڵ������ĩβ��ӽڵ�
	{
		if(key==0&&!allocZeroKey)	//��ΪHashtable��֧��keyֵΪ0,����key����Ϊ0 wjh-2013.2.23
			return NULL;
		TYPE *pObj=NULL;
		if(key==0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			//��ֹ����SetValueʱ��ֵ�ظ�����obj.keyδ��ʼ����һ����������쳣���� wjh-2016.9.12
			if(LoadDefaultObjectInfo)
				LoadDefaultObjectInfo(pObj,key);
			return pObj;
		}
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->atom=obj;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&cursor->atom,key);
		unique_id=max(unique_id,cursor->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &cursor->atom;
	}

    TYPE* GetValue(XHUINT64 key)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
    TYPE& operator[](XHUINT64 key)
	{	//ʹ��[]ʱ�޷����ݷ���ֵ�ж��Ƿ��ҵ���Ӧ�Ĵ洢���ݣ�Ӧ����
		//�磺TYPEΪָ�����ͣ�����һ�������ڵ�keyʱ�᷵��һ�������ָ��,������NULL wht 12-09-19
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
		/*if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return &cursor->atom;	//����һ���ڵ�Ϊĩ�ڵ����ѱ���Ϊɾ����־ʱ������쳣 wjh-2013.10.12
		}*/
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
    XHUINT64 GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//�������ݵõ���Ӧ��key wht 10-09-07
	XHUINT64 GetKey(const TYPE *pObj)
	{
		XHUINT64 key=0;
		DATA_TYPE *temp=((DATA_TYPE*)((BYTE*)pObj - offsetof(DATA_TYPE, atom)));
		if(temp!=NULL&&GetValue(temp->key)==pObj)
			return temp->key;	//Ŀǰ���ַ����ݲ����ж�pObj�Ƿ������wjh-2014.7.18
		/*
		for(temp=head;temp;temp=temp->next)
		{
			if(&temp->atom==pObj)
			{
				key=temp->key;
				break;
			}
		}*/
		return key;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(XHUINT64 key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//������ɾ���ڵ�
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
		unique_id=0;
	}
	void EmptyStack(){POS_STACK.Empty();}
};

template <class TYPE> void SetHash64ObjKey(TYPE *pObj,XHUINT64 key){pObj->SetKey(key);}
template <class TYPE> class CHash64ListEx : public CHash64List<TYPE>{
public:
	CHash64ListEx(){
		LoadDefaultObjectInfo=SetHash64ObjKey;
	}
};
//����ģ����������ָ�뼯�ϣ����漰�����������ڣ������CHashList������ֱֵ�Ӽ�ΪTYPE_PTR
template <class TYPE_PTR> class CHash64Set
{
	XHUINT64 unique_id;
	typedef struct tagDATA_TYPE
	{
		TYPE_PTR atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		XHUINT64 key;
		BOOL bDeleted;
		tagDATA_TYPE(){key=0;bDeleted=FALSE;}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHash64Table<TYPE_PTR> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHash64Set()
	{// ���캯��
		unique_id=0;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
	}
    ~CHash64Set(){Empty();}

//2.���г�Ա��������
public:
	void SetHashFunc(DWORD (*hash_func)(XHUINT64 key,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	void (*LoadDefaultObjectInfo)(TYPE_PTR pObj,XHUINT64 key);
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
			unique_id=max(unique_id,cursor->key);
			if(!cursor->bDeleted)
				hashTable.SetValueAt(cursor->key,cursor->atom);
		}
		pop_stack();
	}
//4.���г�Ա��������
public:
	//��ʱ��GetNewId��������������Ŀ����Ϊ�˱�������дʱ������ʾ��GetNext�������ţ������ú�����������ܱ�AllocateUniqueIdȡ��
	XHUINT64 NewUniqueId(){
		unique_id++;
		return unique_id;
	}
	TYPE_PTR ModifyKeyId(XHUINT64 oldkey, XHUINT64 newkey)
	{
		if(newkey==oldkey)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
		TYPE_PTR ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(cursor->key==oldkey)
			{
				hashTable.RemoveKey(cursor->key);
				cursor->key=newkey;
				hashTable.SetValueAt(cursor->key,cursor->atom);
				ret_obj_ptr=cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	int push_stack()
	{
		if(cursor)
			return POS_STACK.push(cursor);
		else
			return FALSE;  //0
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
    TYPE_PTR SetValue(XHUINT64 key, TYPE_PTR obj)//�ڽڵ������ĩβ��ӽڵ�
	{
		if(key==0)	//��ΪHashtable��֧��keyֵΪ0,����key����Ϊ0 wjh-2013.2.23
			return NULL;
		TYPE_PTR pObj=GetValue(key);
		if(pObj!=NULL)
		{
			pObj=obj;
			return pObj;
		}
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->atom=obj;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(cursor->atom,key);
		unique_id=max(unique_id,cursor->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->atom);
		return cursor->atom;
	}

    TYPE_PTR GetValue(XHUINT64 key)
	{
		TYPE_PTR pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
    TYPE_PTR GetNext(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->next;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
			return cursor->atom;
		/*if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->next;
			}
			return cursor->atom;	//����һ���ڵ�Ϊĩ�ڵ����ѱ���Ϊɾ����־ʱ������쳣 wjh-2013.10.12
		}*/
		else
			return NULL;
	}
    TYPE_PTR GetPrev(BOOL bIterDelete=FALSE)
	{
		if(cursor&&cursor->prev)
		{
	    	cursor = cursor->prev;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
		else
			return NULL;
	}
    TYPE_PTR GetFirst(BOOL bIterDelete=FALSE)
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->next;
			}
			return NULL;
		}
	}
    TYPE_PTR GetTail(BOOL bIterDelete=FALSE)
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE_PTR GetCursor()
	{
		if(cursor)
			return cursor->atom;
		else
			return NULL;
	}
    DWORD GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//�������ݵõ���Ӧ��key wht 10-09-07
	DWORD GetKey(const TYPE_PTR pObj)
	{
		DWORD key=0;
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(temp->atom==pObj)
			{
				key=temp->key;
				break;
			}
		}
		return key;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(XHUINT64 key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//������ɾ���ڵ�
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
		unique_id=0;
	}
	void EmptyStack(){POS_STACK.Empty();}
};
