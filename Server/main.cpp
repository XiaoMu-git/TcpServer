#define WIN32_LEAN_AND_MEAN	// ���������¾ɰ汾�к���ض���

#include <WinSock2.h>
#include <windows.h>
#include <iostream>

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
		char recv_buffer[1024];
		int recv_len = recv(client_socket, recv_buffer, 1024, 0);
		// �����Ѿ��Ͽ�����
		if (0 >= recv_len) break;
		else {
			// ---------- 6.���͸�Client��Ϣ ----------

			if (0 == strcmp(recv_buffer, "close")) {
				char send_message[] = "close";
				// strlen(send_message) + 1�ǽ���β��/0һ������
				send(client_socket, send_message, strlen(send_message) + 1, 0);
			}
			else {
				char send_message[] = "Invalid command.";
				// strlen(send_message) + 1�ǽ���β��/0һ������
				send(client_socket, send_message, strlen(send_message) + 1, 0);
			}
		}
	}

	// ---------- 7.�ر�socket ----------

	closesocket(server_socket);
	WSACleanup();	// �ر�windows socket 2.x����
	system("pause");
	return 0;
}