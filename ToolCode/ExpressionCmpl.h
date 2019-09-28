// ExpressionCmpl.h: interface for the CExpression class.
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "list.h"
#include "HashTable.h"
#include "ArrayList.h"
#include "ExpressionValue.h"

//��ͨ��ջ��
//Ԥ��һ��Ԫ�����İ붯̬���飬һ��Ԥ�����������������������ʱ������䶯̬�ڴ� wjh-2017.12.8
struct PRESET_STACKSIZE4  {static const int COUNT=   4;};
struct PRESET_STACKSIZE8  {static const int COUNT=   8;};
struct PRESET_STACKSIZE16 {static const int COUNT=  16;};
struct PRESET_STACKSIZE32 {static const int COUNT=  32;};
struct PRESET_STACKSIZE64 {static const int COUNT=  64;};
struct PRESET_STACKSIZE128{static const int COUNT=128;};
template <class TYPE> struct IStack{
	virtual TYPE* TopAtom()=0;		//����ջ��Ԫ��
	virtual int push(const TYPE& val)=0;
	virtual bool pop(TYPE& res)=0;
	virtual int GetRemnantSize()=0;
	virtual int GetPushSize()=0;
};
template <class TYPE,class TYPE_PRESET_COUNT> class PRESET_STACK :public IStack<TYPE>
{
	TYPE data[TYPE_PRESET_COUNT::COUNT];
	TYPE *bottom,*top,*cursor;
public:
	PRESET_STACK(){
		bottom = data;
		top=bottom+TYPE_PRESET_COUNT::COUNT-1;
		cursor = bottom-1;//NULL;
	}
	~PRESET_STACK(){;}
	BOOL IsEmpty()
	{
		if(cursor>=bottom)
			return TRUE;
		else
			return FALSE;
	}
	void Empty()
	{
		cursor = bottom-1;
	}
	int GetRemnantCount()
	{
		return (int)(top-cursor);
	}
	int GetPushCount()
	{
		return (int)(cursor-bottom+1);
	}
	virtual int GetRemnantSize(){return GetRemnantCount();}
	virtual int GetPushSize(){return GetPushCount();}
	TYPE* TopAtom()		//����ջ��Ԫ��
	{
		return cursor;
	}
	int push(const TYPE& val)
	{
		if(cursor+1>top)
			return FALSE;
		else
		{
			cursor += 1;
			memcpy(cursor, &val, sizeof(TYPE));
		}
		return (int)(cursor-bottom+1);
	}
	bool pop(TYPE& res)
	{
		if(cursor<bottom)
			return false;
		else
		{
			memcpy(&res, cursor, sizeof(TYPE));
			cursor -= 1;
			return true;
		}
	}
};
template <class TYPE> class PRESET_STACK4 : public PRESET_STACK<TYPE,PRESET_STACKSIZE4>
{
};
template <class TYPE> class PRESET_STACK8 : public PRESET_STACK<TYPE,PRESET_STACKSIZE8>
{
};
template <class TYPE> class PRESET_STACK16 : public PRESET_STACK<TYPE,PRESET_STACKSIZE16>
{
};
template <class TYPE> class PRESET_STACK32 : public PRESET_STACK<TYPE,PRESET_STACKSIZE32>
{
};
template <class TYPE> class PRESET_STACK64 : public PRESET_STACK<TYPE,PRESET_STACKSIZE64>
{
};
template <class TYPE> class PRESET_STACK128 : public PRESET_STACK<TYPE,PRESET_STACKSIZE128>
{
};
//������������������������������ʽ��ֵ�ඨ�壭��������������
/*struct EXPRESSION_VAR
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
};*/
class CMPL_EXPRESSION;
struct EXPRESSION_OPERATION{
protected:
	BYTE m_cnExprParamValCount;
	UINT m_arrExprVal[8];	//[0]��ʾ�������;[1]��ʾ�������;[...]����Ϊ�����Ĳ���
public:
	const static BYTE OPER_SELFVALUE = 0;
	const static BYTE OPER_CALC_BASIC = 1;
	const static BYTE OPER_CALC_LOGIC = 2;
	const static BYTE OPER_CALC_FUNCION	= 3;
	const static BYTE OPER_CALC_ARRELEM = 4;
	BYTE ciType;	//��������: 0.��ֵ����;1.���������;2.�߼������;3.��������4.����Ԫ��ȡֵ
	BYTE ciOperator;//�����
	UINT id,idFunction;
	EXPRESSION_OPERATION();
	//UINT idLeftExprVal,idRightExprVal;
	UINT SetExprVal(int index,UINT uiVal);
	const UINT* get_xExprValArr(){return m_arrExprVal;}
	__declspec(property(get=get_xExprValArr)) UINT* xExprValArr;
	UINT get_idLeftExprVal(){return m_arrExprVal[0];}
	UINT set_idLeftExprVal(UINT idExprVal){return SetExprVal(0,idExprVal);}
	__declspec(property(put=set_idLeftExprVal,get=get_idLeftExprVal)) UINT idLeftExprVal;
	UINT get_idRightExprVal(){return m_arrExprVal[1];}
	UINT set_idRightExprVal(UINT idExprVal){return SetExprVal(1,idExprVal);}
	__declspec(property(put=set_idRightExprVal,get=get_idRightExprVal)) UINT idRightExprVal;
	//����
	bool Calc(CMPL_EXPRESSION* pExprContext,EXPRESSION_VALUE *pRsltValue);
};
typedef struct CMPL_EXPRESSION_OBJ{
	UINT id;
	char name[9];
	virtual int GetObjPropId(const char* szPropKey){return 0;}
	CMPL_EXPRESSION_OBJ(){
		id=0;
		memset(name,0,9);
	}
}*CMPL_EXPRESSION_OBJPTR;
typedef struct CMPL_EXPRESSION_WORD //: public EXPRESSION_VALUE
{
//protected:
	UINT id;
public:
	char name[9];	//���ʽ�����ݴʣ���������������ı����������������ȣ�����
	EXPRESSION_VALUE value;	//���ʵ����ռ�����ֵ
	EXPRESSION_OPERATION calcproc;
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	CMPL_EXPRESSION_WORD(BYTE ciValType=0,BYTE ciOperType=0){
		memset(name,0,9);
		value.ciValType=ciValType;
		calcproc.ciType=ciOperType;
	}
	bool Calc(CMPL_EXPRESSION* pExprContext){
		return calcproc.Calc(pExprContext,&value);
	}
}*CMPL_EXPRESSION_WORDPTR;
class CMPL_EXPRESSION_FUNC{
protected:
	UINT id;
	char name[9];
public:
	CMPL_EXPRESSION_FUNC(UINT _id=0);
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	virtual long GetRsltValType(){return 0;}
	virtual bool Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr);	//����
};
typedef CMPL_EXPRESSION_FUNC* EXPRESSION_FUNCPTR;

class CExpressionLocaleFuncLibrary{
public:
	//static const WORD LOCALEFUNC_0	= 1000;	//CExpressionSysFuncLibrary::MAX_SYSFUNCID+1
	//static const WORD LOCALEFUNC_1	= 1001;	//CExpressionSysFuncLibrary::MAX_SYSFUNCID+2
	//static const UINT MAX_SYSFUNCID		= 999;
public:
	ARRAY_LIST<EXPRESSION_FUNCPTR> functions;	//���ػ����庯��
	CHashStrList<UINT> hashFuncsByLabel;
	CExpressionLocaleFuncLibrary(){;}
};
struct INTERPRET_WORD
{
	int word_type;	//0:������1:����������2:����������3:��������4:�����
	union ITEM{
		char chOperator;
		long liConst;
		double fConst;
		CMPL_EXPRESSION_WORD *pExprWord;
		char valueStr[16];
		bool SafeSetChar(int i,char ch)
		{
			if(i<15)
			{
				valueStr[i]=ch;
				return true;
			}
			return false;
		}
	}value;
	INTERPRET_WORD(){
		word_type=1;//WORDTYPE_CONST_FLOAT;
		memset(&value.valueStr,0,16);
	}
};
class CMPL_EXPRESSION : public CMPL_EXPRESSION_WORD
{
protected:
	CMPL_EXPRESSION* m_pReflectExpr;	//!=NULLʱFromFunction�ᷴ�䵽m_pReflectExpr��
	UINT m_uiMinExprWordId,m_uiBaseIndexExprWord;
	PRESET_ARRAY8<CMPL_EXPRESSION_WORD> words;	//���ʽ�����������Ĵ�
	CExpressionLocaleFuncLibrary localefunctions;
public:
	//�������
	static const BYTE OPERATOR_CALC_ADD		= 1;	//��'+'
	static const BYTE OPERATOR_CALC_DEDUCT	= 2;	//��'-'
	static const BYTE OPERATOR_CALC_MULTIPLY= 3;	//��'*'
	static const BYTE OPERATOR_CALC_DIVIDE	= 4;	//��'/'
	//�߼������
	static const BYTE OPERATOR_LOGIC_GT		= 1;	//����Greater Than'>'
	static const BYTE OPERATOR_LOGIC_LT		= 2;	//С��Less Than'<'
	static const BYTE OPERATOR_LOGIC_EQUAL	= 3;	//����'='
	static const BYTE OPERATOR_LOGIC_NEQUAL	= 4;	//����'!='
	static const BYTE OPERATOR_LOGIC_EnLT	= 5;	//'<='
	static const BYTE OPERATOR_LOGIC_EnGT	= 6;	//'>='
	static const BYTE OPERATOR_LOGIC_NOT	= 7;	//��'!'
	static const BYTE OPERATOR_LOGIC_AND	= 8;	//����'&&'
	static const BYTE OPERATOR_LOGIC_OR		= 9;	//��'||'
public:	//��������������Ա�����Լ��������Ĵ洢��Ա������
	CHashStrList<UINT> hashNamedWordIds;
public:
	CMPL_EXPRESSION(CMPL_EXPRESSION* pReflectExpr=NULL,UINT uidMinExprWord=1);
	CMPL_EXPRESSION_WORD* AppendExprWord(BYTE ciValType=0,BYTE ciOperType=0);
	CMPL_EXPRESSION_WORD* AppendExprWord(INTERPRET_WORD* pInterpretWord);
	virtual CMPL_EXPRESSION_WORD* AppendExprWord(CMPL_EXPRESSION_WORD xExprWord);
	virtual CMPL_EXPRESSION_WORD* SetExprWord(UINT idExprWord,CMPL_EXPRESSION_WORD xExprWord);
	virtual CMPL_EXPRESSION_WORD* FromExprWord(UINT idExprWord);
	virtual bool UpdateExprByInterpretWord(INTERPRET_WORD* pInterpretWord);
	virtual CMPL_EXPRESSION_WORD* FindNamedExprWord(const char* varname);
	CMPL_EXPRESSION_FUNC* FromSysFunction(const char* func_label);
	CMPL_EXPRESSION_FUNC* FromSysFunction(UINT idFunc);
	virtual CMPL_EXPRESSION_OBJ* FromExpressionObj(int idObjType);
	virtual UINT FromFunctionName(const char* func_label);
	virtual CMPL_EXPRESSION_FUNC* FromFunction(UINT idFunc);
	virtual bool InterpretBeforeOperWord(IStack<INTERPRET_WORD> *pWordStack, char currentOperator);
	virtual INTERPRET_WORD CompileOneOperation(INTERPRET_WORD before,char chOperator,INTERPRET_WORD after);
	bool CompileLogicExprStr(const char* exprstr);
public:
	bool IsHasOperation(){return calcproc.ciType>0;}
	bool CompileExprStr(const char* exprstr,BYTE ciExprValType=0xff);
	static CMPL_EXPRESSION_WORD* CompileToExprWord(const char* exprstr,CMPL_EXPRESSION* pContainerExpr,IXhList<CMPL_EXPRESSION_WORDPTR>* pListParamWords=NULL);
	bool Calc();	//���
	virtual bool GetExprObjProperty(void* pExprObj,int iExprObjType,int idObjProp,EXPRESSION_VALUE* pRsltValue){return false;}
};
