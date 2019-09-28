#pragma once
#include "ComplexId.h"
#include "XhCharString.h"

struct CATEGORY{
	long id;
	long idUpper;
	CXhChar16 name;
	CATEGORY(){id=idUpper=0;}
};
struct DESIGN_PARAM_ITEM
{
	KEY4C key;			//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
	UINT uiCategoryId;	//��λΪ1��uiCategoryId&0x80000000>0��ʾ�ò�������ΪĿ¼��
	CXhChar50 sLabel;	//����������ʾ�Ķ����Ʊ�ǩ
	CXhChar100 sNotes;	//�����жԸ������ԵĽ���������
	union{
		double fValue;
		long hPart;
		long iType;
		struct{	//�������
			UINT uiUpperBound;	//�����������������(���ֵ, 0 based index)
			double* arrValues;	//�������ʱ��ָ�����������������ָ����ʱ�ڴ棩
		};
	}value;
	BYTE dataType;	//0.double��ֵ 1.��� 2. ���� 3. ��������
	BOOL bModify;
	void SetKey(DWORD hashkey){key=hashkey;}
	DESIGN_PARAM_ITEM(){uiCategoryId=dataType=0; bModify=FALSE;}
public:	//dataType����������
	static const BYTE TYPE_DOUBLE	= 0;
	static const BYTE TYPE_HANDLE	= 1;
	static const BYTE TYPE_INTEGER	= 2;
	static const BYTE TYPE_DOUBLE_ARR	= 3;
};
//����ʾ�������
struct PARA_SKETCH_ITEM
{
	KEY4C key;
	BYTE m_ciSketchVec;	//1.X����|2.Y����|3.Z����
	long m_hNode1;
	long m_hNode2;
	//
	void SetKey(DWORD hashkey){key=hashkey;}
	PARA_SKETCH_ITEM(){m_ciSketchVec=1,m_hNode1=m_hNode2=0;}
};
