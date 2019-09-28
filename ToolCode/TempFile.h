#pragma once
#include ".\Buffer.h"
#include ".\ArrayList.h"

class CTempFileBuffer : public BUFFER_IO
{
	long m_nInitBuffPoolSize;
	bool m_bInitBuffPool;	//�Ƿ���Ϊ��ʱ�ļ�������˻����ڴ���
	bool InitBuffPool();
protected:
	FILE *m_fp;
	ULONG m_uFilePosition;
	char m_filename[201];
	ULONG m_uBufferLength;		//��ǰ�ļ������С
	bool m_bEnableAutoDelete;
	bool m_bExternalFile;
	DYN_ARRAY<char>BUFF_POOL;
	friend class CTempFilePositionStack;
public:	//����̳к���
	virtual DWORD Read(void *pch, DWORD size);
	virtual DWORD Write(const void *pch,DWORD size);
	virtual bool SeekToBegin();
	virtual bool SeekToEnd();
	virtual bool SeekPosition(DWORD pos);
	virtual bool SeekOffset(DWORD offset);
	virtual DWORD GetCursorPosition(){return m_uFilePosition;}
	virtual DWORD GetLength(){return m_uBufferLength;}
	void ClearContents();	//ֻ������ݣ���������棬��ֻ��file_len��0
public:
	CTempFileBuffer(FILE* fp=NULL);
	CTempFileBuffer(const char* fname);
	virtual ~CTempFileBuffer();
	bool CreateFile(const char *filename,int nBufSize=0);
	bool InitFile(FILE* fp);
	long GetInitBuffSize(){return m_nInitBuffPoolSize;}
	long GetActualBuffSize();	//��ǰ�ļ�ʵ�ʻ����С
	BOOL IsValidFile(){return m_fp!=NULL;}
	int GetFileName(char* filename);
	void CloseFile();
	bool EnableAutoDelete(bool enabled=true){return m_bEnableAutoDelete=enabled;}
	BOOL DeleteFile();	//ɾ����ʱ�ļ�
	bool FlushTmpFile();
};
