#define WIN32_LEAN_AND_MEAN	// 用来避免新旧版本中宏的重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
	// ---------- 1.创建socket ----------

	// AF_INET: ipv4
	// SOCK_STREAM: 数据流类型
	// IPPROTO_TCP: TCP协议
	SOCKET server_socket = INVALID_SOCKET;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket) printf("Socket creating failed.\n");
	else printf("Socket creating successful.\n");

	// ---------- 2.绑定网络端口 ----------

	// sockaddr不利于填写所以使用sockaddr_in来强制转换
	sockaddr_in server_socket_addr_in = {};
	server_socket_addr_in.sin_family = AF_INET;
	// 网络中的数据类型与计算机的不同，需要转化
	server_socket_addr_in.sin_port = htons(12345);
	server_socket_addr_in.sin_addr.S_un.S_addr = INADDR_ANY;	// INADDR_ANY任意ip, inet_addr("127.0.0.1")指定ip
	// 绑定的端口可能被占用
	if (SOCKET_ERROR == bind(server_socket, (sockaddr*)&server_socket_addr_in, sizeof sockaddr_in)) printf("Port binding failed.\n");
	else printf("Port binding successful.\n");

	// ---------- 3.监听网络端口 ----------

	// 5是等待链接的最大数量
	if (SOCKET_ERROR == listen(server_socket, 5)) printf("Listen failed.\n");
	else printf("Listen seccessful.\n");

	// ---------- 4.等待Client连接 ----------

	sockaddr_in client_addr_in = {};
	int client_addr_len = sizeof client_addr_in;
	// 初始化为无效socket
	SOCKET client_socket = INVALID_SOCKET;
	client_socket = accept(server_socket, (sockaddr*)&client_addr_in, &client_addr_len);
	if (INVALID_SOCKET == client_socket) printf("Accept to invalid client.\n");
	else printf("New client join, ip: %s\n", inet_ntoa(client_addr_in.sin_addr));
	while (true) {
		// ---------- 5.接收Client消息 ----------

		Header recv_header;
		int recv_len = recv(client_socket, (char*)&recv_header, sizeof Header, 0);
		// 可能已经断开连接
		if (0 >= recv_len) {
			printf("Client disconnected, server shutdown.\n");
			break;
		}
		else {
			// ---------- 6.返回给Client消息 ----------

			
			Command recv_command;
			if (DATA_LOGIN == recv_header.type) {
				// 处理客户端登录请求
				printf("Received a message of type DATA_LOGIN with a length of %d from %d.\n", recv_header.size, client_socket);
				UserInfo recv_user_info;
				recv(client_socket, (char*)&recv_user_info + sizeof Header, recv_user_info.size - sizeof Header, 0);
				if (!strcmp(recv_user_info.password, "123456")) {
					// 登录成功
					Response send_response;
					send_response.type = DATA_LOGIN_RESPONSE;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
				}
				else {
					// 登陆失败
					Response send_response;
					send_response.type = DATA_LOGIN_RESPONSE;
					// 大于0的相应均为异常, 状态为1原因是密码错误
					send_response.state = 1;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
				}
			}
			else if (DATA_LOGOUT == recv_header.type) {
				// 处理客户端登出请求
				printf("Received a message of type DATA_LOGOUT with a length of %d from %d.\n", recv_header.size, client_socket);
				Response send_response;
				send_response.type = DATA_LOGOUT_RESPONSE;
				send(client_socket, (const char*)&send_response, send_response.size, 0);
			}
			else if (DATA_COMMAND) {
				// 处理客户端发来的命令
				printf("Received a message of type DATA_COMMAND with a length of %d from %d.\n", recv_header.size, client_socket);
				Command recv_command;
				recv(client_socket, (char*)&recv_command + sizeof Header, recv_command.size - sizeof Header, 0);
				if (!strcmp(recv_command.command, "close")) {
					// 关闭服务端
					Response send_response;
					send_response.type = DATA_COMMAND_RESPONSE;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
					break;
				}
				else {
					// 接收到未知命令
					Response send_response;
					send_response.type = DATA_COMMAND_RESPONSE;
					send_response.state = 1;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
				}
			}
			else if (DATA_ERROR == recv_header.type) {
				printf("Received a message of type DATA_ERROR with a length of %d from %d.\n", recv_header.size, client_socket);
			}
			else printf("Received invalid message from %d.\n", client_socket);
		}
	}

	// ---------- 7.关闭socket ----------

	closesocket(server_socket);
	WSACleanup();	// 关闭windows socket 2.x环境
	system("pause");
	return 0;
}