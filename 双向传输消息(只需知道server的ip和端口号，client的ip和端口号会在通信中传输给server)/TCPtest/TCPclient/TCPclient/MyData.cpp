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

bool MySocketClient::SendClientIPPort()//��server����������client�ͻ��˵�ip��ַ�Ͷ˿ں�
{
	char sendbuf[256];
	strcpy(sendbuf,"@ip@");
	strcat(sendbuf,client_ip);
	strcat(sendbuf,"@port@");
	int num=strlen(sendbuf);
	int a=client_port/10000;
	int b=(client_port/1000)%10;
	int c=(client_port/100)%10;
	int d=(client_port/10)%10;
	int e=client_port%10;
	sendbuf[num++]=a+'0';
	sendbuf[num++]=b+'0';
	sendbuf[num++]=c+'0';
	sendbuf[num++]=d+'0';
	sendbuf[num++]=e+'0';
	sendbuf[num]='\0';
	bool result=Send(sendbuf,strlen(sendbuf)+1);
	return result;
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

bool MySocketServer::ReceiveClientIPPort()//��client�ͻ��˽���client�ͻ��˵�ip��ַ�Ͷ˿ں�
{
	char receivebuf[256];
	for(int i=0;i<10;i++);//����10��
	{
		int result=Receive(receivebuf,256);
		if(result==true)
		{
			char *tempstring1=new char[256];
			tempstring1=strstr(receivebuf,"@ip@");
			if(tempstring1==NULL)
			{
				return false;//������������
			}
			for(int j=0;tempstring1[j+4]!='\0';j++)
			{
				if((tempstring1[j+4]>='0' && tempstring1[j+4]<='9') || tempstring1[j+4]=='.')
				{
					client_ip[j]=tempstring1[j+4];
				}
				else if(tempstring1[j+4]=='@')
				{
					client_ip[j]='\0';
					break;
				}
				else
				{
					return false;//������������
				}
			}
			char *tempstring2=new char[256];
			tempstring2=strstr(tempstring1,"@port@");
			if(tempstring2==NULL)
			{
				return false;//������������
			}
			client_port=(tempstring2[6]-'0')*10000+(tempstring2[7]-'0')*1000+(tempstring2[8]-'0')*100+(tempstring2[9]-'0')*10+(tempstring2[10]-'0');
			return true;
		}
		Sleep(500);
	}
	return false;
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