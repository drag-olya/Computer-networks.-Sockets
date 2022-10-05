#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>  

using namespace std;

#pragma warning(disable: 4996)

SOCKET Connection;

void ClientHandler() {
	char msg[256];
	while (true) {
		recv(Connection, msg, sizeof(msg), NULL);
		string var = msg;

		if (var == "exit") {
			break;
			exit(0);
		}
		cout << msg << endl;
		cout << msg << endl;
		cout << msg << endl;
	}
}

int main(int argc, char* argv[]) {

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "WSAStartup failed" << endl;
		EXIT_FAILURE;
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1025 + 18);
	addr.sin_family = PF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Error: failed connect to server.\n";
		return 1;
	}
	cout << "Connected!\n";

	ofstream fout("ClientLog.txt");
	fout << "Заголовок" << "                   Дані" << endl;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	string client_msg;

	while (true) {
		getline(cin, client_msg);
		int msg_size = client_msg.size();
		send(Connection, (char*)&msg_size, sizeof(int), NULL);
		send(Connection, client_msg.c_str(), msg_size, NULL);

		auto time_now = chrono::system_clock::now();
		time_t send_time = chrono::system_clock::to_time_t(time_now);

		fout << client_msg << "               " << ctime(&send_time) << endl;

		Sleep(10);

		if (client_msg == "exit") {
			cout << "\nYou exited. Goodbay" << endl;
			break;
			exit(0);
		}
	}
	fout.close();

	return 0;
}