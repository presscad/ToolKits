// DragEntSet.cpp: implementation of the CDragEntSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DragEntSet.h"
#include "XhCharString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDragEntSet DRAGSET;
CDragEntSet::CDragEntSet()
{
	m_bStartRecord=true;
	m_nClearDegree=0;
	acedSSAdd(NULL,NULL,drag_ent_set);
	m_pDrawCmdTagInfo=NULL;
	m_pRecordingBlockTableRecord=NULL;
}

CDragEntSet::~CDragEntSet()
{
	acedSSFree(drag_ent_set);
}

BOOL CDragEntSet::IsInDragSet(ads_name ent_name)
{
	long i,ll=0;
	ads_name ename;
	acedSSLength(drag_ent_set,&ll);
	for(i=0;i<ll;i++)
	{
		acedSSName(drag_ent_set,i,ename);
		if(ename[0]==ent_name[0]&&ename[1]==ent_name[1])
			return TRUE;
	}
	return FALSE;
}

void CDragEntSet::GetDragScope(SCOPE_STRU& scope)
{
	long i,ll=0;
	ads_name ename;
	f3dPoint vertex;
	AcGePoint3d pt;
	AcDbEntity *pEnt=NULL;
	AcDbObjectId ent_id=0;
	acedSSLength(drag_ent_set,&ll);
	for(i=0;i<ll;i++)
	{
		acedSSName(drag_ent_set,i,ename);
		if(acdbGetObjectId(ent_id,ename)!=Acad::eOk)
			continue;
		if(acdbOpenObject(pEnt,ent_id,AcDb::kForRead)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine* pLine=(AcDbLine*)pEnt;
			pt=pLine->startPoint();
			Cpy_Pnt(vertex,pt);
			scope.VerifyVertex(vertex);
			pt=pLine->endPoint();
			Cpy_Pnt(vertex,pt);
			scope.VerifyVertex(vertex);
		}
		else if(pEnt->isKindOf(AcDbText::desc()))
		{
			AcDbText* pText=(AcDbText*)pEnt;
			pt=pText->position();
			Cpy_Pnt(vertex,pt);
			scope.VerifyVertex(vertex);
		}
		pEnt->close();
	}
}

void CDragEntSet::ClearEntSet()
{
	acedSSFree(drag_ent_set);
	acedSSAdd(NULL,NULL,drag_ent_set);
	dimSpaceList.Empty();
	dimPosList.Empty();
	dimAngleList.Empty();
	m_nClearDegree++;
}

//Ҫ���ʱ��ʵ��ֻ�ܴ���ɶ���ر�״̬�����������д״̬
BOOL CDragEntSet::Add(AcDbObjectId newEntId)
{
	ads_name ent_name;
	if(acdbGetAdsName(ent_name,newEntId)==Acad::eOk)
	{
		if(acedSSAdd(ent_name,drag_ent_set,drag_ent_set)==RTNORM)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}
//Ҫ���ʱ��ʵ��ֻ�ܴ���ɶ���ر�״̬�����������д״̬
BOOL CDragEntSet::Add(ads_name newEntName)
{
	if(acedSSAdd(newEntName,drag_ent_set,drag_ent_set)==RTNORM)
	{
		struct resbuf *pData=acdbEntGet(newEntName);
		acdbEntMod(pData);
		acdbEntUpd(newEntName);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CDragEntSet::Del(AcDbObjectId delEntId)
{
	ads_name ent_name;
	if(acdbGetAdsName(ent_name,delEntId)==Acad::eOk)
	{
		if(acedSSDel(ent_name,drag_ent_set)==RTNORM)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

BOOL CDragEntSet::Del(ads_name delEntName)
{
	if(acedSSDel(delEntName,drag_ent_set)==RTNORM)
		return TRUE;
	else
		return FALSE;

}

long CDragEntSet::GetEntNum()
{
	long ll;
	if(acedSSLength(drag_ent_set,&ll)==RTNORM)
		return ll;
	else
		return 0;
}

BOOL CDragEntSet::GetEntName(long i,ads_name ent_name)
{
	if(acedSSName(drag_ent_set,i,ent_name)==RTNORM)
		return TRUE;
	else
		return FALSE;
}

BOOL CDragEntSet::GetEntId(long i,AcDbObjectId& ent_id)
{
	ads_name ent_name;
	if(acedSSName(drag_ent_set,i,ent_name)==RTNORM)
	{
		if(acdbGetObjectId(ent_id,ent_name)==Acad::eOk)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}
//BeginBlockRecord��Ҫ������һ��ʵ�尴����ʾ����˿�����һ�㲻��ָ�����ڲ��Զ�����GUID�ַ�����Ϊ���������ظ� wht 15-04-01
//����GUID�ַ���
CXhChar200 CreateGuidStr()
{
	GUID guid; 
	CXhChar200 szGUID; 
	if(S_OK==::CoCreateGuid(&guid)) 
	{
		szGUID.Printf("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X} " 
			,guid.Data1,guid.Data2,guid.Data3,guid.Data4[0],guid.Data4[1] ,guid.Data4[2],
			guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]); 
	}
	return szGUID;
}
bool CDragEntSet::BeginBlockRecord(const char* blkname/*=NULL*/)
{
	if(m_pRecordingBlockTableRecord!=NULL||!m_bStartRecord)
		return false;	//��������������ִ��
	CXhChar200 sBlkName;
	if(blkname)
		sBlkName.Copy(blkname);
	else 
		sBlkName=CreateGuidStr();
	//�����麸��ͼ���¼
	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable,AcDb::kForWrite);
	m_pRecordingBlockTableRecord=new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
	m_pRecordingBlockTableRecord->setName(_bstr_t(sBlkName));
#else
	m_pRecordingBlockTableRecord->setName(sBlkName);
#endif
	pBlockTable->add(blockId,m_pRecordingBlockTableRecord);
	pBlockTable->close();//�رտ��
	SetReocrdState(false);	//��������һ��ʵ�ʲ���Ŀ������ظ�����ʱ�ص���¼����
	return true;
}
bool CDragEntSet::EndBlockRecord(AcDbBlockTableRecord* pBlockTableRecord,double* insert_pos,double zoomcoef/*=1.0*/,
								 AcDbObjectId *pOutId /*= NULL*/, long handle /*= 0*/)
{
	if(m_pRecordingBlockTableRecord==NULL)
		return false;
	m_pRecordingBlockTableRecord->close();
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	AcGeScale3d scaleXYZ(zoomcoef,zoomcoef,zoomcoef);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(AcGePoint3d(insert_pos[0],insert_pos[1],insert_pos[2]));
	pBlkRef->setRotation(0);
	pBlkRef->setScaleFactors(scaleXYZ);
	SetReocrdState(true);
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, blockId, pBlkRef))
	{
		if (pOutId)
			*pOutId = blockId;
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pBlkRef->createExtensionDictionary();
			dictObjId = pBlkRef->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, 0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pBlkRef->close();
	}
	m_pRecordingBlockTableRecord=NULL;
	return true;
}
BOOL CDragEntSet::AppendAcDbEntity(AcDbBlockTableRecord *pBlockTableRecord,
	AcDbObjectId& outputId, AcDbEntity* pEntity)
{
	if(pBlockTableRecord->appendAcDbEntity(outputId,pEntity)==Acad::eOk)
	{
		if(!m_bStartRecord)
			return TRUE;	//�����¼����ק������,��Ҫ���ڶ�����ӵ���Ĭ��ģ�Ϳռ���¼��ʱ������������ظ���ӡ�wjh-2014.8.9
		BOOL bRet;
		if(pEntity->isWriteEnabled())
		{
			pEntity->downgradeOpen();
			bRet=Add(outputId);
			pEntity->upgradeOpen();
		}
		else
			bRet=Add(outputId);
		return TRUE;
	}
	else
		return FALSE;
}

//����ק����β��ɾ��ָ��������ʵ��	wht 10-11-30
BOOL CDragEntSet::DelEntFromTail(int nEntNum,BOOL bDeleteEnt/*=FALSE*/)
{
	long i=0,nLen=0;
	if(acedSSLength(drag_ent_set,&nLen)!=RTNORM)
		return FALSE;
	if(nEntNum>=nLen)	//ָ����Ҫɾ����ʵ���������ʵ���������ʵ�弯��
		nEntNum=nLen;
	for(i=nLen-nEntNum;i<nLen;i++)
	{	//ɾ��β��ָ��������ʵ��
		ads_name ent_name;
		if(acedSSName(drag_ent_set,i,ent_name)!=RTNORM)
			continue;
		acedSSDel(ent_name,drag_ent_set);
		if(bDeleteEnt)
		{	//ɾ��ʵ��
			AcDbEntity *pEnt=NULL;
			AcDbObjectId ent_id=0;
			if(acdbGetObjectId(ent_id,ent_name)!=Acad::eOk)
				continue;
			if(acdbOpenObject(pEnt,ent_id,AcDb::kForWrite)!=Acad::eOk)
				continue;
			if(pEnt==NULL)
				continue;
			pEnt->erase();
			pEnt->close();
		}
	}
	return TRUE;
}

//��ȡ��ק����β��ָ��������ʵ�����ڵ����� wht 10-11-30
BOOL CDragEntSet::GetTailEntScope(int nEntNum,ads_point &L_T,ads_point &R_B)
{
	long i=0,nLen=0;
	if(acedSSLength(drag_ent_set,&nLen)!=RTNORM)
		return FALSE;
	if(nEntNum>=nLen)	//ָ����ʵ���������ʵ������
		nEntNum=nLen;
	//
	SCOPE_STRU scope;
	const int nIncrement=50;	//�Ŵ�����������
	for(i=nLen-nEntNum;i<nLen;i++)
	{	
		ads_name ent_name;
		AcDbObjectId entId;
		if(acedSSName(drag_ent_set,i,ent_name)!=RTNORM)
			continue;	//�õ�ʵ������
		if(acdbGetObjectId(entId,ent_name)!=Acad::eOk)
			continue;	//�õ�ʵ��ID
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,entId,AcDb::kForRead)!=Acad::eOk)
		{
			if(pEnt)
				pEnt->close();
			continue;	//���ʵ��ָ��
		}
		if(pEnt==NULL)
			continue;
		AcGePoint3d acad_pnt;
		if(pEnt->isKindOf(AcDbLine::desc()))
		{	//ֱ��
			f3dPoint start,end;
			acad_pnt=((AcDbLine*)pEnt)->startPoint();
			Cpy_Pnt(start,acad_pnt);
			acad_pnt=((AcDbLine*)pEnt)->endPoint();
			Cpy_Pnt(end,acad_pnt);
			//�ʵ��Ŵ���������
			f3dPoint box_vertex[8];
			box_vertex[0].Set(start.x-nIncrement,start.y-nIncrement);
			box_vertex[1].Set(start.x+nIncrement,start.y-nIncrement);
			box_vertex[2].Set(start.x+nIncrement,start.y+nIncrement);
			box_vertex[3].Set(start.x-nIncrement,start.y+nIncrement);
			box_vertex[4].Set(end.x-nIncrement,end.y-nIncrement);
			box_vertex[5].Set(end.x+nIncrement,end.y-nIncrement);
			box_vertex[6].Set(end.x+nIncrement,end.y+nIncrement);
			box_vertex[7].Set(end.x-nIncrement,end.y+nIncrement);
			for(int jj=0;jj<8;jj++)
				scope.VerifyVertex(box_vertex[jj]);
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{	//�����
			f3dPoint start,end;
			AcGePoint2d acad_point;
			((AcDbPolyline*)pEnt)->getPointAt(0,acad_point);
			start.Set(acad_point.x,acad_point.y);
			((AcDbPolyline*)pEnt)->getPointAt(1,acad_point);
			end.Set(acad_point.x,acad_point.y);
			scope.VerifyVertex(start);
			scope.VerifyVertex(end);
			//�ʵ��Ŵ���������
			f3dPoint box_vertex[8];
			box_vertex[0].Set(start.x-nIncrement,start.y-nIncrement);
			box_vertex[1].Set(start.x+nIncrement,start.y-nIncrement);
			box_vertex[2].Set(start.x+nIncrement,start.y+nIncrement);
			box_vertex[3].Set(start.x-nIncrement,start.y+nIncrement);
			box_vertex[4].Set(end.x-nIncrement,end.y-nIncrement);
			box_vertex[5].Set(end.x+nIncrement,end.y-nIncrement);
			box_vertex[6].Set(end.x+nIncrement,end.y+nIncrement);
			box_vertex[7].Set(end.x-nIncrement,end.y+nIncrement);
			for(int jj=0;jj<8;jj++)
				scope.VerifyVertex(box_vertex[jj]);
		}
		else if(pEnt->isKindOf(AcDbPoint::desc()))
		{	//��
			f3dPoint f_pos;
			acad_pnt=((AcDbPoint*)pEnt)->position();
			Cpy_Pnt(f_pos,acad_pnt);
			//�ʵ��Ŵ���������
			f3dPoint box_vertex[4];
			box_vertex[0].Set(f_pos.x-nIncrement,f_pos.y-nIncrement);
			box_vertex[1].Set(f_pos.x+nIncrement,f_pos.y-nIncrement);
			box_vertex[2].Set(f_pos.x+nIncrement,f_pos.y+nIncrement);
			box_vertex[3].Set(f_pos.x-nIncrement,f_pos.y+nIncrement);
			for(int jj=0;jj<4;jj++)
				scope.VerifyVertex(box_vertex[jj]);
		}
		pEnt->close();	//�ر�ʵ��
	}
	//Ϊ�����������϶�����������¶������긳ֵ
	L_T[X]=scope.fMinX;
	L_T[Y]=scope.fMaxY;
	L_T[Z]=0;
	R_B[X]=scope.fMaxX;
	R_B[Y]=scope.fMinY;
	R_B[Z]=0;
	return TRUE;
}

//��ȡ��ק����ʵ�����ڵ����� 
BOOL CDragEntSet::GetDragEntSetScope(ads_point &L_T,ads_point &R_B)
{
	long i=0,nLen=0;
	if(acedSSLength(drag_ent_set,&nLen)!=RTNORM)
		return FALSE;
	SCOPE_STRU scope;
	const int nIncrement=50;	//�Ŵ�����������
	for(i=0;i<nLen;i++)
	{	
		ads_name ent_name;
		AcDbObjectId entId;
		if(acedSSName(drag_ent_set,i,ent_name)!=RTNORM)
			continue;	//�õ�ʵ������
		if(acdbGetObjectId(entId,ent_name)!=Acad::eOk)
			continue;	//�õ�ʵ��ID
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,entId,AcDb::kForRead)!=Acad::eOk)
		{
			if(pEnt)
				pEnt->close();
			continue;	//���ʵ��ָ��
		}
		if(pEnt==NULL)
			continue;
		AcGePoint3d acad_pnt;
		if(pEnt->isKindOf(AcDbLine::desc()))
		{	//ֱ��
			f3dPoint start,end;
			acad_pnt=((AcDbLine*)pEnt)->startPoint();
			Cpy_Pnt(start,acad_pnt);
			acad_pnt=((AcDbLine*)pEnt)->endPoint();
			Cpy_Pnt(end,acad_pnt);
			//�ʵ��Ŵ���������
			f3dPoint box_vertex[8];
			box_vertex[0].Set(start.x-nIncrement,start.y-nIncrement);
			box_vertex[1].Set(start.x+nIncrement,start.y-nIncrement);
			box_vertex[2].Set(start.x+nIncrement,start.y+nIncrement);
			box_vertex[3].Set(start.x-nIncrement,start.y+nIncrement);
			box_vertex[4].Set(end.x-nIncrement,end.y-nIncrement);
			box_vertex[5].Set(end.x+nIncrement,end.y-nIncrement);
			box_vertex[6].Set(end.x+nIncrement,end.y+nIncrement);
			box_vertex[7].Set(end.x-nIncrement,end.y+nIncrement);
			for(int jj=0;jj<8;jj++)
				scope.VerifyVertex(box_vertex[jj]);
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{	//�����
			f3dPoint start,end;
			AcGePoint2d acad_point;
			((AcDbPolyline*)pEnt)->getPointAt(0,acad_point);
			start.Set(acad_point.x,acad_point.y);
			((AcDbPolyline*)pEnt)->getPointAt(1,acad_point);
			end.Set(acad_point.x,acad_point.y);
			scope.VerifyVertex(start);
			scope.VerifyVertex(end);
			//�ʵ��Ŵ���������
			f3dPoint box_vertex[8];
			box_vertex[0].Set(start.x-nIncrement,start.y-nIncrement);
			box_vertex[1].Set(start.x+nIncrement,start.y-nIncrement);
			box_vertex[2].Set(start.x+nIncrement,start.y+nIncrement);
			box_vertex[3].Set(start.x-nIncrement,start.y+nIncrement);
			box_vertex[4].Set(end.x-nIncrement,end.y-nIncrement);
			box_vertex[5].Set(end.x+nIncrement,end.y-nIncrement);
			box_vertex[6].Set(end.x+nIncrement,end.y+nIncrement);
			box_vertex[7].Set(end.x-nIncrement,end.y+nIncrement);
			for(int jj=0;jj<8;jj++)
				scope.VerifyVertex(box_vertex[jj]);
		}
		else if(pEnt->isKindOf(AcDbPoint::desc()))
		{	//��
			f3dPoint f_pos;
			acad_pnt=((AcDbPoint*)pEnt)->position();
			Cpy_Pnt(f_pos,acad_pnt);
			//�ʵ��Ŵ���������
			f3dPoint box_vertex[4];
			box_vertex[0].Set(f_pos.x-nIncrement,f_pos.y-nIncrement);
			box_vertex[1].Set(f_pos.x+nIncrement,f_pos.y-nIncrement);
			box_vertex[2].Set(f_pos.x+nIncrement,f_pos.y+nIncrement);
			box_vertex[3].Set(f_pos.x-nIncrement,f_pos.y+nIncrement);
			for(int jj=0;jj<4;jj++)
				scope.VerifyVertex(box_vertex[jj]);
		}
		pEnt->close();	//�ر�ʵ��
	}
	//Ϊ�����������϶�����������¶������긳ֵ
	L_T[X]=scope.fMinX;
	L_T[Y]=scope.fMaxY;
	L_T[Z]=0;
	R_B[X]=scope.fMaxX;
	R_B[Y]=scope.fMinY;
	R_B[Z]=0;
	return TRUE;
}
