#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

SOCKADDR_IN addr;

SOCKET sListen;
SOCKET sConnect;
SOCKET* Connections;

int addrlen = sizeof(addr);
int ConCounter = 1;

struct Buffer
{
	int ID;
	char Message[256];
};

int ServerThread(int ID)
{
	Buffer sbuffer;

	char* Recv = new char[256];
	ZeroMemory(Recv, 256);

	char* Send = new char[sizeof(Buffer)];
	ZeroMemory(Send, sizeof(Buffer));
	for(;; Sleep(10))
	{
		while(recv(Connections[ID], Recv, 256, NULL) != SOCKET_ERROR)
		{
			sbuffer.ID = ID;
			memcpy(sbuffer.Message, Recv, 256);
			memcpy(Send, &sbuffer, sizeof(Buffer));
			cout << "<Client " << sbuffer.ID << ":> " << sbuffer.Message <<endl;

			for(int a = 1; a != ConCounter; a++)
			{
				if(Connections[a] == Connections[ID])
				{

				}
				else
				{
					send(Connections[a], Send, sizeof(Buffer), NULL);
				}
			}
			ZeroMemory(Recv, 256);
		}
	}

	return 0;
}
int ServerMessage()
{
	Buffer sbuffer;
	int ID = 0;

	string adminMessage;

	char* Send = new char[sizeof(Buffer)];
	ZeroMemory(Send, sizeof(Buffer));

	for(;; Sleep(10))
	{
		getline(cin,adminMessage);

		if (!adminMessage.empty())
		{
			if(adminMessage[0] == '/')
			{
				if(strcmp(adminMessage.c_str(),"/list") == 0)
				{
					sbuffer.ID = ID;
					memcpy(sbuffer.Message, "/", 256);
					memcpy(Send, &sbuffer, sizeof(Buffer));
					for(int i = 1; i != ConCounter; i++){
						if(send(Connections[i], Send, sizeof(Buffer), NULL) != -1)
						{
							cout << "Client " << i << endl;
						}
					}
				}
				else if(strcmp(adminMessage.c_str(),"/personal") == 0)
				{
					int userId;
					cout << "Input message:" << endl;
					getline(cin,adminMessage);
					cout << "Input user ID:" << endl;
					cin >> userId;
					sbuffer.ID = userId;
					memcpy(sbuffer.Message, adminMessage.c_str(), sizeof(adminMessage));
					memcpy(Send, &sbuffer, sizeof(Buffer));
					if(send(Connections[userId], Send, sizeof(Buffer), NULL) != -1)
					{
					
					}
					else 
					{
						cout << "Unknow user" << "\n" << endl;
					}
				}
				else{
					cout << "Unknow command: " << adminMessage << "\n" << endl;
				}
			}
			else
			{
				if(adminMessage.c_str() && ConCounter != 1)
				{
					sbuffer.ID = ID;
					memcpy(sbuffer.Message, adminMessage.c_str(), sizeof(adminMessage));
					memcpy(Send, &sbuffer, sizeof(Buffer));
					for(int a = 1; a != ConCounter; a++)
					{
						send(Connections[a], Send, sizeof(Buffer), NULL);
					}
				}
			}
		}
	}

	return 0;
}

int InitWinSock()
{
	int RetVal = 0;
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2,1);
	RetVal = WSAStartup(DllVersion, &wsaData);

	return RetVal;
}

int main()
{
	int RetVal = 0;
	RetVal = InitWinSock();
	if(RetVal != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	Connections = (SOCKET*)calloc(64, sizeof(SOCKET));

	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	sConnect = socket(AF_INET, SOCK_STREAM, NULL);

	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(1234);
	addr.sin_family      = AF_INET;

	

	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	
	listen(sListen, 64);
	cout << "Print '/list' to see online users" <<endl;
	cout << "Print '/personal' to send private message" <<endl;
	cout << "Waiting for clients...\n" <<endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ServerMessage, 0, NULL, NULL);
	for(;; Sleep(50))
	{
		if(sConnect = accept(sListen, (SOCKADDR*)&addr, &addrlen))
		{
			Connections[ConCounter] = sConnect;

			char* ID = new char[64];
			ZeroMemory(ID, sizeof(ID));

			itoa(ConCounter, ID, 10);
			send(Connections[ConCounter], ID, sizeof(ID), NULL);

			ConCounter = ConCounter + 1;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ServerThread, (LPVOID)(ConCounter - 1), NULL, NULL);
		}
		
	}

	return 0;
}