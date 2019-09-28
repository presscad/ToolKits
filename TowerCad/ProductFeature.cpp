#include "StdAfx.h"
#include ".\ProductFeature.h"
///////////////////////////////////////////////////////////////////////////////////
//PRODUCT_FEATURE �ඨ��
//�������ڶ���*.fac�ļ��о��������Щ���л���������,����Ӧ�������ܵ���������(bool,byte,word,uint)
//�����ض��������ܵ�ʵ�ʱ��ػ���������ֵ��FEATURE_STATE����ʾ wjh-2017.11.20
///////////////////////////////////////////////////////////////////////////////////

//PRODUCT_FEATURE::PRODUCT_FEATURE(void){dwFeatureFlag=0;}
//PRODUCT_FEATURE::~PRODUCT_FEATURE(void){;}
WORD PRODUCT_FEATURE::getFeatureSerial() const
{	//��1~11 bits (1 based index)
	WORD wSerial=(WORD)(dwFeatureFlag&0x000007ff);
	return wSerial;
}
void PRODUCT_FEATURE::setFeatureSerial(WORD serialFeature)
{	//��1~11 bits (1 based index)
	dwFeatureFlag&=0xfffff800;
	dwFeatureFlag|=serialFeature;
}

BYTE PRODUCT_FEATURE::getDataType() const
{	//��12~13 bits (1 based index)
	return (BYTE)((dwFeatureFlag&0x00001800)>>11);
}
void PRODUCT_FEATURE::setDataType(BYTE dataType)
{
	DWORD datatype=dataType&0x03;
	datatype<<=11;
	dwFeatureFlag&=0xffffe7ff;
	dwFeatureFlag|=datatype;
}

BYTE PRODUCT_FEATURE::getBitPos() const
{	//��14~16 bits (1 based index)
	//BYTE bitpos=
	return (BYTE)((dwFeatureFlag&0x0000e000)>>13);
}
void PRODUCT_FEATURE::setBitPos(BYTE bitPos)
{	//��14~16 bits (1 based index)
	DWORD bitpos=bitPos;
	bitpos<<=13;
	dwFeatureFlag&=0xffff1fff;
	dwFeatureFlag|=bitpos;
}

WORD PRODUCT_FEATURE::getBytePos() const
{	//��17~32 bits (1 based index)
	WORD bytePos=(WORD)((dwFeatureFlag&0xffff0000)>>16);
	return bytePos;
}
void PRODUCT_FEATURE::setBytePos(WORD bytePos)
{	//��17~32 bits (1 based index)
	DWORD bytepos=bytePos;
	bytepos<<=16;
	dwFeatureFlag&=0x0000ffff;
	dwFeatureFlag|=bytepos;
}
FEATURE_STATE::FEATURE_STATE(BYTE ciDataType,DWORD stateValue)
{
	_dwState=ciDataType;
	_dwState<<=30;
	_dwState|=(stateValue&0x3fffffff);
}
BYTE FEATURE_STATE::getDataType() const
{	//��31~32 bits (1 based index)
	return (BYTE)((_dwState&0xc0000000)>>30);
}
void FEATURE_STATE::setDataType(BYTE dataType)
{
	DWORD datatype=dataType&0x03;
	datatype<<=30;
	_dwState&=0x3fffffff;
	_dwState|=datatype;
}
DWORD FEATURE_STATE::getDwordState() const
{
	return _dwState&0x3fffffff;
}
void FEATURE_STATE::setDwordState(DWORD state)
{
	_dwState&=0xc0000000;
	_dwState|=state;
}
WORD FEATURE_STATE::getWordState() const
{
	return (WORD)(_dwState&0x0000ffff);
}
void FEATURE_STATE::setWordState(WORD state)
{
	_dwState&=0xc0000000;
	_dwState|=state;
}
BYTE FEATURE_STATE::getByteState() const
{
	return (BYTE)(_dwState&0x000000ff);
}
void FEATURE_STATE::setByteState(BYTE state)
{
	_dwState&=0xc0000000;
	_dwState|=state;
}
bool FEATURE_STATE::getBoolState() const
{
	return (_dwState&0x3fffffff)>0;
}
void FEATURE_STATE::setBoolState(bool state)
{
	state ? _dwState&=0xc0000001 : _dwState&=0xc0000000;
}
