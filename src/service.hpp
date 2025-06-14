#ifndef SERVICE_HPP
#define SERVICE_HPP

#include "ocpplib.hpp"
#include <atomic>

class Service {
public:
	Service(IOCPPServer &ocpp, IWebSocketServer &server) : m_ocpp(ocpp), m_server(server) {
	}
	~Service() = default;

	void add_callbacks();
	void initAndRun();

private:
	IOCPPServer &m_ocpp;
	IWebSocketServer &m_server;
	std::atomic<bool> m_transaction{ false };

	static void onConnection(const std::string &clientId);
	static void onDisconnection(const std::string &clientId);
};

#endif // SERVICE_HPP
