#define WIN32_LEAN_AND_MEAN	// ���������¾ɰ汾�к���ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <iostream>

#include "DataType.h"

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
		if (0 >= recv_len) {
			printf("Client disconnected, server shutdown.\n");
			break;
		}
		else {
			// ---------- 6.���ظ�Client��Ϣ ----------

			
			Command recv_command;
			if (DATA_LOGIN == recv_header.type) {
				// ����ͻ��˵�¼����
				printf("Received a message of type DATA_LOGIN with a length of %d from %d.\n", recv_header.size, client_socket);
				UserInfo recv_user_info;
				recv(client_socket, (char*)&recv_user_info + sizeof Header, recv_user_info.size - sizeof Header, 0);
				if (!strcmp(recv_user_info.password, "123456")) {
					// ��¼�ɹ�
					Response send_response;
					send_response.type = DATA_LOGIN_RESPONSE;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
				}
				else {
					// ��½ʧ��
					Response send_response;
					send_response.type = DATA_LOGIN_RESPONSE;
					// ����0����Ӧ��Ϊ�쳣, ״̬Ϊ1ԭ�����������
					send_response.state = 1;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
				}
			}
			else if (DATA_LOGOUT == recv_header.type) {
				// ����ͻ��˵ǳ�����
				printf("Received a message of type DATA_LOGOUT with a length of %d from %d.\n", recv_header.size, client_socket);
				Response send_response;
				send_response.type = DATA_LOGOUT_RESPONSE;
				send(client_socket, (const char*)&send_response, send_response.size, 0);
			}
			else if (DATA_COMMAND) {
				// ����ͻ��˷���������
				printf("Received a message of type DATA_COMMAND with a length of %d from %d.\n", recv_header.size, client_socket);
				Command recv_command;
				recv(client_socket, (char*)&recv_command + sizeof Header, recv_command.size - sizeof Header, 0);
				if (!strcmp(recv_command.command, "close")) {
					// �رշ����
					Response send_response;
					send_response.type = DATA_COMMAND_RESPONSE;
					send(client_socket, (const char*)&send_response, send_response.size, 0);
					break;
				}
				else {
					// ���յ�δ֪����
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

	// ---------- 7.�ر�socket ----------

	closesocket(server_socket);
	WSACleanup();	// �ر�windows socket 2.x����
	system("pause");
	return 0;
}