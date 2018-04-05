#include <stdio.h>
#include <stdlib.h>
#include "MyData.h"

UINT ProcServer(LPVOID pParam)
{
	MySocketServer *server=new MySocketServer();
	server->server_port=12345;
	server->StartThread();
	Sleep(200);
	char receivebuf[256];
	while(true)
	{
		bool result=server->Receive(receivebuf,256);
		if(result==true)
		{
			printf("receive from client:%s\n",receivebuf);
		}
	}
	delete server;
}

UINT ProcClient(LPVOID pParam)
{
	MySocketClient *client=new MySocketClient();
	strcpy(client->server_ip,"192.168.1.106");
	client->server_port=23456;
	client->Start();
	char sendbuf[256];
	while(true)
	{
		scanf("%s",sendbuf);
		client->Send(sendbuf,strlen(sendbuf)+1);
	}
	delete client;
}

int main()
{
	char ip[256];
	GetLocalIP(ip);
	printf("server:local ip:%s\n",ip);
	HANDLE thread1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcServer,NULL,0,NULL);//启动多线程
	HANDLE thread2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcClient,NULL,0,NULL);//启动多线程
	while(true);
	return 0;
}