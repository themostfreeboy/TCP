//server
#include <stdio.h>                   //����printf�Ⱥ����ĵ���
#include <winsock2.h>                //Socket�ĺ������á�
#pragma comment (lib, "ws2_32")      //C���������������ʱ������.h�ļ��⣬��Ҫ�����Ӧ��lib�ļ��������ͬ��C#��


int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sockaddr;
    sockaddr.sin_family=PF_INET;
    sockaddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");   //��Ҫ�󶨵����ص��ĸ�IP��ַ
    sockaddr.sin_port=htons(9000);                          //��Ҫ�����Ķ˿�
    bind(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));        //���а󶨶���

    listen(s, 1);                                           //��������

    printf("listening on port [%d].\n", 9000);

    while(TRUE)
    {
        SOCKADDR clientAddr;
        int size=sizeof(SOCKADDR);

        SOCKET clientsocket;
        clientsocket=accept(s, &clientAddr, &size);               //������ֱ������tcp�ͻ�������
        printf("***SYS***    New client touched.\n");

        char* msg="Hello, my client.\r\n";
        send(clientsocket, msg, strlen(msg)+sizeof(char), NULL);  //����ĵ���������Ҫע�⣬�Ǽ���һ��char���ȵ�
        printf("***SYS***    HELLO.\n");

        while(TRUE)
        {
            char buffer[MAXBYTE]={0};
            recv(clientsocket, buffer, MAXBYTE, NULL);            //һֱ���տͻ���socket��send����
            printf("***Client***    %s\n", buffer);
        }

        closesocket(clientsocket);                                //�ر�socket
    }

    closesocket(s);������������������������������������������������������//�رռ���socket

    WSACleanup();                                                //ж��

    getchar();                                                   

    exit(0);
}
