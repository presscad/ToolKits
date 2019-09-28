#include "XhCharString.h"
#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"

enum DIMALIGN_TYPE{AlignDefault=1,TopLeft,TopCenter,TopRight,MiddleLeft,MiddleCenter,MiddleRight,BottomLeft,BottomCenter,BottomRight};
typedef struct tagTMADRAWING_DIM_STYLE
{
	CXhChar16 dimStyleName;	//������ʽ����
	double arrowSize;		//��ͷ��С
	double gap;				//�������׼�߼�϶
	unsigned long dimStyleId;	//������ʽ
	tagTMADRAWING_DIM_STYLE(){arrowSize=gap=0;}
	void InitDimStyle(double textHeight=0,double arrowSize=0,double gap=0);
}TMADRAWING_DIM_STYLE;
class CDimStyle{
public:
	TMADRAWING_DIM_STYLE style;
	double textHeight;
	CDimStyle(){textHeight=2.0;}
};

//����ע�ߵ��ı��ߴ��ע
class CSizeTextDim
{
	CXhChar50 dimText;
	DIMALIGN_TYPE alignType;
public:
	static double PRECISION;		//�������־����עʱ�ľ���,ֻȡ1.0,0.5,0.1����ֵ
	static CDimStyle dimStyle;
	f3dPoint origin;		//���붨λ����λ��
	f3dPoint dimStartPos;	//��ע����ʼ��(����ڶ��붨λ����)
	f3dPoint dimEndPos;		//��ע����ֹ��(����ڶ��붨λ����)
	f3dPoint dimPos;		//��ע����λ��(����ڶ��붨λ����)
	double angle;			//����
	double dist;			//��ע����
	int nHits;				//�����ĵȾ�ΪS�ı�ע�����Ժϲ�Ϊһ��S X nHits�ı�ע
	//0.װ����ߴ� 1.�ֲ�ƫ�Ƴߴ� 2.�Ŷ���� 
	//3(-3).������λ�ߴ� (+-������) 4.����浽���������ע
	int iOrder;				//�ֲ��עʱ����ȷ���ߴ綨λ˳��
public:
	BOOL bAllowRelayout;	//�Ƿ������Զ�������עλ��,ĳЩ������������Զ�����λ�� wht 10-08-15
	BOOL bShortenDimStartLine,bShortenDimEndLine;
	CSizeTextDim();
	//precisionֻ��Ϊ"0","0.5","0.0"
	double Fillet();	//����ע����dist����precision����Բ����
	const char* DimText();
	double GetWidth();
	double GetHeight(BOOL bIncShortDimLineOnly=FALSE);
	//�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16;
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly=FALSE,double draw_scale=1.0) const;
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault);
};

class CTextOnlyDrawing{
public:
	static double PRECISION;
	DIMALIGN_TYPE alignType;
	CXhChar50 dimText;		//��ע���ı�����
	f3dPoint origin;		//���붨λ����λ��
	double textHeight;
	f3dPoint dimPos;		//��ע����λ��(����ڶ��붨λ����)
	bool bNeedConvertDistToText;	//��Ҫ��dist��GetPrecisionDimText()�б�Ϊ��ע�ַ���
	double angle;			//����
	double dist;			//һ������dimText���Ӧ��˫�����������Ǳ��룩
	BOOL bInXWing;
	//0.���ӷ������� 1.���Գߴ� +-2(������).��ͼ����  3(-3).����ͼ�� +��ʾ��X�������� -��ʾ��X�Ḻ����
	int iOrder;				//�ֲ��עʱ����ȷ���ߴ綨λ˳��
public:
	CTextOnlyDrawing(){iOrder=0;angle=dist=0;textHeight=2.0;alignType=MiddleLeft;bNeedConvertDistToText=true;}
	double Fillet();
	//precisionֻ��Ϊ"0","0.5","0.0"
	const char* GetPrecisionDimText();
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0); //�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault);
};

class CZhunXianTextDim : public CSizeTextDim
{
public:
	f3dPoint lineStart;
	f3dPoint lineEnd;
	long hRelaBolt;		//������˨���,���ܻ�ͼʱʹ�� wht 11-08-10
	bool bDimZhunJu;	//��ע����׼��
	CZhunXianTextDim() {hRelaBolt=0;bDimZhunJu=true;}
};

class ACDB
{
public:
	enum TextHorzMode      
	{	kTextLeft    = 0,   // TH_LEFT,
		kTextCenter  = 1,   // TH_CENT,
		kTextRight   = 2,   // TH_RIGHT,
		kTextAlign   = 3,   // THV_ALIGN,
		kTextMid     = 4,   // THV_MID,
		kTextFit     = 5 }; // THV_FIT

	enum TextVertMode      
	{	kTextBase    = 0,   // TV_BASE,
		kTextBottom  = 1,   // TV_BOT,
		kTextVertMid = 2,   // TV_MID,
		kTextTop     = 3 }; // TV_TOP
};

class CKaiHeAngleDim
{
	DIMALIGN_TYPE alignType;
public:
	CDimStyle dimStyle;
	CXhChar100 dimText;		//���ϽǱ�ע����(�����Ǹ��ϵ����λ�ü����ϽǶ�)
	f3dPoint origin;		//���붨λ����λ��
	//real_XXX��ʾ�����Ǳ����ߵĳߴ�
	f3dPoint dimPos,real_dim;			//���ϽǱ�עλ��(����ڶ��붨λ����)
	f3dPoint dimStartPos,start_ls_pos;	//���ϽǱ�ע��ʼ��(����ڶ��붨λ����)
	f3dPoint real_start,real_start_ls_pos;
	f3dPoint dimEndPos,end_ls_pos;		//���ϽǱ�ע��ֹ��
	f3dPoint real_end,real_end_ls_pos;
	double dist;		//���Ϸ�Χ
	BOOL bInWingX;
	int start0_end1_else2;	//��ʾ
public:
	CKaiHeAngleDim(){dist=0;bInWingX=0;start0_end1_else2=2;}
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4]);
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition();
};

//���߿���ı�
class CTextFrame{
	double margin;		//�ı���߿�ļ�϶
	double frameWidth;	//�߿�ĳ���
	double frameHeight;	//�߿�ĸ߶�
	double textHeight;	//����߶�
	double WtoH;		//�����߱�
	CXhChar16 dimText,dimText2;	//�߿��б�ע���ı�
public:
	ACDB::TextHorzMode textAlignHori;
	ACDB::TextVertMode textAlignVert;
	int frameStyle;		//-1.�Զ��ж�;0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	f3dPoint dataPos;	//����λ��
	f3dPoint dimPos;	//��עλ��
	long hPart;			//�����������
	int	 cls_id;		//������������
	int	 iCurLayer;		//�ı�����ͼ��
public:
	f3dPoint center,widthVec;	//�����ڻ�ͼʱ��ʱ��ȡ�����ã�ֻӰ��߿�λ�ò�Ӱ��߿��С
	CTextFrame();
	//��ʼ��
	void InitTextFrame(double text_height=2.5,double frame_width=0,
		int frame_style=-1,double space=0.2,double w_to_h=0.7);
	//���ܽ����߿� frameStyle:-1.�Զ��ж�;0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	bool BuildFrame(char* dimText,char* dimText2=NULL,double frameWidth=0,int frameStyle=-1);
	//���������Ի�ȡ����
	char*  Text(){return dimText;}
	char*  Text2(){return dimText2;}
	//�߿�����:0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	int    Style(){return frameStyle;}
	//�߿���(ֱ��)
	double Width(){return frameWidth;}
	//�߿�߶�
	double Height(){return frameHeight;}
	//�߿���ı��ļ�϶
	double BoundarySpace(){return margin;}
	//�õ���ž��ο� //�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0) const;
	//������������ı������Ӷ˵Ķ˵�λ�� 
	//���ݱ����õ������߶˽ڵ�λ�� ����1:1�麸ͼʱ��Ҫ wht 10-09-17
	f3dPoint GetRefLineEndPos(double draw_scale=1.0);
};

class DimStyleTable
{
public:
	static TMADRAWING_DIM_STYLE dimStyle;
	static TMADRAWING_DIM_STYLE dimStyle2;
	static TMADRAWING_DIM_STYLE dimStyle3;	//��ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	static TMADRAWING_DIM_STYLE dimAngleStyle;
	static TMADRAWING_DIM_STYLE dimRadiusStyle;
};

class CDrawNewSysPara
{
public:
	int  iLsSpaceDimStyle;		//0.X�᷽��	  1.Y�᷽��  2.�Զ��ж� 3.����ע  4.�ޱ�ע����(X�᷽��)  4.����ߴ��ߣ��ޱ�ע����(X�᷽��)��Ҫ���ڽ���(��)�����ʼ���ä������
	int  nMaxBoltNumAlongX;		//��X�᷽���ע֧�ֵ������˨����
	int  iJgGDimStyle;			//0.ʼ�˱�ע  1.�м��ע 2.�Զ��ж�
	int iJgZoomSchema;			//�Ǹֻ��Ʒ�����0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����
	BOOL bDimRibPlateSetUpPos;	//��ע�Ǹ��߰尲װλ��
	double	fDimTextSize;		//���ȳߴ��ע�ı���
	int iDimPrecision;			//�ߴ羫ȷ��
	double fKaiHeJiaoThreshold; //���ϽǱ�ע��ֵ(��) 
	BOOL bMaxExtendAngleLength;	//����޶�����Ǹֻ��Ƴ���
	double fRealToDraw;			//������ͼ�����ߣ�ʵ�ʳߴ�/��ͼ�ߴ磬��1:20ʱ��fRealToDraw=20
	double	fDimArrowSize;		//�ߴ��ע��ͷ��
	int  nMaxBoltNumStartDimG;	//������ʼ�˱�ע׼��֧�ֵ������˨��
	CDrawNewSysPara()
	{
		memset(this,0,sizeof(CDrawNewSysPara));
		fRealToDraw=5;
		iJgZoomSchema=3;
		fDimTextSize=3;
		nMaxBoltNumAlongX=100;
	}
};




