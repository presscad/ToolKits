#ifndef __SERVER_TOWERTYPE_H_
#define __SERVER_TOWERTYPE_H_
#include "XhCharString.h"
#include "Buffer.h"
#include "HashTable.h"
#include "f_ent_list.h"
#include "list.h"
#include "MemberProperty.h"
#include "Markup.h"
#include "ServerObject.h"

class CServerTowerFile;
class CServerTowerFileGroup;
typedef void* (*DefCreateLocalTowerFileFunc)(CServerTowerFile *pServerTowerFile);
typedef void (*DefDestroyLocalTowerFileFunc)(void *pLocalTowerFile);
typedef void* (*DefCreateLocalFileGroupFunc)(CServerTowerFileGroup *pServerFileGroup);
typedef void (*DefDestroyLocalFileGroupFunc)(void *pLocalFileGroup);
class CServerTowerFileGroup;

#include "ProcessPart.h"
class CServerTowerPart :  public CServerObject
{
protected:
	BYTE _ciPartType;				//��������
	CProcessAngle angle;
	CProcessPlate plate;
	char* ppifiledata;
	UINT _uiPpiFileLength;
public://���л���ʶ����
	static const UINT SERIALIZE_LABELnID	= 0x01;	    //���͹�����ʶ��Ϣ
	static const UINT SERIALIZE_BASICNONEID	= 0x02;	    //���͹���������Ϣ��Id����ų��⣩
	static const UINT SERIALIZE_BASIC	 = 0x03;	    //���͹���������Ϣ
	static const UINT SERIALIZE_PROCESSDATA = 0x04;	    //�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_PROCESSFILEDATA = 0x08;	    //�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_TMA = 0x0B;				//�������ݷ����ļ���Ϣ
public:
	DECLARE_READONLY_PROPERTY(CProcessPart&,ppi);
	int m_idTowerType;
	int m_idSrvFileObj;	//PPI�ļ����ļ��������洢��ʵ�������ļ�Id;
	static const BYTE SYNC_NONE		= 0x00;
	static const BYTE SYNC_UPLOAD	= 0x01;
	static const BYTE SYNC_DOWNLOAD = 0x02;
	static const BYTE SYNC_JUSTIFY	= 0x04;
	BYTE m_cbSyncState;	//0x00����Ҫͬ����0x01��Ҫ�ϴ��°汾����������0x02�������и��°汾��Ҫ���ص����أ�0x04�������и��°汾��Ҫȷ�ϱ����Ƿ��޸ĺ�������ػ��ϴ�
	struct VERSION_SERIAL{
		UINT uiPpiVerSerial;
		UINT uiTidVerSerial;
		VERSION_SERIAL(){uiPpiVerSerial=uiPpiVerSerial=0;}
	}VerSerial;
	UINT m_uiCurrPpiVerSerial;
	UINT m_uiCurrTidVerSerial;
	CServerTowerPart(UINT key);
	CServerTowerPart(CProcessPart* pPPIPart=NULL);
	~CServerTowerPart();
	const char* get_PpiFileData(){return ppifiledata;};
	UINT get_PpiFileLength(){return _uiPpiFileLength;};
	__declspec(property(get=get_PpiFileLength)) UINT PpiFileLength;
	__declspec(property(get=get_PpiFileData)) const char* PpiFileData;
	bool ClonePart(CServerTowerPart *pNewPart);
	void InitByPPIPart(CProcessPart* pPPIPart);
	virtual void FromPPIBuffer(CBuffer &buffer);
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
typedef CServerTowerPart* CServerTowerPartPtr;
class CServerFile :  public CServerObject
{
public://���л���ʶ����
	static const UINT SERIALIZE_BASIC = 0x01;	//����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_FILEDATA = 0x02;//����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_ALL = 0x03;		//����ʹ�õ����ͻ�����Ϣ
public:
	CBuffer m_dataBuff;
	CXhChar200 m_sFileName;
	int m_idSrvFileObj;	//�ļ����ļ��������洢��ʵ�������ļ�Id;
	bool m_bHasPhysicalFile;
	DWORD dwVersion;
	CXhChar50 m_sDescription;
	CServerFile();
	virtual bool ExtractFromXmlStr(CMarkup& xml);
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};

class CTidFile : public CServerFile
{
public:
	int m_idTowerType;
	CTidFile();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};	
class CTapShellFile : public CServerFile
{
public:
	int m_idTowerType;
	CTapShellFile();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
class CServerTowerFile : public CServerFile
{
	friend CServerTowerFileGroup;
	void *m_pLocalTowerFile;
public:
	static DefCreateLocalTowerFileFunc CreateLocalTowerFileFunc;
	static DefDestroyLocalTowerFileFunc DestroyLocalTowerFileFunc;
public:
	CServerTowerFile();
	~CServerTowerFile();
	int m_idTowerType;
	int m_idFileGroup;
	CServerTowerFileGroup *m_pBelongFileGroup;
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void* GetLocalTowerFile();
	void* BelongModel();
	BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
typedef CServerTowerFile* CServerTowerFilePtr;
class CServerTowerType;
class CServerTowerFileGroup : public CServerObject
{
	void *m_pLocalFileGroup;
	CServerTowerFile* m_pActiveFile;
public:
	static DefCreateLocalFileGroupFunc CreateLocalFileGroupFunc;
	static DefDestroyLocalFileGroupFunc DestroyLocalFileGroupFunc;
public://���л���ʶ����
	static const UINT SERIALIZE_BASIC = 0x01;	        //����ʹ�õ����ͻ�����Ϣ
	static const UINT OPER_USER = 0x02;					//�������ݷ����ļ���Ϣ
	static const UINT OPER_LOCALTAPID = 0x04;					//�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_ATTACHFILES = 0x08;	    //�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_TMA = 0x0B;	            //����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TAP = 0x0D;	            //����ʹ�õ����ͻ�����Ϣ
public:
	CServerTowerFileGroup();
	~CServerTowerFileGroup();
	CXhChar200 m_sName;
	CXhChar200 m_sDescription;
	CXhChar200 m_sSegStr;		//�κŷ�Χ
	CXhChar200 shareSegIncStr;		//����ļ���ͬ��Ч�Ķκ�(�����ڲ�ͬ�������ű���ͬһ�κ����)
	CXhChar200 priorPartLabelStr;	//�ļ��в�����Ч�κŷ�Χ���ض������嵥�ַ���
	UINT m_idActiveFile;		//�����ļ���Զ�̷��������ݿ�Id
	UINT m_uiActFileVerSerial;	//��ǰ�����ļ��İ汾��ˮ��
	UINT m_localIdInTapModel;
	DECLARE_READONLY_VIRTUAL_PROPERTY(CServerTowerFile* ,pActiveFile);
	
	int m_idTowerType;
	CServerTowerType *m_pBelongTowerType;
	//
	CXhChar50 segIncStr;		//����ַ���
	//CHashList<PRIOR_PART> hashPriorPartByPartId;
	CHashListEx<CServerTowerFile> hashTowerFiles;//��ֵ:File.Id;

	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void ToBufferForPrjTowerType(CBuffer &buffer);
	void FromBufferForAssemblyTowerType(CBuffer &buffer);
	void* GetLocalFileGroup();
	void* BelongModel();
	BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
class CAssemblyHeight : public CServerFile
{
public://���л���ʶ����
	static const UINT SERIALIZE_BASIC	 = 0x01;	    //���͹���������Ϣ
	static const UINT SERIALIZE_INFOFILEDATA = 0x02;	    //�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_ALL = 0x03;	    //�������ݷ����ļ���Ϣ
public:
	int m_idTowerType;
	int m_idTidFile;
	UINT localIdInTapModel;
	CServerTowerType *m_pBelongTowerType;
	void *m_pTowerInstance;	//��������
	CXhChar16 placeName;
	CXhChar50 heightName;
	CXhChar100 segIncStr;
	double m_fAntiTheftHeight;
	int m_nManuCount;	//�ӹ�����
	int m_iFoundationStyle;
	CAssemblyHeight();
	~CAssemblyHeight();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
class CTapModel : public CServerObject
{
	CServerTowerType* m_pMapSrvTowerType;
public://���л���ʶ����
	static const UINT SERIALIZE_BASIC = 0x01;		    //����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TIDFILE = 0x02;			//����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TAPSHELLFILE = 0x04;    //����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TOWERFILES = 0x08;	    //�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_ASSEMBLYHEIGHT = 0x10;	//����װ����λ����ʵ����Ϣ
	static const UINT SERIALIZE_TAP_PREFER = 0x1F;		//����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TIDFILE_DATA = 0x20;			//����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TAPSHELLFILE_DATA = 0x40;		//����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TOWERFILES_DATA = 0x80;			//�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_ASSEMBLYHEIGHT_DATA = 0x0100;	//����װ����λ����ʵ����Ϣ
public:
	CTapModel(CServerTowerType* pMapSrvTowerType);
	CXhChar50 GetServerClassName(){return m_sServerClassName;}
	DECLARE_READONLY_VIRTUAL_PROPERTY(CServerTowerType* ,pSrvTowerType);
	//
	CTidFile tidfile;
	CTapShellFile shellfile;
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
class CServerTowerType :  public CServerObject
{
public://���л���ʶ����
	static const UINT SERIALIZE_BASIC = 0x01;	        //����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_FILEGROUP = 0x02;	    //�������ݷ����ļ���Ϣ
	static const UINT SERIALIZE_ASSEMBLYHEIGHT = 0x04;	//����װ����λ����ʵ����Ϣ
	static const UINT SERIALIZE_TMA = 0x03;	            //����ʹ�õ����ͻ�����Ϣ
	static const UINT SERIALIZE_TAP = 0x07;	            //����ʹ�õ����ͻ�����Ϣ
public:
	int state;	//1.�ڷ�������;2.����������;3.�Ѵ浵����
	int m_idBelongProject;	//��������
	CServerTowerType();
	CXhChar200 m_sName;
	CXhChar200 m_sProjName;
	CTidFile m_tidFile;
	ATOM_LIST<CServerTowerFileGroup> m_towerFileGroupList;
	//CAssemblyHeight
	//CXhChar16 placeName;
	//CXhChar50 heightName;
	//CXhChar100 segIncStr;
	CXhSimpleList<CAssemblyHeight> listAssemblyHeights;
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true, DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void* BelongModel(){return NULL;}
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
class CServerManuTask : public CServerObject
{
public:
	CServerManuTask();
	CXhChar200 m_sName;
	CHashListEx<CServerTowerType> hashTowerTypes;
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true, DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
class CServerContents
{
public:
	CServerTowerFile* m_pActiveTowerFile;
	CServerTowerType* m_pActiveTowerType;
public:
	CHashListEx<CServerTowerType> hashTowerTypes;
	CHashListEx<CServerManuTask> hashManuTasks;
	CServerContents();
	void ParseAndUpdateTowerTypesFromBuffer(CBuffer &buffer,int iStateTowerType=0,DWORD dwSerialzeFlag=0);
	void ActivateTowerFile(CServerTowerFile *pTowerFile);
	void ActivateTowerType(CServerTowerType *pTowerType);
	void ParseProcessPartsFromBuffer(CBuffer &buffer,CHashListEx<CProcessPart>* pHashProcessParts);
	void ParseManuTasksFromBuffer(CBuffer &buffer,CHashListEx<CServerManuTask>* pHashTasks);
	CServerTowerFile *GetActiveTowerFile();
	CServerTowerType *GetActiveTowerType();
};
extern CServerContents AgentServer;
template <class TYPE,class TYPE2> class CServerContent
{
	TYPE2 *m_pActiveObj;
public:
	CServerContent(){m_pActiveObj=NULL;}
	~CServerContent(){m_pActiveObj=NULL;}

	ATOM_LIST<TYPE> m_objList;
	TYPE2 *GetActiveObj(){return m_pActiveObj;}
	TYPE2 *ActivateObj(TYPE2 *pObj)
	{
		m_pActiveObj=pObj;
		return m_pActiveObj;
	}
	void FromBuffer(CBuffer &buffer)
	{
		m_pActiveObj=NULL;
		m_objList.Empty();
		int nCount=0;
		buffer.ReadInteger(&nCount);
		for(int i=0;i<nCount;i++)
		{
			TYPE *pObj=m_objList.append();
			pObj->FromBuffer(buffer);
		}
	}
};

class CServerAssemblePart :  public CServerObject
{
public:
	CServerAssemblePart(){m_sServerClassName="AssemblePart";};
	~CServerAssemblePart();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true, DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};

#endif