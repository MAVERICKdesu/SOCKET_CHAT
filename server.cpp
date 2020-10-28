#include<iostream>
#include<windows.h>
#include<winsock.h>
#include<cstring>
#include <map>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
void initialization();

struct messageList {
	char message[100];
	char from[100];
}M[1000];
int messageNum = 0;
map<SOCKET, string> socketToname;


DWORD WINAPI newMessage(LPVOID lparam)//用于检查消息队列是否有新的消息，发送给用户
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	int fsn = 0;
	string name("unname");
	while (1)
	{
		Sleep(10);
		if (socketToname.find(ClientSocket) != socketToname.end())
			name = socketToname.find(ClientSocket)->second;
		if (fsn != messageNum)
		{
			for (; fsn< messageNum; ++fsn)
			{
				if (strcmp(M[fsn].from, name.c_str()) == 0)
					continue;
				char sen[200];
				char cn[10] = "说:";
				strcpy_s(sen, M[fsn].from);
				strcat_s(sen, cn);
				strcat_s(sen, M[fsn].message);
				send(ClientSocket, sen, 200, 0);
			}
		}
	}
}

DWORD WINAPI acceptMessage(LPVOID lparam)//用于检查该用户是否发来了新的消息，添加到消息队列
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	bool first=true;
	char name[100]="unname";
	while (1)
	{
		Sleep(10);
		char recv_buf[100];
		int ret =recv(ClientSocket, recv_buf, 100, 0);
		if (first)
		{
			strcpy_s(name, recv_buf);
			first = false;
			string str(name);
			socketToname.insert(pair<SOCKET, string>(ClientSocket, str));
			cout << recv_buf << " 进入聊天室" << endl << endl;
			continue;
		}
		else
		{
			if (ret == SOCKET_ERROR || ret == 0)
			{
				cout << name << " 退出了聊天室" << endl << endl;
				break;
			}
			cout << name << "发来新消息:" << recv_buf << endl<<endl;
			strcpy_s(M[messageNum].message, recv_buf);
			strcpy_s(M[messageNum].from, name);
			messageNum++;
		}
	}
	return 0;
}

DWORD WINAPI newConnect(LPVOID lparam)
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	HANDLE hThread = CreateThread(NULL, NULL, newMessage, LPVOID(ClientSocket), 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, NULL, acceptMessage, LPVOID(ClientSocket), 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hThread2);
	return 0;
}

void initialization() {
	WORD w_req = MAKEWORD(2, 2);
	WSADATA wsadata;
	int err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化失败" << endl;
	}
	else {
		cout << "初始化成功" << endl;
	}
 
}

int main() {
	int len = 0;
	SOCKET s_server;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5010);
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
	listen(s_server, SOMAXCONN);
	cout << "服务端已启动" << endl;
	while (1)
	{
		len = sizeof(SOCKADDR);
		SOCKET sockConn = accept(s_server, (SOCKADDR*)&accept_addr, &len);//新建sockConn
		HANDLE hThread = CreateThread(NULL, NULL, newConnect, LPVOID(sockConn), 0, NULL);//建立新线程处理改用户的收发消息事件
	}
	closesocket(s_server);
	WSACleanup();
	return 0;
}