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

	// ����Э��Ϊ0, �Զ�ƥ�������Э��
	SOCKET server_socket = INVALID_SOCKET;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket) printf("Socket creating failed.\n");
	else printf("Socket creating successful.\n");

	// ---------- 2.���ӷ����� ----------

	sockaddr_in server_socket_addr_in = {};
	server_socket_addr_in.sin_family = AF_INET;
	server_socket_addr_in.sin_port = htons(12345);
	server_socket_addr_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(server_socket, (sockaddr*)&server_socket_addr_in, sizeof sockaddr_in)) printf("Connection to server failed.\n");
	else printf("Connection to server successful.\n");

	while (true) {
		// ---------- 3.�������������Ϣ ----------
		char send_buffer[1024];
		scanf("%s", send_buffer);
		send(server_socket, send_buffer, strlen(send_buffer) + 1, 0);



		// ---------- 4.���ܷ����������� ----------

		char recv_buffer[1024];
		int recv_len = recv(server_socket, recv_buffer, 1024, 0);
		if (recv_len > 0) {
			printf("Received server message: %s\n", recv_buffer);
			if (0 == strcmp(recv_buffer, "close")) break;
		}
	}

	// ---------- 5.�ر�socket ----------

	closesocket(server_socket);
	WSACleanup();	// �ر�windows socket 2.x����
	system("pause");
	return 0;
}