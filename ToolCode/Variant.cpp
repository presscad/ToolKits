// Variant.cpp: implementation of the Variant class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Variant.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//----------------�칹��Ԫ������������-------------------------------
CVariant::CVariant()
{
	memset(this,0,sizeof(CVariant));
	vType = GRID_VT_EMPTY;		//δָ����������
}
CVariant::CVariant(CVariant& SourceObj)
{
	*this = SourceObj;
}

CVariant::~CVariant()
{
	ClearVariant();
}
void CVariant::ClearVariant()	//����칹���������ڴ沢����洢��
{
	GRID_DATA_TYPE old_type = vType;
	if(vType==GRID_VT_STRING&&sVal)
		free(sVal);		//����ַ����ռ�
	memset(this,0,sizeof(CVariant));	//����洢��
	vType = old_type;	//�ֵ�ԭ��������
}
void CVariant::ChangeToType(GRID_DATA_TYPE data_type)
{
	if(vType==data_type)
		return;
	else
	{
		ClearVariant();
		vType = data_type;
	}
}
	//���ظ�ֵ�������Է���ֵʱ�ڴ�й©
CVariant& CVariant::operator = (const CVariant &var)
{
	if(var.vType==GRID_VT_STRING)
		SetGridString(var.sVal);
	else
		memcpy(this,&var,sizeof(CVariant));
	return *this;
}

bool CVariant::SetGridString(const char *string, bool bForceToString/*=true*/)
{
	if(vType!=GRID_VT_STRING&&!bForceToString||string==NULL)
		return false;	//���Ͳ�ƥ��
	else if(vType!=GRID_VT_STRING)
	{
		ChangeToType(GRID_VT_STRING);
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return true;
	}
	else
	{
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return true;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariant2dArray::CVariant2dArray(VARIANT _var)
{
	var=_var;
}
CVariant2dArray::CVariant2dArray(int rows,int cols)
{
	var.vt=VT_VARIANT|VT_ARRAY;
	SAFEARRAYBOUND rgsabound[2];
	rgsabound[0].lLbound=1;
	rgsabound[0].cElements=rows;
	rgsabound[1].lLbound=1;
	rgsabound[1].cElements=cols;
	var.parray = SafeArrayCreate(VT_VARIANT,2,rgsabound);
}

CVariant2dArray::~CVariant2dArray()
{
}
int CVariant2dArray::RowsCount()
{
	return var.parray->rgsabound[1].cElements;
}
int CVariant2dArray::ColsCount()
{
	return var.parray->rgsabound[0].cElements;
}
int CVariant2dArray::ValidColsCount()
{	//��ǰʮ��Ϊ����̽����Ч�г�
	int nRowsCount=min(10,RowsCount());
	int nColCount=ColsCount();
	VARIANT var;
	int nValidColCount=0;
	for(int i=0;i<nColCount;i++)
	{
		BOOL bHasContent=FALSE;
		for(int j=0;j<nRowsCount;j++)
		{
			if(GetValueAt(j,i,var)&&var.vt!=VT_EMPTY)
			{
				bHasContent=TRUE;
				break;
			}
		}
		if(!bHasContent)
			break;
		nValidColCount++;
	}
	return nValidColCount;
}
bool CVariant2dArray::GetValueAt(int i,int j,VARIANT& value)
{
	value.vt=VT_EMPTY;
	i+=var.parray->rgsabound[1].lLbound;
	j+=var.parray->rgsabound[0].lLbound;
	LONG indices[2]={i,j};
	bool result=FAILED(SafeArrayGetElement(var.parray,&indices[0],&value));
	return !result;
}
bool CVariant2dArray::SetValueAt(int i,int j,VARIANT value)
{
	i+=var.parray->rgsabound[1].lLbound;
	j+=var.parray->rgsabound[0].lLbound;
	LONG indices[2]={i,j};
	return !FAILED(SafeArrayPutElement(var.parray,indices,&value));
}
