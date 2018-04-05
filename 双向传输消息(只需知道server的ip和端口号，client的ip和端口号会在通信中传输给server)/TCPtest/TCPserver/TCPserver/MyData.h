#ifndef __MYDATA_H
#define __MYDATA_H

#include <string>
#include<winsock.h>
#pragma comment(lib,"wsock32.lib")

class MySocketClient//Socket���Client�ͻ�����
{
public:
	char server_ip[256];//server������ip��ַ
	int server_port;//server�������˿ں�
	char client_ip[256];//client�ͻ���ip��ַ
	int client_port;//client�ͻ��˶˿ں�
	SOCKET sock_client;//���ӵ�ָ��
	SOCKADDR_IN addrserver;//�洢server�������������Ϣ
	MySocketClient();//���캯��
	bool Start();//���client�ͻ���������ò���������
	bool Finish();//�Ͽ������ͷ���Դ
	bool Send(const char sendbuf[],int length);//��server��������������
	bool SendClientIPPort();//��server����������client�ͻ��˵�ip��ַ�Ͷ˿ں�
};

class MySocketServer//Socket���Server��������
{
public:
	char server_ip[256];//server������ip��ַ
	int server_port;//server�������˿ں�
	char client_ip[256];//client�ͻ���ip��ַ
	int client_port;//client�ͻ��˶˿ں�
	bool listening;//���ڿ����Ƿ����
	SOCKET sock_server;//���ӵ�ָ��
	SOCKET sock_client;//���ӵ�ָ��
	SOCKADDR_IN addrclient;//�洢client�ͻ��˵������Ϣ
	SOCKADDR_IN addrserver;//�洢server�������������Ϣ
	MySocketServer();//���캯��
	bool StartThread();//������һ���߳���ִ��Start����
	bool Finish();//�Ͽ������ͷ���Դ
	bool Receive(char receivebuf[],int length);//��client�ͻ��˽�������
	bool ReceiveClientIPPort();//��client�ͻ��˽���client�ͻ��˵�ip��ַ�Ͷ˿ں�
private:
	bool Start();//���server������������ò���ʼ����
	static UINT ProcessForStart(LPVOID pParam);//Ϊ�˿���Start�����Ķ��̶߳��ӵĸ�������
};

bool GetLocalIP(char ip[]);//��ȡ����ip��ַ

#endif