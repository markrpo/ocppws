#ifndef IOCPPSERVER_HPP
#define IOCPPSERVER_HPP

#include <string>
#include <functional>
#include "iwebsocket.hpp"
#include "responses.hpp"

using onConnectCallbackOld = void (*)(const std::string&); // Callback function type for connection event a normal function or static member function can be used (less flexible than std::function, we cant use a lambda pasing (this))
using onConnectCallback = std::function<void(const std::string&)>; // Callback function type for connection event, with std::function we can use a lambda function or a normal function (std::function can receive any callable object, pointer (normal or static member function) or lambda function (object))
using onDisconnectCallback = std::function<void(const std::string&)>;

class IOCPPServer : public OcppObserver
{

	
public:

	using UserCallback = std::function<std::string(const std::string&)>; 			// using is a keyword to create an alias (like typedef)
	// UserCallback is a function that takes a string and returns a json object (for future we will use Response::userdata)
	virtual void add_user_callback(std::string key, UserCallback callback) = 0;
	virtual void add_old_on_connect_callback(onConnectCallbackOld callback) = 0;
	virtual void add_on_connect_callback(onConnectCallback callback) = 0;
	virtual void add_on_disconnect_callback(onDisconnectCallback callback) = 0;

};

#endif
