#pragma once
#include "f_ent.h"
#include "XhLdsLm.h"
#include "XhCharString.h"
#include "I_DrawSolid.h"
#include "PartLib.h"
#include "HashTable.h"

class CDataSection
{
protected:
	DWORD m_dwZeroAddrOffset;	//����������ʼ����������ڴ��еĵ�ַƫ��λ��
	DWORD m_dwBufSize;
	char* m_data;
	CXhChar16 m_sVersion;
public:
	CDataSection(){m_dwBufSize=0;m_data=NULL;m_dwZeroAddrOffset=0;}
	char* BufferPtr(){return m_data;}
	DWORD BufferLength(){return m_dwBufSize;}
	DWORD ZeroAddrOffset(){return m_dwZeroAddrOffset;}
	//�������ӷ����ڵ���Ե�ַת�����ļ��洢�ռ��ȫ�ֵ�ַ
	DWORD AddrLtoG(DWORD addr){return addr+m_dwZeroAddrOffset;}
	//�����ļ��洢�ռ��ȫ�ֵ�ַת�������ӷ����ڵ���Ե�ַ
	DWORD AddrGtoL(DWORD addr){return addr-m_dwZeroAddrOffset;}
};
//����
struct TOWER_MODULE
{
	BYTE m_cBodyNo;			//������(��&ͷ��)����Ӧ����ģ����ȣ���
	BYTE m_arrActiveQuadLegNo[4];	//��ģ�͵�ǰ��һ�������޵ĵ�ǰ���Ⱥ�
	CFGWORD m_dwLegCfgWord;	//������ĺ�
};
//����
class CModuleSection : public CDataSection
{
public:
	CModuleSection(char* buf=NULL,DWORD size=0,const char* version=NULL){m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	BYTE GetModuleCount(){return *((BYTE*)m_data);}
	TOWER_MODULE GetModuleAt(int i);
};
//����
struct TOWER_BLOCK
{
	UCS_STRU lcs;			//������ģ�Ϳռ��ڶ����װ���ö�λ����ϵ
	CXhChar50 name;			//����������
	CSolidBody solid;		//ʵ�����ݣ�ע���ֹ���������������������Զ��������⣬�纯����������ֵʱ��
};
//����
class CBlockSection : public CDataSection
{
public:
	CBlockSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL);
	WORD GetBlockCount(){return *((WORD*)m_data);}
	TOWER_BLOCK GetBlockAt(int i);
	void GetBlockBufferAt(int index,CBuffer& blockBuf);
};
struct BOLTINFO
{
	WORD d;			//��˨ֱ��
	WORD L;			//��Ч��(mm)
	WORD Lnt;		//�޿۳�
	WORD down_lim;	//��˨ͨ������
	WORD up_lim;	//��˨ͨ������
	double weight;	//kg
	char spec[18];	//�̶����ȵ���˨��������ַ�����ĩβ0��ֹ�ַ���
public:	//����ʵ����ʾ���ݣ�ע���ֹ���������������������Զ��������⣬�纯����������ֵʱ��
	CSolidBody solidOfCap;	//��ñ
	CSolidBody solidOfBolt;	//��˨��
};
class CBoltSeries : public CDataSection
{
public:
	CBoltSeries(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	BYTE BoltDCount(){return *((BYTE*)m_data);}
	BYTE BoltWasherCount(){return *((BYTE*)(m_data+1));}
	BYTE BoltCapCount(){return *((BYTE*)(m_data+2));}
	WORD BoltSizeCount(){return *((WORD*)(m_data+3));}
	bool GetBoltSizeAt(int i,BOLTINFO& bolt);
	bool GetBoltCapSolidBody(int indexId, CSolidBody& body);
	bool GetBoltSolidBody(int indexId, CSolidBody& body);
};
class CBoltSection : public CDataSection
{
public:
	CBoltSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	WORD BoltSeriesCount(){return *((BYTE*)m_data);}
	CBoltSeries GetBoltSeriesAt(int i);
	void InitLsFamily(int i,CLsFamily* pFamily);
};
class CMaterialLibrary : public CDataSection
{
	BYTE m_byteMatTypeCount;
public:
	CMaterialLibrary(BYTE mat_n,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_byteMatTypeCount=mat_n,m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
};
//����������Ϣ
struct PART_INFO{
	WORD wModelSpace;
	BYTE cPartType;	//�������࣬1:�Ǹ�2:��˨3:�ְ�4:�ֹ�5:����6:�۸�
	WORD wStateFlag;//����״̬���ԣ���λΪ1��ʾ����ͬʱ����Ƕ��ʽ�����͸���ģ�Ϳռ䣬��0x8000
	BYTE cMaterial;	//���ʼ��ַ�
	DWORD dwSeg;	//�κţ���16λΪ�ַ�������λΪ���ֲ���
	float fWidth;	//���(mm)
	float fThick;	//���(mm)
	float fHeight;	//�߶�(mm)
	WORD wLength;	//����(mm)
	float fWeight;	//����(kg)
	BYTE cFuncType;	//��������
	DWORD addrAttachInfo;	//������Ϣ�ļ��ڴ洢��ַ
public:	//������������
	CXhChar50 sPartNo;	//�������
	CXhChar50 spec;		//���
	CXhChar50 sNotes;	//��ע
	CSolidBody solid;	//ʵ����ʾ���ݣ�ע���ֹ���������������������Զ��������⣬�纯����������ֵʱ��
};
class CPartSection : public CDataSection
{
public:
	CPartSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	CMaterialLibrary GetMatLibrary();
	PART_INFO GetPartInfoAt(int indexId);
	void GetPartBufferAt(int indexId,CBuffer& buffer);
};
struct BLOCK_ASSEMBLY
{
	WORD wIndexId;		//����������ʶ��
	GECS acs;		//�ڸ���ģ�Ϳռ��ڵ�װ������ϵ����
	DWORD dwSeg;		//�κ�
	CFGWORD cfgword;	//��ĺ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ��������
	BYTE reserved;		//1���ֽڶ��뱣��λ
};
struct BOLT_ASSEMBLY
{
	bool bInBlockSpace;	//�Ƿ�����װ�䲿���ռ�۹������ԣ�
	BYTE cSeriesId;	//��˨���ϵ�������ţ۹������ԣ�
	WORD wIndexId;	//��˨�����ϵ���ڵ�������ʶ�ţ۹������ԣ�
	WORD  wBlockIndexId;//���������ı�ʶ�����ţ۲���װ����˨���ԣ�
	float grade;		//��˨�ȼ�
	GEPOINT origin;		//��˨װ��ԭ��۹������ԣ�
	GEPOINT work_norm;	//��˨�������ߣ۹������ԣ�
	WORD wL0;			//��˨ͨ��(mm)�۹������ԣ�
	BYTE cPropFlag;		//����Ҫ���ʶ�ֽڣ�ĿǰĩλΪ����ʾ������˨������λ�������۹������ԣ�
	BYTE cDianQuanN,cDianQuanThick;	//��Ȧ����,��Ȧ��ȣ۹������ԣ�
	WORD wDianQuanOffset;	//��Ȧλ��ƫ����������˨���������ع������ߵ�ƫ�������۹������ԣ�
	//ͳ�����ͣ����λΪ����ʾ��˨���������������ֽ�������κţ��۸����ռ�װ����˨���ԣ�
	//���λΪ����ʾ��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ�У�
	//�θ�λΪ1��ʾ�Ƿ�Ϊ��������˨������ͣ�λ��ʾ�������ȵ����޺ţ���Ϊ����ʾ������������
	BYTE cStatFlag;
	//�κ�ǰ׺�ַ����ɺ������ַ������κ����ֲ���;�۸����ռ�װ����˨���ԣ�
	//���ͳ����������λΪ1ʱ���ˣ����ֽڱ�ʾ����˨�Ķκ�ͳ�Ʒ�Χ�ַ����Ĵ洢��ַ
	DWORD dwSeg;			//�۸����ռ�װ����˨���ԣ�
	CXhChar50 statSegStr;	//���ַ�����ʽ���δ洢��Ҫ���ͳ�Ƶ���˨�κ�ͳ�Ʒ�Χ�۸����ռ�װ����˨���ԣ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ���������۸���װ�乹�����ԣ�
	CFGWORD cfgword;		//�۸����ռ�װ����˨���ԣ�
};
class CBoltAssemblySection : public CDataSection
{
public:
	CBoltAssemblySection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	DWORD AssemblyCount(bool bTowerSpace=true);
	BOLT_ASSEMBLY GetAssemblyAt(int indexId,bool bTowerSpace=true);
	void GetBoltInfoBufferAt(int indexId,CBuffer& boltBuffer,bool bTowerSpace=true);
	void GetBoltAssemblyBufferAt(int index,CBuffer& assemblyBuf,bool bTowerSpace=true);
};
struct PART_ASSEMBLY
{
	bool bInBlockSpace;	//�Ƿ�����װ�䲿���ռ�۹������ԣ�
	WORD  wBlockIndexId;//���������ı�ʶ�����ţ۲���װ�乹�����ԣ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ���������۸���װ�乹�����ԣ�
	CFGWORD cfgword;	//����ģ�Ϳռ����������ֹ������ߵ���ĺţ۸���װ�乹�����ԣ�
	DWORD dwIndexId;	//�ڹ�����Ϣ�����еı�ʶ�����ţ۹������ԣ�
	GECS acs;			//�ڹ���ģ�Ϳռ��ڵ�װ������ϵ����۹������ԣ�
	bool bIsRod;
	GEPOINT startPt,endPt; 
	CHashList<DWORD> hashBoltIndexByHoleId;
	PART_ASSEMBLY(){bInBlockSpace=false;wBlockIndexId=0;cLegQuad=0;dwIndexId=0;bIsRod=false;}
};
class CPartAssemblySection : public CDataSection
{
public:
	CPartAssemblySection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	void GetAssemblyAt(int indexId,bool bTowerSpace,PART_ASSEMBLY &assmbly);
	DWORD AssemblyCount(bool bTowerSpace=true);
};
class CAssembleSection : public CDataSection
{
public:
	CAssembleSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	DWORD BlockAssemblyCount();
	BLOCK_ASSEMBLY GetAssemblyAt(int indexId);
	CBoltAssemblySection BoltSection();
	CPartAssemblySection PartSection();
};
class CTowerInstanceSection : public CDataSection
{
public:
	CTowerInstanceSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
};
//////////////////////////////////////////////////////////////////////////
// CPartOrgProcessInfoSection
class CPartOrgProcessInfoSection : public CDataSection
{
public:
	CPartOrgProcessInfoSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
public:
	BOOL GetOrgProcessInfoBufAt(int indexId,CBuffer& orgProcessInfoBuf);
};
//////////////////////////////////////////////////////////////////////////
//������Ϣ
class CAttachDataSection : public CDataSection
{
public:
	CAttachDataSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
public:
	CPartOrgProcessInfoSection PartOrgProcessInfoSection();
};
//////////////////////////////////////////////////////////////////////////
//TID�ļ��洢(�������͸�����Ϣ)
class CTIDBuffer : public CBuffer
{
	GECS mcs;					//������ģ�Ϳռ�����ϵ���������Ļ��
	DWORD m_dwZeroAddrOffset;	//�ļ�ͷ��ָ����������洢��ַ��λ��ƫ����
	CXhChar16 m_sVersion;
	CXhChar50 m_sTowerType;
public:
	CTIDBuffer(void);
	~CTIDBuffer(void);
	void SetZeroAddrOffset(DWORD zeroAddrOffset){m_dwZeroAddrOffset=zeroAddrOffset;}
	DWORD ZeroAddrOffset(){return m_dwZeroAddrOffset;}
	//�������ӷ����ڵ���Ե�ַת�����ļ��洢�ռ��ȫ�ֵ�ַ
	DWORD AddrLtoG(DWORD addr){return addr+m_dwZeroAddrOffset;}
	//�����ļ��洢�ռ��ȫ�ֵ�ַת�������ӷ����ڵ���Ե�ַ
	DWORD AddrGtoL(DWORD addr){return addr-m_dwZeroAddrOffset;}
	//
	CXhChar16 Version(){return m_sVersion;}
	CXhChar50 GetTowerType(){return m_sTowerType;}
	GECS ModelCoordSystem(){return mcs;}
	void InitBuffer(const char* srcBuf, DWORD buf_size);
public:
	CModuleSection ModuleSection();
	CBlockSection BlockSection();
	CBoltSection BoltSection();
	CPartSection PartSection();
	CAssembleSection AssembleSection();
	//CTowerInstanceSection TowerInstanceSection(){};
	//������
	CAttachDataSection AttachDataSection();

};
