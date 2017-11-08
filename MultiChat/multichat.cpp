/*
2017년 1학기 네트워크프로그래밍 숙제 2번 멀티채팅 프로그램
성명 : 박진원  
학번 : 111600
*/

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "resource.h"

#define BUFSIZE		512
#define NICKSIZE	16
#define IPSIZE		20
#define PORTSIZE	10
#define SERVERPORT	49151	// 1:1채팅 서버포트

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgNick(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 소켓 통신 스레드 함수
DWORD WINAPI Receiver(LPVOID arg);
DWORD WINAPI MultiChat(LPVOID arg);
DWORD WINAPI SingleChat(LPVOID arg);
DWORD WINAPI SingleReceiver(LPVOID arg);

// 기타 함수들
BOOL ip_check(char *);
BOOL port_check(char *);
BOOL call_check(char *);

SOCKET sock;	// 전송 구조체(개인 사용자 소켓)
SOCKADDR_IN remoteaddr, myaddr;
char buf[BUFSIZE + 1];	// 데이터 송수신 버퍼
char nick[NICKSIZE + 1], before[NICKSIZE +1];	// 닉네임
char multicastip[IPSIZE + 1];	// 멀티캐스트 ip
char portstr[PORTSIZE + 1];	// 포트번호 문자열용
int remoteport, myport;	// remoteport: 멀티캐스트 포트
int connectFlag;	// 닉네임을 변경해도 새로운 스레드가 생성되지 않도록

HINSTANCE hInst;	// 메인 다이어로그를 저장
HANDLE hReadEvent, hWriteEvent, hReadEvent2, hWriteEvent2;; // 이벤트
HANDLE sThread1, sThread2;	// 1:1 채팅 receiver, sender
HWND hDialog1;	// 메인 채팅화면
HWND hSendButton, hConnectButton; // 보내기 버튼
HWND hEdit1, hEdit2, hEdit3, hEdit4, hEdit5; // 편집 컨트롤
HWND hEdit6, hEdit7, hEdit8, hSendButton2;	// 1:1 채팅

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;	

	// 대화상자 생성
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// 이벤트 제거
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	hDialog1 = hDlg;
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);	// 대화명
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);	// 대화 입력창
		hEdit3 = GetDlgItem(hDlg, IDC_EDIT3);	// 대화 표출창
		hEdit4 = GetDlgItem(hDlg, IDC_EDIT4);	// 멀티캐스트 주소
		hEdit5 = GetDlgItem(hDlg, IDC_EDIT5);	// 포트번호

		hConnectButton = GetDlgItem(hDlg, IDCONNECT);	// 멀티캐스트-포트 접속
		hSendButton = GetDlgItem(hDlg, IDOK);	// 대화 전송

		EnableWindow(hEdit2, FALSE);
		EnableWindow(hSendButton, FALSE);

		SendMessage(hEdit1, EM_SETLIMITTEXT, NICKSIZE, 0);
		SendMessage(hEdit2, EM_SETLIMITTEXT, BUFSIZE, 0);
		SendMessage(hEdit4, EM_SETLIMITTEXT, IPSIZE, 0);
		SendMessage(hEdit5, EM_SETLIMITTEXT, PORTSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기

			GetDlgItemText(hDlg, IDC_EDIT1, nick, NICKSIZE + 1);
			GetDlgItemText(hDlg, IDC_EDIT2, buf, BUFSIZE + 1);

			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetFocus(hEdit2);
			SendMessage(hEdit2, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDCONNECT:
			GetDlgItemText(hDlg, IDC_EDIT4, multicastip, IPSIZE + 1);
			GetDlgItemText(hDlg, IDC_EDIT5, portstr, PORTSIZE + 1);

			// 멀티캐스트 주소, 포트번호 에러 확인
			if (!ip_check(multicastip)) {
				err_display("ip addr error()");
				return TRUE;
			}
			if (!port_check(portstr)) {
				err_display("port number error()");
				return TRUE;
			}
			// 포트번호 -> 정수로
			remoteport = atoi(portstr);
			EnableWindow(hConnectButton, FALSE);	// Connect버튼의 비활성화
			return TRUE;
		case IDJOIN:
			strcpy(before,nick);
			GetDlgItemText(hDlg, IDC_EDIT1, nick, NICKSIZE + 1);
			EnableWindow(hEdit2, TRUE);
			EnableWindow(hSendButton, TRUE);

			// 처음 join이 아닌 경우만 = 대화명 변경 시
			if (connectFlag == 1) {
				strcpy(buf, "Nickname changing : ");
				strcat(buf, before);
				strcat(buf, " -> ");
				strcat(buf, nick);
				if (sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
					err_display("sendto()");
					return TRUE;
				}
				if (sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
					err_display("sendto()");
					return TRUE;
				}
			}
			// 소켓 통신 스레드 생성 ,, 처음 접속시에만 스레드 생성
			else {
				HANDLE hThread;
				CreateThread(NULL, 0, MultiChat, NULL, 0, NULL);
				hThread = CreateThread(NULL, 0, Receiver, (LPVOID)sock, 0, NULL);
				if (hThread == NULL) { closesocket(sock); }
				else { CloseHandle(hThread); }
				++connectFlag;
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// 대화상자 프로시저
BOOL CALLBACK DlgWhisper(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit6 = GetDlgItem(hDlg, IDC_EDIT6);	// 대화 입력창
		hEdit7 = GetDlgItem(hDlg, IDC_EDIT7);	// 대화 출력창
		hEdit8 = GetDlgItem(hDlg, IDC_EDIT8);	// 대화명

		hSendButton2 = GetDlgItem(hDlg, IDOK);	// 전송 버튼

		SendMessage(hEdit6, EM_SETLIMITTEXT, BUFSIZE, 0);
		SendMessage(hEdit8, EM_SETLIMITTEXT, NICKSIZE, 0);
		SetDlgItemText(hDlg, IDC_EDIT8, nick);	// 기존 닉네임 가져오기
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK2:
			EnableWindow(hSendButton2, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent2, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_EDIT6, buf, BUFSIZE + 1);
			SetEvent(hWriteEvent2); // 쓰기 완료 알리기
			SetFocus(hEdit6);
			SendMessage(hEdit6, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL2:
			// 통신 종료
			TerminateThread(sThread1, TRUE);
			TerminateThread(sThread2, TRUE);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_CHANGE:
			strcpy(before, nick);
			GetDlgItemText(hDlg, IDC_EDIT8, nick, NICKSIZE + 1);

			// 닉네임 변경을 알림
			strcpy(buf, "Nickname changing : ");
			strcat(buf, before);
			strcat(buf, " -> ");
			strcat(buf, nick);
			if (sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
				err_display("sendto()");
				return TRUE;
			}
			if (sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
				err_display("sendto()");
				return TRUE;
			}
			// 메인 다이어로그 대화명 칸 갱신
			SetDlgItemText(hDialog1, IDC_EDIT1, nick);

			// 닉네임이 변경되면 통신 종료
			TerminateThread(sThread1, TRUE);
			TerminateThread(sThread2, TRUE);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit3);
	SendMessage(hEdit3, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit3, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}
// 1:1 채팅 출력 함수
void DisplayText2(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit7);
	SendMessage(hEdit7, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit7, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
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
	DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 메인 통신
DWORD WINAPI Receiver(LPVOID arg)
{
	int retval;
	time_t tv;
	struct tm *nowtime;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (recv_sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR 옵션 설정
	BOOL optval = TRUE;
	retval = setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// bind()
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(remoteport);
	retval = bind(recv_sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// 멀티캐스트 그룹 가입
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;
	char peernick[NICKSIZE + 1];
	int check;

	// 멀티캐스트 데이터 받기
	while (1) {
		addrlen = sizeof(peeraddr);
		tv = time(NULL);
		nowtime = localtime(&tv);
		check = 1;

		// 데이터 받기
		// 대화명
		retval = recvfrom(recv_sock, peernick, NICKSIZE + 1, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		peernick[retval] = '\0';
		// 대화 내용
		retval = recvfrom(recv_sock, buf, BUFSIZE + 1, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		buf[retval] = '\0';

		// 내 소켓 정보 얻기
		addrlen = sizeof(myaddr);
		getsockname(sock, (SOCKADDR *)&myaddr, &addrlen);
		myport = ntohs(myaddr.sin_port);

		// 받은 데이터 출력 ; 대화명 [ ip / 포트 ] (시:분:초) : 대화 내용
		DisplayText("%s [%s / %d](%d:%02d:%02d) : %s\r\n", peernick, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec, buf);

		// 1:1 대화 ( 시그널 수신 )
		if (call_check(buf)) {
			check = 0;

			hReadEvent2 = CreateEvent(NULL, FALSE, TRUE, NULL);
			if (hReadEvent == NULL) return 1;
			hWriteEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (hWriteEvent == NULL) return 1;

			// 1:1 채팅 스레드 생성
			sThread1 = CreateThread(NULL, 0, SingleReceiver, NULL, 0, NULL);
			sThread2 = CreateThread(NULL, 0, SingleChat, NULL, 0, NULL);
			if (sThread1 == NULL) {
				DisplayText("Single Receiver Error\r\n");
			}
			if (sThread2 == NULL) {
				DisplayText("Single Sender Error\r\n");
			}
			memset(buf, 0, strlen(buf));	// 버퍼 정리
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), NULL, DlgWhisper);
		}
		// 닉네임 같으면 1:1대화 ( 시그널 전송 )
		else if (strcmp(nick, peernick) == 0 && myport != ntohs(peeraddr.sin_port) && check == 1) {
			char reqtmp[20];
			char tmp[20];

			// 시그널 메세지 생성
			itoa(ntohs(peeraddr.sin_port), tmp, 10);
			strcpy(reqtmp, "CALL ");
			strcat(reqtmp, tmp);

			// 1:1 대화 시그널 (메시지로)
			if (sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
				err_display("sendto()");
				break;
			}
			if (sendto(sock, reqtmp, strlen(reqtmp), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
				err_display("sendto()");
				break;
			}

			hReadEvent2 = CreateEvent(NULL, FALSE, TRUE, NULL);
			if (hReadEvent == NULL) return 1;
			hWriteEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (hWriteEvent == NULL) return 1;

			// 1:1 채팅 스레드 생성
			sThread1 = CreateThread(NULL, 0, SingleReceiver, NULL, 0, NULL);
			sThread2 = CreateThread(NULL, 0, SingleChat, NULL, 0, NULL);
			if (sThread1 == NULL) {
				DisplayText("Single Receiver Error\r\n");
			}
			if (sThread2 == NULL) {
				DisplayText("Single Sender Error\r\n");
			}
			// 새로운 다이어로그
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), NULL, DlgWhisper);
		}	// 1:1 대화 if..else.. end

		SetEvent(hReadEvent); // 읽기 완료 알리기
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(recv_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

DWORD WINAPI MultiChat(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 멀티캐스트 TTL 설정
	int ttl = 2;
	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,	(char *)&ttl, sizeof(ttl));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(multicastip);
	remoteaddr.sin_port = htons(remoteport);
	
	// 입장을 알림
	strcpy(buf, "Entered the Chatting Room");
	if (sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
		err_display("sendto()");
		return TRUE;
	}
	if (sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
		err_display("sendto()");
		return TRUE;
	}

	// 멀티캐스트 데이터 보내기
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

		// 문자열 길이가 0이면 보내지 않음
		if (strlen(buf) == 0) {
			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			continue;
		}

		// 데이터 보내기
		// 대화명
		retval = sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		// 대화내용
		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
	}
	
	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}

// 1:1 통신
DWORD WINAPI SingleChat(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET singleSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (singleSock == INVALID_SOCKET) err_quit("socket()");

	// 멀티캐스트 TTL 설정
	int ttl = 2;
	retval = setsockopt(singleSock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(multicastip);
	remoteaddr.sin_port = htons(SERVERPORT);

	// 멀티캐스트 데이터 보내기
	while (1) {
		WaitForSingleObject(hWriteEvent2, INFINITE); // 쓰기 완료 기다리기

		// 문자열 길이가 0이면 보내지 않음
		if (strlen(buf) == 0) {
			EnableWindow(hSendButton2, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent2); // 읽기 완료 알리기
			continue;
		}

		// 데이터 보내기
		retval = sendto(singleSock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		EnableWindow(hSendButton2, TRUE); // 보내기 버튼 활성화
	}

	// closesocket()
	closesocket(singleSock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

DWORD WINAPI SingleReceiver(LPVOID arg) {
	int retval;
	time_t tv;
	struct tm *nowtime;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET singleRecv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (singleRecv_sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR 옵션 설정
	BOOL optval = TRUE;
	retval = setsockopt(singleRecv_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// bind()
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(SERVERPORT);
	retval = bind(singleRecv_sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// 멀티캐스트 그룹 가입
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(singleRecv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;

	// 멀티캐스트 데이터 받기
	while (1) {
		addrlen = sizeof(peeraddr);
		tv = time(NULL);
		nowtime = localtime(&tv);

		// 데이터 받기
		retval = recvfrom(singleRecv_sock, buf, BUFSIZE + 1, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		buf[retval] = '\0';

		// 받은 데이터 출력 ; [ ip / 포트 ] (시:분:초) : 대화 내용
		DisplayText2("[%s / %d](%d:%02d:%02d) : %s\r\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec, buf);

		SetEvent(hReadEvent2); // 읽기 완료 알리기
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(singleRecv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(singleRecv_sock);
	// 윈속 종료
	WSACleanup();
	return 0;
}

// 기타 함수들
BOOL ip_check(char *ip) {
	// 비어있는 경우
	if (strcmp(ip, "") == 0) return FALSE;
	
	// ip 주소의 형태를 가지었는지 체크
	int len = strlen(ip);
	if (len > 15 || len < 7) return FALSE;

	int nNumCount = 0;
	int nDotCount = 0;
	int i = 0, k = 0;
	char part[4][4];

	for (i = 0; i<len; i++)
	{
		if (ip[i] < '0' || ip[i] > '9')
		{
			if (ip[i] == '.')
			{
				part[k][i%4] = '\0';
				++k;
				++nDotCount;
				nNumCount = 0;
			}
			else
				return FALSE;
		}
		else
		{
			part[k][i] = ip[i%4];
			// 숫자는 세개씩
			if (++nNumCount > 3) return FALSE;
		}
	}

	// 점은 3개
	if (nDotCount != 3)	return FALSE;

	// ip --> part[0].part[1].part[2].part[3] 
	// Class D check --> (224.0.0.0 ~ 239.255.255.255)
	if (atoi(part[0]) < 224 || atoi(part[0]) > 239) return FALSE;
	for (i = 1; i < 4; i++)
		if (atoi(part[i]) < 0 || atoi(part[i]) > 255) return FALSE;

	return TRUE;
}

BOOL port_check(char *port) {
	/*
	49151번은 1:1 채팅용으로 사용
	1024 ~ 49150 까지의 포트번호 허용 (Registered Ports)
	*/
	int len = strlen(port);

	// 숫자가 아닌 경우
	for (int i = 0; i < len; i++)
		if (port[i] < '0' || port[i] >'9') return FALSE;

	// 범위를 벗어나는 경우
	int tmp = atoi(port);
	if (tmp < 1024 || tmp >= 49151) return FALSE;

	return TRUE;
}

BOOL call_check(char *buf) {
	// 나에 대한 시그널인지 확인
	char tmp[BUFSIZE + 1];
	char call[BUFSIZE + 1];

	strcpy(tmp, buf + 5);	// 포트 확인용
	strcpy(call, buf);	// 시그널 확인용
	call[4] = '\0';

	if (strcmp(call, "CALL") != 0) return FALSE;	// CALL 시그널인지 확인
	if (atoi(tmp) == myport) return TRUE;	// 나를 콜한건지 확인

	return FALSE;
}
