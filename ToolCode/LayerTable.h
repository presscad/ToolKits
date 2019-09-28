#pragma once
#ifndef __LAYER_TABLE_H_
#define __LAYER_TABLE_H_
#include "XhCharString.h"
#include "HashTable.h"
#include "f_ent_list.h"

typedef struct tagTOWERATOM_COLOR_INDEX
{
	int crNode;
	int crOutLine;		//�ɼ������߼�һ������
	int crDashLine;		//���ɼ������߼��Ǳ���ʵ��
	int crLs;			//��˨���ż���ע
	int crDimSimTri;	//���������μ���ע
	int crSingleLine;	//��ʵ�������ڵ���ͼ
	int crAxisLine;		//�㻮�ߣ������ߣ�
	int crHuoQuLine;	//������
	int crDragLine;		//������

	tagTOWERATOM_COLOR_INDEX() {
		crLs = 1;		//��˨--��ɫ
		crOutLine = 7;		//������--�ڰ�
		crSingleLine = 7;	//����ͼ--�ڰ�
		crHuoQuLine = 3;	//������--��ɫ
		crDashLine = 1;		//���̻���--��ɫ
		crAxisLine = 2;		//(��)����--����
		crNode = 4;			//�ڵ�--��ɫ
		crDimSimTri = 1;	//��ע--�ڰ�
		crDragLine = 3;		//������--��ɫ
	}

}TOWERATOM_COLOR_INDEX;

//CAD�����Ĳ���������ͼ�����ʽ����
typedef struct tagTMADRAWING_LAYER
{
	int colorIndex;				//ͼ��Ĭ�ϵ�ͼԪ��ʾ��ɫ����
	AcDb::LineWeight lineWeight;//���Ϳ�
	CXhChar16 layerName;		//ͼ������Ӧ��һ���ַ�����
	CXhChar16 lineTypeName;		//ͼ��Ĭ�����͵��ַ�������
	AcDbObjectId layerId;		//ͼ���Ӧ��ACAD�����ʶId
	AcDbObjectId lineTypeId;	//ͼ��Ĭ�����͵�ACAD�����ʶId
	void CopyProperty(tagTMADRAWING_LAYER *pLayer);
}TMADRAWING_LAYER;
class LayerTable
{
public:
	static CHashStrList<TMADRAWING_LAYER> layerHashList;	//ͼ���ϣ����ͼ����Ϊ��ֵ
	static TMADRAWING_LAYER VisibleProfileLayer;	//�ṹͼ�пɼ�������ͼ��
	static TMADRAWING_LAYER BriefLineLayer;			//�����ĵ���ͼ
	static TMADRAWING_LAYER AuxLineLayer;			//���ĵ���ͼ
	static TMADRAWING_LAYER UnvisibleProfileLayer;	//�ṹͼ�в��ɼ�������ͼ��
	static TMADRAWING_LAYER AxisLineLayer;			//�����ߡ��Գ��ߡ���λ����
	static TMADRAWING_LAYER SonJgAxisLineLayer;		//�ӽǸ�����ͼ�� 
	static TMADRAWING_LAYER BreakLineLayer;			//�Ͽ�����
	static TMADRAWING_LAYER BendLineLayer;			//�Ǹֻ������ְ����
	static TMADRAWING_LAYER AssistHuoQuLineLayer;	//����������
	static TMADRAWING_LAYER SimPolyLayer;			//�����α�ע
	static TMADRAWING_LAYER BoltSymbolLayer;		//��˨ͼ��
	static TMADRAWING_LAYER BoltLineLayer;			//��˨��
	static TMADRAWING_LAYER DimSizeLayer;			//�ߴ��ע
	static TMADRAWING_LAYER DimTextLayer;			//���ֱ�ע
	static TMADRAWING_LAYER DamagedSymbolLine;		//�����������
	static TMADRAWING_LAYER CommonLayer;			//���������������ͼԪ����ͼ��

	static TOWERATOM_COLOR_INDEX ColorIndex;

	static void InitLayerTable();
};

//ͼ����������
bool load_linetype(char *LineType);
bool CreateNewLayer(char *newlayer, char* line_type, AcDb::LineWeight line_thick,
	int color_i, AcDbObjectId &layerId, AcDbObjectId &lineTypeId);
void GetCurDwgLayers(ATOM_LIST<CXhChar50> &layer_list);
#endif