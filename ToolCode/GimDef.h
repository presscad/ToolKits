// ModDataFile.h: interface for the CModDataFile class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Buffer.h"
#include "XhCharString.h"
//////////////////////////////////////////////////////////////////////////
struct GIM_HEAD_PROP_ITEM
{
	char m_sFileTag[16];		//�ļ���ʶ
	char m_sFileName[256];		//�ļ�����
	char m_sDesigner[64];		//�����
	char m_sUnit[256];			//��֯��λ
	char m_sSoftName[128];		//�������
	char m_sTime[16];			//����ʱ��
	char m_sSoftMajorVer[8];	//������汾��
	char m_sSoftMinorVer[8];	//����ΰ汾��
	char m_sMajorVersion[8];	//��׼���汾��
	char m_sMinorVersion[8];	//��׼�ΰ汾��
	char m_sBufSize[8];			//�洢�����С
	//
	GIM_HEAD_PROP_ITEM()
	{
		memset(this,0,sizeof(GIM_HEAD_PROP_ITEM));
		strcpy(m_sFileTag,"GIMPKGT");
		strcpy(m_sSoftName,"�ź������ƻ�ͼһ�廯���");
		strcpy(m_sSoftMajorVer,"1");	//������汾��
		strcpy(m_sSoftMinorVer,"0");	//����ΰ汾��
		strcpy(m_sMajorVersion,"1");	//��׼���汾��
		strcpy(m_sMinorVersion,"0");	//��׼�ΰ汾��
	}
	void Clone(GIM_HEAD_PROP_ITEM* pSrcItem)
	{
		strcpy(m_sDesigner, pSrcItem->m_sDesigner);
		strcpy(m_sUnit, pSrcItem->m_sUnit);
		strcpy(m_sSoftMajorVer,pSrcItem->m_sSoftMajorVer);
		strcpy(m_sSoftMinorVer,pSrcItem->m_sSoftMinorVer);
		strcpy(m_sMajorVersion,pSrcItem->m_sMajorVersion);
		strcpy(m_sMinorVersion,pSrcItem->m_sMinorVersion);
	}
	void ToBuffer(BUFFER_IO* pIO)
	{	//д��ָ��ͷ����
		pIO->Write(m_sFileTag,16);			//�ļ���ʶ
		pIO->Write(m_sFileName,256);		//�ļ�����
		pIO->Write(m_sDesigner,64);		//�����
		pIO->Write(m_sUnit,256);			//��֯��λ
		pIO->Write(m_sSoftName,128);		//�������
		pIO->Write(m_sTime,16);			//����ʱ��		
		pIO->Write(m_sSoftMajorVer,8);		//������汾��
		pIO->Write(m_sSoftMinorVer,8);		//����ΰ汾��
		pIO->Write(m_sMajorVersion,8);		//��׼���汾��
		pIO->Write(m_sMinorVersion,8);		//��׼�ΰ汾��
		pIO->Write(m_sBufSize,8);			//�洢�����С
	}
	void FromBuffer(BUFFER_IO* pIO)
	{	//��ȡͷ����
		pIO->Read(m_sFileTag,16);		//�ļ���ʶ
		pIO->Read(m_sFileName,256);	//�ļ�����
		pIO->Read(m_sDesigner,64);		//�����
		pIO->Read(m_sUnit,256);		//��֯��λ
		pIO->Read(m_sSoftName,128);	//�������
		pIO->Read(m_sTime,16);			//����ʱ��		
		pIO->Read(m_sSoftMajorVer,8);	//������汾��
		pIO->Read(m_sSoftMinorVer,8);	//����ΰ汾��
		pIO->Read(m_sMajorVersion,8);	//��׼���汾��
		pIO->Read(m_sMinorVersion,8);	//��׼�ΰ汾��
		pIO->Read(m_sBufSize,8);		//�洢�����С
	}
};
struct TOWER_PRPERTY_ITEM
{
	CXhChar100 m_sVoltGrade;		//��ѹ�ȼ�(��λKV)
	CXhChar100 m_sType;				//�ͺ�
	CXhChar100 m_sTexture;			//�������ʣ��Ǹ���|�ֹ���
	CXhChar100 m_sFixedType;		//�̶���ʽ������
	CXhChar100 m_sTaType;			//��������
	int m_nCircuit;					//��·��
	CXhChar100 m_sCWireSpec;		//�����ͺ�
	CXhChar100 m_sEWireSpec;		//�����ͺ�
	double m_fWindSpeed;			//��������
	double m_fNiceThick;			//�������
	CXhChar100 m_sWindSpan;			//ˮƽ����
	CXhChar100 m_sWeightSpan;		//��ֱ����
	double m_fFrontRulingSpan;		//ǰ�������
	double m_fBackRulingSpan;		//��������
	double m_fMaxSpan;				//��󵵾�
	CXhChar100 m_sAngleRange;		//ת�Ƿ�Χ
	double m_fDesignKV;				//���Kvֵ
	CXhChar100 m_sRatedHeight;		//�������
	CXhChar500 m_sHeightRange;		//���߷�Χ
	CXhChar500 m_sTowerWeight;		//����
	double m_fFrequencyRockAngle;	//��Ƶҡ�ڽǶ�
	double m_fLightningRockAngle;	//�׵�ҡ�ڽǶ�
	double m_fSwitchingRockAngle;	//����ҡ�ڽǶ�
	double m_fWorkingRockAngle;		//������ҵҡ�ڽǶ�
	CXhChar100 m_sManuFacturer;		//��������
	CXhChar100 m_sMaterialCode;		//���ʱ���
	CXhChar100 m_sProModelCode;		//װ��ģ�ͱ��
	//
	TOWER_PRPERTY_ITEM()
	{
		m_sVoltGrade.Copy("220KV");
		m_sTexture.Copy("�Ǹ���");
		m_sFixedType.Copy("����");
		m_sTaType.Copy("������");
		m_nCircuit=1;
		m_fWindSpeed=0;
		m_fNiceThick=0;
		m_fFrontRulingSpan=0;
		m_fBackRulingSpan=0;
		m_fMaxSpan=0;
		m_fDesignKV=0;
		m_fFrequencyRockAngle=0;
		m_fLightningRockAngle=0;
		m_fSwitchingRockAngle=0;
		m_fWorkingRockAngle=0;
	}
	void Clone(TOWER_PRPERTY_ITEM* pSrcItem)
	{
		m_sVoltGrade=pSrcItem->m_sVoltGrade;
		m_sType=pSrcItem->m_sType;
		m_sTexture=pSrcItem->m_sTexture;
		m_sFixedType=pSrcItem->m_sFixedType;
		m_sTaType=pSrcItem->m_sTaType;
		m_nCircuit=pSrcItem->m_nCircuit;
		m_sCWireSpec=pSrcItem->m_sCWireSpec;
		m_sEWireSpec=pSrcItem->m_sEWireSpec;
		m_fWindSpeed=pSrcItem->m_fWindSpeed;
		m_fNiceThick=pSrcItem->m_fNiceThick;
		m_sWindSpan=pSrcItem->m_sWindSpan;
		m_sWeightSpan=pSrcItem->m_sWeightSpan;
		m_fFrontRulingSpan=pSrcItem->m_fFrontRulingSpan;
		m_fBackRulingSpan=pSrcItem->m_fBackRulingSpan;
		m_fMaxSpan=pSrcItem->m_fMaxSpan;
		m_sAngleRange=pSrcItem->m_sAngleRange;
		m_fDesignKV=pSrcItem->m_fDesignKV;
		m_sRatedHeight=pSrcItem->m_sRatedHeight;
		m_sHeightRange=pSrcItem->m_sHeightRange;
		m_sTowerWeight=pSrcItem->m_sTowerWeight;
		m_fFrequencyRockAngle=pSrcItem->m_fFrequencyRockAngle;
		m_fLightningRockAngle=pSrcItem->m_fLightningRockAngle;
		m_fSwitchingRockAngle=pSrcItem->m_fSwitchingRockAngle;
		m_fWorkingRockAngle=pSrcItem->m_fWorkingRockAngle;
		m_sManuFacturer=pSrcItem->m_sManuFacturer;
		m_sMaterialCode=pSrcItem->m_sMaterialCode;
		m_sProModelCode=pSrcItem->m_sProModelCode;
	}
};
