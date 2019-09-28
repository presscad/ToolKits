#include "StdAfx.h"
#include "SocketObject.h"

CSocketObject::CSocketObject(const char* strBindIp/*=NULL*/,u_short bindPort/*=0*/,BOOL bServer/*=TRUE*/)
{
	m_hSocket = (UINT)INVALID_SOCKET;
	m_hAcceptSocket=(UINT)INVALID_SOCKET;
	m_nFunType=0;
	InitSocket(strBindIp,bindPort,bServer);
}
CSocketObject::~CSocketObject()
{
	DestroySocketObject();
}
void CSocketObject::DestroySocketObject()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=(UINT)INVALID_SOCKET;
	}
	if(m_hAcceptSocket!=INVALID_SOCKET)
	{
		closesocket(m_hAcceptSocket);
		m_hAcceptSocket=INVALID_SOCKET;
	}
}
/* ʹ��TCP/IPЭ��
 * strBindIp Ĭ�ϼ����������е�ͨ��
 */
int CSocketObject::InitSocket(const char* strBindIp/*=NULL*/,u_short bindPort/*=0*/,BOOL bServer/*=TRUE*/)
{
	//DestroySocketObject();	�˴����ܵ���DestroySocketObject(),���һ��ȷ��ԭ��? wht 17-02-10
	if(bindPort==0)
	{
		m_hSocket=(UINT)INVALID_SOCKET;
		return m_hSocket;
	}
	SOCKET socketId = INVALID_SOCKET;
	//����һ���ܽ�������ͨ�ŵ��׽���
	socketId = socket(AF_INET,SOCK_STREAM,0);
	if(socketId==SOCKET_ERROR)
	{
		m_nFunType=FUN_SCOKET_ERR;
		return m_nFunType;
	}
	SOCKADDR_IN sockStruct;
	sockStruct.sin_family = AF_INET; //ʹ��TCP/IPЭ��
	if(strBindIp==NULL||strBindIp[0]=='\0')
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	else
		sockStruct.sin_addr.S_un.S_addr = inet_addr(strBindIp);
	sockStruct.sin_port = htons(bindPort);
	if(!bServer) //�ͻ���
	{
		if(connect(socketId,(LPSOCKADDR)&sockStruct,sizeof(sockStruct)) == SOCKET_ERROR)
		{
			m_nFunType=FUN_CONNECT_ERR;
			return m_nFunType;
		}
		else
			m_hSocket = (UINT)socketId;
	}
	else //if( bServer)  //�����
	{
		if(bind(socketId,(sockaddr*)&sockStruct,sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			m_nFunType=FUN_BIND_ERR;
			return m_nFunType;
		}
		else if( listen(socketId,SOMAXCONN) == SOCKET_ERROR )
		{
			m_nFunType=FUN_LISTEN_ERR;
			return m_nFunType;
		}
		else
			return(m_hSocket=(UINT)socketId);
	}
	return (UINT)socketId;
}

bool CSocketObject::GetMachineName(char* hostname)
{
	SOCKETINIT life;
	//��ȡ������������
	char host_name[255]="";
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		return false;
	StrCopy(hostname,host_name,255);
	return strlen(host_name)>0;
}

CXhChar100 CSocketObject::GetMachineIpStr(const char* hostname/*=NULL*/)
{
	in_addr addr;
	GetMachineIp(hostname,&addr);
	CXhChar100 ipstr;
	ipstr.Printf("%d.%d.%d.%d",addr.S_un.S_un_b.s_b1,addr.S_un.S_un_b.s_b2,addr.S_un.S_un_b.s_b3,addr.S_un.S_un_b.s_b4);
	return ipstr;
}

int CSocketObject::GetMachineIp(const char* hostname/*=NULL*/,in_addr* pAddrs/*=NULL*/,int maxReadIpCount/*=1*/)
{
	SOCKETINIT life;
	//��ȡ������������
	char host_name[255]="";
	if(hostname!=NULL)
		StrCopy(host_name,hostname,255);
	else if(gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		return 0;
	struct hostent *pHostent = gethostbyname(host_name);
	if (pHostent == 0)
		return 0;
	//ѭ���ó����ػ�������IP��ַ
	if(pHostent==0)
		return 0;
	int i=0;
	for(i=0;i<pHostent->h_length&&pHostent->h_addr_list&&pHostent->h_addr_list[i]!=0;i++)
	{
		if(pAddrs!=NULL&&i<maxReadIpCount)
			memcpy(&pAddrs[i], pHostent->h_addr_list[i], sizeof(struct in_addr));
		else if(pAddrs!=NULL&&i==maxReadIpCount)
			return maxReadIpCount;
		//computer_ip[0]=addr.S_un.S_un_b.s_b1;
		//computer_ip[1]=addr.S_un.S_un_b.s_b2;
		//computer_ip[2]=addr.S_un.S_un_b.s_b3;
		//computer_ip[3]=addr.S_un.S_un_b.s_b4;
	}
	return i;
}
SOCKET CSocketObject::NetAccept(sockaddr* paddr/*=NULL*/,int addrLen/*=0*/)
{
	CloseAcceptSocket();
	m_hAcceptSocket = INVALID_SOCKET;
	sockaddr addr;
	if(paddr==NULL)
	{
		paddr=&addr;
		addrLen=sizeof(addr);
	}
	m_hAcceptSocket=accept(m_hSocket,paddr,&addrLen);
	if (m_hAcceptSocket==INVALID_SOCKET)
	{
		m_nFunType=FUN_ACCEPT_ERR;
		return m_nFunType;
	}
	else
		return m_hAcceptSocket;
}
BOOL CSocketObject::CloseAcceptSocket()
{
	if(m_hAcceptSocket!=INVALID_SOCKET)
	{
		closesocket(m_hAcceptSocket);
		m_hAcceptSocket=INVALID_SOCKET;
	}
	return TRUE;
}
int CSocketObject::NetRecv(SOCKET sock, char *buf, int len)
{
	int res=recv(sock, buf, len, 0);
	if(res==SOCKET_ERROR||res<0)
	{
		m_nFunType=FUN_RECV_ERR;
		return m_nFunType;
	}
	return res;
}
int CSocketObject::NetSend(SOCKET sock, const char *buf, int len)
{
	int res=send( sock, buf, len,0);
	if(res==SOCKET_ERROR||res<0)
	{
		m_nFunType=FUN_SEND_ERR;
		return m_nFunType;
	}
	return  res;
}
CXhChar200  CSocketObject::GetErrorStr()
{
	CXhChar200 sErr;
	DWORD dwErrCode=WSAGetLastError();
	if(m_nFunType==CSocketObject::FUN_LIB_ERR)
		sErr.Printf("winsocket��汾��");
	else if(m_nFunType==CSocketObject::FUN_SCOKET_ERR)
	{
		if(dwErrCode==WSAEAFNOSUPPORT)
			sErr.Printf("socket()�����룺%d,��֧��ָ���ĵ�ַ�壡",dwErrCode);
		else if(dwErrCode==WSAEINPROGRESS)
			sErr.Printf("socket()�����룺%d,һ���������׽ӿڵ������������У�",dwErrCode);
		else if(dwErrCode==WSAEMFILE)
			sErr.Printf("socket()�����룺%d,�޿����ļ������֣�",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("socket()�����룺%d,�޿��û��������޷������׽ӿڣ�",dwErrCode);
		else if(dwErrCode==WSAEPROTONOSUPPORT)
			sErr.Printf("socket()�����룺%d,��֧��ָ����Э�飡",dwErrCode);
		else if(dwErrCode==WSAEPROTOTYPE)
			sErr.Printf("socket()�����룺%d,ָ����Э�鲻�����ڱ��׽ӿڣ�",dwErrCode);
		else if(dwErrCode==WSAESOCKTNOSUPPORT)
			sErr.Printf("socket()�����룺%d,����ַ���в�֧�ָ������׽ӿڣ�",dwErrCode);
		else 
			sErr.Printf("socket()�����룺%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_SEND_ERR)
	{
		if(dwErrCode==WSAEINTR)
			sErr.Printf("send()�����룺%d��ͨ��һ��WSACancelBlockingCall()��ȡ��һ���������ģ����ã�",dwErrCode);
		else if(dwErrCode==WSAEFAULT)
			sErr.Printf("send()�����룺%d��buf���������û���ַ�ռ��е���Чλ�ã�",dwErrCode);
		else if(dwErrCode==WSAENETRESET)
			sErr.Printf("send()�����룺%d������WINDOWS�׽ӿ�ʵ�ַ��������ӣ��ʸ����ӱ��豻��λ��",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("send()�����룺%d��WINDOWS�׽ӿ�ʵ�ֱ���һ��������������",dwErrCode);
		else if(dwErrCode==WSAENOTCONN)
			sErr.Printf("send()�����룺%d���׽ӿ�δ�����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("send()�����룺%d���׽ӿ�δ��bind()����",dwErrCode);
		else if(dwErrCode==WSAECONNABORTED)
			sErr.Printf("send()�����룺%d�����ڳ�ʱ������ԭ���������·���жϣ�",dwErrCode);
		else 
			sErr.Printf("send()�����룺%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_RECV_ERR)
	{
		if(dwErrCode==WSAESHUTDOWN)
			sErr.Printf("recv()�����룺%d���׽ӿ��ѱ��رգ�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("recv()�����룺%d���׽ӿ�δ��bind()��������",dwErrCode);
		//else if(dwErrCode==WSAENOTCONN)
			//sErr.Printf("�׽ӿ�δ���ӣ�",dwErrCode);
		else if(dwErrCode==WSANOTINITIALISED)
			sErr.Printf("recv()�����룺%d��û�е���WSAStartup()����ʧ�ܣ�",dwErrCode);
		//else if(dwErrCode==WSAENOBUFS)
			//sErr.Printf("WINDOWS�׽ӿ�ʵ�ֱ���һ��������������";
		else if(dwErrCode==WSAENOTCONN)
			sErr.Printf("recv()�����룺%d���׽ӿ�δ�����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("recv()�����룺%d���׽ӿ�δ��bind()����",dwErrCode);
		else if(dwErrCode==WSAECONNABORTED)
			sErr.Printf("recv()�����룺%d�����ڳ�ʱ������ԭ���������·���жϣ�",dwErrCode);
		else 
			sErr.Printf("recv()�����룺%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_CONNECT_ERR)
	{
		if (dwErrCode==WSAEADDRINUSE)
			sErr.Printf("connect()�����룺%d����ַ�Ѿ�ʹ��",dwErrCode);
		else if (dwErrCode==WSAEISCONN)
			sErr.Printf("connect()�����룺%d���׽ӿ���������",dwErrCode);
		else if (dwErrCode==WSAEFAULT)
			sErr.Printf("connect()�����룺%d��namelen��������ȷ",dwErrCode);
		else if (dwErrCode==WSAEINVAL)
			sErr.Printf("connect()�����룺%d���׽ӿ�û��׼������һ��ַ����",dwErrCode);
		else if (dwErrCode==WSAENETUNREACH)
			sErr.Printf("connect()�����룺%d����ǰ�޷��ӱ�������������",dwErrCode);
		else if (dwErrCode==WSAECONNREFUSED)
			sErr.Printf("connect()�����룺%d�����ӳ��Ա�ǿ�ƾܾ�",dwErrCode);
		else if (dwErrCode==WSAEINPROGRESS)
			sErr.Printf("connect()�����룺%d��һ���������׽ӿڵ�������������",dwErrCode);
		else if (dwErrCode==WSAEADDRNOTAVAIL)
			sErr.Printf("connect()�����룺%d���ڱ��ػ������Ҳ�����ָ�ĵ�ַ",dwErrCode);
		else 
			sErr.Printf("connect()�����룺%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_LISTEN_ERR)
	{
		if(dwErrCode==WSAEADDRINUSE)
			sErr.Printf("listen()�����룺%d,��ͼ��listen()ȥ����һ������ʹ���еĵ�ַ��",dwErrCode);
		else if(dwErrCode==WSAEINPROGRESS)
			sErr.Printf("listen()�����룺%d,һ���������׽ӿڵ������������У�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("listen()�����룺%d,���׽ӿ�δ��bind()�������󣬻��ѱ����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEISCONN)
			sErr.Printf("listen()�����룺%d,�׽ӿ��ѱ����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEMFILE)
			sErr.Printf("listen()�����룺%d,�޿����ļ������֣�",dwErrCode);
		else if(dwErrCode==WSAENOTSOCK)
			sErr.Printf("listen()�����룺%d,�����ֲ���һ���׽ӿڣ�",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("listen()�����룺%d,�޿��û������ռ䣡",dwErrCode);
		else if(dwErrCode==WSAEOPNOTSUPP)
			sErr.Printf("listen()�����룺%d,���׽ӿڲ�����listen()���ã�",dwErrCode);
		else
			sErr.Printf("listen()�����룺%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_ACCEPT_ERR)
	{
		if (dwErrCode==WSAEFAULT)
			sErr.Printf("accept()�����룺%d,addrlen����̫С��С��socket�ṹ�Ĵ�С��",dwErrCode);
		else if (dwErrCode==WSAEINTR)
			sErr.Printf("accept()�����룺%d,ͨ��һ��WSACancelBlockingCall()��ȡ��һ���������ģ�����",dwErrCode);
		else if (dwErrCode==WSAEINPROGRESS)
			sErr.Printf("accept()�����룺%d,һ���������׽ӿڵ�������������",dwErrCode);
		else if (dwErrCode==WSAEINVAL)
			sErr.Printf("accept()�����룺%d,��accept()ǰδ����listen()",dwErrCode);
		else if (dwErrCode==WSAEMFILE)
			sErr.Printf("accept()�����룺%d,����accept()ʱ����Ϊ�գ��޿��õ�������",dwErrCode);
		else if (dwErrCode==WSAENOBUFS)
			sErr.Printf("accept()�����룺%d,�޿��û������ռ�",dwErrCode);
		else if (dwErrCode==WSAENOTSOCK)
			sErr.Printf("accept()�����룺%d,�����ֲ���һ���׽ӿ�",dwErrCode);
		else if (dwErrCode==WSAEOPNOTSUPP)
			sErr.Printf("accept()�����룺%d,���׽ӿ����Ͳ�֧���������ӷ���",dwErrCode);
		else if (dwErrCode==WSAEWOULDBLOCK)
			sErr.Printf("accept()�����룺%d,���׽ӿ�Ϊ��������ʽ�������ӿɹ�����",dwErrCode);
		else if (dwErrCode==WSAECONNRESET)
			sErr.Printf("accept()�����룺%d,�������Ӻ���accept����֮ǰ����Զ�̿ͻ��˶Ͽ�����",dwErrCode);
		else
			sErr.Printf("accept()�����룺%d",dwErrCode);
	}
	return sErr;
}
#ifdef __ALFA_TEST_VERSION_
void TestServerSocket()
{
	CSocketObject s;
	s.InitSocket("192.168.2.1",8888,TRUE);
	//if(soket<0)
		//���� 
	while(TRUE)
	{
		SOCKET acceptSoket=s.NetAccept();
		char str[100]="";
		if(s.NetRecv(acceptSoket,str,sizeof(str))<0)
		{//���� 
			return;
		}
		if(s.NetRecv(acceptSoket,str,sizeof(str))<0)
		{
			return;
		}
		CXhChar100 str1="asdffds";
		if(s.NetSend(acceptSoket,str1,sizeof(str1))<0)
		{
			return;
		}
		closesocket(acceptSoket);
	}
}
void TestClientSocket()
{
	CSocketObject socket;
	SOCKET socket1=socket.InitSocket("192.168.2.1",8888,FALSE);
	if(socket1<0)
	{
		//socket.GetErrorStr(CSocketObject::FUN_SCOKET_ERR);
		//����
		return;
	}
	CXhChar100 a ="1234";
	if(socket.NetSend(socket1,a,sizeof(a))<0)
	{
		closesocket(socket1);
		return;
	}
	a ="789";
	if(socket.NetSend(socket1,a,sizeof(a))<0)
	{
		closesocket(socket1);
		return;
	}
	a="";
	if(socket.NetRecv(socket1,a,sizeof(a))<0)
	{
		closesocket(socket1);
		return;
	}
	closesocket(socket1);
}
#endif