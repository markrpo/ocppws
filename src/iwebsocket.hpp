#ifndef IWEBSOCKET_HPP
#define IWEBSOCKET_HPP

#include <string>
#include <functional>

class OcppObserver {
public:
	virtual void notify(const std::string message) = 0;
};

class IWebSocketServer {
public:
	virtual void start_blocking() = 0;
	virtual void start_async() = 0;
	virtual void start_read() = 0;
	virtual std::string read_message(int timeout) = 0;
	virtual void stop() = 0;
	virtual void send(const std::string message) = 0;
	virtual void addobserver(OcppObserver* observer) = 0;
	virtual void removeobserver(OcppObserver* observer) = 0;
	virtual void notifyobservers(const std::string message) = 0;
	virtual bool get_running() = 0;
};


#endif // IWEBSOCKET_HPP
