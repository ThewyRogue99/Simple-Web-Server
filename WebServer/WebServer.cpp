#include "WebServer.h"
#include <sstream>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

void WebServer::onClientConnected(int clientSocket)
{
}

void WebServer::onClientDisconnected(int clientSocket)
{
}

void WebServer::onMessageReceived(int clientSocket, const char* msg, int len)
{
	std::istringstream stream(msg);

	std::vector<std::string> parsed((std::istream_iterator<std::string>(stream)), std::istream_iterator<std::string>());

	std::string content = "<h1> 404 Not Found </h1>";

	std::string requestedFile = "\\index.html";

	std::ostringstream ossTemp;

	if (parsed.size() >= 3)
	{
		if (parsed[0] == "GET")
		{
			if (parsed[1] != "/")
				requestedFile = parsed[1];

			std::ifstream f;
			int acceptIndex = 0;

			for (int i = 0; i < parsed.size(); i++, acceptIndex++)
				if (parsed[i] == "Accept:")
					break;

			if (parsed[acceptIndex + 1].find("text/html") != std::string::npos)
			{
				f.open("..\\WebsiteContents" + requestedFile);
				if (f.good())
				{
					std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
					content = str;
				}

				f.close();
			}
			else
			{
				content = "";
				f.open("..\\WebsiteContents" + requestedFile, std::ios::ate | std::ios::binary);

				if (f.good())
				{
					int size = f.tellg();
					f.seekg(0, std::ios::beg);

					for (int i = 0; i < size; i++)
					{
						char byte;
						f.read(&byte, 1);

						content += byte;
					}
				}

				f.close();
			}
		}
		else
			printf("%s", msg);
	}

	std::ostringstream oss;

	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length:" << content.size() << "\r\n";
	oss << "\r\n";
	oss << content;

	SendToClient(clientSocket, oss.str().c_str(), oss.str().length());
}
