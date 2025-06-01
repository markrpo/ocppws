#include <libwebsockets.h>
#include <vector>

#include <map> 
#include <functional>
#include <nlohmann/json.hpp>

#include "responses.hpp"
#include "iwebsocket.hpp"
#include "iocpp.hpp"

using json = nlohmann::json;

#ifndef OCPPSERVER_HPP
#define OCPPSERVER_HPP


class OCPPServer : public IOCPPServer
{
	
public:
	OCPPServer(IWebSocketServer* ws) : m_ws(ws) {
		m_ws->addobserver(this);
		init_handlers();
	}
	~OCPPServer();

	// UserCallback is a function that takes a string and returns a json object (for future we will use Response::userdata)
	using Handler = std::function<std::string(json&)>; 


	void add_user_callback(std::string key, UserCallback callback) final; // from iocpp.hpp
	void notify(const std::string message, std::string id) final; // from OcppObserver
	void notifyConnected(const std::string id) final; // from OcppObserver
	void notifyDisconnected(const std::string id) final; // from OcppObserver
	void add_old_on_connect_callback(onConnectCallbackOld callback) final; // from iocpp.hpp
	void add_on_connect_callback(onConnectCallback callback) final; // from iocpp.hpp
	void add_on_disconnect_callback(onDisconnectCallback callback) final; // from iocpp.hpp


private:
	
	std::string process_message(std::string& message, size_t len);
	bool get_json(const std::string& message, json& j);

	void init_handlers();

	std::map<std::string, UserCallback> user_callbacks;
	std::map<std::string, Handler> handlers;


	onConnectCallbackOld old_on_connect_callback = nullptr;
	onConnectCallback on_connect_callback = nullptr;
	onDisconnectCallback on_disconnect_callback = nullptr;

	IWebSocketServer* m_ws;


};

#endif
