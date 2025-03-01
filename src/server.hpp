#include <libwebsockets.h>
#include <iostream>
#include <string>
#include <vector>
// map
#include <map> 
#include <functional>
#include <memory>

class OCPPServer {
	
public:
	OCPPServer(int port, std::string path);
	~OCPPServer();

	using UserCallback = std::function<std::string(const std::string&)>; 			// using is a keyword to create an alias (like typedef)
	using Handler = std::function<std::string(const std::string&)>;

	void add_user_callback(std::string key, UserCallback callback);

	void start();
	void stop();
	void send(std::string message);

private:
	int port;
	std::string protocol; // unused
	std::string path;

	struct per_vhost_data__minimal;
	struct per_session_data__minimal;
	struct message_request;
	
	std::vector<struct message_request> message_queue;

	struct lws_context *context;
	struct lws *wsi;
	static struct lws_protocols protocols[];
	static int lwscallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);
	int process_message(char* message, size_t len, struct per_session_data__minimal* pss);

	void init_handlers();

	std::map<std::string, UserCallback> user_callbacks;
	std::map<std::string, Handler> handlers;


};
