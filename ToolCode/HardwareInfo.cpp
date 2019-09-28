#include "StdAfx.h"
#include "HardwareInfo.h"
#include "hardcomm.h"
#include "iphlpapi.h" 
#include "MD5.H"
#include "Buffer.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")

//1.��ȡCPU ID
CXhChar200 GetCpuID()
{
	BOOL bException = FALSE;
	BYTE szCpu[16]  = { 0 };
	UINT uCpuIDH     = 0U;
	UINT uCpuIDL     = 0U;
	__try 
	{
		_asm 
		{
			mov eax, 0
				cpuid
				mov dword ptr szCpu[0], ebx
				mov dword ptr szCpu[4], edx
				mov dword ptr szCpu[8], ecx
				mov eax, 1
				cpuid
				mov uCpuIDH, edx
				mov uCpuIDL, eax
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bException = TRUE;
	}
	CXhChar200 sSystemInfo;
	if( !bException )
		sSystemInfo.Printf("%08X%08X", uCpuIDH, uCpuIDL);
	return sSystemInfo;
}

//2.��ȡ����ID
CXhChar200 GetHardDiskID()
{
	CXhChar200 sRetRext;
	IDINFO IdInfo;
	int    i = 0; 
	//��ʱֻȡ��һ��Ӳ�����к�
	//for(i=0;i<4;i++)
	{
		if(GetPhysicalDriveInfoInNT(i,&IdInfo))
		{
			memcpy(sRetRext, IdInfo.sSerialNumber, 20);
		} 
		else if(GetIdeDriveAsScsiInfoInNT(i,&IdInfo))
		{
			memcpy(sRetRext, IdInfo.sSerialNumber, 20);
		}                             
	}
	return sRetRext;
}
//3.��ȡ��������
CXhChar200 GetComputerName()
{
	char host_name[200]="";
	//��ȡ������������
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		return "";
	return CXhChar200(host_name);
}
//4.��ȡIP��ַ
CXhChar200 GetIP()
{
	char host_name[255]="";
	//��ȡ������������ 
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		return "";
	//�����������ݿ��еõ���Ӧ�ġ�������
	struct hostent *pHostent = gethostbyname(host_name);
	if (pHostent == 0)
		return "";
	CXhChar200 sIP;
	//ѭ���ó����ػ�������IP��ַ
	//BYTE computer_ip[4];
	for (int i = 0; pHostent->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, pHostent->h_addr_list[i], sizeof(struct in_addr));
		sIP.Printf("%s",inet_ntoa(addr));
		//computer_ip[0]=addr.S_un.S_un_b.s_b1;
		//computer_ip[1]=addr.S_un.S_un_b.s_b2;
		//computer_ip[2]=addr.S_un.S_un_b.s_b3;
		//computer_ip[3]=addr.S_un.S_un_b.s_b4;
	}
	return sIP;
}
//5.��ȡMAC��ַ
CXhChar200 GetMacAddress()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD AdapterInfoSize;
	DWORD Err;
	AdapterInfoSize=0;
	Err=GetAdaptersInfo(NULL,&AdapterInfoSize);
	if((Err!=0)&&(Err!=ERROR_BUFFER_OVERFLOW)){
		TRACE("���������Ϣʧ�ܣ�");
		return "";
	}
	//   ����������Ϣ�ڴ�
	pAdapterInfo=(PIP_ADAPTER_INFO)GlobalAlloc(GPTR,AdapterInfoSize);
	if(pAdapterInfo==NULL){
		TRACE("����������Ϣ�ڴ�ʧ��");
		return "";
	}
	if(GetAdaptersInfo(pAdapterInfo,&AdapterInfoSize)!=0){
		TRACE(_T("���������Ϣʧ�ܣ�\n"));
		GlobalFree(pAdapterInfo);
		return   "";
	}     
	CXhChar200 sMac(_T("%02X%02X%02X%02X%02X%02X"),
		pAdapterInfo->Address[0],
		pAdapterInfo->Address[1],
		pAdapterInfo->Address[2],
		pAdapterInfo->Address[3],
		pAdapterInfo->Address[4],
		pAdapterInfo->Address[5]);
	//for(int i=0;i<6;i++)
	//	computer_mac[i]=pAdapterInfo->Address[i];
	GlobalFree(pAdapterInfo);
	return sMac;
}
//6.��ȡBIOS���
CXhChar200 GetBOISSerialNO()
{
	SIZE_T ssize; 

	LARGE_INTEGER so; 
	so.LowPart=0x000f0000;
	so.HighPart=0x00000000; 
	ssize=0xffff; 
	wchar_t strPH[30]=L"\\device\\physicalmemory"; 

	DWORD ba=0;

	UNICODE_STRING struniph; 
	struniph.Buffer=strPH; 
	struniph.Length=0x2c; 
	struniph.MaximumLength =0x2e; 

	OBJECT_ATTRIBUTES obj_ar; 
	obj_ar.Attributes =64;
	obj_ar.Length =24;
	obj_ar.ObjectName=&struniph;
	obj_ar.RootDirectory=0; 
	obj_ar.SecurityDescriptor=0; 
	obj_ar.SecurityQualityOfService =0; 

	HMODULE hinstLib = LoadLibrary("ntdll.dll"); 
	ZWOS ZWopenS=(ZWOS)GetProcAddress(hinstLib,"ZwOpenSection"); 
	ZWMV ZWmapV=(ZWMV)GetProcAddress(hinstLib,"ZwMapViewOfSection"); 
	ZWUMV ZWunmapV=(ZWUMV)GetProcAddress(hinstLib,"ZwUnmapViewOfSection"); 

	CXhChar200 sBios;
	//���ú������������ڴ����ӳ�� 
	HANDLE hSection; 
	if( 0 == ZWopenS(&hSection,4,&obj_ar) && 
		0 == ZWmapV( 
		( HANDLE )hSection,   //��Sectionʱ�õ��ľ�� 
		( HANDLE )0xFFFFFFFF, //��Ҫӳ����̵ľ���� 
		&ba,                  //ӳ��Ļ�ַ 
		0,
		0xFFFF,               //����Ĵ�С 
		&so,                  //�����ڴ�ĵ�ַ 
		&ssize,               //ָ���ȡ�ڴ���С��ָ�� 
		1,                    //�ӽ��̵Ŀɼ̳����趨 
		0,                    //�������� 
		2                     //�������� 
		) )
		//ִ�к���ڵ�ǰ���̵Ŀռ俪��һ��64k�Ŀռ䣬����f000:0000��f000:ffff��������ӳ�䵽���� 
		//ӳ��Ļ�ַ��ba����,���ӳ�䲻������,Ӧ����ZwUnmapViewOfSection�Ͽ�ӳ�� 
	{
		BYTE* pBiosSerial = ( BYTE* )ba;
		UINT uBiosSerialLen = FindAwardBios( &pBiosSerial );
		if( uBiosSerialLen == 0U )
		{
			uBiosSerialLen = FindAmiBios( &pBiosSerial );
			if( uBiosSerialLen == 0U )
			{
				uBiosSerialLen = FindPhoenixBios( &pBiosSerial );
			}
		}
		if( uBiosSerialLen != 0U )
		{
			sBios.Printf("%s",pBiosSerial);
			//CopyMemory( szSystemInfo + uSystemInfoLen, pBiosSerial, uBiosSerialLen );
			//uSystemInfoLen += uBiosSerialLen;
		}
		ZWunmapV( ( HANDLE )0xFFFFFFFF, ( void* )ba );
	}
	return sBios;
}

CXhChar200 GetComputerFingerprintStr()
{
	CXhChar200 idStr("%s\t%s\t%s",(char*)GetBOISSerialNO(),(char*)GetCpuID(),(char*)GetHardDiskID());
	return idStr;
}

DWORD Generate128SecretKey(BYTE key[16])
{
	CBuffer buffer;
	buffer.WriteString(GetCpuID());
	buffer.WriteString(GetHardDiskID());
	buffer.WriteString(GetMacAddress());

	MD5_CTX md5;
	DWORD nDataSize=buffer.GetLength();
	md5.MD5Update((BYTE*)buffer.GetBufferPtr(),nDataSize);
	md5.MD5Final(key);
	return nDataSize;
}

#include "LogFile.h"
void TestHardware()
{
	CLogErrorLife logErrorLife;
	logerr.Log("PC Name: %s\n",GetComputerName());
	logerr.Log("MAC Address: %s\n",GetMacAddress());
	logerr.Log("IP: %s\n",GetIP());
	logerr.Log("CPU ID: %s\n",(char*)GetCpuID());
	logerr.Log("DISK ID: %s\n",(char*)GetHardDiskID());

	BYTE key[16]={0};
	Generate128SecretKey(key);
	CXhChar200 sKey;
	for(int i=0;i<16;i++)
	{
		CXhChar16 ss("%X",key[i]);
		sKey.Append(ss);
	}
	logerr.Log("%s",(char*)sKey);
}