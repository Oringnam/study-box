/*
2017�� 1�б� ��Ʈ��ũ���α׷��� ���� 2�� ��Ƽä�� ���α׷�
���� : ������  
�й� : 111600
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
#define SERVERPORT	49151	// 1:1ä�� ������Ʈ

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgNick(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
// ���� ��� ������ �Լ�
DWORD WINAPI Receiver(LPVOID arg);
DWORD WINAPI MultiChat(LPVOID arg);
DWORD WINAPI SingleChat(LPVOID arg);
DWORD WINAPI SingleReceiver(LPVOID arg);

// ��Ÿ �Լ���
BOOL ip_check(char *);
BOOL port_check(char *);
BOOL call_check(char *);

SOCKET sock;	// ���� ����ü(���� ����� ����)
SOCKADDR_IN remoteaddr, myaddr;
char buf[BUFSIZE + 1];	// ������ �ۼ��� ����
char nick[NICKSIZE + 1], before[NICKSIZE +1];	// �г���
char multicastip[IPSIZE + 1];	// ��Ƽĳ��Ʈ ip
char portstr[PORTSIZE + 1];	// ��Ʈ��ȣ ���ڿ���
int remoteport, myport;	// remoteport: ��Ƽĳ��Ʈ ��Ʈ
int connectFlag;	// �г����� �����ص� ���ο� �����尡 �������� �ʵ���

HINSTANCE hInst;	// ���� ���̾�α׸� ����
HANDLE hReadEvent, hWriteEvent, hReadEvent2, hWriteEvent2;; // �̺�Ʈ
HANDLE sThread1, sThread2;	// 1:1 ä�� receiver, sender
HWND hDialog1;	// ���� ä��ȭ��
HWND hSendButton, hConnectButton; // ������ ��ư
HWND hEdit1, hEdit2, hEdit3, hEdit4, hEdit5; // ���� ��Ʈ��
HWND hEdit6, hEdit7, hEdit8, hSendButton2;	// 1:1 ä��

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;	

	// ��ȭ���� ����
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// �̺�Ʈ ����
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	return 0;
}

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	hDialog1 = hDlg;
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);	// ��ȭ��
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);	// ��ȭ �Է�â
		hEdit3 = GetDlgItem(hDlg, IDC_EDIT3);	// ��ȭ ǥ��â
		hEdit4 = GetDlgItem(hDlg, IDC_EDIT4);	// ��Ƽĳ��Ʈ �ּ�
		hEdit5 = GetDlgItem(hDlg, IDC_EDIT5);	// ��Ʈ��ȣ

		hConnectButton = GetDlgItem(hDlg, IDCONNECT);	// ��Ƽĳ��Ʈ-��Ʈ ����
		hSendButton = GetDlgItem(hDlg, IDOK);	// ��ȭ ����

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
			EnableWindow(hSendButton, FALSE); // ������ ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���

			GetDlgItemText(hDlg, IDC_EDIT1, nick, NICKSIZE + 1);
			GetDlgItemText(hDlg, IDC_EDIT2, buf, BUFSIZE + 1);

			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			SetFocus(hEdit2);
			SendMessage(hEdit2, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDCONNECT:
			GetDlgItemText(hDlg, IDC_EDIT4, multicastip, IPSIZE + 1);
			GetDlgItemText(hDlg, IDC_EDIT5, portstr, PORTSIZE + 1);

			// ��Ƽĳ��Ʈ �ּ�, ��Ʈ��ȣ ���� Ȯ��
			if (!ip_check(multicastip)) {
				err_display("ip addr error()");
				return TRUE;
			}
			if (!port_check(portstr)) {
				err_display("port number error()");
				return TRUE;
			}
			// ��Ʈ��ȣ -> ������
			remoteport = atoi(portstr);
			EnableWindow(hConnectButton, FALSE);	// Connect��ư�� ��Ȱ��ȭ
			return TRUE;
		case IDJOIN:
			strcpy(before,nick);
			GetDlgItemText(hDlg, IDC_EDIT1, nick, NICKSIZE + 1);
			EnableWindow(hEdit2, TRUE);
			EnableWindow(hSendButton, TRUE);

			// ó�� join�� �ƴ� ��츸 = ��ȭ�� ���� ��
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
			// ���� ��� ������ ���� ,, ó�� ���ӽÿ��� ������ ����
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

// ��ȭ���� ���ν���
BOOL CALLBACK DlgWhisper(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit6 = GetDlgItem(hDlg, IDC_EDIT6);	// ��ȭ �Է�â
		hEdit7 = GetDlgItem(hDlg, IDC_EDIT7);	// ��ȭ ���â
		hEdit8 = GetDlgItem(hDlg, IDC_EDIT8);	// ��ȭ��

		hSendButton2 = GetDlgItem(hDlg, IDOK);	// ���� ��ư

		SendMessage(hEdit6, EM_SETLIMITTEXT, BUFSIZE, 0);
		SendMessage(hEdit8, EM_SETLIMITTEXT, NICKSIZE, 0);
		SetDlgItemText(hDlg, IDC_EDIT8, nick);	// ���� �г��� ��������
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK2:
			EnableWindow(hSendButton2, FALSE); // ������ ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent2, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_EDIT6, buf, BUFSIZE + 1);
			SetEvent(hWriteEvent2); // ���� �Ϸ� �˸���
			SetFocus(hEdit6);
			SendMessage(hEdit6, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL2:
			// ��� ����
			TerminateThread(sThread1, TRUE);
			TerminateThread(sThread2, TRUE);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_CHANGE:
			strcpy(before, nick);
			GetDlgItemText(hDlg, IDC_EDIT8, nick, NICKSIZE + 1);

			// �г��� ������ �˸�
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
			// ���� ���̾�α� ��ȭ�� ĭ ����
			SetDlgItemText(hDialog1, IDC_EDIT1, nick);

			// �г����� ����Ǹ� ��� ����
			TerminateThread(sThread1, TRUE);
			TerminateThread(sThread2, TRUE);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}

// ���� ��Ʈ�� ��� �Լ�
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
// 1:1 ä�� ��� �Լ�
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
	DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ���� ���
DWORD WINAPI Receiver(LPVOID arg)
{
	int retval;
	time_t tv;
	struct tm *nowtime;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (recv_sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR �ɼ� ����
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

	// ��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	char peernick[NICKSIZE + 1];
	int check;

	// ��Ƽĳ��Ʈ ������ �ޱ�
	while (1) {
		addrlen = sizeof(peeraddr);
		tv = time(NULL);
		nowtime = localtime(&tv);
		check = 1;

		// ������ �ޱ�
		// ��ȭ��
		retval = recvfrom(recv_sock, peernick, NICKSIZE + 1, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		peernick[retval] = '\0';
		// ��ȭ ����
		retval = recvfrom(recv_sock, buf, BUFSIZE + 1, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		buf[retval] = '\0';

		// �� ���� ���� ���
		addrlen = sizeof(myaddr);
		getsockname(sock, (SOCKADDR *)&myaddr, &addrlen);
		myport = ntohs(myaddr.sin_port);

		// ���� ������ ��� ; ��ȭ�� [ ip / ��Ʈ ] (��:��:��) : ��ȭ ����
		DisplayText("%s [%s / %d](%d:%02d:%02d) : %s\r\n", peernick, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec, buf);

		// 1:1 ��ȭ ( �ñ׳� ���� )
		if (call_check(buf)) {
			check = 0;

			hReadEvent2 = CreateEvent(NULL, FALSE, TRUE, NULL);
			if (hReadEvent == NULL) return 1;
			hWriteEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (hWriteEvent == NULL) return 1;

			// 1:1 ä�� ������ ����
			sThread1 = CreateThread(NULL, 0, SingleReceiver, NULL, 0, NULL);
			sThread2 = CreateThread(NULL, 0, SingleChat, NULL, 0, NULL);
			if (sThread1 == NULL) {
				DisplayText("Single Receiver Error\r\n");
			}
			if (sThread2 == NULL) {
				DisplayText("Single Sender Error\r\n");
			}
			memset(buf, 0, strlen(buf));	// ���� ����
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), NULL, DlgWhisper);
		}
		// �г��� ������ 1:1��ȭ ( �ñ׳� ���� )
		else if (strcmp(nick, peernick) == 0 && myport != ntohs(peeraddr.sin_port) && check == 1) {
			char reqtmp[20];
			char tmp[20];

			// �ñ׳� �޼��� ����
			itoa(ntohs(peeraddr.sin_port), tmp, 10);
			strcpy(reqtmp, "CALL ");
			strcat(reqtmp, tmp);

			// 1:1 ��ȭ �ñ׳� (�޽�����)
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

			// 1:1 ä�� ������ ����
			sThread1 = CreateThread(NULL, 0, SingleReceiver, NULL, 0, NULL);
			sThread2 = CreateThread(NULL, 0, SingleChat, NULL, 0, NULL);
			if (sThread1 == NULL) {
				DisplayText("Single Receiver Error\r\n");
			}
			if (sThread2 == NULL) {
				DisplayText("Single Sender Error\r\n");
			}
			// ���ο� ���̾�α�
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), NULL, DlgWhisper);
		}	// 1:1 ��ȭ if..else.. end

		SetEvent(hReadEvent); // �б� �Ϸ� �˸���
	}

	// ��Ƽĳ��Ʈ �׷� Ż��
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(recv_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

DWORD WINAPI MultiChat(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// ��Ƽĳ��Ʈ TTL ����
	int ttl = 2;
	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,	(char *)&ttl, sizeof(ttl));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(multicastip);
	remoteaddr.sin_port = htons(remoteport);
	
	// ������ �˸�
	strcpy(buf, "Entered the Chatting Room");
	if (sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
		err_display("sendto()");
		return TRUE;
	}
	if (sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr)) == SOCKET_ERROR) {
		err_display("sendto()");
		return TRUE;
	}

	// ��Ƽĳ��Ʈ ������ ������
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���

		// ���ڿ� ���̰� 0�̸� ������ ����
		if (strlen(buf) == 0) {
			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸���
			continue;
		}

		// ������ ������
		// ��ȭ��
		retval = sendto(sock, nick, strlen(nick), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		// ��ȭ����
		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
	}
	
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}

// 1:1 ���
DWORD WINAPI SingleChat(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET singleSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (singleSock == INVALID_SOCKET) err_quit("socket()");

	// ��Ƽĳ��Ʈ TTL ����
	int ttl = 2;
	retval = setsockopt(singleSock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(multicastip);
	remoteaddr.sin_port = htons(SERVERPORT);

	// ��Ƽĳ��Ʈ ������ ������
	while (1) {
		WaitForSingleObject(hWriteEvent2, INFINITE); // ���� �Ϸ� ��ٸ���

		// ���ڿ� ���̰� 0�̸� ������ ����
		if (strlen(buf) == 0) {
			EnableWindow(hSendButton2, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent2); // �б� �Ϸ� �˸���
			continue;
		}

		// ������ ������
		retval = sendto(singleSock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		EnableWindow(hSendButton2, TRUE); // ������ ��ư Ȱ��ȭ
	}

	// closesocket()
	closesocket(singleSock);

	// ���� ����
	WSACleanup();
	return 0;
}

DWORD WINAPI SingleReceiver(LPVOID arg) {
	int retval;
	time_t tv;
	struct tm *nowtime;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET singleRecv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (singleRecv_sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR �ɼ� ����
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

	// ��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(singleRecv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;

	// ��Ƽĳ��Ʈ ������ �ޱ�
	while (1) {
		addrlen = sizeof(peeraddr);
		tv = time(NULL);
		nowtime = localtime(&tv);

		// ������ �ޱ�
		retval = recvfrom(singleRecv_sock, buf, BUFSIZE + 1, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		buf[retval] = '\0';

		// ���� ������ ��� ; [ ip / ��Ʈ ] (��:��:��) : ��ȭ ����
		DisplayText2("[%s / %d](%d:%02d:%02d) : %s\r\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec, buf);

		SetEvent(hReadEvent2); // �б� �Ϸ� �˸���
	}

	// ��Ƽĳ��Ʈ �׷� Ż��
	retval = setsockopt(singleRecv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(singleRecv_sock);
	// ���� ����
	WSACleanup();
	return 0;
}

// ��Ÿ �Լ���
BOOL ip_check(char *ip) {
	// ����ִ� ���
	if (strcmp(ip, "") == 0) return FALSE;
	
	// ip �ּ��� ���¸� ���������� üũ
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
			// ���ڴ� ������
			if (++nNumCount > 3) return FALSE;
		}
	}

	// ���� 3��
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
	49151���� 1:1 ä�ÿ����� ���
	1024 ~ 49150 ������ ��Ʈ��ȣ ��� (Registered Ports)
	*/
	int len = strlen(port);

	// ���ڰ� �ƴ� ���
	for (int i = 0; i < len; i++)
		if (port[i] < '0' || port[i] >'9') return FALSE;

	// ������ ����� ���
	int tmp = atoi(port);
	if (tmp < 1024 || tmp >= 49151) return FALSE;

	return TRUE;
}

BOOL call_check(char *buf) {
	// ���� ���� �ñ׳����� Ȯ��
	char tmp[BUFSIZE + 1];
	char call[BUFSIZE + 1];

	strcpy(tmp, buf + 5);	// ��Ʈ Ȯ�ο�
	strcpy(call, buf);	// �ñ׳� Ȯ�ο�
	call[4] = '\0';

	if (strcmp(call, "CALL") != 0) return FALSE;	// CALL �ñ׳����� Ȯ��
	if (atoi(tmp) == myport) return TRUE;	// ���� ���Ѱ��� Ȯ��

	return FALSE;
}
