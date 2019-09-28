#include "StdAfx.h"
#include "TSPAlgorithm.h"
#include <cmath>
#include <time.h>

 
CTSPAlgorithm::CTSPAlgorithm()
{
	m_ciAlgType = 0;
	nPtNum=0;
	m_fDist=0;
	m_bLoop=TRUE;
}
CTSPAlgorithm::~CTSPAlgorithm()
{

}
//�������·�����ܳ���
double CTSPAlgorithm::TotalDist(PATH path)
{
	double fSumDist=0;
	for(int i=1;i<nPtNum;i++)
		fSumDist+=distArr[path.indexArr[i]][path.indexArr[i+1]];
	if(m_bLoop)
		fSumDist+=distArr[path.indexArr[nPtNum]][path.indexArr[1]];
	return fSumDist;
}
//��ȡ���ݽ��г�ʼ��
void CTSPAlgorithm::InitData(ARRAY_LIST<f3dPoint>& ptArr,BYTE ciAlgType /*= 0*/)
{
	nPtNum=ptArr.GetSize()-1;
	if (ciAlgType == 1 || (ciAlgType == 0 && nPtNum <= 5))
		m_ciAlgType = 1;	//̰���㷨����·���Ż�
	if (ciAlgType == 2 || (ciAlgType == 0 && nPtNum > 5 && nPtNum < 15))
		m_ciAlgType = 2;	//�����㷨����·���Ż�
	if (ciAlgType == 3 || (ciAlgType == 0 && nPtNum >= 15))
		m_ciAlgType = 3;	//ģ���˻��㷨����·���Ż�(���ǻ���)
	if (m_ciAlgType == 2 && nPtNum >= 20)
		m_ciAlgType = 3;	//���ݲ��ԣ������㷨ʱ��������20�ٶȻ����
	m_bLoop = (m_ciAlgType == 3) ? TRUE : FALSE;
	//��¼��������֮��ľ���(�����ʼ��)
	for(int i=1;i<=nPtNum;i++)
	{
		for(int j=i;j<=nPtNum;j++)
		{
			if(i==j)
				distArr[i][j]= 0;		
			else
			{
				distArr[i][j]=DISTANCE(ptArr[i],ptArr[j]);
				distArr[j][i]=distArr[i][j];
			}
		}
	}
	//���㵱ǰ·�����ܳ�
	for(int i=1;i<=nPtNum;i++)
		xBestPath.indexArr[i]=i;
	xBestPath.fSumDist=TotalDist(xBestPath);
}
void CTSPAlgorithm::Swap(int &a ,int &b)
{
	int temp ;
	temp = a ;
	a = b ;
	b = temp ;
}
//�½��������
PATH CTSPAlgorithm::GetNext(PATH cur_path)
{
    UINT x, y;
    PATH ret;
    ret=cur_path;
    do {
        x = rand()%nPtNum+1;
        y = rand()%nPtNum+1;
    }while(x == y);
	Swap(ret.indexArr[x],ret.indexArr[y]);
	ret.fSumDist=TotalDist(ret);
    return ret;
}
//�˻�ͽ��¹���  
void CTSPAlgorithm::Annealing()
{
    double T=INIT_T;		//�¶�
	PATH cur_path=xBestPath;
    UINT P_t=0, A_t=0; 
    while(true)
    {
        for (int i=1;i<IN_LOOP;i++)
        {
            PATH new_path=GetNext(cur_path);                    //��ȡ��·��
            double delta= new_path.fSumDist-cur_path.fSumDist;
            if (delta<0.0)
            {
                cur_path = new_path;
                P_t = 0;
                A_t = 0;
            }
            else
            {
                double rnd = rand()%10000 /10000.0;
                double p = exp(-delta/T);
                if (p > rnd)
                    cur_path = new_path;
                P_t++;
            }
            if (P_t >=P_LIMIT)
            {
                A_t++;
                break;
            }
        }
        if(cur_path.fSumDist<xBestPath.fSumDist)
			 xBestPath=cur_path;		 
        if(A_t>=OUT_LOOP || T<FINAL_T) 
			break;
        T=T*RATE;   //����                                       
    }	
}
//�����㷨
void CTSPAlgorithm::Backtrack(int i)
{
	double fDist1=0,fDist2=0;
	if(i==nPtNum)
	{	//�������ײ�������������Ž����¼
		fDist1=distArr[xBestPath.indexArr[nPtNum-1]][xBestPath.indexArr[nPtNum]];
		if(m_bLoop)
			fDist2=distArr[xBestPath.indexArr[nPtNum]][xBestPath.indexArr[1]];
		if(fDist1!=0 && m_fDist+fDist1+fDist2<=xBestPath.fSumDist)
		{
			for(int k=1;k<=nPtNum;k++)
				bestIndexArr[k]= xBestPath.indexArr[k];
			xBestPath.fSumDist=m_fDist+fDist1+fDist2;
		}
	}
	else
	{
		for(int j=i;j<nPtNum;j++)
		{
			fDist1=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[j]];
			if(fDist1!=0 && m_fDist+fDist1<xBestPath.fSumDist)
			{
				Swap(xBestPath.indexArr[i],xBestPath.indexArr[j]);
				m_fDist+=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[i]];
				Backtrack(i+1);
				m_fDist-=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[i]];
				Swap(xBestPath.indexArr[i],xBestPath.indexArr[j]);
			}
		}
	}
}
//̰���㷨(��֤ÿ������)
void CTSPAlgorithm::Greedy(int iStart)
{
	Swap(xBestPath.indexArr[1],xBestPath.indexArr[iStart]);
	for(int i=2;i<nPtNum;i++)
	{
		double fMinDist=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[i]];
		for(int j=i+1;j<=nPtNum;j++)
		{
			double fDist=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[j]];
			if(fMinDist>fDist)
			{
				fMinDist=fDist;
				Swap(xBestPath.indexArr[i],xBestPath.indexArr[j]);
			}
		}
	}
	for(int i=1;i<=nPtNum;i++)
		bestIndexArr[i]=xBestPath.indexArr[i];
}
//��������˳��
void CTSPAlgorithm::RearRange(int iStart)
{ 
    int index=0,k=1;
	for(index=1;index<=nPtNum;index++)
	{
		if(xBestPath.indexArr[index]==iStart)
			break;
	}
	for(int j=index;j<=nPtNum;j++)
		bestIndexArr[k++]=xBestPath.indexArr[j];
	for(int j=1;j<=index-1;j++)
		bestIndexArr[k++]=xBestPath.indexArr[j];
}
//��������·��
void CTSPAlgorithm::CalBestPath(int iStart,int* arr)
{ 
	if(m_ciAlgType==1)
	{	//̰���㷨����·���Ż�
		Greedy(iStart);
	}
	else if (m_ciAlgType == 2)
	{	//���ݷ�����·���Ż�
		Backtrack(2);
	}
	else //if(m_ciAlgType == 3)
	{	//ģ���˻��㷨����·���Ż�()
		Annealing();
		RearRange(iStart); 
	}
	for(int i=1;i<=nPtNum;i++)
		arr[i]=bestIndexArr[i];
}