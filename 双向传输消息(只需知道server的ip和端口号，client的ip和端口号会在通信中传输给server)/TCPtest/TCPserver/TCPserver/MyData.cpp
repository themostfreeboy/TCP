#pragma once

#include "MyData.h"

//MySocketClient//Socket编程Client客户端类
MySocketClient::MySocketClient()//构造函数
{
	GetLocalIP(server_ip);
	server_port=12345;
	GetLocalIP(client_ip);
	client_port=23456;
}

bool MySocketClient::Start()//完成client客户端相关配置并建立连接
{
	WORD wVersionRequest;
	WSADATA wsadata;
	wVersionRequest=MAKEWORD(2,0);
	int err=WSAStartup(wVersionRequest,&wsadata);
	if(err!=0)
	{
		return false;//配置连接失败
	}
	if(LOBYTE(wsadata.wVersion)!=2||(HIBYTE(wsadata.wVersion)!=0))
	{
		WSACleanup();
		return false;//配置连接失败
	}
	sock_client=socket(AF_INET,SOCK_STREAM,0);
	if(sock_client==INVALID_SOCKET)
	{
		return false;//配置连接失败
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
			return false;//重连10次仍然连接失败
		}
	}
	return true;//配置连接成功
}

bool MySocketClient::Finish()//断开连接释放资源
{
	closesocket(sock_client);
	WSACleanup();
	return true;
}

bool MySocketClient::Send(const char sendbuf[],int length)//向Server服务器发送数据
{
	if(sendto(sock_client,sendbuf,length,0,(SOCKADDR*)&addrserver,sizeof(SOCKADDR))!=SOCKET_ERROR)
	{
		return true;//发送成功
	}
	return false;//发送失败
}

bool MySocketClient::SendClientIPPort()//向server服务器发送client客户端的ip地址和端口号
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

//MySocketServer//Socket编程Server服务器类
MySocketServer::MySocketServer()//构造函数
{
	GetLocalIP(server_ip);
	server_port=12345;
	GetLocalIP(client_ip);
	client_port=23456;
	listening=true;
}

bool MySocketServer::StartThread()//新启用一个线程来执行Start函数
{
	HANDLE thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MySocketServer::ProcessForStart,this,0,NULL);//启动多线程
	return true;
}

bool MySocketServer::Finish()//断开连接释放资源
{
	closesocket(sock_client);
	WSACleanup();
	return true;
}

bool MySocketServer::Receive(char receivebuf[],int lenth)//从client客户端接收数据
{
	if(recv(sock_client,receivebuf,lenth,0)!=SOCKET_ERROR)
	{
		return true;//接收成功
	}
	return false;//接收失败
}

bool MySocketServer::ReceiveClientIPPort()//从client客户端接收client客户端的ip地址和端口号
{
	char receivebuf[256];
	for(int i=0;i<10;i++);//接收10次
	{
		int result=Receive(receivebuf,256);
		if(result==true)
		{
			char *tempstring1=new char[256];
			tempstring1=strstr(receivebuf,"@ip@");
			if(tempstring1==NULL)
			{
				return false;//接收数据有误
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
					return false;//接收数据有误
				}
			}
			char *tempstring2=new char[256];
			tempstring2=strstr(tempstring1,"@port@");
			if(tempstring2==NULL)
			{
				return false;//接收数据有误
			}
			client_port=(tempstring2[6]-'0')*10000+(tempstring2[7]-'0')*1000+(tempstring2[8]-'0')*100+(tempstring2[9]-'0')*10+(tempstring2[10]-'0');
			return true;
		}
		Sleep(500);
	}
	return false;
}

bool MySocketServer::Start()//完成server服务器相关配置并开始监听
{
	WORD wVersionRequest;
	WSADATA wsadata;
	wVersionRequest=MAKEWORD(2,0);
	int err=WSAStartup(wVersionRequest,&wsadata);
	if(err!=0)
	{
		return false;//配置连接失败
	}
	if(LOBYTE(wsadata.wVersion)!=2||(HIBYTE(wsadata.wVersion)!=0))
	{
		WSACleanup();
		return false;//配置连接失败
	}
	sock_server=socket(AF_INET, SOCK_STREAM, 0);
	if (sock_server==INVALID_SOCKET)
	{
		return false;//配置连接失败
	}
	memset(&addrserver,0,sizeof(addrserver));
	addrserver.sin_addr.S_un.S_addr = (htonl(INADDR_ANY));
	addrserver.sin_family=AF_INET;
	addrserver.sin_port=htons(server_port);
	if(bind(sock_server,(SOCKADDR*)&addrserver,sizeof(SOCKADDR))==0)
	{
		int length=sizeof(addrclient);
		listen(sock_server,128);//开始监听
		while(listening==true)
		{
			sock_client=accept(sock_server,(SOCKADDR*)&addrclient,&length);
			if(sock_client==INVALID_SOCKET)
			{
				return false;//配置连接失败
			}
		}
	}
	return true;//配置连接成功
}

UINT MySocketServer::ProcessForStart(LPVOID pParam)//为了开启Start函数的多线程而加的辅助函数
{
	MySocketServer *server=(MySocketServer*)pParam;
	server->Start();
	return 0;
}

bool GetLocalIP(char ip[])//获取本机ip地址
{
	WORD wVersionRequest;
	WSADATA wsadata;
	wVersionRequest=MAKEWORD(2,0);
	int err=WSAStartup(wVersionRequest,&wsadata);
	if(err!=0)
	{
		return false;//配置失败
	}
	if(LOBYTE(wsadata.wVersion)!=2||(HIBYTE(wsadata.wVersion)!=0))
	{
		WSACleanup();
		return false;//配置失败
	}
	char szHostName[MAX_PATH+1];
    gethostname(szHostName,MAX_PATH);//获得本机主机名
    hostent *hn;
    hn=gethostbyname(szHostName);//根据本机主机名得到本机IP
	if(hn==NULL)
    {
		return false;//获取失败
    }
    strcpy(ip,inet_ntoa(*(in_addr *)hn->h_addr_list[0]));//把ip转换成字符串形式
	return true;//获取成功
}