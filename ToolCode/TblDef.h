#ifndef __TABLE_DEF_H
#define __TABLE_DEF_H

	//��ʽ��Ķ�λģʽ
enum TBL_LOCATE_STYLE{
	TOP_LEFT = 0,		//���Ͻ�
	TOP_RIGHT = 1,		//���Ͻ�
	BOTTOM_LEFT = 2,	//���½�
	BOTTOM_RIGHT = 3,	//���½�
};
enum TBL_TEXT_ALIGN{
	TEXTALIGN_RIGHT_CENTER  = 0,	//������
	TEXTALIGN_MIDDLE_CENTER = 1,	//������
	TEXTALIGN_BOTTOM_RIGHT  = 2,	//���½�
	TEXTALIGN_TOP_LEFT	  = 3,	//���½�
	TEXTALIGN_LEFT_CENTER	  = 4,	//������
};
	//���Ԫ��(��ʾ)ģʽ
enum GRID_MODE{
	STRING_GRID		= 0,	//��ʾ�ַ���
	GRAPH_BLOCK_GRID= 1,	//��ʾͼ��
	PIC_BLOCK_GRID	= 2,	//��ʾOLEͼƬ
};
	//�������������ݵ���������
#ifndef _DEF_GRID_DATA_V_TYPE
#define _DEF_GRID_DATA_V_TYPE
enum GRID_DATA_TYPE{
	GRID_VT_EMPTY	= 0,	// δָ��
	GRID_VT_NULL	= 1,	// �գ��ݱ�����
	GRID_VT_I2		= 2,	// 2�ֽڴ���������
	GRID_VT_I4		= 3,	// 4�ֽڴ���������
	GRID_VT_R4		= 4,	// 4�ֽڸ�����
	GRID_VT_R8		= 5,	// 8�ֽڸ�����
	GRID_VT_DATE	= 6,	// ����
	GRID_VT_STRING	= 7,	// �ַ���
	GRID_VT_POINTER = 8,	// ָ��
	GRID_VT_BOOL	= 9,	// ������; TRUE=1, FALSE=0
	// ����ö�ٳ����ڴ���Ч
};
#endif
GRID_DATA_TYPE IntToGridDataType(int nType);
class CGridVariant{
public:
	GRID_DATA_TYPE vType;
	union{
		BOOL					bVal;     // GRID_VT_BOOL
		__int16					iVal;     // GRID_VT_I2
		__int32					lVal;     // GRID_VT_I4
		float					fltVal;   // GRID_VT_R4
		double					dblVal;   // GRID_VT_R8
		DATE					date;     // GRID_VT_DATE
		char*					sVal;     // GRID_VT_STRING
		void*					pVal;// GRID_VT_POINTER
	};
public:
	void Import(FILE* fp);
	void Export(FILE* fp);
	CGridVariant();
	CGridVariant(CGridVariant& SourceObj);
	~CGridVariant();
	CGridVariant& operator = (const CGridVariant &var);
	void ChangeToType(GRID_DATA_TYPE data_type);
	void ClearVariant();
	BOOL SetGridString(char *string,BOOL bForceToString=TRUE);
};
#ifndef __DECLARE_GRID_DATA_STRU__
#define __DECLARE_GRID_DATA_STRU__
typedef struct tagGRID_DATA_STRU
{
	int data_type;	//0.�����ı�1.ʵʱ�ı�2.ʵʱ����3.��ͼ����
	long type_id;
	char sContent[MAX_PATH];
	char sContentPrefix[16];
	char sContentSuffix[16];
	double min_x,min_y,max_x,max_y;
	double fTextHigh,fHighToWide;
	//1.TopLeft    2.TopCenter    3.TopRight
	//4.MiddleLeft 5.MiddleCenter 6.MiddleRight
	//7.BottomLeft 8.BottomCenter 9.BottomRight
	int align_type;	
	tagGRID_DATA_STRU(){
		data_type=0;
		type_id=0;
		strcpy(sContent,"");
		strcpy(sContentPrefix,"");
		strcpy(sContentSuffix,"");
		min_x=min_y=max_x=max_y=0;
		fTextHigh=fHighToWide=0;
		align_type=0;
	}
}GRID_DATA_STRU;
#endif
class CGrid
{
public:
	//���Ԫ��������
	class CFontStyle{
	public:
		//�����ʽ
		LONG lfHeight; 
		LONG lfWidth; 
		LONG lfEscapement; 
		LONG lfOrientation; 
		LONG lfWeight; 
		BYTE lfItalic; 
		BYTE lfUnderline; 
		BYTE lfStrikeOut; 
		BYTE lfCharSet; 
		BYTE lfOutPrecision; 
		BYTE lfClipPrecision; 
		BYTE lfQuality; 
		BYTE lfPitchAndFamily; 
		TCHAR* lfFaceName; 
		COLORREF crTextColor;
		//���ֶ��뷽ʽ
		UINT m_nTextAlignFlags;
		//�����ڱ߾�
		double m_fLeftDist,m_fRightDist,m_fTopDist,m_fBottomDist;
	public:
		CFontStyle();
		~CFontStyle();
		void Import(FILE* fp);
		void Export(FILE* fp);
		CFontStyle& operator =(const CFontStyle& fontstyle);
		CFontStyle& operator =(const LOGFONT& fontstyle);
		BOOL operator ==(const CFontStyle& fontstyle)const;
		BOOL operator !=(const CFontStyle& fontstyle)const;
		operator LOGFONT()const;
		int SetFaceName(const TCHAR *face_name);
		void CopyFromLOGFONT(const LOGFONT &logfont);
		void CopyToLOGFONT(LOGFONT &logfont)const;
	};
	//��ֱ�ϲ��鵥Ԫ��ˮƽ�ϲ��鵥Ԫ���������鵥Ԫ��Ӧ����[��ʵ��Ԫ]��
	struct{
		BOOL bTitle;		//���ڱ�ʶ�Ƿ�Ϊ���Ԫ���еı�����
		long type_id;		//���ڱ�ʶ��Ԫ�����������
		long ext_feature;	//������չ,�Ժ��������
	}mark;
	BOOL m_bVGridVirtual,m_bHGridVirtual;
	BOOL m_bSelectStatus;
	int iRow,iColumn;			//����λ�ú�
#ifdef __DRAWING_CONTEXT_
	double textsize;
	double text_angle;			//������ת����
	static double PREFER_TEXT_SIZE;
#else
	CFontStyle font_style;			//�ָ�
#endif
	GRID_MODE grid_mode;		//���Ԫ��(��ʾ)ģʽ

	CGridVariant data;			//����е���������
	DWORD feature;				//�������һЩ����(���û���)
public:
	CGrid();
	~CGrid();
public:
#ifndef __DRAWING_CONTEXT_
	HRESULT ReadGridFromStream(IStream *pStream);
	HRESULT WriteGridToStream(IStream *pStream);
#endif
	void Import(FILE* fp);
	void Export(FILE* fp);
	void Copy(const CGrid& grid);
	CGrid& operator = (CGrid& grid);
};

#endif
