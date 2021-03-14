#include "TcpListener.h"
#include <iostream>
#include <string>
#include <sstream>

int TcpListener::init()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
		return wsOk;

	// Create a socket
	listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenerSocket == INVALID_SOCKET)
		return WSAGetLastError();

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(listenerPort);
	inet_pton(AF_INET, IPAddress, &hint.sin_addr);

	if (bind(listenerSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
		return WSAGetLastError();

	// Tell Winsock the socket is for listening 
	if (listen(listenerSocket, SOMAXCONN) == SOCKET_ERROR)
		return WSAGetLastError();

	FD_ZERO(&listenerMaster);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(listenerSocket, &listenerMaster);

    return 0;
}

int TcpListener::run()
{
	// this will be changed by the \quit command (see below, bonus not in video!)
	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = listenerMaster;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == listenerSocket)
			{
				// Accept a new connection
				SOCKET client = accept(listenerSocket, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &listenerMaster);

				onClientConnected(client);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					onClientDisconnected(sock);
					closesocket(sock);
					FD_CLR(sock, &listenerMaster);
				}
				else
				{
					onMessageReceived(sock, buf, bytesIn);
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(listenerSocket, &listenerMaster);
	closesocket(listenerSocket);

	// Message to let users know what's happening.
	//string msg = "Server is shutting down. Goodbye\r\n";

	while (listenerMaster.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = listenerMaster.fd_array[0];

		// Send the goodbye message
		//send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &listenerMaster);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	return 0;
}

void TcpListener::SendToClient(int clientSocket, const char* msg, int len)
{
	send(clientSocket, msg, len, 0);
}

void TcpListener::BroadcastToClients(int broadcastingSocket, const char* msg, int len)
{
	for (int i = 0; i < listenerMaster.fd_count; i++)
	{
		SOCKET outSock = listenerMaster.fd_array[i];
		if (outSock != listenerSocket && outSock != broadcastingSocket)
		{
			SendToClient(outSock, msg, len);
		}
	}
}

void TcpListener::onClientConnected(int clientSocket)
{
}

void TcpListener::onClientDisconnected(int clientSocket)
{
}

void TcpListener::onMessageReceived(int clientSocket, const char* msg, int len)
{
}