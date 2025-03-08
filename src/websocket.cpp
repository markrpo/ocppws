#include <iostream>
#include <cstring>
#include <csignal>
#include <thread>

#include "websocket.hpp"

WebSocketServer::WebSocketServer(int port, std::string path) {
    this->port = port;
    this->path = path;
}

struct WebSocketServer::per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	struct per_session_data__minimal *pss_list;
};

struct WebSocketServer::per_session_data__minimal {
	struct lws *wsi;
	std::string buffer;
	std::vector<std::string> messages;

	struct per_session_data__minimal *pss_list;
};

int WebSocketServer::lwscallback(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	// when establishing a connection pss is created. Each time a callback is called pss will point to the user data
	struct per_session_data__minimal *pss =	(struct per_session_data__minimal *)user;
	// lws_protocol_vh_priv_zalloc() is used to allocate memory for the vhost data based on the size of the struct we pass (in this case per_vhost_data__minimal)
	struct per_vhost_data__minimal *vhd = (struct per_vhost_data__minimal *) lws_protocol_vh_priv_get(lws_get_vhost(wsi), lws_get_protocol(wsi));
	// get the user context (in this case the OCPPServer object) even if the callback is part of the class it cant access the class members because it is static
	// (and static functions can't access non-static members!) so we need to pass the user context to the callback that is asigned when creating the context
	WebSocketServer *server = (WebSocketServer *)lws_context_user(lws_get_context(wsi));

	int m;
	
	switch (reason) {
	case LWS_CALLBACK_PROTOCOL_INIT:
		printf("Protocol initialized\n");
		vhd = (struct per_vhost_data__minimal* )lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi), lws_get_protocol(wsi), sizeof(struct per_vhost_data__minimal));
		vhd->context = lws_get_context(wsi);
		vhd->vhost = lws_get_vhost(wsi);
		vhd->protocol = lws_get_protocol(wsi);
		break;

	case LWS_CALLBACK_ESTABLISHED:
		printf("Connection established\n");
	   	// add the new pss to the linked-list:
		lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
		pss->wsi = wsi;
		break;

	case LWS_CALLBACK_CLOSED:
		printf("Connection closed\n");
		// remove the pss from the linked-list
		lws_ll_fwd_remove(struct per_session_data__minimal, pss_list, pss, vhd->pss_list);
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		printf("Server writeable\n");
		if (pss->messages.size() > 0) {
			std::string message = pss->messages.back(); // get the last message
			pss->messages.pop_back(); // remove the last message
			unsigned char *buf = (unsigned char *)malloc(LWS_SEND_BUFFER_PRE_PADDING + strlen(message.c_str()) + LWS_SEND_BUFFER_POST_PADDING);
			if (!buf) {
				return -1;
			}
			memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING, message.c_str(), strlen(message.c_str()));
			m = lws_write(wsi, buf + LWS_SEND_BUFFER_PRE_PADDING, strlen(message.c_str()), LWS_WRITE_TEXT);
			free(buf);
		}
		break;

	case LWS_CALLBACK_RECEIVE:{
		printf("Received message\n");
		unsigned char *data = (unsigned char *)in;
		size_t data_len = len;
		pss->buffer.append((char *)data, data_len);
		if (lws_is_final_fragment(wsi)) {
				printf("full message: %s\n", pss->buffer.c_str());
				std::unique_lock<std::mutex> lock(server->m_mutex);
				server->m_messages.push_back(pss->buffer);
				server->m_condition.notify_one();
				pss->buffer.clear();
		}
		break;
	}
	default:
		break;
	}

	return 0;
}

// Protocol struct
struct lws_protocols WebSocketServer::protocols[] = {
    {
        "ocpp1.6",
        lwscallback,
        sizeof(struct per_session_data__minimal),
        128,
    },
    { NULL, NULL, 0, 0 } /* Terminador */
};

void WebSocketServer::start() {
	struct lws_context_creation_info info;
	int n = 0;

	memset(&info, 0, sizeof info);
	info.port = this->port;
	info.protocols = this->protocols;
	info.vhost_name = "OCPPServer";
	info.user = this;
	info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

	this->context = lws_create_context(&info);
	if (!this->context) {
		fprintf(stderr, "Error creating libwebsocket context\n");
		return;
	}

	std::thread t1([this] {
		std::unique_lock<std::mutex> lock(m_mutex);

		while(m_running){
			m_condition.wait(lock);
			std::cout << "Process thread notified" << std::endl;
			while(!m_messages.empty()){
			std::string message = m_messages.front();
				m_messages.erase(m_messages.begin());
				lock.unlock();
				this->notifyobservers(message);
				lock.lock(); 
			}
		}
	});


	
	while (n >= 0) {
		n = lws_service(this->context, 0);
	}

	lws_context_destroy(this->context);
	std::cout << "Server stopped" << std::endl;

}

void WebSocketServer::stop() {

}

void WebSocketServer::send(std::string message) {

}

void WebSocketServer::addobserver(OcppObserver *observer) {
    this->observers.push_back(observer);
}

void WebSocketServer::removeobserver(OcppObserver *observer) {
    this->observers.erase(std::remove(this->observers.begin(), this->observers.end(), observer), this->observers.end());
}

void WebSocketServer::notifyobservers(std::string message) {
    for (auto observer : this->observers) {
		observer->notify(message);
    }
}
