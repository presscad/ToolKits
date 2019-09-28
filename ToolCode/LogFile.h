#pragma once
#include "stdio.h"
struct ILog2File{
	static const BYTE WARNING_LEVEL0_NONE		= 0;	//������κξ�ʾ��Ϣ
	static const BYTE WARNING_LEVEL1_IMPORTANT	= 1;	//�������Ҫ��ʾ��Ϣ
	static const BYTE WARNING_LEVEL2_COMMON		= 2;	//���һ�㼶�����ϵľ�ʾ��Ϣ
	static const BYTE WARNING_LEVEL3_UNIMPORTANT= 3;	//���������̫��Ҫ���ϵľ�ʾ��Ϣ
	static const BYTE WARNING_LEVEL_ALL			= 0xff;	//���ȫ����ʾ��Ϣ
	virtual bool SetCurrTempRelation(int iWarningType,long hRelaObj1=0,long hRelaObj2=0){return false;}
	//ciLowestWarningLevel,���Ϊ1������ֵԽ��ʾ�ȼ�Խ�� wjh-2016.6.19
	virtual void SetWarningLevel(BYTE ciLowestWarningLevel=WARNING_LEVEL_ALL)=0;
	virtual BYTE WarningLevel()=0;
	virtual void InsertBlankRow()=0;
	//virtual bool InitLogFile(char* fname);
	//virtual char* LogFilePath();
	virtual char* LatestLog()=0;
	virtual bool LevelLog(BYTE ciWarningLevel,const char *format,...)=0;
	virtual bool Log(const char *format,...)=0;
	//Log(char *format,...)�ὫĳЩ������Ϣ�е�'%'���ַ�ת��Ĩ�� wjh-2014.5.23
	virtual bool LogString(char *error, bool jumpToNextLine=true)=0;
	virtual bool ClearContents()=0;
	virtual bool IsHasContents()=0;
};
class CLogFile : public ILog2File
{
	FILE* m_fp;
	bool m_bHasContens,m_bInsertBlankRow,m_bInsertTitleRow;
	bool m_bAppendTimestamp;	//�Ƿ���ÿ��ĩβ׷��ʱ��� wjh-2017.10.8
	char file_name[200];
	void CloseFile();
	bool m_bInLifeCycle;
	BYTE m_ciLowestWarningLevel;		//��ʱ������־��¼
	bool m_bGlobalDisabled;	//ȫ��������־��¼
	char _internal_tmp_str[201];
	char _internal_titlestr[201];
	bool (*_FireReflectTo)(int iWarningType,long hRelaObj[2],const char* errmsg);
	int _iTempReflectWarningType;	//��ʱ�洢�����������ڽ���־��Ϣ���䵽��������
	long _hTempReflectRelaObj[2];	//��ʱ�洢�����������ڽ���־��Ϣ���䵽��������
private:
	friend class CLogErrorLife;
	bool IsInLifeCycle(){return m_bInLifeCycle;}
	void RegisterLifeCycle(){m_bInLifeCycle=true;}
	void UnRegisterLifeCycle(){m_bInLifeCycle=false;}
	void AttachReflector(bool (*_ReflectFunc)(int iWarningType,long hRelaObj[2],const char* errmsg));
	void UnAttachReflector();
public:
	CLogFile(char* fname=NULL,bool overwriteMode=true);
	~CLogFile(void);
	bool IsEnabled(){return !m_bGlobalDisabled;}
	void GlobalEnabled(bool enabled=true){m_bGlobalDisabled=!enabled;}
	bool IsTimestampEnabled(){return !m_bAppendTimestamp;}
	void EnableTimestamp(bool enabled=true){m_bAppendTimestamp=enabled;}
	virtual bool SetCurrTempRelation(int iWarningType,long hRelaObj1=0,long hRelaObj2=0);
	//ciLowestWarningLevel,���Ϊ1������ֵԽ��ʾ�ȼ�Խ�� wjh-2016.6.19
	virtual void SetWarningLevel(BYTE ciLowestWarningLevel=WARNING_LEVEL_ALL){m_ciLowestWarningLevel=ciLowestWarningLevel;}
	virtual BYTE WarningLevel(){return m_ciLowestWarningLevel;}
	void InsertBlankRow(){m_bInsertBlankRow=true;}
	void InsertTitleRow(const char* title);
	void CancelSuspendTitleRow(){m_bInsertBlankRow=false;}
	bool InitLogFile(char* fname,bool overwriteMode=true);
	char* LogFilePath();
	char* LatestLog(){return _internal_tmp_str;}
	virtual bool LevelLog(BYTE ciWarningLevel,const char *format,...);
	bool Log(const char *format,...);
	//Log(char *format,...)�ὫĳЩ������Ϣ�е�'%'���ַ�ת��Ĩ�� wjh-2014.5.23
	bool LogString(char *error, bool jumpToNextLine=true);
	bool ClearContents();
	bool IsHasContents(){return m_bHasContens;}
	void ShowToScreen();
};
extern CLogFile logto,logerr;

class CLogErrorLife
{
	CLogFile* m_pLogFile;
	bool m_bEnabled;
	bool m_bShowScreen;
	bool m_bRestoreWarningLevel;
	bool m_bRestoreEnableState,m_bLegacyEnableState;
	BYTE m_ciWarningLevel;
	bool m_bAttachReflector;
	void (*_FireStartTaskPanelFunc)();
public:
	bool EnableShowScreen(bool showscreen=true){return m_bShowScreen=showscreen;}
	CLogErrorLife(CLogFile* pLogFile=NULL,bool (*_ReflectFunc)(int iWarningType,long hRelaObj[2],const char* errmsg)=NULL,void (*StartTaskPanel)()=NULL);
	bool AttachReflector(bool (*_ReflectFunc)(int iWarningType,long hRelaObj[2],const char* errmsg));
	void DetachReflector();
	bool SetTemporyWarningLevel(BYTE ciLowestWarningLevel=CLogFile::WARNING_LEVEL_ALL);
	bool EnableExportLogTempory();
	~CLogErrorLife();
};
