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

bool MySocketClient::SendFile(const char filepath[],int sleeptime)//向server服务器发送文件
{
	errno_t err;
	FILE *fp_read=NULL;
	err=fopen_s(&fp_read,filepath,"rb");//先检验文件是否存在并可以打开
	if(err!=0)
	{
		return false;//文件打开失败
	}
	fclose(fp_read);

	char filename[256];
	int filename_num=0;
	unsigned char md5[17];
	unsigned long filesize=md5_file_calc(filepath,md5);
	if(filesize==0)
	{
		return false;//文件md5以及文件大小计算失败
	}
	for(int i=strlen(filepath)-1;i>=0;i--)
	{
		if(filepath[i]=='\\')
		{
			for(int j=i+1;filepath[j]!='\0';j++)
			{
				filename[filename_num++]=filepath[j];
			}
			filename[filename_num]='\0';
			break;
		}
		else if(i==0 && filepath[i]!='\\')
		{
			strcpy(filename,filepath);
			filename_num=strlen(filename);
		}
	}
	char sendbuf[1024];
	int sendbuf_num=0;
	strcpy(sendbuf,"@filestart@");
	strcat(sendbuf,"@filename@");
	strcat(sendbuf,filename);
	strcat(sendbuf,"@filesize@");
	sendbuf_num=strlen(sendbuf);
	for(int i=0;i<4;i++)
	{
		unsigned char  tempfilesize=(filesize>>(8*(3-i)))&(0xff);
		sendbuf[sendbuf_num++]=tempfilesize;
	}
	StringPlus(sendbuf,"@md5@",sendbuf_num);
	for(int i=0;i<16;i++)//由于md5数组中可能存在0('\0')所以不能用StringPlus
	{
		sendbuf[sendbuf_num++]=md5[i];
	}
	sendbuf[sendbuf_num++]='\0';
	Send(sendbuf,sendbuf_num);//发送文件名、文件大小、文件md5校检值
	Sleep(sleeptime);
	err=fopen_s(&fp_read,filepath,"rb");
	if(err!=0)
	{
		return false;//文件打开失败
	}
	char tempchar=0;
	for(int i=0;i<filesize/500;i++)
	{
		strcpy(sendbuf,"@datastart@");
		sendbuf_num=strlen(sendbuf);
		for(int j=0;j<500;j++)
		{
			if(fscanf(fp_read,"%c",&tempchar)!=EOF)
			{
				sendbuf[sendbuf_num++]=tempchar;
			}
			else
			{
				return false;//读取数据过程中出错
			}
		}
		StringPlus(sendbuf,"@dataend@",sendbuf_num);
		if(filesize%500==0)
		{
			StringPlus(sendbuf,"@fileend@",sendbuf_num);
		}
		Send(sendbuf,sendbuf_num);
		Sleep(sleeptime);
	}
	if(filesize%500!=0)
	{
		strcpy(sendbuf,"@datastart@");
		sendbuf_num=strlen(sendbuf);
		for(int i=0;i<filesize%500;i++)
		{
			if(fscanf(fp_read,"%c",&tempchar)!=EOF)
			{
				sendbuf[sendbuf_num++]=tempchar;
			}
			else
			{
				return false;//读取数据过程中出错
			}
		}
		StringPlus(sendbuf,"@dataend@",sendbuf_num);
		StringPlus(sendbuf,"@fileend@",sendbuf_num);
		Send(sendbuf,sendbuf_num);
		Sleep(sleeptime);
	}
	fclose(fp_read);
	return true;
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

bool MySocketServer::ReceiveFile(const char filepath[])//从client客户端接收文件
{
	char filename[256];
	int filename_num=0;
	char new_filepath[256];
	int new_filepath_num=0;
	unsigned char original_md5[17];
	unsigned long original_filesize=0;
	char receivebuf[1024];
	while(Receive(receivebuf,1024)==false);
	int num1=-1,num2=-1;
	if(StringCheck(receivebuf,"@filestart@",1024)!=0)
	{
		return false;//数据格式有误
	}
	if(StringCheck(receivebuf,"@filename@",1024)!=11)
	{
		return false;//数据格式有误
	}
	num1=StringCheck(receivebuf,"@filesize@",1024);
	num2=StringCheck(receivebuf,"@md5@",1024);
	if(num1==-1 || num2==-1)
	{
		return false;//数据格式有误
	}
	if(num2-num1!=14)
	{
		return false;//数据格式有误
	}
	if(receivebuf[num2+21]!='\0')
	{
		return false;//数据格式有误
	}
	for(int i=21;i<num1;i++)
	{
		filename[filename_num++]=receivebuf[i];
	}
	filename[filename_num]='\0';
	strcpy(new_filepath,filepath);
	new_filepath_num=strlen(filepath);
	if(new_filepath[new_filepath_num-1]!='\\')
	{
		new_filepath[new_filepath_num]='\\';
		new_filepath[new_filepath_num+1]='\0';
	}
	strcat(new_filepath,filename);
	original_filesize=0;
	unsigned char tempfilesize=0;
	for(int i=0;i<4;i++)
	{
		tempfilesize=receivebuf[num1+10+i];
		original_filesize<<=8;
		original_filesize+=tempfilesize;
	}
	for(int i=0;i<16;i++)
	{
		original_md5[i]=receivebuf[num2+5+i];
	}
	errno_t err;
	FILE *fp_write=NULL;
	err=fopen_s(&fp_write,new_filepath,"wb");//先检验文件是否存在并可以打开
	if(err!=0)
	{
		return false;//文件打开失败
	}
	for(int i=0;i<original_filesize/500;i++)
	{
		while(Receive(receivebuf,1024)==false);
		if(StringCheck(receivebuf,"@datastart@",1024)!=0)
		{
			return false;//数据格式有误
		}
		if(StringCheck(receivebuf,"@dataend@",1024)!=511)
		{
			return false;//数据格式有误
		}
		if(i==original_filesize/500-1 && original_filesize%500==0)
		{
			if(StringCheck(receivebuf,"@fileend@",1024)!=520)
			{
				return false;//数据格式有误
			}
		}
		for(int j=0;j<500;j++)
		{
			fprintf(fp_write,"%c",receivebuf[j+11]);
		}
	}
	if(original_filesize%500!=0)
	{
		while(Receive(receivebuf,1024)==false);
		if(StringCheck(receivebuf,"@datastart@",1024)!=0)
		{
			return false;//数据格式有误
		}
		if(StringCheck(receivebuf,"@dataend@",1024)!=11+original_filesize%500)
		{
			return false;//数据格式有误
		}
		if(StringCheck(receivebuf,"@fileend@",1024)!=20+original_filesize%500)
		{
			return false;//数据格式有误
		}
		for(int i=0;i<original_filesize%500;i++)
		{
			fprintf(fp_write,"%c",receivebuf[i+11]);
		}
	}
	fclose(fp_write);
	unsigned char real_md5[17];
	unsigned long real_filesize=0;
	real_filesize=md5_file_calc(new_filepath,real_md5);
	if(real_filesize!=original_filesize)
	{
		return false;//数据大小有误
	}
	for(int i=0;i<16;i++)
	{
		if(real_md5[i]!=original_md5[i])
		{
			return false;//数据md5值不符，数据有损
		}
	}
	return true;
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

bool StringPlus(char original_string[],const char plus_string[],int &num)//为了避免original_string数组中已经含有0('\0')导致strcat失效
{
	for(int i=0;plus_string[i]!='\0';i++)
	{
		original_string[num++]=plus_string[i];
	}
	return true;
}

int StringCheck(const char original_string[],const char check_string[],const int num)//为了避免original_string数组中已经含有0('\0')导致strstr失效
{
	bool flag=false;
	for(int i=0;i<num;i++)
	{
		for(int j=0;i+j<num&&check_string[j]!='\0';j++)
		{
			if(original_string[i+j]==check_string[j])
			{
				flag=true;
			}
			else
			{
				flag=false;
				break;
			}
		}
		if(flag==true)
		{
			return i;//返回找到的第一个匹配字符的数组下标
		}
	}
	return -1;//未找到
}