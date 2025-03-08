#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <libwebsockets.h>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "iwebsocket.hpp"

class WebSocketServer : public IWebSocketServer {
public:
    WebSocketServer(int port, std::string path);

    void start() override;
    void stop() override;

	void send(const std::string message) override;

	void addobserver(OcppObserver* observer) override;
	void removeobserver(OcppObserver* observer) override;
	void notifyobservers(const std::string message) override;

	struct per_session_data__minimal;

	struct per_vhost_data__minimal {
		struct lws_context *context;
		struct lws_vhost *vhost;
		const struct lws_protocols *protocol;

		struct WebSocketServer::per_session_data__minimal *pss_list;
	};

	struct per_session_data__minimal {
		struct lws *wsi;
		std::string buffer;
		std::vector<std::string> messages;

		struct WebSocketServer::per_session_data__minimal *pss_list;
	};



private:
	int port;
	std::string protocol;
	std::string path;

	struct message_request {
		std::string id;
		std::string action;
		std::string response;
	};

	std::vector<std::string> m_messages;

	struct lws_context *context;
	struct lws *wsi;

	static int lwscallback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

	static struct lws_protocols protocols[];

	std::vector<OcppObserver*> observers;

	std::mutex m_mutex;
	std::condition_variable m_condition;

	int m_running;

};

#endif // WEBSOCKET_HPP
