#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>  

#pragma warning(disable: 4996)
using namespace std;


// function to count how many correct digits
int corr_dig(string guess, string num) {

	int corr = 0;

	for (int i = 0; i < 4; i++) {
		char a = guess[i];
		if (num.find(a) != string::npos) {
			corr++;
		}
	}
	return corr;
}


// function to count how many digits are on the right spot
int right_spot(int guess, int num) {

	int spot = 0;

	for (int i = 0; i < 4; i++) {
		if (guess % 10 == num % 10) {
			spot++;
		}
		guess /= 10;
		num /= 10;

	}
	return spot;
}


int main(int argc, char* argv[]) {

	srand(time(0));
	int number = rand() % (10000 - 1000) + 1000;


	// Initialize Winsock
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "WSAStartup failed" << endl;
		EXIT_FAILURE;
	}

	// Create a SOCKET for the server to listen for client connections.
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "Error at socket()" << endl;
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1025 + 18);
	addr.sin_family = PF_INET;


	// Bind the socket.
	if (bind(ListenSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		cout << "bind() failed." << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}


	// Listen for incoming connection requests on the created socket
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Error listening on socket." << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;

	}

	cout << "Listening on socket..." << endl;

	// Accept a client socket
	SOCKET ClientSocket;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "accept failed" << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Client Connected!" << endl;

		char msg1[256] = "Hello. It's a game 'Guessing 4-digit number'\n";
		char msg2[256] = "You can try it yourself or generate some random numbers to check\n";
		char msg3[256] = "If you want to play - enter the number you want to check.\n"
			"To generate random numbers, enter 'random'\n";
		send(ClientSocket, msg1, sizeof(msg1), NULL);
		send(ClientSocket, msg2, sizeof(msg2), NULL);
		send(ClientSocket, msg3, sizeof(msg3), NULL);

		int msg_size;

		ofstream fout("ServerLog.txt");
		fout << "Заголовок" << "                   Дані" << endl;

		while (true) {
			recv(ClientSocket, (char*)&msg_size, 256, NULL);
			char* res_msg = new char[msg_size + 1];
			res_msg[msg_size] = '\0';
			recv(ClientSocket, res_msg, msg_size, NULL);

			auto time_now = chrono::system_clock::now();

			time_t res_time = chrono::system_clock::to_time_t(time_now);

			fout << res_msg << "               " << ctime(&res_time) << endl;
			cout << res_msg << "               " << ctime(&res_time) << endl;

			string var = res_msg;

			if (var == "Who") {
				char msg_who[256] = "\nThis programme is created by Olha Drahomeretska."
					"\nVar 18, game 'Guessing the 4-digit number'\n";
				send(ClientSocket, msg_who, sizeof(msg_who), NULL);
			}

			else if (var == "random") {
				char msg_hm[256] = "\nHow many numbers to generate?";
				send(ClientSocket, msg_hm, sizeof(msg_hm), NULL);
				recv(ClientSocket, (char*)&msg_size, sizeof(int), NULL);
				char* res_msg = new char[msg_size + 1];
				res_msg[msg_size] = '\0';
				recv(ClientSocket, res_msg, msg_size, NULL);

				string amount = res_msg;
				srand(time(0));
				try {
					for (int i = 0; i < stoi(amount); i++)
					{
						int rnum = rand() % (10000 - 1000) + 1000;
						int corr = corr_dig(to_string(rnum), to_string(number));
						int spot = right_spot(rnum, number);

						string msg_rnum = "\nRundom number: " + to_string(rnum);
						string msg_corr = "\n" + to_string(corr) + " correct digits.\n" + to_string(spot) + " on the right spot.\n";
						send(ClientSocket, msg_rnum.c_str(), sizeof(msg_rnum), NULL);
						send(ClientSocket, msg_corr.c_str(), sizeof(msg_corr), NULL);
						Sleep(10);
					}
				}
				catch (const invalid_argument& e) {
					char msg_er[256] = "Wrong input.\n";
					send(ClientSocket, msg_er, sizeof(msg_er), NULL);
				}
			}
			else if (var == "end game") {
				string msg_num = "\nThe number was " + to_string(number);
				char msg_eg1[256] = "You ended the game.";
				char msg_eg2[256] = "If you want to start again - enter 'new game'.\n";
				send(ClientSocket, msg_num.c_str(), sizeof(msg_num), NULL);
				send(ClientSocket, msg_eg1, sizeof(msg_eg1), NULL);
				send(ClientSocket, msg_eg2, sizeof(msg_eg2), NULL);
			}
			else if (var == "new game") {
				srand(time(0));
				number = rand() % (10000 - 1000) + 1000;
				char msg_ng[256] = "\nYou started a new game.\n";
				send(ClientSocket, msg_ng, sizeof(msg_ng), NULL);
			}
			else if (var == "exit") {
				char msg_ex[256] = "exit";
				send(ClientSocket, msg_ex, sizeof(msg_ex), NULL);
				break;
				exit(0);
			}

			else {
				try {
					if (stoi(var) == number) {
						char msg_gues1[256] = "\nCongratulations! You've guessed the number\n";
						char msg_gues2[256] = "If you want to start again - enter 'new game'.\n";
						send(ClientSocket, msg_gues1, sizeof(msg_gues1), NULL);
						send(ClientSocket, msg_gues2, sizeof(msg_gues2), NULL);


					}
					else if (1000 <= stoi(var) && stoi(var) <= 9999) {
						int corr = corr_dig(var, to_string(number));
						int spot = right_spot(stoi(var), number);
						string msg_corr = "\n" + to_string(corr) + " correct digits.";
						string msg_spot = to_string(spot) + " on the right spot.\n";
						send(ClientSocket, msg_corr.c_str(), sizeof(msg_corr), NULL);
						send(ClientSocket, msg_spot.c_str(), sizeof(msg_spot), NULL);
					}
					else {
						char msg_wn[256] = "Wrong number. Try again.\n";
						send(ClientSocket, msg_wn, sizeof(msg_wn), NULL);
					}
				}
				catch (const invalid_argument& e) {
					char msg_er[256] = "Wrong input. Try again.\n";
					send(ClientSocket, msg_er, sizeof(msg_er), NULL);
				}
			}
			delete[] res_msg;
		}
		fout.close();
	}
	return 0;
}