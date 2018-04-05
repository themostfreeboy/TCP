#pragma once

#include "MyData.h"

//MySocketClient//Socket���Client�ͻ�����
MySocketClient::MySocketClient()//���캯��
{
	GetLocalIP(server_ip);
	server_port=12345;
	GetLocalIP(client_ip);
	client_port=23456;
}

bool MySocketClient::Start()//���client�ͻ���������ò���������
{
	WORD wVersionRequest;
	WSADATA wsadata;
	wVersionRequest=MAKEWORD(2,0);
	int err=WSAStartup(wVersionRequest,&wsadata);
	if(err!=0)
	{
		return false;//��������ʧ��
	}
	if(LOBYTE(wsadata.wVersion)!=2||(HIBYTE(wsadata.wVersion)!=0))
	{
		WSACleanup();
		return false;//��������ʧ��
	}
	sock_client=socket(AF_INET,SOCK_STREAM,0);
	if(sock_client==INVALID_SOCKET)
	{
		return false;//��������ʧ��
	}
	memset(&addrserver,0,sizeof(addrserver));
	addrserver.sin_addr.S_un.S_addr=inet_addr(server_ip);
	addrserver.sin_family=AF_INET;
	addrserver.sin_port=htons(server_port);
	for(int i=0;connect(sock_client,(SOCKADDR*)&addrserver,sizeof(SOCKADDR))!=0;i++)
	{
		Sleep(10);
		if(i==10)
		{
			return false;//����10����Ȼ����ʧ��
		}
	}
	return true;//�������ӳɹ�
}

bool MySocketClient::Finish()//�Ͽ������ͷ���Դ
{
	closesocket(sock_client);
	WSACleanup();
	return true;
}

bool MySocketClient::Send(const char sendbuf[],int length)//��Server��������������
{
	if(sendto(sock_client,sendbuf,length,0,(SOCKADDR*)&addrserver,sizeof(SOCKADDR))!=SOCKET_ERROR)
	{
		return true;//���ͳɹ�
	}
	return false;//����ʧ��
}

//MySocketServer//Socket���Server��������
MySocketServer::MySocketServer()//���캯��
{
	GetLocalIP(server_ip);
	server_port=12345;
	GetLocalIP(client_ip);
	client_port=23456;
	listening=true;
}

bool MySocketServer::StartThread()//������һ���߳���ִ��Start����
{
	HANDLE thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MySocketServer::ProcessForStart,this,0,NULL);//�������߳�
	return true;
}

bool MySocketServer::Finish()//�Ͽ������ͷ���Դ
{
	closesocket(sock_client);
	WSACleanup();
	return true;
}

bool MySocketServer::Receive(char receivebuf[],int lenth)//��client�ͻ��˽�������
{
	if(recv(sock_client,receivebuf,lenth,0)!=SOCKET_ERROR)
	{
		return true;//���ճɹ�
	}
	return false;//����ʧ��
}

bool MySocketServer::Start()//���server������������ò���ʼ����
{
	WORD wVersionRequest;
	WSADATA wsadata;
	wVersionRequest=MAKEWORD(2,0);
	int err=WSAStartup(wVersionRequest,&wsadata);
	if(err!=0)
	{
		return false;//��������ʧ��
	}
	if(LOBYTE(wsadata.wVersion)!=2||(HIBYTE(wsadata.wVersion)!=0))
	{
		WSACleanup();
		return false;//��������ʧ��
	}
	sock_server=socket(AF_INET, SOCK_STREAM, 0);
	if (sock_server==INVALID_SOCKET)
	{
		return false;//��������ʧ��
	}
	memset(&addrserver,0,sizeof(addrserver));
	addrserver.sin_addr.S_un.S_addr = (htonl(INADDR_ANY));
	addrserver.sin_family=AF_INET;
	addrserver.sin_port=htons(server_port);
	if(bind(sock_server,(SOCKADDR*)&addrserver,sizeof(SOCKADDR))==0)
	{
		int length=sizeof(addrclient);
		listen(sock_server,128);//��ʼ����
		while(listening==true)
		{
			sock_client=accept(sock_server,(SOCKADDR*)&addrclient,&length);
			if(sock_client==INVALID_SOCKET)
			{
				return false;//��������ʧ��
			}
		}
	}
	return true;//�������ӳɹ�
}

UINT MySocketServer::ProcessForStart(LPVOID pParam)//Ϊ�˿���Start�����Ķ��̶߳��ӵĸ�������
{
	MySocketServer *server=(MySocketServer*)pParam;
	server->Start();
	return 0;
}

bool GetLocalIP(char ip[])//��ȡ����ip��ַ
{
	WORD wVersionRequest;
	WSADATA wsadata;
	wVersionRequest=MAKEWORD(2,0);
	int err=WSAStartup(wVersionRequest,&wsadata);
	if(err!=0)
	{
		return false;//����ʧ��
	}
	if(LOBYTE(wsadata.wVersion)!=2||(HIBYTE(wsadata.wVersion)!=0))
	{
		WSACleanup();
		return false;//����ʧ��
	}
	char szHostName[MAX_PATH+1];
    gethostname(szHostName,MAX_PATH);//��ñ���������
    hostent *hn;
    hn=gethostbyname(szHostName);//���ݱ����������õ�����IP
	if(hn==NULL)
    {
		return false;//��ȡʧ��
    }
    strcpy(ip,inet_ntoa(*(in_addr *)hn->h_addr_list[0]));//��ipת�����ַ�����ʽ
	return true;//��ȡ�ɹ�
}