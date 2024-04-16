#define WIN32_LEAN_AND_MEAN	// 用来避免新旧版本中宏的重定义

#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include "DataType.h"

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

		char command[32];
		scanf("%s", command);
		if (!strcmp(command, "login")) {
			// 登录请求
			UserInfo send_user_info;
			send_user_info.type = DATA_LOGIN;
			scanf("%s %s", send_user_info.username, send_user_info.password);
			send(server_socket, (const char*)&send_user_info, send_user_info.size, 0);
		}
		else {
			Command send_command;
			strcpy(send_command.command, command);
			send(server_socket, (const char*)&send_command, send_command.size, 0);
		}

		// ---------- 4.接受服务器的数据 ----------

		Header recv_header;
		int recv_len = recv(server_socket, (char*)&recv_header, sizeof Header, 0);
		if (0 >= recv_len) break;
		else {
			if (DATA_LOGIN_RESPONSE == recv_header.type) {
				// 处理登录请求的响应
				Response recv_response;
				recv(server_socket, (char*)&recv_response + sizeof Header, recv_response.size - sizeof Header, 0);
				if (!recv_response.state) printf("Login successed.\n");
				else printf("Login failed.\n");
			}
			else if (DATA_LOGOUT_RESPONSE == recv_header.type) {
				// 处理登出请求的响应
				Response recv_response;
				recv(server_socket, (char*)&recv_response + sizeof Header, recv_response.size - sizeof Header, 0);
				if (!recv_response.state) printf("Logout successful.\n");
				else printf("Logout failed.\n");
			}
			else if (DATA_COMMAND_RESPONSE == recv_header.type) {
				// 处理命令的响应
				Response recv_response;
				recv(server_socket, (char*)&recv_response + sizeof Header, recv_response.size - sizeof Header, 0);
				if (!recv_response.state) printf("Command execution successful.\n");
				else printf("Command execution failed.\n");
			}
			else if (DATA_ERROR == recv_header.type) {
				printf("Received a message of type DATA_ERROR with a length of %d from server.\n", recv_header.size);
			}
			else printf("Received invalid message from server.\n");
		};
	}

	// ---------- 5.关闭socket ----------

	closesocket(server_socket);
	WSACleanup();	// 关闭windows socket 2.x环境
	system("pause");
	return 0;
}