#include <iostream>
#include <WinSock2.h>
#include<ws2tcpip.h>
#include<thread>


using namespace std;

#pragma comment(lib,"ws2_32.lib")

/* Client Code 

initialize winsock

create socket

connect to the server

send/recieve message

close the socket

*/


bool initialize() {
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		return false;
	}
	return true;
}

void SendMsg(SOCKET sock) {

	cout << "Enter Your chat name : " << endl;
	string name;
	getline(cin, name);
	cout << "Enter Your message : " << endl;
	string message;
	while (1) {
		getline(cin, message);

		string msg = name + " : " + message;
		int bytesend = send(sock, msg.c_str(), msg.length(), 0);
		if (bytesend == SOCKET_ERROR) {
			cout << "send failed with error: " << WSAGetLastError() << endl;
			break;
		}

		if (message == "quit") {
			cout << "stopping application" << endl;
			break;
		}

	}
	closesocket(sock);
	WSACleanup();
}

void RecieveMsg(SOCKET sock) {
	char buffer[4096];
	string msg = "";

	while (1) {
		int bytesrecv = recv(sock, buffer, sizeof(buffer), 0);
		if (bytesrecv <= 0) {
			cout << "disconnected from the server: " << WSAGetLastError() << endl;
			break;
		}

		else {
			msg = string(buffer, bytesrecv);
			cout << msg << endl;
		}
	}

	closesocket(sock);
	WSACleanup();
}

int main() {

	if (!initialize()) {
		cout << "Failed to initialize winsock" << endl;
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	


	if (sock == INVALID_SOCKET) {
		cout << "Failed to create socket" << endl;
		return 1;
	}
	 
	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(sock, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Failed to connect to server" << endl;
		closesocket(sock);
		WSACleanup();
	    return 1;
	}

	cout << "Successfully Connected to server" << endl;

	thread senderT(SendMsg, sock);
	thread recieverT(RecieveMsg, sock);

	senderT.join();
	recieverT.join(); 

	return 0;
}


