#include "StdAfx.h"
#include "WlanConnect.h"

CWlanConnect::CWlanConnect(const char* SSIDFromWIFI,const char* wsDescription,const char* sIpAddress)
{
	m_hClient = NULL;
	m_pInterfaceList = NULL;
	m_pInterface=NULL;
	m_pNetworkList=NULL;
	m_pNetwork=NULL;
	m_dwWlanApiRetCode=0;
	m_dwRetCode=0;
	if(SSIDFromWIFI!=NULL&&wsDescription!=NULL&&sIpAddress!=NULL)
	{
		strcpy(IP_ADDRESS,sIpAddress);
		strcpy(SSID_WIFI,SSIDFromWIFI);
		strcpy(SIF_DESCRIPTION,wsDescription);

		DWORD dwCurVersion=0;
		DWORD dwApiVersion=2; 
		//1.��Wlan��ʼ��m_hClient,��ѯm_pInterfaceList
		m_dwWlanApiRetCode=WlanOpenHandle(dwApiVersion, NULL, &dwCurVersion, &m_hClient);
		if(ERROR_SUCCESS==m_dwWlanApiRetCode&&m_hClient)
			m_dwWlanApiRetCode=WlanEnumInterfaces(m_hClient, NULL, &m_pInterfaceList);
		else 
			m_dwRetCode=1;	//WalnOpenHandleʧ��
		//2.����SIF_DESCRIPTION,���Ҷ�Ӧm_pInterface
		if(ERROR_SUCCESS==m_dwWlanApiRetCode&&m_pInterfaceList)
		{
			if(m_pInterfaceList->dwNumberOfItems==1)
				m_pInterface = (PWLAN_INTERFACE_INFO )&m_pInterfaceList->InterfaceInfo[0];
			else
			{
				for(int nIfNumber=0; nIfNumber < (int)m_pInterfaceList->dwNumberOfItems; nIfNumber++)
				{
					m_pInterface = (PWLAN_INTERFACE_INFO )&m_pInterfaceList->InterfaceInfo[nIfNumber];
					if(_wcsicmp(_bstr_t(wsDescription),m_pInterface->strInterfaceDescription)==0)
						break;
					else
						m_pInterface=NULL;
				}
			}
		}
		else
			m_dwRetCode=2;	//WlanEnumInterfaces ʧ��
		//3.�ӽӿ��в�ѯ�����б���ȡָ������
		if(m_pInterface)
		{
			m_dwWlanApiRetCode=WlanGetAvailableNetworkList(m_hClient, &m_pInterface->InterfaceGuid,0x00000000, NULL, &m_pNetworkList);
			if(ERROR_SUCCESS==m_dwWlanApiRetCode&&m_pNetworkList)
			{
				for(int nNetworkNumber = 0; nNetworkNumber< (int)m_pNetworkList->dwNumberOfItems; nNetworkNumber++)
				{
					m_pNetwork = (PWLAN_AVAILABLE_NETWORK) &m_pNetworkList->Network[nNetworkNumber];
					if (strcmp(SSIDFromWIFI,(char*)m_pNetwork->dot11Ssid.ucSSID)==0)
					{
						m_dwWlanApiRetCode=SetProfile();
						if(m_dwWlanApiRetCode!=ERROR_SUCCESS)
							m_dwRetCode=6;
						break;
					}
					else
						m_pNetwork=NULL;
				}
				if(m_pNetwork==NULL)
					m_dwRetCode = 5;//δ�������豸
			}
			else
				m_dwRetCode = 4;//δ�ҵ�ָ������
		}
		else
			m_dwRetCode=3;	//δ�ҵ�����ӿ�
	}
}

CWlanConnect::~CWlanConnect(void)
{
	if(m_pNetwork)
		m_pNetwork=NULL;
	if(m_pInterface)
		m_pInterface=NULL;
	
	if(m_pInterfaceList!=NULL)
	{
		WlanFreeMemory(m_pInterfaceList);
		m_pInterfaceList=NULL;
	}
	if(m_pNetworkList!=NULL)
	{
		WlanFreeMemory(m_pNetworkList);
		m_pNetworkList=NULL;
	}
	if (m_hClient)
	{
		WlanCloseHandle(m_hClient, NULL);
		m_hClient = NULL;
	}
	m_dwRetCode=0;
	m_dwWlanApiRetCode=0;
}

BOOL CWlanConnect::IsConnected()
{
	BOOL bConnect=FALSE;
	if(m_pInterface&&wlan_interface_state_connected ==m_pInterface->isState)//��ѯ�������б�
	{
		PIP_ADAPTER_INFO pIpAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
		ULONG stSize= sizeof(IP_ADAPTER_INFO);
		int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
		if (ERROR_BUFFER_OVERFLOW == nRel)
		{
			free(pIpAdapterInfo);
			pIpAdapterInfo = (IP_ADAPTER_INFO *)malloc(stSize);//�ṹ��ָ�룬��Ҫ�����ڴ�ռ�
			GetAdaptersInfo(pIpAdapterInfo,&stSize);    
		}
		if(pIpAdapterInfo)
		{
			PIP_ADAPTER_INFO pAdapter=pIpAdapterInfo;
			while (pAdapter!=NULL)  
			{	//Ѱ����Ӧ��������
				if(wcsicmp(_bstr_t(pAdapter->Description) ,m_pInterface->strInterfaceDescription)==0)
					break;
				pAdapter = pAdapter->Next;
			}
			IP_ADDR_STRING *pIpAddrString =pAdapter!=NULL?&(pAdapter->GatewayList):NULL;
			while (pIpAddrString) 
			{
				if (strcmp(pIpAddrString->IpAddress.String,IP_ADDRESS)==0)
				{
					bConnect=TRUE;
					break;
				}
				pIpAddrString=pIpAddrString->Next;
			}
			free(pIpAdapterInfo);
		}
	}
	return bConnect;
}

int CWlanConnect::InternalConnectNetWork()
{
	if(m_pNetwork==NULL)
	{
		m_dwRetCode = 5;//δ�������豸
		return m_dwRetCode;
	}
	const size_t cSize = strlen((char*)m_pNetwork->dot11Ssid.ucSSID)+1;
	wchar_t* wSSID= new wchar_t[cSize];
	mbstowcs (wSSID, (char*)m_pNetwork->dot11Ssid.ucSSID, cSize);
	WLAN_CONNECTION_PARAMETERS wlanConnPara;
	wlanConnPara.wlanConnectionMode =wlan_connection_mode_profile; 
	wlanConnPara.strProfile =wSSID;                      
	wlanConnPara.pDot11Ssid = &m_pNetwork->dot11Ssid;                                 
	wlanConnPara.dot11BssType = m_pNetwork->dot11BssType;         
	wlanConnPara.pDesiredBssidList = NULL;                         
	wlanConnPara.dwFlags = 0;
	m_dwWlanApiRetCode=WlanConnect(m_hClient, &m_pInterface->InterfaceGuid, &wlanConnPara, NULL);
	delete wSSID;
	return m_dwWlanApiRetCode;
}

int CWlanConnect::ConnectNetwork()
{
	if(!IsConnected()) //�Ƿ����ָ��wifi�����Ѿ�����
	{
		if(m_pInterface&&wlan_interface_state_connected ==m_pInterface->isState)//ɨ�赽wifi���������Ѿ�����
		{
			m_dwWlanApiRetCode=WlanDisconnect(m_hClient, &m_pInterface->InterfaceGuid, NULL);			
			if (m_dwWlanApiRetCode==ERROR_SUCCESS)
			{
				m_dwWlanApiRetCode=InternalConnectNetWork();
				if(m_dwWlanApiRetCode!=ERROR_SUCCESS)
					m_dwRetCode=8;
			}
			else
				m_dwRetCode=7;
		}
		else
		{
			m_dwWlanApiRetCode=InternalConnectNetWork();
			if(m_dwWlanApiRetCode!=ERROR_SUCCESS)
				m_dwRetCode=8;
		}
	}
	return m_dwWlanApiRetCode;

}

int  CWlanConnect::SetProfile()
{ 
	int nReturnNum=1;
	DWORD dwReasonCode;
	CString sProfileXML("<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>");  //Profile XML��
	LPCWSTR wcProfileXML = NULL;
	CString szTemp("");
	/*ͷ*/
	szTemp = (char*)m_pNetwork->dot11Ssid.ucSSID;    
	sProfileXML+= szTemp;
	sProfileXML+="</name><SSIDConfig><SSID><name>";
	sProfileXML+= szTemp;   
	sProfileXML+="</name></SSID></SSIDConfig>";
	sProfileXML+="<connectionType>";
	switch(m_pNetwork->dot11BssType)    //��������
	{
	case dot11_BSS_type_infrastructure:
		sProfileXML+="ESS";
		break;
	case dot11_BSS_type_independent:
		sProfileXML+="IBSS";
		break;
	case dot11_BSS_type_any:
		sProfileXML+="Any";
		break;
	}
	/*MSM*/
	sProfileXML+="</connectionType><MSM><security><authEncryption><authentication>";
	switch( m_pNetwork->dot11DefaultAuthAlgorithm)    //������ܷ�ʽ
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
		sProfileXML+="open";
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		sProfileXML+="shared";
		break;
	case DOT11_AUTH_ALGO_WPA:
		sProfileXML+="WPA";
		break;
	case DOT11_AUTH_ALGO_WPA_PSK:
		sProfileXML+="WPAPSK";
		break;
	case DOT11_AUTH_ALGO_WPA_NONE:
		sProfileXML+="none";
		break;
	case DOT11_AUTH_ALGO_RSNA:
		sProfileXML+="WPA2";
		break;
	case DOT11_AUTH_ALGO_RSNA_PSK:
		sProfileXML+="WPA2PSK";
		break;
	}
	sProfileXML+="</authentication><encryption>";
	switch(m_pNetwork->dot11DefaultCipherAlgorithm)
	{
	case DOT11_CIPHER_ALGO_NONE:
		sProfileXML+="none";
		break;
	case DOT11_CIPHER_ALGO_WEP40:
		sProfileXML+= "WEP";
		break;
	case DOT11_CIPHER_ALGO_TKIP:
		sProfileXML+= "TKIP";
		break;
	case DOT11_CIPHER_ALGO_CCMP:
		sProfileXML+="AES";
		break;
	case DOT11_CIPHER_ALGO_WEP104:
		sProfileXML+="WEP";
		break;        
	case DOT11_CIPHER_ALGO_WEP:
		sProfileXML+="WEP";
		break;
	}
	sProfileXML+="</encryption></authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial>";
	sProfileXML+=(char*)m_pNetwork->dot11Ssid.ucSSID;
	/*β*/
	sProfileXML+="</keyMaterial></sharedKey></security></MSM></WLANProfile>";
	/*XML����ת����˫�ֽ�*/
	wcProfileXML=sProfileXML.AllocSysString();
	/*�����������*/
	m_dwWlanApiRetCode=WlanSetProfile(m_hClient, &m_pInterface->InterfaceGuid,0x00 ,wcProfileXML, NULL, TRUE, NULL	, &dwReasonCode);
	return m_dwWlanApiRetCode;
	
}

CString CWlanConnect::GetLastError()
{
	CString sError;
	if(ERROR_SUCCESS==m_dwWlanApiRetCode)
		return sError;
	//const DWORD ERROR_FUNC_WLANOPEN						= 1;	//WlanOpenHandle()
	//const DWORD ERROR_FUNC_WLANENUMINTERFACES				= 2;	//WlanEnumInterfaces()
	//const DWORD ERROR_FUNC_NULLINTERFACE					= 3;	//NullInterface
	//const DWORD ERROR_FUNC_WLANGETAVAILABLENETWORKLIST	= 4;	//WlanGetAvailableNetworkList()
	//const DWORD ERROR_FUNC_NULLNETWORK					= 5;	//NullNetwork
	//const DWORD ERROR_FUNC_WLANSETPROFILE					= 6;	//WlanSetProfile()
	//const DWORD ERROR_FUNC_WLANDISCONNECT					= 7;	//WlanDisconnect()
	//const DWORD ERROR_FUNC_WLANCONNECT					= 8;	//WlanConnect()
	CString funArr[]={"",
					  "WlanOpenHandle()",
					  "WlanEnumInterfaces()",
					  "NullInterface()",
					  "WlanGetAvailableNetworkList()",
					  "NullNetwork()",
					  "WlanSetProfile()",
					  "WlanDisconnect()",
					  "WlanConnect()"};
	if(m_pInterface==NULL||m_pNetworkList==NULL)
		sError="δ�ҵ������������������������!";
	else if(m_pInterface&&m_pNetworkList&&m_pNetwork==NULL)
		sError="δ�ҵ�ָ��wifi�����wifi�豸!";
	if(m_dwRetCode>=1&&m_dwRetCode<=8)
	{
		if(ERROR_BAD_PROFILE==m_dwWlanApiRetCode)
			sError.Append("����������SetProfile�û��ļ���ʽ����!");
		else if(ERROR_ACCESS_DENIED==m_dwWlanApiRetCode)
			sError.Append("����������û���㹻��Ȩ��!");
		else if(ERROR_NDIS_DOT11_POWER_STATE_INVALID==m_dwWlanApiRetCode)
			sError.Append("����������wifi���رգ��޷�����!");
		else if(ERROR_REMOTE_SESSION_LIMIT_EXCEEDED==m_dwWlanApiRetCode)
			sError.Append("������������ǰwifi���ӹ���!");
		else if(ERROR_INVALID_PARAMETER==m_dwWlanApiRetCode)
			sError.Append("����������PARAMETER���ݲ���ȷ!");
		else if(ERROR_INVALID_HANDLE==m_dwWlanApiRetCode)
			sError.Append("�����������޷�������!");
		else if(ERROR_NOT_ENOUGH_MEMORY==m_dwWlanApiRetCode)
			sError.Append("����������û���㹻���ڴ�ռ�!");
		sError.AppendFormat("������:%s,�����룺#d",funArr[m_dwRetCode],m_dwWlanApiRetCode);
	}
	return sError;
}

