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

UINT ProcClient(LPVOID pParam)
{
	client->Start();
	client->SendClientIPPort();
	HANDLE thread2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ProcServer,NULL,0,NULL);//启动多线程
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