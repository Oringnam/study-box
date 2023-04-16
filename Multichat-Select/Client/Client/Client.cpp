/*
2017�� 1�б� ��Ʈ��ũ���α׷��� ���� 3��
���� : ������
�й� : 111600
Ŭ���̾�Ʈ
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

#define BUFSIZE     512							 // ���� �޽��� ��ü ũ��
#define MSGSIZE     (BUFSIZE-3*sizeof(int)-16)	// ä�� �޽��� �ִ� ����

#define CHATTING    1000                  // �޽��� Ÿ��: ä��
#define JOINTEST	1001				  // �޽��� Ÿ��: ��ȭ�� Ȯ��, ���
#define REJECT		1002				  // �޽��� Ÿ��: ��ȭ�� �ź�
#define DISPLAY		1003				  // �޽��� Ÿ��: ����� ���÷���
#define WHISPER		1004				  // �޽��� Ÿ��: �ӼӸ�

// ���� �޽��� ����
// sizeof(COMM_MSG) == 256
struct COMM_MSG
{
	int  type;
	char dummy[BUFSIZE - sizeof(int)];
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
	char	nick[32][16];
};

// ������
static HINSTANCE     hInst; // ���� ���α׷� �ν��Ͻ� �ڵ�
static HWND          hButton_SendMsg; // '�޽��� ����' ��ư
static HWND          hEdit_Status; // ���� �޽��� ���
static HWND			 hEdit_Nick;	// �г��� ����
static HWND			 hEdit_Room1;	// ��1
static HWND			 hEdit_Room2;	// ��2
static HWND			 hMsgBox;		// �޽����ڽ�
static HWND			 hEdit_Display;	// ����� ���÷���

static char          ipaddr[64]; // ���� IP �ּ�
static char			 whisper[16]; // �ӼӸ� ����� ��ȭ��
static u_short       port; // ���� ��Ʈ ��ȣ
static u_short		 room;	// ���� ���� ��
static u_short		 view;	// ���� �����ִ� ���� �̸�(����� ��)
static HANDLE        hClientThread; // ������ �ڵ�
static volatile BOOL bStart; // ��� ���� ����
static SOCKET        sock; // Ŭ���̾�Ʈ ����
static HANDLE        hReadEvent, hWriteEvent; // �̺�Ʈ �ڵ�
static CHAT_MSG      chatmsg; // ä�� �޽��� ����

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI ReadThread(LPVOID arg);
DWORD WINAPI WriteThread(LPVOID arg);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
void DisplayUser(ROOM_INFO *);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
// ��Ÿ �Լ���
BOOL ip_check(char *);
BOOL port_check(char *);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// ���� �ʱ�ȭ
	chatmsg.type = CHATTING;

	// ��ȭ���� ����
	hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_LOGIN), NULL, DlgProc);

	// ���� ����
	WSACleanup();
	return 0;
}

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit_IP;				// IP�ּ� �Է� ĭ
	static HWND hEdit_Port;				// Port ��ȣ �Է� ĭ
	static HWND hButton_Connect;		// ���� ��ư
	static HWND hButton_Join;			// ���� ��ư
	static HWND hEdit_Msg;				// �޽��� �Է� ĭ
	static HWND hEdit_Combo;			// �޺��ڽ�

	static char portstr[11];			// ��Ʈ ��ȣ ���ڿ�
	CHAT_MSG systemmsg;					// �ý��� �޽���
	char item_room[][10] = {"1�� ��", "2�� ��"};
	int i;

	switch (uMsg) {
	case WM_INITDIALOG:

		// ** ��Ʈ�� �ڵ� ���
		hMsgBox = hDlg;
		// ����
		hEdit_IP = GetDlgItem(hDlg, IDC_IP);
		hEdit_Port = GetDlgItem(hDlg, IDC_PORT);
		hButton_Connect = GetDlgItem(hDlg, IDCONNECT);
		// ����
		hEdit_Nick = GetDlgItem(hDlg, IDC_NICK);
		hEdit_Room1 = GetDlgItem(hDlg, IDC_ROOM1);
		hEdit_Room2 = GetDlgItem(hDlg, IDC_ROOM2);
		hButton_Join = GetDlgItem(hDlg, IDJOIN);
		// �޽��� ����
		hEdit_Status = GetDlgItem(hDlg, IDC_DISPLAY);
		hButton_SendMsg = GetDlgItem(hDlg, IDSEND);
		hEdit_Msg = GetDlgItem(hDlg, IDC_MSG);
		// ���� ����â
		hEdit_Combo = GetDlgItem(hDlg, IDC_COMBO);
		hEdit_Display = GetDlgItem(hDlg, IDC_LIST);

		// ��Ʈ�� �ʱ�ȭ
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

			// ��Ƽĳ��Ʈ �ּ�, ��Ʈ��ȣ ���� Ȯ��
			if (!ip_check(ipaddr)) {
				MessageBox(hDlg, "IP addr error()","�ٽ�!", MB_ICONINFORMATION);
				return TRUE;
			}
			if (!port_check(portstr)) {
				MessageBox(hDlg, "Port number error()", "�ٽ�!", MB_ICONINFORMATION);
				return TRUE;
			}

			port = atoi(portstr);
			EnableWindow(hEdit_IP, FALSE);
			EnableWindow(hEdit_Port, FALSE);
			EnableWindow(hButton_Connect, FALSE);

			// ��� Ŭ���̾�Ʈ ����
			hClientThread = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
			if (hClientThread == NULL) {
				MessageBox(hDlg, "Ŭ���̾�Ʈ�� ������ �� �����ϴ�."
					"\r\n���α׷��� �����մϴ�.", "����!", MB_ICONERROR);
				EndDialog(hDlg, 0);
			}

			while (bStart == FALSE); // ���� ���� ���� ��ٸ�
			EnableWindow(hButton_Join, TRUE);	// �� ���� ��ư Ȱ��ȭ

			// �޺��ڽ� Ȱ��ȭ --> ä�ù��� ����ڵ� Ȯ�� ����
			for (i = 0; i<2; i++)
				SendMessage(hEdit_Combo, CB_ADDSTRING, 0, (LPARAM)item_room[i]);

			return TRUE;

		case IDC_COMBO:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				systemmsg.type = DISPLAY;
				// ���� ���� (����� ������) ���� �ִ� ���� ��ȣ
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
				// ����Ʈ �ϳ� ���ý�
				i = SendMessage(hEdit_Display, LB_GETCURSEL, 0, 0);
				SendMessage(hEdit_Display, LB_GETTEXT, i, (LPARAM)whisper);
				return TRUE;
			}
			return TRUE;

		case IDC_WHISP:
			// �ӼӸ� ����
			chatmsg.type = WHISPER;
			chatmsg.peerRoom = view;
			strcpy(chatmsg.whisp, whisper);
			return TRUE;

		case IDC_ALL:
			// ��ü�� ����
			chatmsg.type = CHATTING;

			return TRUE;

		case IDC_ROOM1:
			room = 0;
			return TRUE;

		case IDC_ROOM2:
			room = 1;
			return TRUE;

		case IDJOIN:
			// ��ȭ�� Ȯ��, ����ϱ� ���� ����
			systemmsg.type = JOINTEST;
			systemmsg.room = room;
			GetDlgItemText(hDlg, IDC_NICK, systemmsg.buf, MSGSIZE);
			// ������ ������			
			if (send(sock, (char *)&systemmsg, BUFSIZE, 0) == SOCKET_ERROR) {
				return TRUE;
			}

			EnableWindow(hButton_SendMsg, TRUE);
			SetFocus(hEdit_Msg);
			
			return TRUE;

		case IDSEND:
			// �б� �ϷḦ ��ٸ�
			WaitForSingleObject(hReadEvent, INFINITE);
			GetDlgItemText(hDlg, IDC_MSG, chatmsg.buf, MSGSIZE);
			// ���� �ϷḦ �˸�
			SetEvent(hWriteEvent);
			// �Էµ� �ؽ�Ʈ ��ü�� ���� ǥ��
			SendMessage(hEdit_Msg, EM_SETSEL, 0, -1);
			return TRUE;

		case IDCANCEL:
			if (MessageBox(hDlg, "������ �����Ͻðڽ��ϱ�?",
				"����", MB_YESNO | MB_ICONQUESTION) == IDYES)
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

// ���� ��� ������ �Լ�
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

	MessageBox(NULL, "������ �����߽��ϴ�.", "����!", MB_ICONINFORMATION);

	// �б� & ���� ������ ����
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	if (hThread[0] == NULL || hThread[1] == NULL) {
		MessageBox(NULL, "�����带 ������ �� �����ϴ�."
			"\r\n���α׷��� �����մϴ�.",
			"����!", MB_ICONERROR);
		exit(1);
	}

	// ��� ���� ����
	bStart = TRUE;

	// ������ ���� ���
	retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	retval -= WAIT_OBJECT_0;
	if (retval == 0)
		TerminateThread(hThread[1], 1);
	else
		TerminateThread(hThread[0], 1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	// ��� ����
	bStart = FALSE;

	MessageBox(NULL, "������ ������ �������ϴ�", "�˸�", MB_ICONINFORMATION);
	EnableWindow(hButton_SendMsg, FALSE);

	closesocket(sock);
	return 0;
}

// ������ �ޱ�
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

		// �޽��� ���� Ÿ�Կ� ����
		// REJECT ; �г��� �ٽ� �Է�
		if (comm_msg.type == REJECT) {
			MessageBox(hMsgBox, "�̹� �����ϴ� ��ȭ���Դϴ�.", "�ٽ�!", MB_ICONINFORMATION);
			EnableWindow(hButton_SendMsg, FALSE);
			SetFocus(hEdit_Nick);
		}
		// CHATTING ; �Ϲ� �޽���
		else if (comm_msg.type == CHATTING) {
			chat_msg = (CHAT_MSG *)&comm_msg;
			if (chat_msg->room == room) {
				DisplayText("[��ü] %s\r\n", chat_msg->buf);
			}
		}
		// DISPLAY ; ä�ù� ����� ǥ��
		else if (comm_msg.type == DISPLAY) {
			roomInfo = (ROOM_INFO *)&comm_msg;
			DisplayUser(roomInfo);
		}
		// �ӼӸ�
		else if (comm_msg.type == WHISPER) {
			chat_msg = (CHAT_MSG *)&comm_msg;			
			DisplayText("[�ӼӸ�] %s\r\n", chat_msg->buf);			
		}
	}	// while end

	return 0;
}

// ������ ������
DWORD WINAPI WriteThread(LPVOID arg)
{
	int retval;

	// ������ ������ ���
	while (1) {
		// ���� �Ϸ� ��ٸ���
		WaitForSingleObject(hWriteEvent, INFINITE);

		// ���ڿ� ���̰� 0�̸� ������ ����
		if (strlen(chatmsg.buf) == 0) {
			// '�޽��� ����' ��ư Ȱ��ȭ
			EnableWindow(hButton_SendMsg, TRUE);
			// �б� �Ϸ� �˸���
			SetEvent(hReadEvent);
			continue;
		}

		// ������ ������
		retval = send(sock, (char *)&chatmsg, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}

		// '�޽��� ����' ��ư Ȱ��ȭ
		EnableWindow(hButton_SendMsg, TRUE);
		// �б� �Ϸ� �˸���
		SetEvent(hReadEvent);
	}

	return 0;
}

// ����Ʈ ��Ʈ�ѿ� ���ڿ� ���
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

// ä�ù� ����� ���
void DisplayUser(ROOM_INFO *roomInfo) {
	// �ʱ�ȭ
	SendMessage(hEdit_Display, LB_RESETCONTENT, 0, 0);
	// ����Ʈ �Է�
	for(int i=0; i<roomInfo->count; i++)
		SendMessage(hEdit_Display, LB_ADDSTRING, 0, (LPARAM)roomInfo->nick[i]);
}

// ����� ���� ������ ���� �Լ�
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

// ��Ÿ �Լ���
// ip �ּ� �˻�
BOOL ip_check(char *ip) {
	// ����ִ� ���
	if (strcmp(ip, "") == 0) return FALSE;

	// ip �ּ��� ���¸� ���������� üũ
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
			// ���ڴ� ������
			if (++nNumCount > 3) return FALSE;
		}
	}

	// ���� 3��
	if (nDotCount != 3)	return FALSE;

	// ������ ��Ʈ �ι��� �߰�
	part[k][j] = '\0';
	// ip --> part[0].part[1].part[2].part[3] 
	// Class A ~ Class C + ���� ��
	if (atoi(part[0]) < 1 || atoi(part[0]) > 223) return FALSE;
	for (i = 1; i < 3; i++)
		if (atoi(part[i]) < 0 || atoi(part[i]) > 255) return FALSE;
	if (atoi(part[3]) < 1 || atoi(part[3]) > 254) return FALSE;

	return TRUE;
}
// ��Ʈ��ȣ �˻�
BOOL port_check(char *port) {
	int len = strlen(port);

	// ���ڰ� �ƴ� ���
	for (int i = 0; i < len; i++)
		if (port[i] < '0' || port[i] >'9') return FALSE;

	// ������ ����� ���
	int tmp = atoi(port);
	if (tmp < 1024 || tmp > 49151) return FALSE;

	return TRUE;
}
