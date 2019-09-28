#pragma once
#include <stdlib.h>
#include "XhCharString.h"
//���϶κ��࣬�κ������������ǰ׺�ַ����κ����ֲ����������Ϊ65536
union SEGI
{
	long iSeg;
	struct{
		WORD number;
		char prefix[2];
	}key;
	SEGI(long seg_i=0){iSeg=seg_i;}
	SEGI(const char* segStr)
	{
		iSeg=0;
		if(segStr==NULL||strlen(segStr)==0)
			return;
		int prefix_n=0,suffix_n=0,serial_n=0;
		//�κ���ų���5λʱ������(���ڼ������Ϊ16λ,serial_str������5��Ϊ16) wht 17-07-11
		char prefix[2]="",suffix[2]="",serial_str[16]="";
		for(int i=0;i<(int)strlen(segStr);i++)
		{
			if(segStr[i]<0)
				continue;
			if(isdigit(segStr[i]))
			{	//���ֲ���
				if(serial_n<16)
				{
					serial_str[serial_n]=segStr[i];
					serial_n++;
				}
				else
					return;
			}
			else if(serial_n==0)
			{	//ǰ׺
				if(prefix_n<2)
					prefix[prefix_n++]=segStr[i];
				else //�������2��ǰ׺�ַ�
					return;
			}
			else if(serial_n>0)
			{	//��׺
				if(suffix_n<2)
					suffix[suffix_n++]=segStr[i];
				else //�������2����׺�ַ�
					return;
			}
		}
		//numberǰ��λֵ��ʾǰ��׺���ͣ�00����ʾprefix[0,1]��Ϊǰ׺��01:��ʾprefix[0]Ϊǰ׺��prefix[1]Ϊ��׺��11��ʾprefix[0,1]Ϊ��׺
		WORD serial=0;
		if(serial_n>0)
			serial=(WORD)atoi(serial_str);
		if(prefix_n==0&&suffix_n==0)		//��ǰ��׺
			key.number=serial;
		else if(prefix_n==1&&suffix_n==1)	//һ��ǰ׺һ����׺
		{
			if(prefix_n==1)
				key.prefix[0]=prefix[0];
			if(suffix_n==1)
				key.prefix[1]=suffix[0];
			key.number=0X3FFF&serial;
			key.number|=0x4000;
		}
		else if(prefix_n==0&&(suffix_n==2||suffix_n==1))	//������׺��һ����׺
		{
			key.prefix[0]=suffix[0];
			key.prefix[1]=suffix[1];
			key.number=0X3FFF&serial;
			key.number|=0xC000;
		}
		else //if(suffix_n==0&&(prefix_n==2||prefix_n==1))//����ǰ׺��һ��ǰ׺
		{
			key.prefix[0]=prefix[0];
			key.prefix[1]=prefix[1];
			key.number=0X3FFF&serial;
		}
		/*
		int prefix_n=0;
		for(int i=0;i<2&&i<(int)strlen(segStr);i++)
		{
			if(segStr[i]<0||segStr[i]>255||isdigit(segStr[i])!=0)	//�ַ�
				continue;
			key.prefix[i]=segStr[i];
			if(i==1)
				key.prefix[0]=segStr[0];
			prefix_n=i+1;
		}
		WORD serial=(WORD)atoi(&segStr[prefix_n]);
		//numberǰ��λֵ��ʾǰ��׺���ͣ�00����ʾprefix[0,1]��Ϊǰ׺��01:��ʾprefix[0]Ϊǰ׺��prefix[1]Ϊ��׺��11��ʾprefix[0,1]Ϊ��׺
		key.number=0X3FFF&serial;*/
	}
	SEGI(char* prefix,WORD seg_i)
	{
		key.number=seg_i;
		key.prefix[0]=key.prefix[1]=0;
		if(prefix)
		{
			key.prefix[0]=prefix[0];
			if(strlen(prefix)>=2)
				key.prefix[1]=prefix[1];
		}
	}
	operator long()const{return iSeg;}
	CXhChar16 Prefix()const
	{
		char buf[3]="";
		BYTE flag=(key.number&0XC000)>>14;
		if(flag==0)		//key.prefix[0,1]Ϊǰ׺
			memcpy(buf,key.prefix,2);
		else if(flag==1)//key.prefix[0]Ϊǰ׺
			buf[0]=key.prefix[0];
		return CXhChar16(buf);
	}
	CXhChar16 Suffix()const
	{
		char buf[3]="";
		BYTE flag=(key.number&0XC000)>>14;
		if(flag==3)		//key.prefix[0,1]Ϊ��׺
			memcpy(buf,key.prefix,2);
		else if(flag==1)	//key.prefix[1]Ϊ��׺
			buf[0]=key.prefix[1];
		return CXhChar16(buf);
	}
	WORD Digital ()const{return key.number&0X3FFF;}
	CXhChar16 ToString()const
	{
		char prefix[3]="",suffix[3]="";
		BYTE flag=(key.number&0XC000)>>14;
		if(flag==0)		//key.prefix[0,1]Ϊǰ׺
			memcpy(prefix,key.prefix,2);
		else if(flag==3)//key.prefix[0,1]Ϊ��׺
			memcpy(suffix,key.prefix,2);
		else //if(flag==1)//key.prefix[0]Ϊǰ׺, key.prefix[1]Ϊ��׺
		{
			prefix[0]=key.prefix[0];
			suffix[0]=key.prefix[1];
		}
		if(key.number>=0)
			return CXhChar16("%s%d%s",prefix,Digital(),suffix);
		else
			return CXhChar16("%s%s",prefix,suffix);
	}
};

//���ݼ�����ȡ�κż��������к�
bool ParsePartNo(const char* sPartNo,SEGI* pSegI,char* sSerialNo,char* materialPrefix=NULL,char *sSeparator=NULL);