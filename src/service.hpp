#ifndef SERVICE_HPP
#define SERVICE_HPP

#include "ocpplib.hpp"

class Service {
public:
	Service(IOCPPServer &ocpp, IWebSocketServer &server) : m_ocpp(ocpp), m_server(server) {
	}

	void add_callbacks();
	void initAndRun();

private:
	IOCPPServer &m_ocpp;
	IWebSocketServer &m_server;

};

#endif // SERVICE_HPP
