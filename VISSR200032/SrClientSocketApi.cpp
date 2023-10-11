// Sample program that a client host connects SR series readers.

#include <stdio.h>
#include <winsock2.h>
#include "stdafx.h"

#define READER_COUNT	1	// number of readers to connect
#define RECV_DATA_MAX	10240

static struct {
	char *readerIpAdddress;	// IP address
	int readerCommandPort;	// Command port number
	int readerDataPort;		// Data port number
} s_readerInfo[READER_COUNT] = {
	{ "169.254.0.1", 9004, 9004 },
	//{ "192.168.100.101", 9003, 9004 }
};

static SOCKET s_commandSocket[READER_COUNT];	// socket for command
static SOCKET s_dataSocket[READER_COUNT];		// socket for data

// Function to initialize
void SrClientSocket_Init()
{
	for (int i = 0; i < READER_COUNT; i++) {
		s_commandSocket[i] = INVALID_SOCKET;
		s_dataSocket[i]    = INVALID_SOCKET;
	}
}

// Function to connect
void SrClientSocket_Connect(int commandPort,int dataPort)
{
	// Activate winsock.
	WSADATA data;
	WSAStartup(MAKEWORD(2,0), &data);

	for (int i = 0; i < READER_COUNT; i++) {
		struct sockaddr_in dst;
		memset(&dst, 0, sizeof(dst));
		s_readerInfo[i].readerCommandPort = commandPort;
		// Connect to the command port.
		dst.sin_port             = htons(s_readerInfo[i].readerCommandPort);
		dst.sin_family           = AF_INET;
		dst.sin_addr.S_un.S_addr = inet_addr(s_readerInfo[i].readerIpAdddress);

		// Close the socket if opend.
		if (s_commandSocket[i] != INVALID_SOCKET) {
			closesocket(s_commandSocket[i]);
			s_commandSocket[i] =INVALID_SOCKET;
		}

		// Create a new socket.
		s_commandSocket[i] = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(s_commandSocket[i], (struct sockaddr *) &dst, sizeof(dst)) != 0){
			CString s;
			s.Format(_T("%S\r\ncannot connect to the command port."), s_readerInfo[i].readerIpAdddress);		
			//AfxMessageBox(s);

			closesocket(s_commandSocket[i]);
			s_commandSocket[i] = INVALID_SOCKET;
			continue;
		}

		// Connect to the data port.
		s_readerInfo[i].readerDataPort = dataPort;
		dst.sin_port = htons(s_readerInfo[i].readerDataPort);

		// Close the socket if opend.
		if (s_dataSocket[i] != INVALID_SOCKET) {
			closesocket(s_dataSocket[i]);
			s_dataSocket[i] = INVALID_SOCKET;
		}
		// If the same port number is used for command port and data port, unify the sockets and skip a new connection.
		if(commandPort == dataPort){
			s_dataSocket[i] = s_commandSocket[i];
		}
		else{
			// Create a new socket.
			s_dataSocket[i] = socket(AF_INET, SOCK_STREAM, 0);

			if(connect(s_dataSocket[i], (struct sockaddr *) &dst, sizeof(dst)) != 0){
				CString s;
				s.Format(_T("%S\r\ncannot connect to data port."), s_readerInfo[i].readerIpAdddress);		
				AfxMessageBox(s);

				closesocket(s_dataSocket[i]);
				s_dataSocket[i] = INVALID_SOCKET;
				continue;
			}
		}

		// Set 100 milliseconds to receive timeout.
		int timeout = 100;
		setsockopt(s_dataSocket[i],	SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	}
}

// Function to disconnect
void SrClientSocket_Disconnect()
{
	for (int i = 0; i < READER_COUNT; i++) {
		if (s_commandSocket[i] != INVALID_SOCKET) {
			closesocket(s_commandSocket[i]);
			s_commandSocket[i] = INVALID_SOCKET;
		}

		if (s_dataSocket[i] != INVALID_SOCKET) {
			closesocket(s_dataSocket[i]);
			s_dataSocket[i] = INVALID_SOCKET;
		}
	}

	WSACleanup();
}

// Function to set timing ONc
void SrClientSocket_Lon()
{
	// Send "LON" command.
	char command[] = "LON\r"; // Send "LON" command.

	for (int i = 0; i < READER_COUNT; i++) {
		if (s_commandSocket[i] != INVALID_SOCKET) {
			send(s_commandSocket[i], command, strlen(command), 0);
		}
		else {
			CString s;
			s.Format(_T("%S is disconnected."), s_readerInfo[i].readerIpAdddress);		
			AfxMessageBox(s);
		}
	}
}

// Function to set timing OFF
void SrClientSocket_Loff()
{
	// Send "LOFF" command.
	char command[] = "LOFF\r";	// CR is terminator

	for (int i = 0; i < READER_COUNT; i++) {
		if (s_commandSocket[i] != INVALID_SOCKET) {
			send(s_commandSocket[i], command, strlen(command), 0);
		}
		else {
			CString s;
			s.Format(_T("%S is disconnected."), s_readerInfo[i].readerIpAdddress);		
			AfxMessageBox(s);
		}
	}
}

// Function to receive data
void SrClientSocket_Receive()
{
	char recvBytes[RECV_DATA_MAX];
	int recvSize = 0;
	
	for (int i = 0; i < READER_COUNT; i++) {
		if (s_dataSocket[i] != INVALID_SOCKET) {
			recvSize = recv(s_dataSocket[i], recvBytes, sizeof(recvBytes), 0);
		}
		else {
			CString s;
			s.Format(_T("%S is disconnected."), s_readerInfo[i].readerIpAdddress);		
			AfxMessageBox(s);
			continue;
		}

		if (recvSize > 0) {
			recvBytes[recvSize] = 0;	// Terminating null to handle as string.

			CString s;
			s.Format(_T("%S\r\n%s"), s_readerInfo[i].readerIpAdddress, (CString)recvBytes);		
			AfxMessageBox(s,MB_ICONINFORMATION,0);
		}
		else {
			CString s;
			s.Format(_T("%S has no data."), s_readerInfo[i].readerIpAdddress);		
			AfxMessageBox(s);
		}
	}
}

