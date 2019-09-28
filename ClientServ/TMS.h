#pragma once

#ifdef __CONNECT_REMOTE_SERVER_
#include "Buffer.h"
#include "soapH.h"
#include "soapTMSServiceSoapProxy.h"
#include "XhCharString.h"
#include "list.h"
#include "HashTable.h"

class CTmsServer
{
public:
	int m_idSession;
	TMSServiceSoapProxy* m_pServer;
	struct VERSION_SERIAL{
		BYTE activePpi0Tid1;	//0.ppiΪ���� 1.TidΪ���� 2.��ͬ��
		UINT uiPpiVerSerial;
		UINT uiTidVerSerial;
		VERSION_SERIAL(){activePpi0Tid1=uiPpiVerSerial=uiPpiVerSerial=0;}
	};
	CHashStrList<VERSION_SERIAL> hashPartsVerSerial;
public:
	CTmsServer(void);
	~CTmsServer(void);
public:
	bool InitTowerPartsVersionSerial(int towerTypeId);
	//clsName��ЧֵΪ:"TowerTypeFileGroup","TowerTypeFile","TowerAssembly","TowerPart","AssemblePart",
	bool QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff);
	bool DownloadObjects(const char* clsName, int* idarr, int countOfObj=1,UINT serialize=0,BUFFER_IO* pExterLocalBuff=NULL);
	bool DownloadFileById(const char* clsName,int fileId,BUFFER_IO* pExterLocalBuff);
 	bool DeleteObject(const char* clsName,int objId);
	int  SaveOrUpdateObject(const char* clsName,void* pLocalObject,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);	//serialize=0��ʾȡĬ��ֵ
	bool SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj=1,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);
	bool SaveOrUpdateObjects(const char* clsName,IXhEnumerator* pIter,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);
	bool MatchTowerParts(IXhEnumerator* pIter,int towerTypeId,DWORD serialize=0);
public:	//�ļ��ϴ�����
	/// <summary>
	/// �����ļ�
	/// </summary>
	/// <param name="sessionId"></param>
	/// <param name="clsName"></param>
	/// <param name="byteArr"></param>
	/// <returns></returns>
	bool UploadFileObject(int idFileObj, BUFFER_IO* pFileDataBuff,bool overwrite=true, bool compressed=false,char* errorMsg=NULL);
	bool UploadFileObject(int idFileObj, int startposition,CBuffer* pFileDataBuff, bool compressed=false);
	bool FinalizeFileObjectData(int idFileObj, int file_length, bool isOverriteFile,char* errorMsg=NULL);
	//����һ��ָ������Tap����װģ�͵�����Զ���ļ������ṩ�߶��󣬷���ѹ��ǰ���������ļ��������ֽ��ܳ���
	UINT OpenTapModelDataProvider(int idTowerType,int* pVirtualFileObjId, bool compressed=false);
	//����һ��ָ��ServerObj������Զ���ļ������ṩ�߶��󣬷���ѹ��ǰ���������ļ��������ֽ��ܳ���
	UINT OpenServerObjectDataProvider(int idObject,const char* cls_name,int* pVirtualFileObjId, bool compressed=false);
	//����һ��Զ���ļ������ṩ�߶��󣬷���ѹ��ǰ�����ļ��������ֽ��ܳ���
	UINT OpenFileObjectDataProvider(int idFileObj, bool compressed);
	UINT DownloadFileObject(int idFileObj, int startposition, int download_size, bool compressed,BUFFER_IO* pFileDataBuff);
	UINT DownloadFileObject(int idFileObj, bool compressed,BUFFER_IO* pFileDataBuff);
	bool CloseFileObjectDataProvider(int idFileObj);
public:
	void SetServerUrl(const char* url);
	bool LoginUser(const char* userName, const char* password, const char* fingerprint=NULL);
	bool DownloadTowerTypeFile(void* pTowerTypeFile);
	bool QueryStateTowerTypes(BUFFER_IO* pOutLocalBuff,int state=1,int fromdate=0,int todate=0,DWORD dwSerialzeFlag=0,bool bFilterByUserId=true);
	int  QueryProcessParts(BUFFER_IO* pOutLocalBuff,int towerTypeId,CXhChar16* arrLabels,int labelCount=1);
	bool GetObjectsVersion(const char* clsName, int* idarr, int countOfObj=1,UINT* ver_arr=NULL);
	bool QueryStateManuTasks(BUFFER_IO* pOutLocalBuff,int state=1);
	//
	bool UploadServerFile(void* pServerFile,const char* file_cls_name,BUFFER_IO *pSrcBuff,bool overwrite=true);
	bool DownloadServerFile(void* pServerFile,BUFFER_IO* pDestBuff=NULL);
};
extern CTmsServer TMS;
//extern SOAP_NMAC struct Namespace namespaces[];
#endif
