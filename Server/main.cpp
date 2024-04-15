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
		char recv_buffer[1024];
		int recv_len = recv(client_socket, recv_buffer, 1024, 0);
		// 可能已经断开连接
		if (0 >= recv_len) break;
		else {
			// ---------- 6.发送给Client消息 ----------

			if (0 == strcmp(recv_buffer, "close")) {
				char send_message[] = "close";
				// strlen(send_message) + 1是将结尾的/0一并发送
				send(client_socket, send_message, strlen(send_message) + 1, 0);
			}
			else {
				char send_message[] = "Invalid command.";
				// strlen(send_message) + 1是将结尾的/0一并发送
				send(client_socket, send_message, strlen(send_message) + 1, 0);
			}
		}
	}

	// ---------- 7.关闭socket ----------

	closesocket(server_socket);
	WSACleanup();	// 关闭windows socket 2.x环境
	system("pause");
	return 0;
}