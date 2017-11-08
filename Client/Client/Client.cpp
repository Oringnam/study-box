/*
2017년 1학기 네트워크프로그래밍 숙제 3번
성명 : 박진원
학번 : 111600
클라이언트
*/
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

/**/
#define SERVERIP	"127.0.0.1"
#define SERVERPORT  9000

#define BUFSIZE     512							 // 전송 메시지 전체 크기
#define MSGSIZE     (BUFSIZE-3*sizeof(int)-16)	// 채팅 메시지 최대 길이

#define CHATTING    1000                  // 메시지 타입: 채팅
#define JOINTEST	1001				  // 메시지 타입: 대화명 확인, 등록
#define REJECT		1002				  // 메시지 타입: 대화명 거부
#define DISPLAY		1003				  // 메시지 타입: 사용자 디스플레이
#define WHISPER		1004				  // 메시지 타입: 귓속말

// 공통 메시지 형식
// sizeof(COMM_MSG) == 256
struct COMM_MSG
{
	int  type;
	char dummy[BUFSIZE - sizeof(int)];
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
	char	nick[32][16];
};

// 변수들
static HINSTANCE     hInst; // 응용 프로그램 인스턴스 핸들
static HWND          hButton_SendMsg; // '메시지 전송' 버튼
static HWND          hEdit_Status; // 받은 메시지 출력
static HWND			 hEdit_Nick;	// 닉네임 관리
static HWND			 hEdit_Room1;	// 방1
static HWND			 hEdit_Room2;	// 방2
static HWND			 hMsgBox;		// 메시지박스
static HWND			 hEdit_Display;	// 사용자 디스플레이

static char          ipaddr[64]; // 서버 IP 주소
static char			 whisper[16]; // 귓속말 대상의 대화명
static u_short       port; // 서버 포트 번호
static u_short		 room;	// 현재 나의 방
static u_short		 view;	// 내가 보고있는 방의 이름(사용자 뷰)
static HANDLE        hClientThread; // 스레드 핸들
static volatile BOOL bStart; // 통신 시작 여부
static SOCKET        sock; // 클라이언트 소켓
static HANDLE        hReadEvent, hWriteEvent; // 이벤트 핸들
static CHAT_MSG      chatmsg; // 채팅 메시지 저장

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI ReadThread(LPVOID arg);
DWORD WINAPI WriteThread(LPVOID arg);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
void DisplayUser(ROOM_INFO *);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 기타 함수들
BOOL ip_check(char *);
BOOL port_check(char *);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// 변수 초기화
	chatmsg.type = CHATTING;

	// 대화상자 생성
	hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_LOGIN), NULL, DlgProc);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit_IP;				// IP주소 입력 칸
	static HWND hEdit_Port;				// Port 번호 입력 칸
	static HWND hButton_Connect;		// 접속 버튼
	static HWND hButton_Join;			// 입장 버튼
	static HWND hEdit_Msg;				// 메시지 입력 칸
	static HWND hEdit_Combo;			// 콤보박스

	static char portstr[11];			// 포트 번호 문자열
	CHAT_MSG systemmsg;					// 시스템 메시지
	char item_room[][10] = {"1번 방", "2번 방"};
	int i;

	switch (uMsg) {
	case WM_INITDIALOG:

		// ** 컨트롤 핸들 얻기
		hMsgBox = hDlg;
		// 접속
		hEdit_IP = GetDlgItem(hDlg, IDC_IP);
		hEdit_Port = GetDlgItem(hDlg, IDC_PORT);
		hButton_Connect = GetDlgItem(hDlg, IDCONNECT);
		// 입장
		hEdit_Nick = GetDlgItem(hDlg, IDC_NICK);
		hEdit_Room1 = GetDlgItem(hDlg, IDC_ROOM1);
		hEdit_Room2 = GetDlgItem(hDlg, IDC_ROOM2);
		hButton_Join = GetDlgItem(hDlg, IDJOIN);
		// 메시지 관련
		hEdit_Status = GetDlgItem(hDlg, IDC_DISPLAY);
		hButton_SendMsg = GetDlgItem(hDlg, IDSEND);
		hEdit_Msg = GetDlgItem(hDlg, IDC_MSG);
		// 접속 유저창
		hEdit_Combo = GetDlgItem(hDlg, IDC_COMBO);
		hEdit_Display = GetDlgItem(hDlg, IDC_LIST);

		// 컨트롤 초기화
		SendMessage(hEdit_Msg, EM_SETLIMITTEXT, MSGSIZE, 0);
		EnableWindow(hButton_SendMsg, FALSE);
		EnableWindow(hButton_Join, FALSE);
		SetDlgItemText(hDlg, IDC_IP, SERVERIP);
		SetDlgItemInt(hDlg, IDC_PORT, SERVERPORT, FALSE);
		SendMessage(hEdit_Room1, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(hEdit_Room2, BM_SETCHECK, BST_UNCHECKED, 0);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCONNECT:
			GetDlgItemText(hDlg, IDC_IP, ipaddr, sizeof(ipaddr));
			GetDlgItemText(hDlg, IDC_PORT, portstr, sizeof(portstr));

			// 멀티캐스트 주소, 포트번호 에러 확인
			if (!ip_check(ipaddr)) {
				MessageBox(hDlg, "IP addr error()","다시!", MB_ICONINFORMATION);
				return TRUE;
			}
			if (!port_check(portstr)) {
				MessageBox(hDlg, "Port number error()", "다시!", MB_ICONINFORMATION);
				return TRUE;
			}

			port = atoi(portstr);
			EnableWindow(hEdit_IP, FALSE);
			EnableWindow(hEdit_Port, FALSE);
			EnableWindow(hButton_Connect, FALSE);

			// 통신 클라이언트 생성
			hClientThread = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
			if (hClientThread == NULL) {
				MessageBox(hDlg, "클라이언트를 시작할 수 없습니다."
					"\r\n프로그램을 종료합니다.", "실패!", MB_ICONERROR);
				EndDialog(hDlg, 0);
			}

			while (bStart == FALSE); // 서버 접속 성공 기다림
			EnableWindow(hButton_Join, TRUE);	// 방 입장 버튼 활성화

			// 콤보박스 활성화 --> 채팅방의 사용자들 확인 가능
			for (i = 0; i<2; i++)
				SendMessage(hEdit_Combo, CB_ADDSTRING, 0, (LPARAM)item_room[i]);

			return TRUE;

		case IDC_COMBO:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				systemmsg.type = DISPLAY;
				// 현재 내가 (사용자 정보를) 보고 있는 방의 번호
				view = SendMessage(hEdit_Combo, CB_GETCURSEL, 0, 0);
				systemmsg.peerRoom = view;
				if (send(sock, (char *)&systemmsg, BUFSIZE, 0) == SOCKET_ERROR) {
					return TRUE;
				}
				return TRUE;
			}
			return TRUE;

		case IDC_LIST:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				// 리스트 하나 선택시
				i = SendMessage(hEdit_Display, LB_GETCURSEL, 0, 0);
				SendMessage(hEdit_Display, LB_GETTEXT, i, (LPARAM)whisper);
				return TRUE;
			}
			return TRUE;

		case IDC_WHISP:
			// 귓속말 설정
			chatmsg.type = WHISPER;
			chatmsg.peerRoom = view;
			strcpy(chatmsg.whisp, whisper);
			return TRUE;

		case IDC_ALL:
			// 전체말 설정
			chatmsg.type = CHATTING;

			return TRUE;

		case IDC_ROOM1:
			room = 0;
			return TRUE;

		case IDC_ROOM2:
			room = 1;
			return TRUE;

		case IDJOIN:
			// 대화명 확인, 등록하기 위한 절차
			systemmsg.type = JOINTEST;
			systemmsg.room = room;
			GetDlgItemText(hDlg, IDC_NICK, systemmsg.buf, MSGSIZE);
			// 데이터 보내기			
			if (send(sock, (char *)&systemmsg, BUFSIZE, 0) == SOCKET_ERROR) {
				return TRUE;
			}

			EnableWindow(hButton_SendMsg, TRUE);
			SetFocus(hEdit_Msg);
			
			return TRUE;

		case IDSEND:
			// 읽기 완료를 기다림
			WaitForSingleObject(hReadEvent, INFINITE);
			GetDlgItemText(hDlg, IDC_MSG, chatmsg.buf, MSGSIZE);
			// 쓰기 완료를 알림
			SetEvent(hWriteEvent);
			// 입력된 텍스트 전체를 선택 표시
			SendMessage(hEdit_Msg, EM_SETSEL, 0, -1);
			return TRUE;

		case IDCANCEL:
			if (MessageBox(hDlg, "정말로 종료하시겠습니까?",
				"질문", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				closesocket(sock);
				EndDialog(hDlg, IDCANCEL);
			}
			return TRUE;
		}	// command switch end
		return FALSE;
	}	// msg switch end

	return FALSE;
}	// DlgProc()

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons(port);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	MessageBox(NULL, "서버에 접속했습니다.", "성공!", MB_ICONINFORMATION);

	// 읽기 & 쓰기 스레드 생성
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	if (hThread[0] == NULL || hThread[1] == NULL) {
		MessageBox(NULL, "스레드를 시작할 수 없습니다."
			"\r\n프로그램을 종료합니다.",
			"실패!", MB_ICONERROR);
		exit(1);
	}

	// 통신 시작 여부
	bStart = TRUE;

	// 스레드 종료 대기
	retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	retval -= WAIT_OBJECT_0;
	if (retval == 0)
		TerminateThread(hThread[1], 1);
	else
		TerminateThread(hThread[0], 1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	// 통신 종료
	bStart = FALSE;

	MessageBox(NULL, "서버가 접속을 끊었습니다", "알림", MB_ICONINFORMATION);
	EnableWindow(hButton_SendMsg, FALSE);

	closesocket(sock);
	return 0;
}

// 데이터 받기
DWORD WINAPI ReadThread(LPVOID arg)
{
	int retval;
	COMM_MSG comm_msg;
	CHAT_MSG *chat_msg;
	ROOM_INFO *roomInfo;

	while (1) {
		retval = recvn(sock, (char *)&comm_msg, BUFSIZE, 0);
		if (retval == 0 || retval == SOCKET_ERROR) {
			break;
		}

		// 메시지 수신 타입에 따라
		// REJECT ; 닉네임 다시 입력
		if (comm_msg.type == REJECT) {
			MessageBox(hMsgBox, "이미 존재하는 대화명입니다.", "다시!", MB_ICONINFORMATION);
			EnableWindow(hButton_SendMsg, FALSE);
			SetFocus(hEdit_Nick);
		}
		// CHATTING ; 일반 메시지
		else if (comm_msg.type == CHATTING) {
			chat_msg = (CHAT_MSG *)&comm_msg;
			if (chat_msg->room == room) {
				DisplayText("[전체] %s\r\n", chat_msg->buf);
			}
		}
		// DISPLAY ; 채팅방 사용자 표출
		else if (comm_msg.type == DISPLAY) {
			roomInfo = (ROOM_INFO *)&comm_msg;
			DisplayUser(roomInfo);
		}
		// 귓속말
		else if (comm_msg.type == WHISPER) {
			chat_msg = (CHAT_MSG *)&comm_msg;			
			DisplayText("[귓속말] %s\r\n", chat_msg->buf);			
		}
	}	// while end

	return 0;
}

// 데이터 보내기
DWORD WINAPI WriteThread(LPVOID arg)
{
	int retval;

	// 서버와 데이터 통신
	while (1) {
		// 쓰기 완료 기다리기
		WaitForSingleObject(hWriteEvent, INFINITE);

		// 문자열 길이가 0이면 보내지 않음
		if (strlen(chatmsg.buf) == 0) {
			// '메시지 전송' 버튼 활성화
			EnableWindow(hButton_SendMsg, TRUE);
			// 읽기 완료 알리기
			SetEvent(hReadEvent);
			continue;
		}

		// 데이터 보내기
		retval = send(sock, (char *)&chatmsg, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}

		// '메시지 전송' 버튼 활성화
		EnableWindow(hButton_SendMsg, TRUE);
		// 읽기 완료 알리기
		SetEvent(hReadEvent);
	}

	return 0;
}

// 에디트 컨트롤에 문자열 출력
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[1024];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit_Status);
	SendMessage(hEdit_Status, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit_Status, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// 채팅방 사용자 출력
void DisplayUser(ROOM_INFO *roomInfo) {
	// 초기화
	SendMessage(hEdit_Display, LB_RESETCONTENT, 0, 0);
	// 리스트 입력
	for(int i=0; i<roomInfo->count; i++)
		SendMessage(hEdit_Display, LB_ADDSTRING, 0, (LPARAM)roomInfo->nick[i]);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
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

// 기타 함수들
// ip 주소 검사
BOOL ip_check(char *ip) {
	// 비어있는 경우
	if (strcmp(ip, "") == 0) return FALSE;

	// ip 주소의 형태를 가지었는지 체크
	int len = strlen(ip);
	if (len > 15 || len < 7) return FALSE;

	int nNumCount = 0;
	int nDotCount = 0;
	int i = 0, k = 0, j = 0;
	char part[4][4];

	for (i = 0; i<len; i++)
	{
		if (ip[i] < '0' || ip[i] > '9')
		{
			if (ip[i] == '.')
			{
				part[k][j] = '\0';
				j = 0;
				++k;
				++nDotCount;
				nNumCount = 0;
			}
			else 
				return FALSE;
			
		}
		else
		{
			part[k][j] = ip[i];
			++j;
			// 숫자는 세개씩
			if (++nNumCount > 3) return FALSE;
		}
	}

	// 점은 3개
	if (nDotCount != 3)	return FALSE;

	// 마지막 파트 널문자 추가
	part[k][j] = '\0';
	// ip --> part[0].part[1].part[2].part[3] 
	// Class A ~ Class C + 루프 백
	if (atoi(part[0]) < 1 || atoi(part[0]) > 223) return FALSE;
	for (i = 1; i < 3; i++)
		if (atoi(part[i]) < 0 || atoi(part[i]) > 255) return FALSE;
	if (atoi(part[3]) < 1 || atoi(part[3]) > 254) return FALSE;

	return TRUE;
}
// 포트번호 검사
BOOL port_check(char *port) {
	int len = strlen(port);

	// 숫자가 아닌 경우
	for (int i = 0; i < len; i++)
		if (port[i] < '0' || port[i] >'9') return FALSE;

	// 범위를 벗어나는 경우
	int tmp = atoi(port);
	if (tmp < 1024 || tmp > 49151) return FALSE;

	return TRUE;
}
