#pragma once
#include <stdlib.h>
#include "XhCharString.h"

struct PARTSIZE{
	char ciSizeType;	//'L':�Ǹ�;'-':�ְ�;'T':�ֹ�;'M':��˨������
	union{
		char szMaterial[5];
		float sfBoltGrade;		//��˨����
	};
	union{
		float sfWidth;	//֫��
		float sfDia;	//�ֹ��⾶
		float sfMd;		//��˨ֱ��
	};
	union{
		float sfThick;	//֫��֫��
		float sfBoltL;	//��˨��Ч����
	};
	float sfWideOfShortLeg;	//����֫�ǸֵĽ�խ֫��
	PARTSIZE(const char* sizestr=NULL){
		ciSizeType=0;
		StrCopy(szMaterial,"Q235",5);
		sfWidth=sfThick=sfWideOfShortLeg=0;
		if(sizestr!=NULL)
			ParseSizeStr(sizestr);
	}
	bool ParseSizeStr(const char* sizestr)
	{
		if(sizestr==NULL)
			return false;
		CXhChar16 itemstr;
		CXhChar50 szSizeStr=sizestr;
		char *key=NULL,*pchType=NULL,*pszSizeStr=szSizeStr;
		if((pchType=strchr(szSizeStr,'L'))!=NULL||(pchType=strchr(szSizeStr,'-'))!=NULL)
		{	//�Ǹֻ�ְ�
			ciSizeType=*pchType;
			if(pchType>pszSizeStr)
			{
				itemstr.NCopy(pszSizeStr,pchType-pszSizeStr);
				StrCopy(szMaterial,itemstr,5);
			}
			pszSizeStr=pchType+1;
			if((key=strtok(pszSizeStr,"Xx*"))!=NULL)
				sfWidth=(float)atof(key);
			else
				return false;
			if((key=strtok(NULL,"Xx*"))!=NULL)
			{
				sfThick=(float)atof(key);
				if((key=strtok(NULL,"Xx*"))!=NULL)
					sfWideOfShortLeg=(float)atof(key);
			}
		}
		else if((pchType=strstr(szSizeStr,"��"))!=NULL||(pchType=strstr(szSizeStr,"��"))!=NULL)
		{	//�ֹ�
			ciSizeType='T';
			if(pchType>pszSizeStr)
			{
				itemstr.NCopy(pszSizeStr,pchType-pszSizeStr);
				StrCopy(szMaterial,itemstr,5);
			}
			pszSizeStr=pchType+1;
			if((key=strtok(pszSizeStr,"Xx*"))!=NULL)
				this->sfDia=(float)atof(key);
			else
				return false;
			if((key=strtok(NULL,"Xx*"))!=NULL)
				this->sfThick=(float)atof(key);
		}
		else if((pchType=strchr(szSizeStr,'M'))!=NULL)
		{	//��˨
			ciSizeType=*pchType;
			if(pchType>pszSizeStr)
			{
				itemstr.NCopy(pszSizeStr,pchType-pszSizeStr);
				sfBoltGrade=(float)atof(itemstr);
			}
			pszSizeStr=pchType+1;
			if((key=strtok(pszSizeStr,"Xx*"))!=NULL)
				this->sfMd=(float)atof(key);
			else
				return false;
			if((key=strtok(NULL,"Xx*"))!=NULL)
				this->sfBoltL=(float)atof(key);
		}
		else
			return false;
		return true;
	}
};
