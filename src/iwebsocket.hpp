#ifndef IWEBSOCKET_HPP
#define IWEBSOCKET_HPP

#include <string>
#include <functional>

struct message_request {
		std::string id;
		std::string message;

		message_request(std::string id, std::string message) : id(id), message(message) {}

};


class OcppObserver {
public:
	virtual void notify(const std::string message, const std::string id) = 0;
	virtual void notifyConnected(const std::string id) = 0;
	virtual void notifyDisconnected(const std::string id) = 0;
};

class IWebSocketServer {
public:
	virtual void start_blocking() = 0;
	virtual void start_async() = 0;
	virtual void start_read() = 0;
	virtual message_request read_message(int timeout) = 0;
	virtual void stop() = 0;
	virtual void send(const std::string message, const std::string id) = 0;
	virtual void addobserver(OcppObserver* observer) = 0;
	virtual void removeobserver(OcppObserver* observer) = 0;
	virtual void notifyobservers(const std::string message, std::string id) = 0;
	virtual void notifyObserversConnected(const std::string id) = 0;
	virtual void notifyObserversDisconnected(const std::string id) = 0;
	virtual bool get_running() = 0;
};


#endif // IWEBSOCKET_HPP
