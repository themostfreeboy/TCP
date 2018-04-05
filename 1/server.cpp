//server
#include <stdio.h>                   //用于printf等函数的调用
#include <winsock2.h>                //Socket的函数调用　
#pragma comment (lib, "ws2_32")      //C语言引用其他类库时，除了.h文件外，还要加入对应的lib文件（这个不同于C#）


int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sockaddr;
    sockaddr.sin_family=PF_INET;
    sockaddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");   //需要绑定到本地的哪个IP地址
    sockaddr.sin_port=htons(9000);                          //需要监听的端口
    bind(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));        //进行绑定动作

    listen(s, 1);                                           //启动监听

    printf("listening on port [%d].\n", 9000);

    while(TRUE)
    {
        SOCKADDR clientAddr;
        int size=sizeof(SOCKADDR);

        SOCKET clientsocket;
        clientsocket=accept(s, &clientAddr, &size);               //阻塞，直到有新tcp客户端连接
        printf("***SYS***    New client touched.\n");

        char* msg="Hello, my client.\r\n";
        send(clientsocket, msg, strlen(msg)+sizeof(char), NULL);  //这里的第三个参数要注意，是加了一个char长度的
        printf("***SYS***    HELLO.\n");

        while(TRUE)
        {
            char buffer[MAXBYTE]={0};
            recv(clientsocket, buffer, MAXBYTE, NULL);            //一直接收客户端socket的send操作
            printf("***Client***    %s\n", buffer);
        }

        closesocket(clientsocket);                                //关闭socket
    }

    closesocket(s);　　　　　　　　　　　　　　　　　　　　　　　　　　　//关闭监听socket

    WSACleanup();                                                //卸载

    getchar();                                                   

    exit(0);
}
