#pragma once
#include ".\Hashtable.h"
class CObjNoManager;
class CObjNoGroup
{
	friend class CObjNoManager;
	long m_idInit;	//��ʼid(����С��idֵ),Ĭ��Ϊ1
	long m_idGroup;
	long m_nMaxNo;
	CHashList<long>list;
public:
	CObjNoGroup(int nMaxNo=0);
	void InitMaxNo(unsigned int nMaxNo);
	int EnumIdleNo();
	void SetNoState(int iNo,BOOL bInUse=TRUE);
	long GetUsedNoCount();
	long GetFirstUsedNo();
	long GetNextUsedNo();
public:
	void EmptyNo();
};
class CObjNoManager
{
	CHashList<CObjNoGroup>hashNoGroup;
public:
	CObjNoManager();
	CObjNoGroup* AppendGroup(long ID,long nMaxNo);
	CObjNoGroup* FromGroupID(long ID);
	void EmptyGroupNo();
	void EmptyGroup();
};
