#define WIN32_LEAN_AND_MEAN	// ���������¾ɰ汾�к���ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <iostream>

#include "DataPackage.h"

#pragma comment(lib, "ws2_32.lib")	// windows�ϵ����ӿ⣬����ֱ�������������

int main() {
	// WORD��unsigned short, MAKEWORD()�ǽ��������ֱַ���Ϊ8biteƴ��
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);	// ����windows socket 2.x����
	// ---------- 1.����socket ----------

	// AF_INET: ipv4
	// SOCK_STREAM: ����������
	// IPPROTO_TCP: TCPЭ��
	SOCKET server_socket = INVALID_SOCKET;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket) printf("Socket creating failed.\n");
	else printf("Socket creating successful.\n");

	// ---------- 2.������˿� ----------

	// sockaddr��������д����ʹ��sockaddr_in��ǿ��ת��
	sockaddr_in server_socket_addr_in = {};
	server_socket_addr_in.sin_family = AF_INET;
	// �����е����������������Ĳ�ͬ����Ҫת��
	server_socket_addr_in.sin_port = htons(12345);
	server_socket_addr_in.sin_addr.S_un.S_addr = INADDR_ANY;	// INADDR_ANY����ip, inet_addr("127.0.0.1")ָ��ip
	// �󶨵Ķ˿ڿ��ܱ�ռ��
	if (SOCKET_ERROR == bind(server_socket, (sockaddr*)&server_socket_addr_in, sizeof sockaddr_in)) printf("Port binding failed.\n");
	else printf("Port binding successful.\n");

	// ---------- 3.��������˿� ----------

	// 5�ǵȴ����ӵ��������
	if (SOCKET_ERROR == listen(server_socket, 5)) printf("Listen failed.\n");
	else printf("Listen seccessful.\n");

	// ---------- 4.�ȴ�Client���� ----------

	sockaddr_in client_addr_in = {};
	int client_addr_len = sizeof client_addr_in;
	// ��ʼ��Ϊ��Чsocket
	SOCKET client_socket = INVALID_SOCKET;
	client_socket = accept(server_socket, (sockaddr*)&client_addr_in, &client_addr_len);
	if (INVALID_SOCKET == client_socket) printf("Accept to invalid client.\n");
	else printf("New client join, ip: %s\n", inet_ntoa(client_addr_in.sin_addr));
	while (true) {
		// ---------- 5.����Client��Ϣ ----------

		Header recv_header;
		int recv_len = recv(client_socket, (char*)&recv_header, sizeof Header, 0);
		// �����Ѿ��Ͽ�����
		if (0 >= recv_len) break;
		else {
			// ---------- 6.���ظ�Client��Ϣ ----------

			UserInfo user_info;
			Header send_header;
			Result send_message;
			if (CMD_ERROR == recv_header.cmd) {
				printf("Received error command.\n");
				send_header.set(0, CMD_ERROR);
				send(client_socket, (const char*)&send_header, sizeof Header, 0);
			}
			else if (CMD_LOGIN == recv_header.cmd) {
				printf("Received login command.\n");
				send_header.set(sizeof Result, CMD_RESULT);
				recv(client_socket, (char*)&user_info, sizeof UserInfo, 0);
				if (!strcmp(user_info.username, "xiaomu") && !strcmp(user_info.password, "123456")) send_message.set(1, "");
				else send_message.set(0, "");
				send(client_socket, (const char*)&send_header, sizeof Header, 0);
				send(client_socket, (const char*)&send_message, sizeof Result, 0);
			}
			else if (CMD_QUIT == recv_header.cmd) {
				printf("Received quit command.\n");
				send_message.set(1, "");
				send(client_socket, (const char*)&send_message, sizeof Result, 0);
				break;
			}
		}
	}

	// ---------- 7.�ر�socket ----------

	closesocket(server_socket);
	WSACleanup();	// �ر�windows socket 2.x����
	system("pause");
	return 0;
}