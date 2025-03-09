#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <libwebsockets.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>

#include "iwebsocket.hpp"

class WebSocketServer : public IWebSocketServer {
public:
    WebSocketServer(int port, std::string path);

    void start_blocking() override;
	void start_async() override;
	void start_read() override;
	message_request read_message(int timeout) override;
	void stop() override;

	void send(const std::string message, const std::string id) override;

	void addobserver(OcppObserver* observer) override;
	void removeobserver(OcppObserver* observer) override;
	void notifyobservers(const std::string message, std::string id) override;

	bool get_running() override;

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
		std::string id;

		struct WebSocketServer::per_session_data__minimal *pss_list;
	};



private:
	int port;
	std::string protocol;
	std::string path;

	std::vector<message_request> m_messages;
	std::vector<message_request> m_messages_write;

	struct lws_context *context;
	struct lws *wsi;

	static std::string getpath(struct lws *wsi);
	static std::string getid(std::string path);
	static int lwscallback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

	static struct lws_protocols protocols[];

	std::vector<OcppObserver*> observers;

	std::mutex m_mutex;
	std::condition_variable m_condition;

	std::mutex m_mutex_messages;
	std::condition_variable m_condition_messages;

	std::thread m_thread;

	bool m_running;
};

#endif // WEBSOCKET_HPP
