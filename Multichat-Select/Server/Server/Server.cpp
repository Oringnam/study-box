/*
2017�� 1�б� ��Ʈ��ũ���α׷��� ���� 3��
���� : ������
�й� : 111600
����
*/

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SERVERPORT	9000
#define BUFSIZE		512
#define NICKSIZE	16
#define MSGSIZE		BUFSIZE - 3 * sizeof(int) - 16

#define CHATTING    1000                  // �޽��� Ÿ��: ä��
#define JOINTEST	1001				  // �޽��� Ÿ��: ��ȭ�� Ȯ��, ���
#define REJECT		1002				  // �޽��� Ÿ��: ��ȭ�� �ź�
#define DISPLAY		1003				  // �޽��� Ÿ��: ����� ���÷���
#define WHISPER		1004				  // �޽��� Ÿ��: �ӼӸ�

// ���� ���� ������ ���� ����ü�� ����
struct SOCKETINFO
{
	SOCKET	sock;
	char	nick[NICKSIZE];
	char	buf[MSGSIZE];
	int		room;
	int		recvbytes;
	BOOL	isNew;
};

// ä�� �޽��� ����
// sizeof(CHAT_MSG) == 256
struct CHAT_MSG
{
	int		type;
	int		room;
	char	buf[MSGSIZE];
	char	whisp[16];	// �ӼӸ� ����� ��ȭ��
	int		peerRoom;
};

// ä�� ���� ���� ����
struct ROOM_INFO {
	int		type;
	int		count;
	char	nick[32][NICKSIZE];
};

int nTotalSockets = 0;
SOCKETINFO *SocketInfoArray[FD_SETSIZE];
CHAT_MSG chatmsg;
ROOM_INFO roomInfo[2];

// ���� ���� �Լ�
BOOL AddSocketInfo(SOCKET sock);
BOOL AddRoomInfo(SOCKETINFO *ptr);
void RemoveSocketInfo(int nIndex);
void RemoveRoomInfo(SOCKETINFO *ptr);
// �޽��� ����
void make_message(SOCKETINFO *);
void make_whisp(SOCKETINFO *);

// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	FD_SET rset;
	SOCKET client_sock;
	int addrlen, i, j;
	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;

	while (1) {
		// ���� �� �ʱ�ȭ
		FD_ZERO(&rset);
		FD_SET(listen_sock, &rset);
		for (i = 0; i < nTotalSockets; i++) {
			FD_SET(SocketInfoArray[i]->sock, &rset);
		}

		// select()
		retval = select(0, &rset, NULL, NULL, NULL);
		if (retval == SOCKET_ERROR) {
			err_display("select()");
			break;
		}

		// ���� �� �˻�(1): Ŭ���̾�Ʈ ���� ����
		if (FD_ISSET(listen_sock, &rset)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				// ������ Ŭ���̾�Ʈ ���� ���
				printf("Ŭ���̾�Ʈ ����: [%s]:%d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
				// ���� ���� �߰�
				AddSocketInfo(client_sock);
			}
		}

		// ���� �� �˻�(2): ������ ���
		for (i = 0; i < nTotalSockets; i++) {
			SOCKETINFO *ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock, &rset)) {
				// ������ �ޱ�
				retval = recv(ptr->sock, (char *)&chatmsg, BUFSIZE, 0);
				if (retval == 0 || retval == SOCKET_ERROR) {
					RemoveRoomInfo(ptr);
					RemoveSocketInfo(i);
					continue;
				}

				// �޽��� ���� Ÿ�Կ� ����
				// JOINTEST ; ����ڰ� �濡 ���� --> �ΰ����� �Է� (�� ��ȣ, ��ȭ��)
				if (chatmsg.type == JOINTEST) {
					BOOL check = TRUE;
					for (j = 0; j < nTotalSockets; j++) {
						SOCKETINFO *ptr2 = SocketInfoArray[j];
						if (ptr != ptr2 && strcmp(chatmsg.buf, ptr2->nick) == 0 && ptr2->room == chatmsg.room) {
							// �ź� �޽��� -> REJECT Ÿ������ ����
							check = FALSE;
							chatmsg.type = REJECT;
							retval = send(ptr->sock, (char *)&chatmsg, BUFSIZE, 0);
							break;
						}
					}
					if (check) {
						// �ű� ȸ���̶��
						if (ptr->isNew) {
							ptr->room = chatmsg.room;
							strcpy(ptr->nick, chatmsg.buf);
							// �� ����
							AddRoomInfo(ptr);
						}
						// ��ȭ�� ������ ���
						else {
							// ���� roomInfo ����
							RemoveRoomInfo(ptr);
							// ���� �߰�
							ptr->room = chatmsg.room;
							strcpy(ptr->nick, chatmsg.buf);
							AddRoomInfo(ptr);
						}
					}
				}
				// DISPLAY ; ä�ù��� ����ڵ� ǥ��
				else if (chatmsg.type == DISPLAY) {
					roomInfo[chatmsg.peerRoom].type = DISPLAY;
					if (send(ptr->sock, (char *)&roomInfo[chatmsg.peerRoom], BUFSIZE, 0) == SOCKET_ERROR) {
						err_display("send()");
					}
				}
				// �ӼӸ�
				else if (chatmsg.type == WHISPER) {
					for (j = 0; j < nTotalSockets; j++) {
						SOCKETINFO *ptr2 = SocketInfoArray[j];
						// �ӼӸ� ����� ��, �г��Ӱ� ���ٸ� = �ӼӸ� ����̶��
						if (ptr2->room == chatmsg.peerRoom && strcmp(ptr2->nick, chatmsg.whisp) == 0) {
							// �ӼӸ� �����
							make_whisp(ptr);
							// ������ ������
							if (send(ptr->sock, (char *)&chatmsg, BUFSIZE, 0) == SOCKET_ERROR) {
								err_display("send()");
							}
							// ���濡�� ������
							chatmsg.room = chatmsg.peerRoom;
							if (send(ptr2->sock, (char *)&chatmsg, BUFSIZE, 0) == SOCKET_ERROR) {
								err_display("send()");
							}
						}
					}
				}
				// CHATTING ; �Ϲ� �޽���
				else {
					strcpy(ptr->buf, chatmsg.buf);
					make_message(ptr);

					// ���� ����Ʈ �� ����
					ptr->recvbytes += retval;

					if (ptr->recvbytes == BUFSIZE) {
						// ���� ����Ʈ �� ����
						ptr->recvbytes = 0;

						// ���� ������ ��� Ŭ���̾�Ʈ���� �����͸� ����!
						for (j = 0; j < nTotalSockets; j++) {
							SOCKETINFO *ptr2 = SocketInfoArray[j];
							retval = send(ptr2->sock, (char *)&chatmsg, BUFSIZE, 0);
							if (retval == SOCKET_ERROR) {
								err_display("send()");
								RemoveRoomInfo(ptr2);
								RemoveSocketInfo(j);
								--j; // ���� �ε��� ����
								continue;
							}
						}
					}
				}	// �޽��� Ÿ�Կ� ���� if.. else
			}	// if sock�� �µǾ��� ��
		}	// ������ ���
	}	// while

	return 0;
}

// ���� ���� �߰�
BOOL AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= FD_SETSIZE) {
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return FALSE;
	}

	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->isNew = TRUE;
	SocketInfoArray[nTotalSockets++] = ptr;

	return TRUE;
}

// �� ���� �߰�
BOOL AddRoomInfo(SOCKETINFO *ptr) {
	int roomNum = ptr->room;

	if (roomInfo[roomNum].count >= 32) {
		printf("���� ���� á���ϴ�.\n");
		return FALSE;
	}

	// �г��� �߰�
	strcpy(roomInfo[roomNum].nick[(roomInfo[roomNum].count)++], ptr->nick);
	// ���� �ű�ȸ���� �ƴϴ�
	ptr->isNew = FALSE;

	return TRUE;
}

// ���� ���� ����
void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO *ptr = SocketInfoArray[nIndex];

	// ������ Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
	printf("Ŭ���̾�Ʈ ����: [%s]:%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->sock);
	delete ptr;

	if (nIndex != (nTotalSockets - 1))
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];

	--nTotalSockets;
}

void RemoveRoomInfo(SOCKETINFO *ptr) {
	int roomNum = ptr->room;
	int count = roomInfo[roomNum].count - 1;

	for (int i = 0; i < roomInfo[roomNum].count; i++) {
		if (strcmp(roomInfo[roomNum].nick[i], ptr->nick) == 0) {
			ZeroMemory(roomInfo[roomNum].nick[i], sizeof(roomInfo[roomNum].nick[i]));
			strcpy(roomInfo[roomNum].nick[i], roomInfo[roomNum].nick[count]);
			--roomInfo[roomNum].count;
			break;
		}
	}
}

// �޽��� ; �г��� + ��ȭ����
void make_message(SOCKETINFO *ptr) {
	char tmp[MSGSIZE];

	// ���� ; �г��� : ��ȭ����
	strcpy(tmp, ptr->nick);
	strcat(tmp, " : ");
	strcat(tmp, ptr->buf);
	strcpy(chatmsg.buf, tmp);
	chatmsg.room = ptr->room;
}

// �޽��� ; �ӼӸ�
void make_whisp(SOCKETINFO *ptr) {
	char tmp[MSGSIZE];
	char tmpr[2];

	// ���� ; �������(���ȣ)->�޴»��(���ȣ) : ��ȭ����
	strcpy(ptr->buf, chatmsg.buf);
	strcpy(tmp, ptr->nick);
	strcat(tmp, "(");
	itoa(ptr->room + 1, tmpr, 10);
	strcat(tmp, tmpr);
	strcat(tmp, ")");
	strcat(tmp, " -> ");
	strcat(tmp, chatmsg.whisp);
	strcat(tmp, "(");
	itoa(chatmsg.peerRoom + 1, tmpr, 10);
	strcat(tmp, tmpr);
	strcat(tmp, ")");
	strcat(tmp, " : ");
	strcat(tmp, ptr->buf);
	strcpy(chatmsg.buf, tmp);
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}