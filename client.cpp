#include<iostream>
#include<winsock.h>
#include<cstring>
#include<windows.h>
#include<queue>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
void initialization();
char name[100];
string messList[100];
string quit("/quit");
int mess = 0;
bool online = true;

void clsScreen()
{
	Sleep(100);
	system("cls");
	for (int i = 0; i < mess; ++i)
		cout << messList[i].c_str() << endl;
}

DWORD WINAPI newMessage(LPVOID lparam)//用于检查消息队列是否有新的消息，发送给用户
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	while (online)
	{
		Sleep(10);
		char recv_buf[200];
		Sleep(100);
		recv(ClientSocket, recv_buf, 200, 0);
		cout << recv_buf << endl;
		string str(recv_buf);
		messList[mess] = str;
		++mess;
	}
	return 0;
}
DWORD WINAPI sentMessage(LPVOID lparam)//检查用户输入，发给服务器
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	bool first = true;
	while (online)
	{
		if (first)
		{
			cout << endl << "输入您的姓名";
			first = false;
		}
		char sent_buf[100];
		cin >> sent_buf;
		clsScreen();
		strcpy_s(name, sent_buf);
		string o(sent_buf);
		if (o == quit && !first)
			exit(0);
		send(ClientSocket, sent_buf, 100, 0);
	}
	online = false;
	return 0;
}
int main() {
	SOCKET s_server;
	SOCKADDR_IN server_addr;
	initialization();
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(5010);
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
	HANDLE hThread = CreateThread(NULL, NULL, newMessage, LPVOID(s_server), 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, NULL, sentMessage, LPVOID(s_server), 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hThread2);
	closesocket(s_server);
	WSACleanup();
	return 0;
}
void initialization() {
	WORD w_req = MAKEWORD(2, 2);
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化失败" << endl;
	}
	else {
		cout << "初始化成功" << endl;
	}

}
