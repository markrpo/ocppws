#ifndef IOCPPSERVER_HPP
#define IOCPPSERVER_HPP

#include <string>
#include <functional>
#include "iwebsocket.hpp"
#include "responses.hpp"

class IOCPPServer : public OcppObserver
{
	
public:

	using UserCallback = std::function<std::string(const std::string&)>; 			// using is a keyword to create an alias (like typedef)
	// UserCallback is a function that takes a string and returns a json object (for future we will use Response::userdata)
	virtual void add_user_callback(std::string key, UserCallback callback) = 0;	


};

#endif
