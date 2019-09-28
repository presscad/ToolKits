#ifndef __QUERY_H_
#define __QUERY_H_

#include "ProcessPart.h"

//�빹��ֱ���йصĹ����������ϵ��alfa1
double QueryIceReviseCoefAlfa1(double diameter);
//high_pos(m)�߶�λ��,������ȵĸ߶ȵ���ϵ��alfa2
double QueryIceReviseCoefAlfa2(double high_pos);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL restore_Ls_guige(char* guige, BOLT_INFO& ls);
BOOL restore_Ls_guige(char* guige, double* grade,long* d, long* L );
BOOL restore_Ls_guige(CString guige, BOLT_INFO& ls);
#endif
//BOOL getjgzj(JGZJ& jgzj,double wing_wide);
void AddSteelMatRecord(CComboBox *pCMB);
void AddJgRecord(CComboBox *pCMB);
void AddTubeRecord(CComboBox *pCMB,BOOL bOnlyAddTubeD=FALSE);
void AddBianRecord(CComboBox *pCMB);
void AddSlotRecord(CComboBox* pCMB);
void AddLsRecord(CComboBox* pCMB,long hLsFamily=0);
//int FindTubeIndex(double D,double thick);
//int FindBianIndex(double wide,double thick);
#endif