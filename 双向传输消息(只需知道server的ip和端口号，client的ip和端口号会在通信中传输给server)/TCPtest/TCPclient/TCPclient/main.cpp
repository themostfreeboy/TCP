#include <stdio.h>
#include <stdlib.h>
#include "MyData.h"

MySocketClient *client=new MySocketClient();

UINT ProcServer(LPVOID pParam)
{
	MySocketServer *server=new MySocketServer();
	server->server_port=23456;
	server->StartThread();
	Sleep(200);
	char receivebuf[256];
	while(true)
	{
		bool result=server->Receive(receivebuf,256);
		if(result==true)
		{
			printf("receive from server:%s\n",receivebuf);
		}
	}
	server->Finish();
	delete server;
}

UINT ProcClient(LPVOID pParam)
{
	client->Start();
	client->SendClientIPPort();
	HANDLE thread2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcServer,NULL,0,NULL);//启动多线程
	char sendbuf[256];
	while(true)
	{
		scanf("%s",sendbuf);
		client->Send(sendbuf,strlen(sendbuf)+1);
	}
	client->Finish();
	delete client;
}

int main()
{
	char ip[256];
	GetLocalIP(ip);
	printf("client:local ip:%s\n",ip);
	printf("input server ip:");
	scanf("%s",client->server_ip);
	//printf("input server port(default:12345):");
	//scanf("%d",&client->server_port);
	client->server_port=12345;
	HANDLE thread1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcClient,NULL,0,NULL);//启动多线程
	while(true);
	return 0;
}