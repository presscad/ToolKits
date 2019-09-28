// DragEntSet.h: interface for the CDragEntSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAGENTSET_H__D92C66C8_D139_42A1_B476_BA1DAF61598B__INCLUDED_)
#define AFX_DRAGENTSET_H__D92C66C8_D139_42A1_B476_BA1DAF61598B__INCLUDED_

#include "dbid.h"
#include "dbsymtb.h"
#include "adsdef.h"	// Added by ClassView
#include "f_ent_list.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagSETUPDRAWINGDIM_STRU
{
	AcDbObjectId startPointId;
	AcDbObjectId endPointId;
	double dimPosOffset;	//��עλ���Ի����ƫ�������Զ�λ����Y�᷽���ƫ����)
	double dist;			//���
	BOOL bDisplayDimLine;	//�Ƿ���ʾ�ߴ���
	tagSETUPDRAWINGDIM_STRU(){memset(this,0,sizeof(tagSETUPDRAWINGDIM_STRU));}
}SETUPDRAWINGDIM_STRU;
typedef struct tagSETUPPOSDIM_STRU
{
	AcDbObjectId posPointId;
	double dimPosOffset;	//��עλ���Ի����ƫ�������Զ�λ����Y�᷽���ƫ����)
	double posDist;			//��λ���Ը˼���ʼ�˵Ķ�λ����
	BOOL bDisplayDimLine;	//�Ƿ���ʾ�ߴ���
	tagSETUPPOSDIM_STRU(){memset(this,0,sizeof(tagSETUPPOSDIM_STRU));}
}SETUPPOSDIM_STRU;
typedef struct tagSETUPANGULARDIM_STRU
{
	AcDbObjectId vertexPointId;		//�нǵĽǶ����ǵ�
	AcDbObjectId lineStartPointId;	//�нǵ���ʼ�߱�ǵ�
	AcDbObjectId lineEndPointId;	//�нǵ���ֹ�߱�ǵ�
	AcDbObjectId arcPointId;		//��ע�Ƕȵı�עԲ����ǵ�
	tagSETUPANGULARDIM_STRU(){memset(this,0,sizeof(tagSETUPANGULARDIM_STRU));}
}SETUPANGULARDIM_STRU;

class CDrawLineAngleTagInfo;
class CDrawCmdRunTagInfo
{
public:
	void *pDrawCmd;
	CDrawCmdRunTagInfo(){pDrawCmd=NULL;}
	virtual bool IsDrawLinePart(){return false;}
	virtual bool IsDrawAnglePart(){return false;}
	virtual f3dPoint DimDrawPos(){return f3dPoint(0,0,0);}
	virtual f3dPoint StartDrawPos(){return f3dPoint(0,0,0);}
	virtual f3dPoint EndDrawPos(){return f3dPoint(0,0,0);}
	CDrawLineAngleTagInfo* AngleDrawTagInfo(){return (CDrawLineAngleTagInfo*)this;}
};
class CDrawDimensionTagInfo : public CDrawCmdRunTagInfo
{
public:
	f3dPoint m_xDimDrawPos;
	virtual f3dPoint DimDrawPos(){return m_xDimDrawPos;}
};
class CDrawPartTagInfo : public CDrawCmdRunTagInfo
{
public:
	//�������Ʒ�ʽ:
	//�Ǹּ�:'X':��������X֫;'x':��������X֫;'Y':��������Y֫;'y':��������Y֫;'Z'��������ʾ���ƽ���ͼ
	//���:0.ƽ�Ż���;1.��ֱ�������
	//��˨:0.��ֱ����;1.��(ƽ)�Ż���;2.δ�ҵ���˨ͼ��,��Բ�׻���
	char m_cDrawStyle;
	CDrawPartTagInfo(){m_cDrawStyle=0;pDrawCmd=NULL;}
	virtual bool IsDrawLinePart(){return false;}
	virtual bool IsDrawAnglePart(){return false;}
	virtual f3dPoint DimDrawPos(){return f3dPoint(0,0,0);}
	virtual f3dPoint StartDrawPos(){return f3dPoint(0,0,0);}
	virtual f3dPoint EndDrawPos(){return f3dPoint(0,0,0);}
};
class CDrawLinePartTagInfo : public CDrawPartTagInfo
{
public:
	f3dPoint m_xStartDrawPos,m_xEndDrawPos;	//ʼ�ն˵Ļ��ƻ�׼λ��
	virtual bool IsDrawLinePart(){return true;}
	virtual f3dPoint StartDrawPos(){return m_xStartDrawPos;}
	virtual f3dPoint EndDrawPos(){return m_xEndDrawPos;}
};
class CDrawLineAngleTagInfo : public CDrawLinePartTagInfo
{
public:
	long start_std_odd,end_std_odd;
	CDrawLineAngleTagInfo(){start_std_odd=end_std_odd=0;}
	virtual bool IsDrawAnglePart(){return true;}
};
class CDragEntSet  
{
	bool m_bStartRecord;//��ʼ��¼
	int m_nClearDegree;	//��ק���ϱ���մ���
	AcDbObjectId blockId;
	AcDbBlockTableRecord *m_pRecordingBlockTableRecord;
public:
	CDrawCmdRunTagInfo *m_pDrawCmdTagInfo;
	f3dPoint axis_vec;
	ATOM_LIST<SETUPDRAWINGDIM_STRU> dimSpaceList;	//�����ϵļ���ע
	ATOM_LIST<SETUPPOSDIM_STRU> dimPosList;		//�˼���ĳһ��λ����Ը˼���ʼ�˵Ķ�λ��ע
	ATOM_LIST<SETUPANGULARDIM_STRU> dimAngleList;		//�˼���ĳһ��λ����Ը˼���ʼ�˵Ķ�λ��ע
	ads_name drag_ent_set;	//���ڴ洢��ʱ��ק�����ɵ�ʵ�弯
	BOOL GetEntId(long i,AcDbObjectId &ent_id);
	BOOL GetEntName(long i,ads_name ent_name);
	long GetEntNum();
	BOOL Del(ads_name delEntName);
	BOOL Del(AcDbObjectId delEntId);
	BOOL Add(ads_name newEntName);
	BOOL Add(AcDbObjectId newEntId);
	bool SetReocrdState(bool start=true){return m_bStartRecord=start;}
	bool BeginBlockRecord(const char* blkname=NULL);
	AcDbBlockTableRecord *RecordingBlockTableRecord(){return m_pRecordingBlockTableRecord;}
	bool EndBlockRecord(AcDbBlockTableRecord* pBlockTableRecord,double* insert_pos,double zoomcoef=1.0,
						AcDbObjectId *pOutId=NULL,long handle=0);
	BOOL AppendAcDbEntity(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId& outputId, AcDbEntity* pEntity);
	BOOL IsInDragSet(ads_name ent_name);
	void GetDragScope(SCOPE_STRU& scope);	//��ȡ��ק���Ϸ�Χ
	//����ק����β��ɾ��ָ��������ʵ��	wht 10-11-30
	BOOL DelEntFromTail(int nEntNum,BOOL bDeleteEnt=FALSE);	
	//��ȡ��ק����β��ָ��������ʵ�����ڵ����� wht 10-11-30
	BOOL GetTailEntScope(int nEntNum,ads_point &L_T,ads_point &R_B);
	//��ȡ��ק����ʵ�����ڵ����� 
	BOOL GetDragEntSetScope(ads_point &L_T,ads_point &R_B);
	void InitClearDegree(){m_nClearDegree=0;}
	int GetClearDegree(){return m_nClearDegree;}
	void ClearEntSet();
	CDragEntSet();
	virtual ~CDragEntSet();
};
extern CDragEntSet DRAGSET;

#endif // !defined(AFX_DRAGENTSET_H__D92C66C8_D139_42A1_B476_BA1DAF61598B__INCLUDED_)
