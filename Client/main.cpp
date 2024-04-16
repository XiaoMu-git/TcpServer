#define WIN32_LEAN_AND_MEAN	// 用来避免新旧版本中宏的重定义

#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include "DataPackage.h"

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
		Header send_header;
		UserInfo user_info;
		char cmd[32];
		scanf("%s", cmd);
		if (!strcmp(cmd, "quit")) {
			send_header.set(0, CMD_QUIT);
			send(server_socket, (const char*)&send_header, sizeof Header, 0);
		}
		else if (!strcmp(cmd, "login")) {
			char username[32], password[32];
			scanf("%s %s", username, password); 
			send_header.set(sizeof UserInfo, CMD_LOGIN);
			user_info.set(username, password);
			send(server_socket, (const char*)&send_header, sizeof Header, 0);
			send(server_socket, (const char*)&user_info, sizeof UserInfo, 0);
		}

		// ---------- 4.接受服务器的数据 ----------

		Header recv_header;
		int recv_len = recv(server_socket, (char*)&recv_header, sizeof Header, 0);
		if (0 >= recv_len) break;
		else {
			Result recv_result;
			switch (recv_header.cmd) {
			case CMD_ERROR:
				printf("Received error command.\n");
				break;
			case CMD_RESULT:
				recv(server_socket, (char*)&recv_result, sizeof Result, 0);
				if (CMD_LOGIN == send_header.cmd) {
					if (recv_result.result == true) printf("Login successful.\n");
					else printf("Login failed.\n");
				}
				else if (CMD_QUIT == send_header.cmd) {
					if (recv_result.result == true) printf("Quit successful.\n");
					else printf("Quit failed.\n");
				}
			}
		};
	}

	// ---------- 5.关闭socket ----------

	closesocket(server_socket);
	WSACleanup();	// 关闭windows socket 2.x环境
	system("pause");
	return 0;
}