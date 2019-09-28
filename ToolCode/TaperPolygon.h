#pragma once

#include "f_ent.h"
//��������Ի�׼ƽ��ԳƵĶ�����γɵ�һ�������壨һ�����������治ƽ�У���ĳ��������խ��
struct TAPER_POLYGON {
	GECS cs;			//��׼����ϵ��X-Yƽ��Ϊ��׼ƽ�棬ԭ��Ϊ��λʰȡԭ�㣨�����ڶ����巶Χ�ڣ�
	double dfMinZ,dfMaxZ;		//���������ԭ��ͻ����Z����С�����Χֵ
	long _nCount;	//������
	GEPOINT* pxArrPrjVertices;
public:
	long get_nCount() { return _nCount; }
	__declspec(property(get = get_nCount)) long nCount;
	double MaxZ()const { return dfMaxZ; }
	double MinZ()const { return dfMinZ; }
	TAPER_POLYGON();
	~TAPER_POLYGON();
	bool InitPolyVertexes(const double* pxOrg,const double* pxPlaneNormal,GEPOINT* xarrVertices, int count);
	void ClearScope();
	bool IsIncludePoint(double px, double py, double pz, double tolerance = EPS);
	bool IsIncludePoint(const double* ptcoords, double tolerance = EPS);
	bool IsIncludeLine(const double* pLineStart, const double* pLineEnd, double tolerance = EPS);
};
