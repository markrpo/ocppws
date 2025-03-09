#include <iostream>
#include <cstring>
#include <csignal>

#include "ocpp.hpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

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

void OCPPServer::notify(std::string message) {
	std::cout << "Notified: " << message << std::endl;
	std::string response = OCPPServer::process_message(message, message.size());
	ws->send(response);
}
