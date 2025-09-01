#include <iostream>
#include <cstring>
#include <csignal>

#include "valijson_nlohmann.hpp"
#include "nlohmann/json.hpp"		
#include "ocpp.hpp"
#include "ijson.hpp"

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

void OCPPServer::addZTimestamp(json& payload) {
	if (payload.contains("timestamp")) {
		std::string timestamp = payload["timestamp"];

		if (timestamp.back() == 'Z') {
			return; // already has 'Z'
		}

    	if (timestamp.size() >= 6) {
			std::string suffix = timestamp.substr(timestamp.size() - 6);
			if ((suffix[0] == '+' || suffix[0] == '-') &&
     			std::isdigit(suffix[1]) && std::isdigit(suffix[2]) &&
          		suffix[3] == ':' &&
         		std::isdigit(suffix[4]) && std::isdigit(suffix[5])) {
       			return; // already has timezone info
      		}
    	}	
		payload["timestamp"] = timestamp + "Z"; // append 'Z'	
	}
}

bool OCPPServer::validate_json(const json& message, const json& schema) {
	valijson::Schema schema_obj;
	valijson::SchemaParser parser;
	json messagePayload = message;

	addZTimestamp(messagePayload);

	valijson::adapters::NlohmannJsonAdapter schemaDocumentAdapter(schema);

	try {
		parser.populateSchema(schemaDocumentAdapter, schema_obj);
	} catch (std::exception& e) {
		return false;
	}

	valijson::Validator validator(valijson::Validator::kStrongTypes);
	valijson::ValidationResults results;
	valijson::adapters::NlohmannJsonAdapter messageAdapter(messagePayload);

	if (!validator.validate(schema_obj, messageAdapter, &results)) {
		std::cerr << "JSON validation failed: " << std::endl;
		for (auto it = results.begin(); it != results.end(); ++it) {
			std::cerr << it->description << std::endl;
		}
		std::cerr << "Message: " << message.dump() << std::endl;
		std::cerr << "Message with Z: " << messagePayload.dump() << std::endl;
		std::cerr << "Schema: " << schema.dump() << std::endl;
		return false;
	}

	return true;
}

void OCPPServer::make_call_error(const std::string& id, const std::string& errorCode, const std::string& errorDescription, json& response_json) {
	response_json.push_back(4);
	response_json.push_back(id);
	response_json.push_back(errorCode);
	response_json.push_back(errorDescription);
	response_json.push_back(json::object());
}

void OCPPServer::make_call_result(const std::string& id, const json& payload, json& response_json) {
	response_json.push_back(3);
	response_json.push_back(id);
	response_json.push_back(payload);
}

std::string OCPPServer::generic_handler(const std::string& message, json& ocppMessage) {

		std::string callback_response;
		json half_response_json;
		json response_json = json::array();													
		std::string id = ocppMessage[1];																	// the id is the second element of the message (the first is the version and the third is the message type)

		auto it = this->user_callbacks.find(message);													// find is a method of the map class that returns an iterator (it) to the element if it is found
		if (it != this->user_callbacks.end()) {																		// when it is == to end() it means that the element was not found because it is the last element (end of the map)
			
			callback_response = it->second(ocppMessage.dump());																// it->second is the value of the element in the map (in this case the lambda user callback)
			std::cout << "Callback response: " << callback_response << std::endl;
			if (get_json(callback_response, half_response_json) && !callback_response.empty()) {	// if the callback response is a valid json object
				std::cout << "Callback response is a valid json object" << std::endl;
				make_call_result(id, half_response_json, response_json);
			} else {
				std::cout << "Callback response is not a valid json object or empty" << std::endl;
				make_call_error(id, "NotImplemented", "This action is not supported", response_json);
			}
		} else {
			std::cout << "User callback not found" << std::endl;
			make_call_error(id, "NotImplemented", "This action is not supported", response_json);
		}
		return response_json.dump();	
}

void OCPPServer::init_handlers() {	

	this->handlers["BootNotification"] = [this](json& msg) -> std::string {
		json bootSchema;
	    get_json(OcppJsons::bootNotificationRequest(), bootSchema);
		if (!validate_json(msg[3], bootSchema)) {
			std::cerr << "Error: invalid BootNotification message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid BootNotification message", invalidResponse);
			return invalidResponse.dump();

		}
		return generic_handler("BootNotification", msg);
	};

	this->handlers["StatusNotification"] = [this](json& msg) -> std::string {
		json statusSchema;
	    get_json(OcppJsons::statusNotificationRequest(), statusSchema);
		if (!validate_json(msg[3], statusSchema)) {
			std::cerr << "Error: invalid StatusNotification message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid StatusNotification message", invalidResponse);
			return invalidResponse.dump();
		}
		return generic_handler("StatusNotification", msg);
	};
	
	this->handlers["Heartbeat"] = [this](json& msg) -> std::string {
		json heartSchema;
	    get_json(OcppJsons::heartbeatRequest(), heartSchema);
		if (!validate_json(msg[3], heartSchema)) {
			std::cerr << "Error: invalid Heartbeat message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid Heartbeat message", invalidResponse);
			return invalidResponse.dump();
		}
		return generic_handler("Heartbeat", msg);
	};

	this->handlers["Authorize"] = [this](json& msg) -> std::string {
		json authSchema;
	    get_json(OcppJsons::authorizeRequest(), authSchema);
		if (!validate_json(msg[3], authSchema)) {
			std::cerr << "Error: invalid Authorize message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid Authorize message", invalidResponse);
			return invalidResponse.dump();
		}
		return generic_handler("Authorize", msg);
	};

	this->handlers["StartTransaction"] = [this](json& msg) -> std::string {
		json startSchema;
	    get_json(OcppJsons::startTransactionRequest(), startSchema);
		if (!validate_json(msg[3], startSchema)) {
			std::cerr << "Error: invalid StartTransaction message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid StartTransaction message", invalidResponse);
			return invalidResponse.dump();
		}

		return generic_handler("StartTransaction", msg);
	};

	this->handlers["StopTransaction"] = [this](json& msg) -> std::string {
		json stopSchema;
	    get_json(OcppJsons::stopTransactionRequest(), stopSchema);
		if (!validate_json(msg[3], stopSchema)) {
			std::cerr << "Error: invalid StopTransaction message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid StopTransaction message", invalidResponse);
			return invalidResponse.dump();
		}
		return generic_handler("StopTransaction", msg);
	};

	this->handlers["MeterValues"] = [this](json& msg) -> std::string {
		json meterSchema;
	    get_json(OcppJsons::meterValuesRequest(), meterSchema);
		if (!validate_json(msg[3], meterSchema)) {
			std::cerr << "Error: invalid MeterValues message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid MeterValues message", invalidResponse);
			return invalidResponse.dump();
		}
		return generic_handler("MeterValues", msg);
	};

	this->handlers["DataTransfer"] = [this](json& msg) -> std::string {
		json dataSchema;
	    get_json(OcppJsons::dataTransferRequest(), dataSchema);
		if (!validate_json(msg[3], dataSchema)) {
			std::cerr << "Error: invalid DataTransfer message" << std::endl;
			json invalidResponse;
			make_call_error(msg[1], "ProtocolError", "Invalid DataTransfer message", invalidResponse);
			return invalidResponse.dump();
		}
		return generic_handler("DataTransfer", msg);
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
