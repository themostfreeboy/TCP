#ifndef __MYDATA_H
#define __MYDATA_H

#include <string>
#include<winsock.h>
#pragma comment(lib,"wsock32.lib")

class MySocketClient//Socket编程Client客户端类
{
public:
	char server_ip[256];//server服务器ip地址
	int server_port;//server服务器端口号
	char client_ip[256];//client客户端ip地址
	int client_port;//client客户端端口号
	SOCKET sock_client;//连接的指针
	SOCKADDR_IN addrserver;//存储server服务器的相关信息
	MySocketClient();//构造函数
	bool Start();//完成client客户端相关配置并建立连接
	bool Finish();//断开连接释放资源
	bool Send(const char sendbuf[],int length);//向server服务器发送数据
	bool SendClientIPPort();//向server服务器发送client客户端的ip地址和端口号
};

class MySocketServer//Socket编程Server服务器类
{
public:
	char server_ip[256];//server服务器ip地址
	int server_port;//server服务器端口号
	char client_ip[256];//client客户端ip地址
	int client_port;//client客户端端口号
	bool listening;//用于控制是否监听
	SOCKET sock_server;//连接的指针
	SOCKET sock_client;//连接的指针
	SOCKADDR_IN addrclient;//存储client客户端的相关信息
	SOCKADDR_IN addrserver;//存储server服务器的相关信息
	MySocketServer();//构造函数
	bool StartThread();//新启用一个线程来执行Start函数
	bool Finish();//断开连接释放资源
	bool Receive(char receivebuf[],int length);//从client客户端接收数据
	bool ReceiveClientIPPort();//从client客户端接收client客户端的ip地址和端口号
private:
	bool Start();//完成server服务器相关配置并开始监听
	static UINT ProcessForStart(LPVOID pParam);//为了开启Start函数的多线程而加的辅助函数
};

bool GetLocalIP(char ip[]);//获取本机ip地址

#endif