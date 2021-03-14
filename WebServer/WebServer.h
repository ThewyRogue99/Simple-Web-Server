#pragma once

#include "TcpListener.h"

class WebServer : public TcpListener
{
public:

	WebServer(const char* IP_Address, int port) : TcpListener(IP_Address, port) { }

protected:

	virtual void onClientConnected(int clientSocket) override;

	virtual void onClientDisconnected(int clientSocket) override;

	virtual void onMessageReceived(int clientSocket, const char* msg, int len) override;

};

