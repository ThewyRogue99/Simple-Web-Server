#include "WebServer.h"

void main()
{
	WebServer webServer("0.0.0.0", 80);

	if (webServer.init() != 0)
		return;

	webServer.run();
	system("pause");
}