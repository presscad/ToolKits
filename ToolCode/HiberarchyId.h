#pragma once

#include "WinDef.h"
//��㼶�ṹ�Ĳ㼶Id, ���㼶��Idֵ����С�ڵ���0x3fffffff
class HIBERARCHY{
protected:
	BYTE holder[2];	//holder[0]����㼶ռλ������λ0x0f��ʾ-1��ռλ��ǰ�ļ�0xf0��ʾ-2��ռλ��holder[1]��ʾ����㼶ռλ�������븺��㼶ռλ��ͬ
	BYTE hiberarchy[6];	//���㼶��ʵ��id�洢λ,�ɵ͵������δ洢��-1��ռλ=>-2��ռλ=>+1��ռλ=>+2��ռλ
public:
	HIBERARCHY(DWORD up1,DWORD up2,DWORD down1,DWORD down2){SetHiberarchy(up1,up2,down1,down2);}
	HIBERARCHY(_int64 hiberid=0);
	_int64 Hiber64() const;
	DWORD HiberUpId(int hiberarchy1or2) const;	//hiber��ʾ����Ĳ㼶��ֻ����1��2
	DWORD HiberDownId(int hiberarchy1or2) const;	//hiber��ʾ����Ĳ㼶��ֻ����1��2
	DWORD ModifyUpId(int hiberarchy1or2,DWORD hiber);	//hiber��ʾ����Ĳ㼶��ֻ����1��2
	DWORD ModifyDownId(int hiberarchy1or2,DWORD hiber);	//hiber��ʾ����Ĳ㼶��ֻ����1��2
	bool SetHiberarchy(DWORD up1=0,DWORD up2=0,DWORD down1=0,DWORD down2=0);
	bool IsEqual(const HIBERARCHY& hiberarchy) const{return Hiber64()==hiberarchy.Hiber64();}
};
struct HIBERID : public HIBERARCHY{
	DWORD masterId;	//���ݶ������������(����)��ʶId
	HIBERID(long master_id=0){masterId=master_id;}
	HIBERID(long master_id, HIBERARCHY hiberachy);
	bool IsEqual(const HIBERID& hiberid);
};
