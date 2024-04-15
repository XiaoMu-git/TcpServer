#define WIN32_LEAN_AND_MEAN	// 用来避免新旧版本中宏的重定义

#include <WinSock2.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")	// windows上的链接库，可以直接在设置中添加

int main() {
	// WORD是unsigned short, MAKEWORD()是将两个数字分别作为8bite拼接
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);	// 启动windows socket 2.x环境

	// ---------- 1.建立socket ----------

	// 网络协议为0, 自动匹配服务器协议
	SOCKET server_socket = INVALID_SOCKET;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket) printf("Socket creating failed.\n");
	else printf("Socket creating successful.\n");

	// ---------- 2.连接服务器 ----------

	sockaddr_in server_socket_addr_in = {};
	server_socket_addr_in.sin_family = AF_INET;
	server_socket_addr_in.sin_port = htons(12345);
	server_socket_addr_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(server_socket, (sockaddr*)&server_socket_addr_in, sizeof sockaddr_in)) printf("Connection to server failed.\n");
	else printf("Connection to server successful.\n");

	while (true) {
		// ---------- 3.向服务器发送消息 ----------
		char send_buffer[1024];
		scanf("%s", send_buffer);
		send(server_socket, send_buffer, strlen(send_buffer) + 1, 0);



		// ---------- 4.接受服务器的数据 ----------

		char recv_buffer[1024];
		int recv_len = recv(server_socket, recv_buffer, 1024, 0);
		if (recv_len > 0) {
			printf("Received server message: %s\n", recv_buffer);
			if (0 == strcmp(recv_buffer, "close")) break;
		}
	}

	// ---------- 5.关闭socket ----------

	closesocket(server_socket);
	WSACleanup();	// 关闭windows socket 2.x环境
	system("pause");
	return 0;
}