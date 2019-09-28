#pragma once
struct DEG
{
protected:
	double m_dfDegAngle;
public:
	DEG(double dfDegAngle=0);
	DEG(const double* vec2d);
	operator double(){return m_dfDegAngle;}
	double Unify();	//ͳһ��0<=m_dfDegAngle<360��
	bool IsColinearDegAngle(double degAng,double tolerance=1);
	bool IsInSector(double dfMinDegAngle,double dfMaxDegAngle,bool bIncPeerSector=true);
	double FromVector(const double* vec2d);
};

