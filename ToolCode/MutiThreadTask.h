// MutiThreadTask.h: interface for the CMutiThreadTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTITHREADTASK_H__529550A0_C82C_42D5_B57C_6033D415C69C__INCLUDED_)
#define AFX_MUTITHREADTASK_H__529550A0_C82C_42D5_B57C_6033D415C69C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxmt.h"

class CMutiThreadTask  
{
	CEvent idleEvent;
	CCriticalSection lock;
	typedef struct tagDATA_TYPE
	{
		tagDATA_TYPE *next;
		void *task;
	}DATA_TYPE;
	DATA_TYPE *head;
	DATA_TYPE *tail;
	HANDLE m_hThread;
	bool m_bThreadRunning;
	void Start();
public:
	void *m_pContext;	//����������
	bool m_bWaitOnIdle;
	int hits;
	CMutiThreadTask();
	virtual ~CMutiThreadTask();
	DWORD (WINAPI *TaskFunc)(LPVOID lpParam);	//ָ������ִ�к�����ָ��
	void Wait();	//�������߳��ڵ���
	void Awake();
	void OverRunningState(){m_bThreadRunning=false;}
	void AddTask(void *Task);
	void* GetHeadTask();
	HANDLE GetThreadHandle(){return m_hThread;}
	void RemoveHead();
};

#endif // !defined(AFX_MUTITHREADTASK_H__529550A0_C82C_42D5_B57C_6033D415C69C__INCLUDED_)
