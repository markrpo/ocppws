#include <iostream>
#include <cstring>
#include <csignal>

#include "server.hpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

OCPPServer::OCPPServer(int port, std::string path) {
    this->port = port;
    this->path = path;

	this->init_handlers();	
}

void OCPPServer::init_handlers() {
	this->handlers["BootNotification"] = [this](json& msg) -> std::string { 			// the lambda function captures the this pointer (OCPPServer object) and spects a json object as argument (returns a string)
		json callback_response;
		json response_json;
		std::string id = msg[1];

		auto it = this->user_callbacks.find("BootNotification");													// find is a method of the map class that returns an iterator (it) to the element if it is found
		if (it != this->user_callbacks.end()) {																		// when it is == to end() it means that the element was not found because it is the last element (end of the map)
			
			callback_response = it->second(msg.dump());																// it->second is the value of the element in the map (in this case the lambda user callback)
			std::cout << "Callback response: " << callback_response.dump() << std::endl;
			response_json = { 3, id, callback_response };
			std::cout << "Full response: " << response_json.dump() << std::endl;

		} else {

			std::cout << "User callback not found" << std::endl;

		}
		return response_json.dump();
	};

	this->handlers["StatusNotification"] = [this](json& msg) -> std::string {
		json callback_response;
		json response_json;
		std::string id = msg[1];

		auto it = this->user_callbacks.find("StatusNotification");
		if (it != this->user_callbacks.end()) {
			
			callback_response = it->second(msg.dump());
			if (callback_response.empty()) {
				std::cout << "Empty response" << std::endl;
				response_json = { 3, id, json::object() };
				std::cout << "Full response: " << response_json.dump() << std::endl;
			} else {
				std::cout << "Callback response: " << callback_response.dump() << std::endl;
				response_json = { 3, id, callback_response };
				std::cout << "Full response: " << response_json.dump() << std::endl;
			}

		} else {

			std::cout << "User callback not found" << std::endl;

		}
		return response_json.dump();
	};
	
	this->handlers["Heartbeat"] = [this](json& msg) -> std::string {
		json callback_response;
		json response_json;
		std::string id = msg[1];

		auto it = this->user_callbacks.find("Heartbeat");
		if (it != this->user_callbacks.end()) {
			
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response.dump() << std::endl;
			response_json = { 3, id, callback_response };
			std::cout << "Full response: " << response_json.dump() << std::endl;

		} else {

			std::cout << "User callback not found" << std::endl;

		}
		return response_json.dump();
	};
}

void OCPPServer::add_user_callback(std::string message_type, UserCallback callback) {
	this->user_callbacks[message_type] = callback;
}

OCPPServer::~OCPPServer() {
    std::cout << "Destructor" << std::endl;
}

struct OCPPServer::per_session_data__minimal {
	struct lws *wsi;
	std::vector<std::string> messages;

	std::string buffer;

	// pointer to the next pss
	struct per_session_data__minimal *pss_list;
};

struct OCPPServer::per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;
	
	// pointer to the first pss of the linked-list, the first pss points to the next pss
	struct per_session_data__minimal *pss_list;
};


int OCPPServer::lwscallback(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	// when establishing a connection pss is created. Each time a callback is called pss will point to the user data
	struct per_session_data__minimal *pss =	(struct per_session_data__minimal *)user;
	// lws_protocol_vh_priv_zalloc() is used to allocate memory for the vhost data based on the size of the struct we pass (in this case per_vhost_data__minimal)
	struct per_vhost_data__minimal *vhd = (struct per_vhost_data__minimal *) lws_protocol_vh_priv_get(lws_get_vhost(wsi), lws_get_protocol(wsi));
	// get the user context (in this case the OCPPServer object) even if the callback is part of the class it cant access the class members because it is static
	// (and static functions can't access non-static members!) so we need to pass the user context to the callback that is asigned when creating the context
	OCPPServer *server = (OCPPServer *)lws_context_user(lws_get_context(wsi));

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
			if (server->process_message(pss->buffer, data_len, pss) != 0) {
				printf("Error processing message\n");
			} else {
				pss->buffer.clear();
			}
		}
		break;
	}
	default:
		break;
	}

	return 0;
}

// Protocol struct
struct lws_protocols OCPPServer::protocols[] = {
    {
        "ocpp1.6",
        lwscallback,
        sizeof(struct per_session_data__minimal),
        128,
    },
    { NULL, NULL, 0, 0 } /* Terminador */
};

void OCPPServer::start() {
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

	lws_context_destroy(this->context);
	std::cout << "Server stopped" << std::endl;

}

void OCPPServer::stop() {

}

struct OCPPServer::message_request {
	std::string id;
    std::string message;
    std::string response;
};

int OCPPServer::process_message(std::string& message, size_t len, struct per_session_data__minimal *pss) {
	json ocpp_message;
	try {
		ocpp_message = json::parse(message);
	} catch (json::parse_error& e) {
		std::cerr << "Error parsing message: " << e.what() << std::endl;
		return 1;
	}
	// catch the message type example: [2,"93dbbb36-fbd3-4251-94f1-440ea6382234","BootNotification",{"chargePointModel":"WALLE","chargePointVendor":"ZIV"}]
	// if there is no message type return an error
	if (ocpp_message.size() < 3) {
		std::cerr << "Error: message type not found" << std::endl;
		return 1;
	}
	std::string message_type = ocpp_message[2];
	std::cout << "Message type: " << message_type << std::endl;

	auto it = this->handlers.find(message_type);
	if (it != this->handlers.end()) {
		std::string response = it->second(ocpp_message);
		if (response.empty()) {
			std::cout << "No response" << std::endl;
			return 1;
		} else {
			std::cout << "Response: " << response << std::endl;
			// if there is a response add it to the messages vector of the pss and call lws_callback_on_writable to send the message
			pss->messages.push_back(response);
			lws_callback_on_writable(pss->wsi);
		}
	} else {
		std::cerr << "Error: message type not found" << std::endl;
		return 1;
	}
    
	return 0;
}


void OCPPServer::send(std::string message) {

}

