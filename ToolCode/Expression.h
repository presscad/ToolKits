// Expression.h: interface for the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESSION_H__01230132_F1FB_485D_9A6C_B0BFAAB4C1CA__INCLUDED_)
#define AFX_EXPRESSION_H__01230132_F1FB_485D_9A6C_B0BFAAB4C1CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "list.h"
#include "ExpressionValue.h"

//������������������������������ʽ��ֵ�ඨ�壭��������������
struct EXPRESSION_VAR
{
	union{
		double fValue;			//������ֵ
		struct{	//�������
			UINT uiUpperBound;	//�����������������(���ֵ, 0 based index)
			double* arrValues;	//�������ʱ��ָ�����������������ָ����ʱ�ڴ棩
		};
	};
	char variableStr[9];	//��������
	EXPRESSION_VAR(){memset(this,0,sizeof(EXPRESSION_VAR));}
};
class CExpression;
struct EXPRESSION_FUNC{
protected:
	UINT id;
public:
	char name[19];
	EXPRESSION_FUNC(UINT _id=0);
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	virtual long GetRsltValType();
	virtual bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr);	//����
};
/*
	�����ʽֵ���ࡣ
	������ֻ���м�(+)��(-)��(*)��(/(���ָ������㡣
	����֮ǰ��Ҫ�Ȱѱ��ʽ�����õı���������ӵ�varList�У�Ȼ��ѱ��ʽ���ַ���
	������ʽ���뵽SolveExpression(...)�У����շ���˫�����ͼ�������
	����������������������ı��ʽ��ϵͳ���׳�char* �����ַ����쳣
	����ƽֱ���󽻺���
	INT_X(line1_startx,line1_starty,line1_endx,line1_endy,
	      line2_start_x,line2_start_y,line2_end_x,line2_end_y)	����line1^line2�����X�������
	INT_Y(line1_startx,line1_starty,line1_endx,line1_endy,
		  line2_start_x,line2_start_y,line2_end_x,line2_end_y)	����line1^line2�����Y�������
*/
class CExpression  
{
protected:
	virtual EXPRESSION_VAR * FindExpressionVariable(char* var);
	virtual EXPRESSION_FUNC* FromFunctionName(const char* func_label);

	double Int2dLine(char* paraStr,int x0_y1);
public:
	CExpression();
	virtual ~CExpression();
	CXhSimpleList<EXPRESSION_VAR> varList;
	EXPRESSION_VAR* VarAt(int index);
	static void ToExpression(double f,int precision,char* destStr,int maxCount);
	double SolveExpression(const char* expression);
	bool SolveLogicalExpression(const char* expression);//Ŀǰ�ñ��ʽ��ֻ�ܳ���һ��'<'��'>'��'='��'<='��'>='��'!='�߼�������
public:	//�߼������
	static const int OPER_TYPE_GREATER		= 1;	//>
	static const int OPER_TYPE_LESS			= 2;	//<
	static const int OPER_TYPE_EQUAL		= 3;	//=
	static const int OPER_TYPE_NOTEQUAL		= 4;	//!=
	static const int OPER_TYPE_NOTGREATER	= 5;	//<=
	static const int OPER_TYPE_NOTLESS		= 6;	//>=
};

#endif // !defined(AFX_EXPRESSION_H__01230132_F1FB_485D_9A6C_B0BFAAB4C1CA__INCLUDED_)
