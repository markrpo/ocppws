#include "ocpp.hpp"
#include "websocket.hpp"
#include "service.hpp"
#include <iostream>  


int main() {
	std::cout << "Heartbeat Response: " << OcppJsons::heartbeatResponse() << std::endl;
	std::string path = "/";
	WebSocketServer server(9000, path);
	OCPPServer ocpp(&server);
	Service service(ocpp, server);
	service.initAndRun();
}
