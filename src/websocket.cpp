#include <iostream>
#include <cstring>
#include <csignal>

#include "websocket.hpp"

WebSocketServer::WebSocketServer(int port, std::string path) {
    this->port = port;
    this->path = path;
}

std::string WebSocketServer::getpath(struct lws *wsi) {
	int uri_len = lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI);
	if (uri_len == 0) {
		return "";
	}
	char *uri = (char *)malloc(uri_len + 1);
	if (!uri) {
		return "";
	}
	if (lws_hdr_copy(wsi, uri, uri_len + 1, WSI_TOKEN_GET_URI) != uri_len) {
		free(uri);
		return "";
	}
	std::string uri_str(uri);
	free(uri);
	return uri_str;
}

int WebSocketServer::lwscallback(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	// when establishing a connection pss is created. Each time a callback is called pss will point to the user data
	struct WebSocketServer::per_session_data__minimal *pss =	(struct WebSocketServer::per_session_data__minimal *)user;
	// lws_protocol_vh_priv_zalloc() is used to allocate memory for the vhost data based on the size of the struct we pass (in this case per_vhost_data__minimal)
	struct WebSocketServer::per_vhost_data__minimal *vhd = (struct WebSocketServer::per_vhost_data__minimal *) lws_protocol_vh_priv_get(lws_get_vhost(wsi), lws_get_protocol(wsi));
	// get the user context (in this case the OCPPServer object) even if the callback is part of the class it cant access the class members because it is static
	// (and static functions can't access non-static members!) so we need to pass the user context to the callback that is asigned when creating the context
	WebSocketServer *server = (WebSocketServer *)lws_context_user(lws_get_context(wsi));

	//std::unique_lock<std::mutex> lock(server->m_mutex_messages);

	int m;
	
	switch (reason) {
	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
		{	
		printf("Filter protocol connection\n");

		std::string uri_str = server->getpath(wsi);

		// get the text after the last slash
		std::string id = uri_str.substr(uri_str.find_last_of("/") + 1);
		pss->id = id;
		// check if any chargebox with the same id is already connected
		struct WebSocketServer::per_session_data__minimal *pss2 = vhd->pss_list;
		while (pss2) {
			if (pss2->id == id) {
				printf("Chargebox with id %s already connected\n", id.c_str());
				return -1;
			}
			pss2 = pss2->pss_list;
		}
		}
		break;

	case LWS_CALLBACK_PROTOCOL_INIT:
		printf("Protocol initialized\n");
		vhd = (struct WebSocketServer::per_vhost_data__minimal* )lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi), lws_get_protocol(wsi), sizeof(struct WebSocketServer::per_vhost_data__minimal));
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
		lws_ll_fwd_remove(struct WebSocketServer::per_session_data__minimal, pss_list, pss, vhd->pss_list);
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
		lws_cancel_service(server->context);
		break;

	case LWS_CALLBACK_RECEIVE:
		{
		printf("Message received from chargebox %s\n", pss->id.c_str());
		unsigned char *data = (unsigned char *)in;
		size_t data_len = len;
		pss->buffer.append((char *)data, data_len);
		if (lws_is_final_fragment(wsi)) {
				printf("full message: %s\n", pss->buffer.c_str());
				std::unique_lock<std::mutex> lock(server->m_mutex);
				message_request message(pss->id, pss->buffer); 
				server->m_messages.push_back(message);
				server->m_condition.notify_one();
				pss->buffer.clear();
		}
		break;
	}
	case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
		std::cout << "Event wait cancelled, size of messages: " << server->m_messages_write.size() << std::endl;
		if (!server->m_messages_write.empty()) {
			message_request message = server->m_messages_write.front();
			server->m_messages_write.erase(server->m_messages_write.begin());
			WebSocketServer::per_session_data__minimal *pss = vhd->pss_list;
			while (pss) {
				if (pss->id == message.id) {
					pss->messages.push_back(message.message);
					lws_callback_on_writable(pss->wsi);
					break;
				}
				pss = pss->pss_list;
			}
		}
		break;
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
        sizeof(struct WebSocketServer::per_session_data__minimal), // per_session_data_size
        1024, // rx_buffer_size
    },
    { NULL, NULL, 0, 0 } /* Terminador */
};

void WebSocketServer::start_blocking() {
	
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

	m_running = true;

	std::thread t1([this] {
		std::unique_lock<std::mutex> lock(m_mutex);

		while(m_running){
			m_condition.wait(lock);
			std::cout << "Process thread notified" << std::endl;
			while(!m_messages.empty()){
				std::string message = m_messages.front().message;
				std::string id = m_messages.front().id;
				m_messages.erase(m_messages.begin());
				lock.unlock();
				this->notifyobservers(message, id);
				lock.lock(); 
			}
		}
	});


	
	while (n >= 0) {
		n = lws_service(this->context, 0);
	}

	m_running = false;
	m_condition.notify_all();

	lws_context_destroy(this->context);
	std::cout << "Server stopped" << std::endl;
}

void WebSocketServer::start_async() {
    m_running = true;
    m_thread = std::thread(&WebSocketServer::start_blocking, this);
	m_thread.detach();
}

void WebSocketServer::start_read() {
		m_running = true;

		m_thread = std::thread([this] {
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

		while (n >= 0) {
			n = lws_service(this->context, 0);
		}

		m_running = false;
		m_condition.notify_all();

		lws_context_destroy(this->context);
		std::cout << "Server stopped" << std::endl;
	});

	m_thread.detach();
}

message_request WebSocketServer::read_message(int timeout) {
	std::unique_lock<std::mutex> lock(m_mutex);
	if (!m_messages.empty()) {
			message_request message = m_messages.front();
		m_messages.erase(m_messages.begin());
		return message;
	}
	m_condition.wait_for(lock, std::chrono::seconds(timeout));
	if (m_messages.empty()) {
		std::cout << "Timeout" << std::endl;
		return message_request("", "");
	}
	std::cout << "Message received" << std::endl;
	message_request message = m_messages.front();
	m_messages.erase(m_messages.begin());
	return message;
}

bool WebSocketServer::get_running() {
    return m_running;
}


void WebSocketServer::stop() {

}

void WebSocketServer::send(const std::string message, const std::string id) {
	std::cout << "Sending message to chargebox " << id << ": " << message << std::endl;
	std::unique_lock<std::mutex> lock(m_mutex_messages);
	m_messages_write.push_back(message_request(id, message));
	lws_cancel_service(this->context);
}

void WebSocketServer::addobserver(OcppObserver *observer) {
    this->observers.push_back(observer);
}

void WebSocketServer::removeobserver(OcppObserver *observer) {
    this->observers.erase(std::remove(this->observers.begin(), this->observers.end(), observer), this->observers.end());
}

void WebSocketServer::notifyobservers(std::string message, std::string id) {
    for (auto observer : this->observers) {
		observer->notify(message, id);
    }
}

