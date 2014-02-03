#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

SOCKADDR_IN addr;

SOCKET sConnect;

struct Buffer
{
	int ID;
	char Message[256];
};

int ClientThread()
{
	Buffer sbuffer;

	char buffer[sizeof(sbuffer)] = {0};

	for(;; Sleep(10))
	{
		while(recv(sConnect, buffer, sizeof(sbuffer), NULL) != SOCKET_ERROR)
		{
			memcpy(&sbuffer, buffer, sizeof(sbuffer));
			if(sbuffer.ID == 0 && sbuffer.Message[0] == '/')
			{
				//there may be server's commands
			}
			else if(sbuffer.ID == 0)
			{
				cout << "<Server:> " << sbuffer.Message <<endl;
			}
			else{
				cout << "<Client " << sbuffer.ID << ":> " << sbuffer.Message <<endl;
			}
		}
	}

	return 0;
}

int main()
{
	system("cls");

	int RetVal = 0;

	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2,1);
	RetVal = WSAStartup(DllVersion, &wsaData);
	if(RetVal != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	sConnect = socket(AF_INET, SOCK_STREAM, NULL);
	string ip;
	
	do
	{
		cout << "Enter server ip:\n" <<endl;
		cin >> ip;
	}
	while (inet_addr(ip.c_str()) == INADDR_NONE);

	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port        = htons(1234);
	addr.sin_family      = AF_INET;

	RetVal = connect(sConnect, (SOCKADDR*)&addr, sizeof(addr));

	if(RetVal != 0)
	{
		MessageBoxA(NULL, "Could not connect to server", "Error", MB_OK | MB_ICONERROR);
		main();
	}
	else
	{
		int ID;
		char* cID = new char[64];
		ZeroMemory(cID, 64);

		recv(sConnect, cID, 64, NULL);
		ID = atoi(cID);

		cout << "Connected" <<endl;
		cout << "You are Client No: " << ID << "\n" <<endl;

        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientThread, NULL, NULL, NULL);
		
		for(;; Sleep(10))
		{
			string buffer;
			getline(cin,buffer);
			if (!buffer.empty())
			{
				send(sConnect, buffer.c_str(), sizeof(buffer), NULL);
			}
		}
	}

	return 0;
}