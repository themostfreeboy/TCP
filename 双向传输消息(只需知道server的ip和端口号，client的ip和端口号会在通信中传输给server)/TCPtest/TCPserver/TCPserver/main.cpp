#include <stdio.h>
#include <stdlib.h>
#include "MyData.h"

MySocketClient *client=new MySocketClient();

UINT ProcClient(LPVOID pParam)
{
	client->Start();
	char sendbuf[256];
	while(true)
	{
		scanf("%s",sendbuf);
		client->Send(sendbuf,strlen(sendbuf)+1);
	}
	client->Finish();
	delete client;
}

UINT ProcServer(LPVOID pParam)
{
	MySocketServer *server=new MySocketServer();
	server->server_port=12345;
	//printf("server port:%d\n",server->server_port);
	server->StartThread();
	Sleep(200);
	bool receiveresult=false;
	while(receiveresult==false)
	{
		receiveresult=server->ReceiveClientIPPort();
	}
	strcpy(client->server_ip,server->client_ip);
	client->server_port=server->client_port;
	HANDLE thread2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcClient,NULL,0,NULL);//启动多线程
	char receivebuf[256];
	while(true)
	{
		bool result=server->Receive(receivebuf,256);
		if(result==true)
		{
			printf("receive from client:%s\n",receivebuf);
		}
	}
	server->Finish();
	delete server;
}

int main()
{
	char ip[256];
	GetLocalIP(ip);
	printf("server:local ip:%s\n",ip);
	HANDLE thread1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcServer,NULL,0,NULL);//启动多线程
	while(true);
	return 0;
}