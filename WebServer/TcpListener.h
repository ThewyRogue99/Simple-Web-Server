#pragma once

#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

class TcpListener
{
public:

	TcpListener(const char* IP_Address, int port, int end_Key = VK_END) :
		IPAddress(IP_Address), listenerPort(port) { }

	int init();

	int run();

protected:

	virtual void onClientConnected(int clientSocket);

	virtual void onClientDisconnected(int clientSocket);

	virtual void onMessageReceived(int clientSocket, const char* msg, int len);

	void SendToClient(int clientSocket, const char* msg, int len);

	void BroadcastToClients(int broadcastingSocket, const char* msg, int len);

private:

	const char*	IPAddress;

	int	listenerPort;

	int	listenerSocket;

	fd_set listenerMaster;

};

