#include <libwebsockets.h>
#include <vector>
// map
#include <map> 
#include <functional>
#include "iwebsocket.hpp"

#include "responses.hpp"

using json = nlohmann::json;

#ifndef OCPPSERVER_HPP
#define OCPPSERVER_HPP

class OCPPServer : public OcppObserver {
	
public:
	OCPPServer(IWebSocketServer* ws) : ws(ws) {
		init_handlers();
	}
	~OCPPServer();

	using UserCallback = std::function<json(const std::string&)>; 			// using is a keyword to create an alias (like typedef)
	// UserCallback is a function that takes a string and returns a json object (for future we will use Response::userdata)
	using Handler = std::function<std::string(json&)>; 

	void add_user_callback(std::string key, UserCallback callback);

	void notify(const std::string message) override;


private:
	
	int process_message(std::string& message, size_t len);

	void init_handlers();

	std::map<std::string, UserCallback> user_callbacks;
	std::map<std::string, Handler> handlers;

	IWebSocketServer* ws;

};

#endif
