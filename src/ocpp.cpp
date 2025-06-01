#include <iostream>
#include <cstring>
#include <csignal>

#include "nlohmann/json.hpp"		
#include "ocpp.hpp"

using json = nlohmann::json;

bool OCPPServer::get_json(const std::string& message, json& ocppMessage) {
	try {
		ocppMessage = json::parse(message);
	} catch (json::parse_error& e) {
		std::cerr << "Error parsing message: " << e.what() << std::endl;
		return false;
	} catch (json::type_error& e) {
		std::cerr << "Error parsing message: " << e.what() << std::endl;
		return false;
	} catch (json::other_error& e) {
		std::cerr << "Error parsing message: " << e.what() << std::endl;
		return false;
	} catch (std::exception& e) {
		std::cerr << "Error parsing message: " << e.what() << std::endl;
		return false;
	}
	return true;
}

void OCPPServer::init_handlers() {

	this->handlers["BootNotification"] = [this](json& msg) -> std::string { 			// the lambda function captures the this pointer (OCPPServer object) and spects a json object as argument (returns a string)
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();													
		std::string id = msg[1];

		auto it = this->user_callbacks.find("BootNotification");													// find is a method of the map class that returns an iterator (it) to the element if it is found
		if (it != this->user_callbacks.end()) {																		// when it is == to end() it means that the element was not found because it is the last element (end of the map)
			
			callback_response = it->second(msg.dump());																// it->second is the value of the element in the map (in this case the lambda user callback)
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {													// if the callback response is a valid json object
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);													
				response_json.push_back(id);								
				response_json.push_back(half_response_json);													

			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}

		} else {

			std::cout << "User callback not found" << std::endl;

		}
		return response_json.dump();
	};

	this->handlers["StatusNotification"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("StatusNotification");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
		} else {
			std::cout << "User callback not found" << std::endl;
		}
		return response_json.dump();
	};
	
	this->handlers["Heartbeat"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("Heartbeat");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
		} else {
			std::cout << "User callback not found" << std::endl;
		}
		return response_json.dump();
	};

	this->handlers["Authorize"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("Authorize");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
		} else {
			std::cout << "User callback not found" << std::endl;
		}
		return response_json.dump();
	};

	this->handlers["StartTransaction"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("StartTransaction");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
		} else {
			std::cout << "User callback not found" << std::endl;
		}
		return response_json.dump();
	};

	this->handlers["StopTransaction"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("StopTransaction");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
		} else {
			std::cout << "User callback not found" << std::endl;
		}
		return response_json.dump();
	};

	this->handlers["MeterValues"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("MeterValues");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
		} else {
			std::cout << "User callback not found" << std::endl;
		}
		return response_json.dump();
	};

	this->handlers["DataTransfer"] = [this](json& msg) -> std::string {
		std::string callback_response;
		json half_response_json;
		json response_json = json::array();
		std::string id = msg[1];

		auto it = this->user_callbacks.find("DataTransfer");
		if (it != this->user_callbacks.end()) {
			callback_response = it->second(msg.dump());
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json)) {
				std::cout << "Callback response is a valid json object" << std::endl;
				response_json.push_back(3);
				response_json.push_back(id);
				response_json.push_back(half_response_json);
			} else {
				std::cout << "Callback response is not a valid json object" << std::endl;
			}
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


std::string OCPPServer::process_message(std::string& message, size_t len) {
	json ocpp_message;
	try {
		ocpp_message = json::parse(message);
	} catch (json::parse_error& e) {
		std::cerr << "Error parsing message: " << e.what() << std::endl;
		return "";
	}
	// catch the message type example: [2,"93dbbb36-fbd3-4251-94f1-440ea6382234","BootNotification",{"chargePointModel":"WALLE","chargePointVendor":"ZIV"}]
	// if there is no message type return an error
	if (ocpp_message.size() < 3) {
		std::cerr << "Error: message type not found" << std::endl;
		return "";
	}
	std::string message_type = ocpp_message[2];
	std::cout << "Message type: " << message_type << std::endl;

	auto it = this->handlers.find(message_type);
	if (it != this->handlers.end()) {
		std::string response = it->second(ocpp_message);
		if (response.empty()) {
			std::cout << "No response" << std::endl;
			return "";
		} else {
			std::cout << "Response: " << response << std::endl;
			return response;
		}
	} else {
		std::cerr << "Error: message type not found" << std::endl;
		return "";
	}
    
	return "";
}

void OCPPServer::notify(std::string message, std::string id) {
	std::cout << "Notified: " << message << "From charger: " << id << std::endl;
	std::string response = OCPPServer::process_message(message, message.size());
	m_ws->send(response, id);
}

void OCPPServer::notifyConnected(std::string id) {
	std::cout << "Charger connected: " << id << std::endl;
	if (this->on_connect_callback != nullptr) {
		this->on_connect_callback(id);
	}
	if (this->old_on_connect_callback != nullptr) {
		this->old_on_connect_callback(id);
	}
}

void OCPPServer::notifyDisconnected(std::string id) {
	std::cout << "Charger disconnected: " << id << std::endl;
	if (this->on_disconnect_callback != nullptr) {
		this->on_disconnect_callback(id);
	}
}

void OCPPServer::add_on_connect_callback(onConnectCallback callback) {
	this->on_connect_callback = callback;
}

void OCPPServer::add_on_disconnect_callback(onDisconnectCallback callback) {
	this->on_disconnect_callback = callback;
}

void OCPPServer::add_old_on_connect_callback(onConnectCallbackOld callback) {
	this->old_on_connect_callback = callback;
}
