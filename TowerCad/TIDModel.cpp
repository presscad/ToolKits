#include "StdAfx.h"
#include ".\tidmodel.h"
#include "ParseAdaptNo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD CFG_NO[32]={ 0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,
	0X00000100,0X00000200,0X00000400,0X00000800,0X00001000,0X00002000,0X00004000,0X00008000,
	0X00010000,0X00020000,0X00040000,0X00080000,0X00100000,0X00200000,0X00400000,0X00800000,
	0X01000000,0X02000000,0X04000000,0X08000000,0X10000000,0X20000000,0X40000000,0X80000000};

//����
TOWER_MODULE CModuleSection::GetModuleAt(int i)
{
	TOWER_MODULE module;
	BYTE* pBodyNo=(BYTE*)BufferPtr()+1+21*i;
	module.m_cBodyNo=*pBodyNo;
	memcpy(&module.m_dwLegCfgWord,pBodyNo+1,20);
	for(BYTE j=1;j<=160;j++)
	{
		if(module.m_dwLegCfgWord.IsHasNo(j))
		{
			module.m_arrActiveQuadLegNo[0]=module.m_arrActiveQuadLegNo[1]=
				module.m_arrActiveQuadLegNo[2]=module.m_arrActiveQuadLegNo[3]=j;
			break;
		}
	}
	return module;
}
//����
CBlockSection::CBlockSection(DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/){
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
}
TOWER_BLOCK CBlockSection::GetBlockAt(int i)
{
	TOWER_BLOCK block;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(2+100*i);
	buf.ReadPoint(block.lcs.origin);
	buf.ReadPoint(block.lcs.axis_x);
	buf.ReadPoint(block.lcs.axis_y);
	buf.ReadPoint(block.lcs.axis_z);
	DWORD name_addr;
	buf.ReadDword(&name_addr);
	buf.SeekPosition(AddrGtoL(name_addr));
	buf.ReadString(block.name,50);
	return block;
}
void CBlockSection::GetBlockBufferAt(int index,CBuffer& blockBuf)
{
	blockBuf.ClearContents();
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(2+100*index);
	blockBuf.Write(buf,96);
	DWORD dwAddr;
	buf.ReadDword(&dwAddr);
	buf.SeekPosition(AddrGtoL(dwAddr));
	char name[50]="";
	buf.ReadString(name,50);
	blockBuf.WriteString(name,50);
}
//��˨ϵ��
bool CBoltSeries::GetBoltSizeAt(int i,BOLTINFO& bolt)
{
	CBuffer buf(BufferPtr(),BufferLength());
	BYTE d_count;
	WORD size_count;
	buf.ReadByte(&d_count);
	buf.Offset(2);	//ƫ��1B��Ȧ������1B��ñ����
	buf.ReadWord(&size_count);
	buf.Offset(23);	//ƫ��23B��˨������
	buf.Offset(i*36);
	buf.ReadWord(&bolt.d);			//��˨ֱ��
	buf.ReadWord(&bolt.L);			//��Ч��(mm)
	buf.ReadWord(&bolt.Lnt);		//�޿۳�
	buf.ReadWord(&bolt.up_lim);		//ͨ������
	buf.ReadWord(&bolt.down_lim);	//ͨ������
	buf.ReadDouble(&bolt.weight);	//kg
	buf.Read(bolt.spec,18);	//�̶����ȵ���˨��������ַ�����ĩβ0��ֹ�ַ���
	//������˨��ʵ�������ڴ�
	DWORD offset,len;
	buf.SeekPosition(5+23+size_count*36+d_count*6+4*i);
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	bolt.solidOfBolt.AttachBuffer(buf.GetCursorBuffer(),len);
	//������ñʵ�������ڴ�
	buf.SeekPosition(5+23+size_count*36);
	for(BYTE di=0;di<d_count;di++)
	{
		WORD rd;
		buf.ReadWord(&rd);
		if(rd==bolt.d)
		{
			buf.ReadDword(&offset);	//��˨ñʵ�����ݴ洢��ַ
			buf.SeekPosition(AddrGtoL(offset));
			buf.ReadDword(&len);
			bolt.solidOfCap.AttachBuffer(buf.GetCursorBuffer(),len);
			break;
		}
		else
			buf.Offset(4);
	}
	return true;
}
bool CBoltSeries::GetBoltCapSolidBody(int indexId, CSolidBody& body)
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(5+BoltSizeCount()*32+indexId*6-4);
	DWORD offset,len;
	buf.Offset(2);	//��˨ֱ��
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	body.ReadFrom(&buf,len);
	return true;
}
bool CBoltSeries::GetBoltSolidBody(int indexId, CSolidBody& body)
{
	CBuffer buf(BufferPtr(),BufferLength());
		buf.SeekPosition(5+BoltSizeCount()*32+BoltDCount()*6+indexId*4-4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	body.ReadFrom(&buf,len);
	return true;
}
CBoltSeries CBoltSection::GetBoltSeriesAt(int i)
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(1+i*4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	return CBoltSeries(ZeroAddrOffset()+buf.GetCursorPosition(),buf.GetCursorBuffer(),len,m_sVersion);
}
void CBoltSection::InitLsFamily(int i,CLsFamily* pFamily)
{
	CBoltSeries boltSeries=GetBoltSeriesAt(i);
	pFamily->m_nDisplayWasher=boltSeries.BoltWasherCount();
	pFamily->m_nLsCap=boltSeries.BoltCapCount();
	int nRec=boltSeries.BoltSizeCount();
	pFamily->CreateFamily(nRec);
	char sGrade[50]="";
	for(int j=0;j<pFamily->GetCount();j++)
	{
		LS_XING_HAO* pLsXingHao=pFamily->RecordAt(j);
		BOLTINFO boltInfo;
		boltSeries.GetBoltSizeAt(j,boltInfo);
		pLsXingHao->d=boltInfo.d;
		pLsXingHao->valid_len=boltInfo.L;
		pLsXingHao->no_thread_len=boltInfo.Lnt;
		pLsXingHao->up_lim=boltInfo.up_lim;
		pLsXingHao->down_lim=boltInfo.down_lim;
		strcpy(pLsXingHao->guige,boltInfo.spec);
		sscanf(pLsXingHao->guige,"%[^M]",sGrade);
		pLsXingHao->grade=atof(sGrade);
		pLsXingHao->weight=boltInfo.weight;
	}
	//��ȡ��˨������
	CBuffer buf(boltSeries.BufferPtr(),boltSeries.BufferLength());
	buf.SeekPosition(5);
	char series_name[23]="";
	buf.Read(series_name,23);	//�̶����ȵ���˨�����ƣ���ĩβ0��ֹ�ַ���
	strncpy(pFamily->name,series_name,16);
}
//���ʿ�
CMaterialLibrary CPartSection::GetMatLibrary()
{
	CBuffer buf(BufferPtr(),BufferLength());
	BYTE mat_n;
	buf.ReadByte(&mat_n);
	DWORD part_n;
	buf.ReadDword(&part_n);
	return CMaterialLibrary(mat_n,buf.GetCursorBuffer(),mat_n*8);
}
//����
PART_INFO CPartSection::GetPartInfoAt(int indexId)
{
	BYTE mat_n;
	DWORD part_n;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.ReadByte(&mat_n);
	buf.ReadDword(&part_n);
	buf.Offset(mat_n*8);
	buf.Offset((indexId-1)*33);
	PART_INFO part;
	buf.ReadWord(&part.wModelSpace);
	buf.ReadByte(&part.cPartType);
	buf.ReadWord(&part.wStateFlag);
	buf.ReadByte(&part.cMaterial);
	buf.ReadDword(&part.dwSeg);
	buf.ReadFloat(&part.fWidth);
	buf.ReadFloat(&part.fThick);
	buf.ReadFloat(&part.fHeight);
	buf.ReadWord(&part.wLength);
	buf.ReadFloat(&part.fWeight);
	buf.ReadByte(&part.cFuncType);
	buf.ReadDword(&part.addrAttachInfo);
	//��ȡ������Ϣ
	buf.SeekPosition(AddrGtoL(part.addrAttachInfo));
	DWORD size;
	buf.ReadDword(&size);
	part.solid.AttachBuffer(buf.GetCursorBuffer(),size);
	buf.Offset(size);
	buf.ReadString(part.sPartNo,50);
	buf.ReadString(part.spec,50);
	buf.ReadString(part.sNotes,50);
	buf.ReadDword(&size);	//��������������Ϣ�洢����
	buf.Offset(size);		//����Ҫ���������Ϣ�������Ե��˲�����Ϣ
	return part;
}
void CPartSection::GetPartBufferAt(int indexId,CBuffer& partBuffer)
{
	partBuffer.ClearContents();
	CBuffer sectionBuf(BufferPtr(),BufferLength());
	BYTE mat_n;
	sectionBuf.ReadByte(&mat_n);
	DWORD iValue,iOffset,size;
	sectionBuf.ReadDword(&iValue);
	sectionBuf.Offset(mat_n*8);
	sectionBuf.Offset((indexId-1)*33);	//������indexId������Ϣ��
	partBuffer.Write(sectionBuf,29);	//��ȡ����������Ϣ
	sectionBuf.ReadDword(&iOffset);
	sectionBuf.SeekPosition(AddrGtoL(iOffset));
	sectionBuf.ReadDword(&size);
	partBuffer.WriteDword(size);		//��ȡ����ʵ����Ϣ�ڴ��С
	partBuffer.Write(sectionBuf,size);	//��ȡ����ʵ����Ϣ
	CXhChar50 sText;
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//��ȡ�������
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//��ȡ�������
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//��ȡ��ע
	sectionBuf.ReadDword(&size);		
	partBuffer.WriteDword(size);		//��ȡ����������Ϣ�ڴ��С
	partBuffer.Write(sectionBuf,size);	//��ȡ����������Ϣ
}
DWORD CAssembleSection::BlockAssemblyCount()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_count;
	buf.ReadDword(&block_count);
	return block_count;
}
BLOCK_ASSEMBLY CAssembleSection::GetAssemblyAt(int indexId)
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.Offset(4+(indexId-1)*128);
	BLOCK_ASSEMBLY assembly;
	buf.ReadWord(&assembly.wIndexId);
	buf.ReadPoint(assembly.acs.origin);
	buf.ReadPoint(assembly.acs.axis_x);
	buf.ReadPoint(assembly.acs.axis_y);
	buf.ReadPoint(assembly.acs.axis_z);
	buf.ReadDword(&assembly.dwSeg);
	buf.ReadByte(&assembly.cLegQuad);
	buf.Read(&assembly.cfgword,24);
	buf.ReadByte(&assembly.reserved);
	return assembly;
}
CBoltAssemblySection CAssembleSection::BoltSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	buf.ReadDword(&size);
	return CBoltAssemblySection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size);
}
DWORD CBoltAssemblySection::AssemblyCount(bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD assemble_n;
	if(bTowerSpace)
		buf.Offset(4);
	buf.ReadDword(&assemble_n);
	return assemble_n;
}
BOLT_ASSEMBLY CBoltAssemblySection::GetAssemblyAt(int indexId,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);

	BOLT_ASSEMBLY assembly;
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//��������ģ�Ϳռ�����˨װ����Ϣ����
		buf.Offset((indexId-1)*92);	//������ǰ������ʶָ��ļ�¼
		assembly.bInBlockSpace=false;
		assembly.wBlockIndexId=0;
	}
	else
	{
		buf.Offset((indexId-1)*64);
		assembly.bInBlockSpace=true;
	}
	//��������
	buf.ReadByte(&assembly.cSeriesId);
	buf.ReadWord(&assembly.wIndexId);
	buf.ReadFloat(&assembly.grade);
	//��������
	buf.ReadPoint(assembly.origin);
	buf.ReadPoint(assembly.work_norm);
	buf.ReadWord(&assembly.wL0);
	buf.ReadByte(&assembly.cPropFlag);
	buf.ReadByte(&assembly.cDianQuanN);
	buf.ReadByte(&assembly.cDianQuanThick);
	buf.ReadWord(&assembly.wDianQuanOffset);
	if(!bTowerSpace)	//�����ռ�����˨װ������Ӧ�Ĳ���������ʶ
		buf.ReadWord(&assembly.wBlockIndexId);
	else //if(bTowerSpace)
	{					//����ģ�Ϳռ�����˨װ������Ӧ�Ĳ���������ʶ
		buf.ReadByte(&assembly.cStatFlag);
		buf.ReadDword(&assembly.dwSeg);
		buf.ReadByte(&assembly.cLegQuad);
		buf.Read(&assembly.cfgword,24);
		if(assembly.cStatFlag&0x80)	//��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ��
		{
			buf.SeekPosition(AddrGtoL(assembly.dwSeg));
			buf.ReadString(assembly.statSegStr,50);
		}
	}
	return assembly;
}
void CBoltAssemblySection::GetBoltInfoBufferAt(int indexId,CBuffer& boltBuffer,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//��������ģ�Ϳռ�����˨װ����Ϣ����
		buf.Offset((indexId-1)*92);	//������ǰ������ʶָ��ļ�¼
	}
	else
		buf.Offset((indexId-1)*64);
	boltBuffer.ClearContents();
	boltBuffer.Write(buf,7);
}
void CBoltAssemblySection::GetBoltAssemblyBufferAt(int indexId,CBuffer& assemblyBuf,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//��������ģ�Ϳռ�����˨װ����Ϣ����
		buf.Offset((indexId-1)*92);	//������ǰ������ʶָ��ļ�¼
	}
	else
		buf.Offset((indexId-1)*64);
	buf.Offset(7);
	assemblyBuf.ClearContents();
	assemblyBuf.Write(buf,55);
	WORD wBlockIndexId=0;
	if(bTowerSpace)
	{					
		assemblyBuf.WriteWord(wBlockIndexId);
		BYTE cFlag=0,cQuad=0;
		DWORD dwSeg=0;
		CFGWORD cfgword;
		CXhChar50 statSetStr;
		// ���λΪ����ʾ��˨���������������ֽ�������κţ�
		// ���λΪ����ʾ��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ�У�
		// �θ�λΪ����ʾ�Ƿ�Ϊ��������˨��
		// ����ͣ�λ��ʾ�������ȵ����޺ţ���Ϊ����ʾ�����������塣
		buf.ReadByte(&cFlag);
		buf.ReadDword(&dwSeg);
		buf.ReadByte(&cQuad);
		buf.Read(&cfgword,24);
		if(cFlag&0x80)		//��˨ͬʱ��������κ�
		{	//
			buf.SeekPosition(AddrGtoL(dwSeg));
			buf.ReadString(statSetStr,50);
			ATOM_LIST<SEGI> segNoList;
			if(GetSortedSegNoList(CString(statSetStr),segNoList)>0)
				dwSeg=segNoList.GetFirst()->iSeg;
			else 
				dwSeg=0;
		}
		assemblyBuf.WriteDword(dwSeg);
		assemblyBuf.WriteString(statSetStr);
		assemblyBuf.WriteByte(cQuad);	//�Ȳ�������������
		assemblyBuf.Write(&cfgword,24);	//��ĺ�
	}
	else
	{
		buf.ReadWord(&wBlockIndexId);
		assemblyBuf.WriteWord(wBlockIndexId);
	}
}
CPartAssemblySection CAssembleSection::PartSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	buf.ReadDword(&size);
	buf.Offset(size);	//������˨װ����
	buf.ReadDword(&size);
	return CPartAssemblySection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size);
}
DWORD CPartAssemblySection::AssemblyCount(bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD assemble_n;
	if(bTowerSpace)
		buf.Offset(4);
	buf.ReadDword(&assemble_n);
	return assemble_n;
}
void CPartAssemblySection::GetAssemblyAt(int indexId,bool bTowerSpace,PART_ASSEMBLY &assembly)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_part_n,tower_part_n,rod_n;
	buf.ReadDword(&block_part_n);
	buf.ReadDword(&tower_part_n);
	buf.ReadDword(&rod_n);
	DWORD addrSingleLinePos=0;
	if(bTowerSpace)
	{
		buf.Offset(block_part_n*(106+4));	//��������ģ�Ϳռ��й���װ����Ϣ����
		buf.Offset((indexId-1)*(129+4));	//������ǰ������ʶָ��ļ�¼
		assembly.bInBlockSpace=false;
		assembly.wBlockIndexId=0;
		buf.ReadDword(&assembly.dwIndexId);
		buf.ReadDword(&addrSingleLinePos);
		buf.ReadByte(&assembly.cLegQuad);
		buf.Read(&assembly.cfgword,24);
	}
	else
	{
		buf.Offset((indexId-1)*(106+4));	//������ǰ������ʶָ��ļ�¼
		assembly.bInBlockSpace=true;
		assembly.cLegQuad=0;
		buf.ReadDword(&assembly.dwIndexId);
		buf.ReadDword(&addrSingleLinePos);
		buf.ReadWord(&assembly.wBlockIndexId);
	}
	buf.ReadPoint(assembly.acs.origin);
	buf.ReadPoint(assembly.acs.axis_x);
	buf.ReadPoint(assembly.acs.axis_y);
	buf.ReadPoint(assembly.acs.axis_z);
	//��ȡװ���¼��˨����Ϣ��ַ
	DWORD addrHoleInfo=0;
	buf.ReadDword(&addrHoleInfo);
	if(addrHoleInfo>0)
	{	//��ȡ������Ϣ
		buf.SeekPosition(AddrGtoL(addrHoleInfo));
		DWORD size;
		buf.ReadDword(&size);
		assembly.hashBoltIndexByHoleId.Empty();
		for(DWORD i=0;i<size;i++)
		{
			DWORD dwBoltIndex=0,dwHoleId=0;
			buf.ReadDword(&dwHoleId);
			buf.ReadDword(&dwBoltIndex);
			assembly.hashBoltIndexByHoleId.SetValue(dwHoleId,dwBoltIndex);
		}
	}
	//��ȡ�˼���������
	if(addrSingleLinePos>0)
	{
		buf.SeekPosition(AddrGtoL(addrSingleLinePos));
		buf.ReadPoint(assembly.startPt);
		buf.ReadPoint(assembly.endPt);
		assembly.bIsRod=true;
	}
}
//////////////////////////////////////////////////////////////////////////
// 
CPartOrgProcessInfoSection CAttachDataSection::PartOrgProcessInfoSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(0);
	DWORD pos=buf.ReadDword();
	DWORD sect_len=0;
	if(pos>0)
	{
		buf.SeekPosition(AddrGtoL(pos));
		sect_len=buf.ReadDword();
	}
	return CPartOrgProcessInfoSection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),sect_len);
}
//////////////////////////////////////////////////////////////////////////
//CPartOrgProcessInfoSection
BOOL CPartOrgProcessInfoSection::GetOrgProcessInfoBufAt(int indexId,CBuffer& orgProcessInfoBuf)
{
	if(BufferLength()<=0)	//û�и�����Ϣ
		return FALSE;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.Offset((indexId-1)*4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	if(offset==0)
		return FALSE;
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	if(len>0)
		orgProcessInfoBuf.Write(buf.GetCursorBuffer(),len);
	else
		return FALSE;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//CIIDBuffer
//////////////////////////////////////////////////////////////////////////////////////////////
CTIDBuffer::CTIDBuffer(void)
{
	m_dwZeroAddrOffset=0;
}

CTIDBuffer::~CTIDBuffer(void)
{
}
CModuleSection CTIDBuffer::ModuleSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(0));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CModuleSection(GetCursorBuffer(),sect_len,m_sVersion);
}
CBlockSection CTIDBuffer::BlockSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(4));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CBlockSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}
CBoltSection CTIDBuffer::BoltSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(8));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CBoltSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}
CPartSection CTIDBuffer::PartSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(12));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CPartSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}
CAssembleSection CTIDBuffer::AssembleSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(16));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CAssembleSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}

CAttachDataSection CTIDBuffer::AttachDataSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(16));	//�������ݷ��������һ������֮�󣬴˴��������һ������������ת
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	SeekPosition(pos+sect_len+4);	//+4Ϊsect_len���ڵ�4���ֽ�
	DWORD attach_sect_len=GetLength()-GetCursorPosition();
	return CAttachDataSection(GetCursorPosition(),GetCursorBuffer(),attach_sect_len,m_sVersion);
}
void CTIDBuffer::InitBuffer(const char* srcBuf, DWORD buf_size)
{
	ClearBuffer();
	Write(srcBuf,buf_size);
	SeekToBegin();
	CXhChar50 sDocType;
	ReadString(sDocType,50);
	if(strstr(sDocType,"Xerofox tower solid data media file")==NULL)
		throw "��Ԥ�ڵ��ļ���ʽ���ļ���ʧ��!";
	char* key=strtok(sDocType,"-");
	char* ver=strtok(NULL,"-");
	double version=1000;
	if(ver!=NULL)
		version=atof(ver);
	if(version>1.2)
		throw "���ļ��汾��̫��,����ֱ�Ӷ�ȡ���ļ�";
	m_sVersion.Copy(ver);
	ReadString(m_sTowerType,50);
	ReadPoint(mcs.axis_x);
	ReadPoint(mcs.axis_y);
	ReadPoint(mcs.axis_z);
	SetZeroAddrOffset(GetCursorPosition());
}