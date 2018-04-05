#include <stdio.h>
#include <stdlib.h>
#include "MyData.h"

MySocketClient *client=new MySocketClient();

UINT ProcClient(LPVOID pParam)
{
	client->Start();
	char filepath[256];
	while(true)
	{
		printf("请输入待发送的文件路径(支持文件拖拽)\n");
		scanf_s("%s",filepath,256);
		printf("正在发送...\n");
		bool sendresult=client->SendFile(filepath,200);
		if(sendresult==true)
		{
			printf("发送成功\n");
		}
		else
		{
			printf("发送失败\n");
		}
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
	while(true)
	{
		bool result=server->ReceiveFile(".\\");
		if(result==true)
		{
			printf("接收成功\n");
		}
		else
		{
			printf("接收失败或者接收文件有损\n");
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