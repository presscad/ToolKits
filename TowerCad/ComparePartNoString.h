#ifndef __COMPARE_PART_NO_H_
#define __COMPARE_PART_NO_H_
#include "SegI.h"

//�κ�����
int CompareSegI(const SEGI &seg1,const SEGI &seg2);
//�Թ����ַ�����������
int ComparePartNoString(const char *str1,const char *str2,char* materialPrefixCharSet=NULL);
//�ȽϷֶ����������ַ�������Ǹֹ��
int CompareMultiSectionString(const char *str1,const char *str2);
//�Ƚ�������˨���Ĵ�С
int CompareBoltSizeString(const char *str1,const char *str2);
#endif