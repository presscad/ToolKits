#pragma once
#include "XhCharString.h"

//1.��ȡCPU ID
CXhChar200 GetCpuID();
//2.��ȡ����ID
CXhChar200 GetHardDiskID();
//3.��ȡ��������
CXhChar200 GetComputerName();
//4.��ȡIP��ַ
CXhChar200 GetIP();
//5.��ȡMAC��ַ
CXhChar200 GetMacAddress();
//6.��ȡBIOS���
CXhChar200 GetBOISSerialNO();

//
CXhChar200 GetComputerFingerprintStr();
//
DWORD Generate128SecretKey(BYTE key[16]);

