#include "stdafx.h"
#include "SegI.h"
//#include "f_ent.h"
#include "math.h"
#include "ComparePartNoString.h"

typedef struct tapKEY_SECTION
{
	int  m_iStrType;//�ַ��������� 1--����(С�����Ϊ����)  2--�����ַ� 3--16��������
	char sInfo[51];
	tapKEY_SECTION(){memset(this,0,sizeof(tapKEY_SECTION));}
}KEY_SECTION;
#define      LOCAL_EPS              1e-9
//���ַ����ֽ��ֻ�����ֻ���ĸ�����ɶ�
static void StrSubsection(const char *str,KEY_SECTION strSection[],int *pSectionNum,BOOL bPartNo)
{
	int i=0,nSubLen=0,nSection=0;
	int nStrLen=strlen(str);
	//���ַ���һ�ֽ��ֻ�����ֻ���ĸ�����ɶ�
	if(nStrLen>2&&str[0]=='0'&&(str[1]=='X'||str[1]=='x'))
	{	//����Ϊ16���������ַ���
		for(i=2;i<nStrLen;i++)
		{
			BYTE ch=str[i];
			if(isdigit(ch)||(toupper(ch)>='A'&&toupper(ch)<='F'))
				continue;
			break;
		}
		if(i==nStrLen)	
		{	//��ǰ�ַ���Ϊ16�������� wht 11-07-25
			nSection=1;
			strSection[0].m_iStrType=3;	//16��������
			strncpy(strSection[0].sInfo,str,50);
			if(pSectionNum)
				*pSectionNum=nSection;
			return;
		}
	}
	//����������ʱ������������ַ�Ϊ��ĸ����Ը���ĸ wht 12-03-10
	int iStart=0;
	if(bPartNo&&nStrLen>1&&isalpha((BYTE)str[0])&&isdigit((BYTE)str[1]))
		iStart=1;
	for(i=iStart;i<nStrLen;i++)
	{
		if(i==iStart)
		{	//�жϵ�һ���ַ�������
			if(isdigit((BYTE)str[i])||str[i]=='.')
				strSection[nSection].m_iStrType=1;
			else
				strSection[nSection].m_iStrType=2;
		}
		else
		{
			if((isdigit((BYTE)str[i])||str[i]=='.')&&(!isdigit((BYTE)str[i-1])&&str[i-1]!='.'))
			{	//��ǰ�ַ�Ϊ���� ǰһ�ַ�Ϊ������ ��ʼһ���µ��ַ���
				nSection++;
				strSection[nSection].m_iStrType=1;
				nSubLen=0;
			}
			else if((!isdigit((BYTE)str[i])&&str[i]!='.')&&(isdigit((BYTE)str[i-1])||str[i-1]=='.'))
			{	//��ǰ�ַ�Ϊ������ ǰһ�ַ�Ϊ���� ��ʼһ���µ��ַ���
				nSection++;
				strSection[nSection].m_iStrType=2;
				nSubLen=0;
			}
		}
		strSection[nSection].sInfo[nSubLen]=str[i];
		nSubLen++;
	}
	if (strSection[0].m_iStrType==1)//��һ�α���������
	{
		int nSize = strlen(strSection[0].sInfo);
		if(bPartNo)
		{
			for(i=0;i<nSize;i++)//�ų���˨
			{
				if(strSection[0].sInfo[i]=='.')
				{
					bPartNo=FALSE;
					break;
				}
			}
		}
		if (bPartNo)
		{
			if(nSection==0)//��ִ�����
			{
				nSection+=2;
				int n = nSize-2;
				for(i=0;i<nSubLen-1;i++)
					strSection[nSection].sInfo[i] = strSection[0].sInfo[i+n];
				strSection[0].sInfo[n] = '\0';
				strSection[1].sInfo[0] = '-';
				strSection[1].m_iStrType=2;
				strSection[nSection].m_iStrType=1;
			}
			else if(nSection>0)//���120-4��120A���͵�
			{
				if((strcmp(strSection[1].sInfo,"-")!=0&&strcmp(strSection[1].sInfo,"_")!=0)
					||((strcmp(strSection[1].sInfo,"-")==0||strcmp(strSection[1].sInfo,"_")==0)
					&&strlen(strSection[2].sInfo)<3))
				{
					int n = nSection+2;
					for(i=n;i>2;i--)
						sprintf(strSection[i].sInfo,"%s",strSection[i-2].sInfo);
					int m = nSize-2;
					for(i=0;i<m+1;i++)
						strSection[2].sInfo[i] = strSection[0].sInfo[i+m];
					strSection[0].sInfo[m] = '\0';
					strSection[1].sInfo[0] = '-';
					strSection[1].sInfo[1] = '\0';
					strSection[1].m_iStrType=2;
					strSection[2].sInfo[2] = '\0';
					strSection[2].m_iStrType=1;
					nSection+=2;
				}
			}
		}
	}
	if(pSectionNum)
		*pSectionNum=nSection+1;
}

//�Էֺöε��ַ������бȽ� -1��ʾstrSection1<strSection2,0��ʾ���, 1��ʾstrSection1>strSection2
static int CompareStrBySection(KEY_SECTION strSection1[],KEY_SECTION strSection2[],int nSectionNum1,int nSectionNum2)
{
	int nSectionNumMin=0;
	if(nSectionNum1<nSectionNum2)
		nSectionNumMin=nSectionNum1;
	else
		nSectionNumMin=nSectionNum2;
	for(int i=0;i<nSectionNumMin;i++)
	{
		int nOffsetType=strSection1[i].m_iStrType-strSection2[i].m_iStrType;
		//���ַ������Ͳ�ͬ ֻ���������strSection1[i].m_iStrType==1&&strSection1[i].m_iStrType==2 ����
		//strSection1[i].m_iStrType==2&&strSection1[i].m_iStrType==1 ��nOffsetType������Ϊ����ֵ
		//if(strSection1[i].m_iStrType==1) return -1; else return 1;
		if(nOffsetType!=0)	 
			return nOffsetType;
		//���ַ���������ͬ
		else if(strSection1[i].m_iStrType==3)
		{	//���ַ�����Ϊ16�����ַ��� wht 11-07-25
			long handle1=0,handle2=0;
			sscanf(strSection1[i].sInfo,"%X",&handle1);
			sscanf(strSection2[i].sInfo,"%X",&handle2);
			if(handle1==handle2)
				continue;
			else if(handle1>handle2)
				return 1;
			else //if(handle1<handle2)
				return -1;
		}
		else if(strSection1[i].m_iStrType==1)
		{	//���ַ��ξ�Ϊ���� ת��Ϊ���� ��ֵС������ֵ���ǰ��
			double fResult=atof(strSection1[i].sInfo)-atof(strSection2[i].sInfo);
			if(fabs(fResult)<LOCAL_EPS)
				continue;	//��ֵ���������Ƚ���һ���ַ���
			else if(fResult>0)
				return 1;
			else 
				return -1;
		}
		else if(strSection1[i].m_iStrType==2)
		{	//���ַ������ݾ�Ϊ�������ַ�
			int nRetCode=stricmp(strSection1[i].sInfo,strSection2[i].sInfo);
			if(nRetCode==0)
				continue;	//���ַ������������Ƚ���һ���ַ���
			else
				return nRetCode;
		}
	}

	if(nSectionNum1==nSectionNum2)
		return 0;
	else if(nSectionNum1<nSectionNum2)
		return -1;
	else 
		return 1;

}

int CompareMultiSectionString(const char *str1,const char *str2)
{
	//��������ַ����Ľṹ����
	KEY_SECTION firstStr[16];
	KEY_SECTION secondStr[16];
	//ѭ���������ַ�����
	int nSectionNum1=1,nSectionNum2=1;
	//�������������ַ������зֶ�
	StrSubsection(str1,firstStr,&nSectionNum1,FALSE);
	StrSubsection(str2,secondStr,&nSectionNum2,FALSE);
	//�Էֺöε��ַ�����������
	return CompareStrBySection(firstStr,secondStr,nSectionNum1,nSectionNum2);
}
int CompareSegI(const SEGI &seg1,const SEGI &seg2)
{	//�ȱȶκ�
	CXhChar16 sPrefix1=seg1.Prefix(),sPrefix2=seg2.Prefix();
	if( (sPrefix1.EqualNoCase("XK")||sPrefix1.EqualNoCase("XZ")||sPrefix1.EqualNoCase("XL"))&&
		(sPrefix2.EqualNoCase("XK")||sPrefix2.EqualNoCase("XZ")||sPrefix2.EqualNoCase("XL")))
	{	//ʮ�ֲ������ʱ����֤һ��ʮ�ֲ�����ڼ�(XK��XZ��XL)
		if(seg1.Digital()>seg2.Digital())
			return 1;
		else if(seg1.Digital()<seg2.Digital())
			return -1;
		else
		{	//������
			int iSerial1=0,iSerial2=0;
			if(sPrefix1.EqualNoCase("XZ"))
				iSerial1=1;
			else if(sPrefix1.EqualNoCase("XL"))
				iSerial1=2;
			if(sPrefix2.EqualNoCase("XZ"))
				iSerial2=1;
			else if(sPrefix2.EqualNoCase("XL"))
				iSerial2=2;
			int nRetCode=0;
			if(iSerial1>iSerial2)
				nRetCode=1;
			else if(iSerial1<iSerial2)
				nRetCode=-1;
			else
				nRetCode=0;
			return nRetCode;
		}
	}
	else
	{
		if(seg1.Prefix().GetLength()==2||seg2.Prefix().GetLength()==2)
		{
			if(seg1.Prefix().GetLength()>seg2.Prefix().GetLength())
				return 1;
			else if(seg1.Prefix().GetLength()<seg2.Prefix().GetLength())
				return -1;
			int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
			if(comp_seg!=0)
				return comp_seg;
		}
		if(seg1.Digital()>seg2.Digital())
			return 1;
		else if(seg1.Digital()<seg2.Digital())
			return -1;
		int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
		return comp_seg;
	}
}
//�Ƚ����������ŵĴ�С
int ComparePartNoString(const char *str1,const char *str2,char* materialPrefixCharSet/*=NULL*/)
{	
	//��������������Ϊ�κź���ˮ��
	SEGI seg1,seg2;
	char serial_str1[16],serial_str2[16];
	char postfix1[8]={0},postfix2[8]={0};
	memset(serial_str1,0,16);
	memset(serial_str2,0,16);
	if(!ParsePartNo(str1,&seg1,serial_str1,materialPrefixCharSet)||!ParsePartNo(str2,&seg2,serial_str2,materialPrefixCharSet))
	{
		if(strlen(str1)==strlen(str2))
			return stricmp(str1,str2);
		else
			return strlen(str1)-strlen(str2);
	}
	size_t len1=strlen(serial_str1);
	size_t len2=strlen(serial_str2);
	int index=7;
	while(len1>0&&isdigit((BYTE)serial_str1[len1-1])==0)
	{	//������ˮ�ź����ж���ַ������
		if(index>=0)
			postfix1[index]=serial_str1[len1-1];
		//else	//���,�����û�����ڳ����տ�ԭ��ֱ�Ӱѱ�ź���������������ַ�(��ո�) wjh-2017.6.19
		serial_str1[len1-1]=0;
		len1--;
		index--;
	}
	index=max(index,0);
	memmove(postfix1,&postfix1[index+1],7-index);//ȥ��ǰ��Ŀ��ַ�
	postfix1[7-index]=0;
	index=7;
	while(len2>0&&isdigit((BYTE)serial_str2[len2-1])==0)
	{	//������ˮ�ź����ж���ַ������
		if(index>=0)
			postfix2[index]=serial_str2[len2-1];
		//else	//���,�����û�����ڳ����տ�ԭ��ֱ�Ӱѱ�ź���������������ַ�(��ո�) wjh-2017.6.19
		serial_str2[len2-1]=0;
		len2--;
		index--;
	}
	index=max(index,0);
	memmove(postfix2,&postfix2[index+1],7-index);//ȥ��ǰ��Ŀ��ַ�
	postfix2[7-index]=0;
	//�ȱȶκ�
	CXhChar16 sPrefix1=seg1.Prefix(),sPrefix2=seg2.Prefix();
	if( (sPrefix1.EqualNoCase("XK")||sPrefix1.EqualNoCase("XZ")||sPrefix1.EqualNoCase("XL"))&&
		(sPrefix2.EqualNoCase("XK")||sPrefix2.EqualNoCase("XZ")||sPrefix2.EqualNoCase("XL")))
	{	//ʮ�ֲ������ʱ����֤һ��ʮ�ֲ�����ڼ�(XK��XZ��XL)
		if(seg1.Digital()>seg2.Digital())
			return 1;
		else if(seg1.Digital()<seg2.Digital())
			return -1;
		else
		{
			int nRetCode=stricmp(serial_str1,serial_str2);
			if(nRetCode==0)
			{	//������
				int iSerial1=0,iSerial2=0;
				if(sPrefix1.EqualNoCase("XZ"))
					iSerial1=1;
				else if(sPrefix1.EqualNoCase("XL"))
					iSerial1=2;
				if(sPrefix2.EqualNoCase("XZ"))
					iSerial2=1;
				else if(sPrefix2.EqualNoCase("XL"))
					iSerial2=2;
				if(iSerial1>iSerial2)
					nRetCode=1;
				else if(iSerial1<iSerial2)
					nRetCode=-1;
				else
					nRetCode=0;
			}
			return nRetCode;
		}
	}
	else
	{
		if(seg1.Prefix().GetLength()==2||seg2.Prefix().GetLength()==2)
		{
			if(seg1.Prefix().GetLength()>seg2.Prefix().GetLength())
				return 1;
			else if(seg1.Prefix().GetLength()<seg2.Prefix().GetLength())
				return -1;
			int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
			if(comp_seg!=0)
				return comp_seg;
		}
		if(seg1.Digital()>seg2.Digital())
			return 1;
		else if(seg1.Digital()<seg2.Digital())
			return -1;
		int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
		if(comp_seg!=0)
			return comp_seg;
		//�κ���ͬ����ˮ��
		int comp_serial=0;
		if(strlen(serial_str1)==strlen(serial_str2))
			comp_serial=stricmp(serial_str1,serial_str2);
		else
			comp_serial=atoi(serial_str1)-atoi(serial_str2);
		if(postfix1[0]==0&&postfix2[0]==0)
			return comp_serial;
		else if(comp_serial==0)	//��ˮ����ͬ�Ⱥ�׺
			return stricmp(postfix1,postfix2);
		else
			return comp_serial;
	}
}

//�Ƚ������������Ĵ�С
int ComparePartSpecString(char *str1,char *str2,BOOL bPartNo)
{
	//��������ַ����Ľṹ����
	KEY_SECTION firstStr[16];
	KEY_SECTION secondStr[16];
	//ѭ���������ַ�����
	int nSectionNum1=1,nSectionNum2=1;
	//�������������ַ������зֶ�
	StrSubsection(str1,firstStr,&nSectionNum1,bPartNo);
	StrSubsection(str2,secondStr,&nSectionNum2,bPartNo);
	//�Էֺöε��ַ�����������
	return CompareStrBySection(firstStr,secondStr,nSectionNum1,nSectionNum2);
}
//�Ƚ�������˨���Ĵ�С
int CompareBoltSizeString(const char *str1,const char *str2)
{
	char temstr[51]="";
	double grade_arr[2]={0};
	int d_arr[2]={0};
	int l_arr[2]={0};
	for(int i=0;i<2;i++)
	{
		if(i==0)
			strncpy(temstr,str1,50);
		else
			strncpy(temstr,str2,50);
		char* splitter=strchr(temstr,'M');
		size_t pos=splitter-temstr;
		*splitter='\0';
		splitter+=1;
		if(pos>0)	//����������Ϣ
			grade_arr[i]=atoi(temstr);
		char* key=strtok(splitter,"XM");
		d_arr[i]=atoi(key);
		key=strtok(NULL,"XM");
		l_arr[i]=atoi(key);
	}
	if(grade_arr[0]<grade_arr[1])
		return -1;
	else if(grade_arr[0]>grade_arr[1])
		return 1;
	else if(d_arr[0]<d_arr[1])
		return -1;
	else if(d_arr[0]>d_arr[1])
		return 1;
	else if(l_arr[0]<l_arr[1])
		return -1;
	else if(l_arr[0]>l_arr[1])
		return 1;
	else
		return strlen(str1)-strlen(str2);
}
