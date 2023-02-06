#include <stdio.h>
#include <WinSock2.h> // 包含网络编程的头文件，引入静态库

#pragma comment(lib, "ws2_32.lib")

int merror(int redata, int error, char *showinfo)
{
    if (redata == error)
    {
        perror(showinfo);
        getchar();
        return -1;
    }
    return 0;
}

void sendhtml(SOCKET s, char *filename);

int main()
{
    printf("Welcom use WebServer!\n");
    WSADATA wsdata;
    // MAKEWORD : short 2 Byte
    int isok = WSAStartup(MAKEWORD(2, 2), &wsdata); // 确定 socket 版本信息
    merror(isok, WSAEINVAL, "申请 socket 失败！");

    // 1 协议簇，决定 socket 的地址类型
    // 2 传输类型 SOCK_STREAM 流传输
    // 3 指定的传输协议 TCP
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET 使用 IPv4 地址
    merror(server, INVALID_SOCKET, "申请 socket 失败！");

    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET; // 和创建时 必须一样 IPv4
    // htons() 网络中与主机上的数据存储有区别
    // 网络: 大端存储（高位地址 存 低位数据）
    // 主机：小端存储（高位地址 存 高位数据）
    seraddr.sin_port = htons(80);
    seraddr.sin_addr.s_addr = INADDR_ANY; // 监听任意地址

    isok = bind(server, &seraddr, sizeof(seraddr));
    merror(isok, SOCKET_ERROR, "绑定失败！\n");

    isok = listen(server, 5);
    merror(isok, SOCKET_ERROR, "监听失败！\n");

    struct sockaddr_in claddr;
    int cllen = sizeof(claddr);
    while (1)
    {
        SOCKET client = accept(server, &claddr, &cllen); // 谁连进来了
        merror(client, INVALID_SOCKET, "连接失败！\n");

        char recvdata[1024] = "";
        recv(client, recvdata, 1024, 0);
        printf("%s 共接收到 %d Byte 数据", recvdata, strlen(recvdata));

        /*char sendata[1024] = "<h1 style =\" color:red;\"> hello, I'm server! </h1>";
        send(client, sendata, strlen(sendata), 0);*/

        char *filename = "./index.html";
        sendhtml(client, filename);

        closesocket(client);
    }

    closesocket(server);
    WSACleanup();

    getchar();
    return 0;
}

void sendhtml(SOCKET s, char *filename)
{
    // fopen use _CRT_SECURE_NO_WARNINGS
    FILE *pfile = fopen(filename, "r");
    if (!pfile)
    {
        printf("打开文件失败！\n");
        return;
    }
    char tmp[1024] = "";
    do
    {
        fgets(tmp, 1024, pfile);
        send(s, tmp, strlen(tmp), 0);

    } while (!feof(pfile));
}
