//client
#include <stdio.h>                      //�������롢��������ĵ���,printf, gets
#include <winsock2.h>                   //socketͷ�ļ�
#include <Windows.h>                    //Ϊ�˷�����ԣ����Լ����˵ȴ�2��Ž�������server�������õ���sleep����

#pragma comment (lib, "ws2_32")         //socket���ļ�



int main()
{
    Sleep(2000);                        //��˯2��������server

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
           recv�����е�bufferlength�����ǿ��Թ̶�ֵ��
           send�����е�bufferlength�������̶ܹ�ֵ����Ҫ��ʵ�ʳ��ȣ����ҿ��ǵ�'\0'�ַ���
        */
    }

    closesocket(s);

    WSACleanup();

    getchar();

    exit(0);
}
