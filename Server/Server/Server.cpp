/*
2017년 1학기 네트워크프로그래밍 숙제 3번
성명 : 박진원
학번 : 111600
서버
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

#define CHATTING    1000                  // 메시지 타입: 채팅
#define JOINTEST	1001				  // 메시지 타입: 대화명 확인, 등록
#define REJECT		1002				  // 메시지 타입: 대화명 거부
#define DISPLAY		1003				  // 메시지 타입: 사용자 디스플레이
#define WHISPER		1004				  // 메시지 타입: 귓속말

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	SOCKET	sock;
	char	nick[NICKSIZE];
	char	buf[MSGSIZE];
	int		room;
	int		recvbytes;
	BOOL	isNew;
};

// 채팅 메시지 형식
// sizeof(CHAT_MSG) == 256
struct CHAT_MSG
{
	int		type;
	int		room;
	char	buf[MSGSIZE];
	char	whisp[16];	// 귓속말 대상의 대화명
	int		peerRoom;
};

// 채팅 방의 대한 정보
struct ROOM_INFO {
	int		type;
	int		count;
	char	nick[32][NICKSIZE];
};

int nTotalSockets = 0;
SOCKETINFO *SocketInfoArray[FD_SETSIZE];
CHAT_MSG chatmsg;
ROOM_INFO roomInfo[2];

// 소켓 관리 함수
BOOL AddSocketInfo(SOCKET sock);
BOOL AddRoomInfo(SOCKETINFO *ptr);
void RemoveSocketInfo(int nIndex);
void RemoveRoomInfo(SOCKETINFO *ptr);
// 메시지 관련
void make_message(SOCKETINFO *);
void make_whisp(SOCKETINFO *);

// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
	FD_SET rset;
	SOCKET client_sock;
	int addrlen, i, j;
	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;

	while (1) {
		// 소켓 셋 초기화
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

		// 소켓 셋 검사(1): 클라이언트 접속 수용
		if (FD_ISSET(listen_sock, &rset)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				// 접속한 클라이언트 정보 출력
				printf("클라이언트 접속: [%s]:%d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
				// 소켓 정보 추가
				AddSocketInfo(client_sock);
			}
		}

		// 소켓 셋 검사(2): 데이터 통신
		for (i = 0; i < nTotalSockets; i++) {
			SOCKETINFO *ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock, &rset)) {
				// 데이터 받기
				retval = recv(ptr->sock, (char *)&chatmsg, BUFSIZE, 0);
				if (retval == 0 || retval == SOCKET_ERROR) {
					RemoveRoomInfo(ptr);
					RemoveSocketInfo(i);
					continue;
				}

				// 메시지 수신 타입에 따라
				// JOINTEST ; 사용자가 방에 입장 --> 부가정보 입력 (방 번호, 대화명)
				if (chatmsg.type == JOINTEST) {
					BOOL check = TRUE;
					for (j = 0; j < nTotalSockets; j++) {
						SOCKETINFO *ptr2 = SocketInfoArray[j];
						if (ptr != ptr2 && strcmp(chatmsg.buf, ptr2->nick) == 0 && ptr2->room == chatmsg.room) {
							// 거부 메시지 -> REJECT 타입으로 보냄
							check = FALSE;
							chatmsg.type = REJECT;
							retval = send(ptr->sock, (char *)&chatmsg, BUFSIZE, 0);
							break;
						}
					}
					if (check) {
						// 신규 회원이라면
						if (ptr->isNew) {
							ptr->room = chatmsg.room;
							strcpy(ptr->nick, chatmsg.buf);
							// 방 관리
							AddRoomInfo(ptr);
						}
						// 대화명 변경의 경우
						else {
							// 기존 roomInfo 제거
							RemoveRoomInfo(ptr);
							// 새로 추가
							ptr->room = chatmsg.room;
							strcpy(ptr->nick, chatmsg.buf);
							AddRoomInfo(ptr);
						}
					}
				}
				// DISPLAY ; 채팅방의 사용자들 표시
				else if (chatmsg.type == DISPLAY) {
					roomInfo[chatmsg.peerRoom].type = DISPLAY;
					if (send(ptr->sock, (char *)&roomInfo[chatmsg.peerRoom], BUFSIZE, 0) == SOCKET_ERROR) {
						err_display("send()");
					}
				}
				// 귓속말
				else if (chatmsg.type == WHISPER) {
					for (j = 0; j < nTotalSockets; j++) {
						SOCKETINFO *ptr2 = SocketInfoArray[j];
						// 귓속말 대상의 방, 닉네임과 같다면 = 귓속말 대상이라면
						if (ptr2->room == chatmsg.peerRoom && strcmp(ptr2->nick, chatmsg.whisp) == 0) {
							// 귓속말 만들기
							make_whisp(ptr);
							// 나한테 보내기
							if (send(ptr->sock, (char *)&chatmsg, BUFSIZE, 0) == SOCKET_ERROR) {
								err_display("send()");
							}
							// 상대방에게 보내기
							chatmsg.room = chatmsg.peerRoom;
							if (send(ptr2->sock, (char *)&chatmsg, BUFSIZE, 0) == SOCKET_ERROR) {
								err_display("send()");
							}
						}
					}
				}
				// CHATTING ; 일반 메시지
				else {
					strcpy(ptr->buf, chatmsg.buf);
					make_message(ptr);

					// 받은 바이트 수 누적
					ptr->recvbytes += retval;

					if (ptr->recvbytes == BUFSIZE) {
						// 받은 바이트 수 리셋
						ptr->recvbytes = 0;

						// 현재 접속한 모든 클라이언트에게 데이터를 보냄!
						for (j = 0; j < nTotalSockets; j++) {
							SOCKETINFO *ptr2 = SocketInfoArray[j];
							retval = send(ptr2->sock, (char *)&chatmsg, BUFSIZE, 0);
							if (retval == SOCKET_ERROR) {
								err_display("send()");
								RemoveRoomInfo(ptr2);
								RemoveSocketInfo(j);
								--j; // 루프 인덱스 보정
								continue;
							}
						}
					}
				}	// 메시지 타입에 따른 if.. else
			}	// if sock이 셋되었을 때
		}	// 데이터 통신
	}	// while

	return 0;
}

// 소켓 정보 추가
BOOL AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= FD_SETSIZE) {
		printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
		return FALSE;
	}

	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->isNew = TRUE;
	SocketInfoArray[nTotalSockets++] = ptr;

	return TRUE;
}

// 방 정보 추가
BOOL AddRoomInfo(SOCKETINFO *ptr) {
	int roomNum = ptr->room;

	if (roomInfo[roomNum].count >= 32) {
		printf("방이 가득 찼습니다.\n");
		return FALSE;
	}

	// 닉네임 추가
	strcpy(roomInfo[roomNum].nick[(roomInfo[roomNum].count)++], ptr->nick);
	// 이제 신규회원이 아니다
	ptr->isNew = FALSE;

	return TRUE;
}

// 소켓 정보 삭제
void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO *ptr = SocketInfoArray[nIndex];

	// 종료한 클라이언트 정보 출력
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
	printf("클라이언트 종료: [%s]:%d\n",
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

// 메시지 ; 닉네임 + 대화내용
void make_message(SOCKETINFO *ptr) {
	char tmp[MSGSIZE];

	// 형태 ; 닉네임 : 대화내용
	strcpy(tmp, ptr->nick);
	strcat(tmp, " : ");
	strcat(tmp, ptr->buf);
	strcpy(chatmsg.buf, tmp);
	chatmsg.room = ptr->room;
}

// 메시지 ; 귓속말
void make_whisp(SOCKETINFO *ptr) {
	char tmp[MSGSIZE];
	char tmpr[2];

	// 형태 ; 보낸사람(방번호)->받는사람(방번호) : 대화내용
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

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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