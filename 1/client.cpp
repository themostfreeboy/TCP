//client
#include <stdio.h>                      //用于输入、输出函数的调用,printf, gets
#include <winsock2.h>                   //socket头文件
#include <Windows.h>                    //为了方便调试，所以加入了等待2秒才进行连接server，这里用到了sleep函数

#pragma comment (lib, "ws2_32")         //socket库文件



int main()
{
    Sleep(2000);                        //沉睡2秒再连接server

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sockaddr;
    sockaddr.sin_family=PF_INET;
    sockaddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    sockaddr.sin_port=htons(9000);

    connect(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));

    char buffer[MAXBYTE]={0};
    recv(s, buffer, MAXBYTE, NULL);
    printf("***SERVER***%s", buffer);

    while(TRUE)
    {
        char* mymsg=new char[100000];
        printf("You can chat with server now:\n");
        gets(mymsg);
        send(s, mymsg, strlen(mymsg)+sizeof(char), NULL);
        /*
           recv函数中的bufferlength参数是可以固定值的
           send函数中的bufferlength参数不能固定值，需要看实际长度，并且考虑到'\0'字符串
        */
    }

    closesocket(s);

    WSACleanup();

    getchar();

    exit(0);
}
