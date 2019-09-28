#pragma once
#include "ArrayList.h"
#include "f_ent.h"

#define MAXN     300          //������
#define INIT_T   2000         //��ʼ�¶�
#define RATE     0.95         //�¶�˥����
#define FINAL_T  1E-10        //��ֹ�¶�
#define IN_LOOP  13000        //�ڲ�ѭ������
#define OUT_LOOP  2000        //���ѭ������
#define P_LIMIT  10000        //����ѡ�����
//
struct PATH{
	int indexArr[MAXN];		//���α��������
	double fSumDist;		//·���ܳ���
};
class CTSPAlgorithm
{
private:
	int nPtNum;                     //�ڵ�����
	double m_fDist;					//�û���¼��ʱ�������
	double distArr[MAXN][MAXN];		//��������֮��ľ���
	PATH xBestPath;                 //���ŵı���·��
	int bestIndexArr[MAXN];			//���ŵ��������
	BOOL m_bLoop;					//·���Ƿ񹹳ɻ�
	BYTE m_ciAlgType;				//�㷨����
protected:
	double TotalDist(PATH path);
	PATH GetNext(PATH cur_path);
	void RearRange(int iStart);
	void Annealing();			//�˻��㷨
	void Backtrack(int i);		//�����㷨
	void Greedy(int iStart);	//̰���㷨 
	void Swap(int &a ,int &b);
public:
	CTSPAlgorithm();
	~CTSPAlgorithm();
	void InitData(ARRAY_LIST<f3dPoint>& ptArr,BYTE ciAlgType=0);
	void CalBestPath(int iStart,int* arr);
};