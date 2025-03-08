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
	virtual void start() = 0;	
	virtual void stop() = 0;
	virtual void send(const std::string message) = 0;
	virtual void addobserver(OcppObserver* observer) = 0;
	virtual void removeobserver(OcppObserver* observer) = 0;
	virtual void notifyobservers(const std::string message) = 0;
};


#endif // IWEBSOCKET_HPP
