#include "service.hpp"
#include <iostream>

void Service::add_callbacks() {

	/* This two examples only work with static member functions or normal functions
	 * m_ocpp.add_old_on_connect_callback(onConnection);
	 * m_ocpp.add_on_disconnect_callback(onDisconnection);	*/
	m_ocpp.add_on_connect_callback([this](const std::string &id) {
		std::cout << "New connection established with ID: " << id << std::endl;
	});
	m_ocpp.add_on_disconnect_callback([this](const std::string &id) {
		std::cout << "Connection with ID: " << id << " has been disconnected" << std::endl;
	});

	m_ocpp.add_user_callback("BootNotification", [](const std::string &payload) -> std::string { // lambda function  [] is used to capture the variables from the enclosing scope (in this case, the main function)
																						// The function takes a string as input and returns a string as output (-> is not necessary, but it makes the code more readable)
		std::cout << "BootNotification: " << payload << std::endl;
		std::string response = Responses::BootNotificationResponse(Responses::BootStatus::Accepted, Responses::get_utc_time() , 60);
		return response;
	});

	m_ocpp.add_user_callback("Heartbeat", [](const std::string &payload) -> std::string {
		std::cout << "Heartbeat: " << payload << std::endl;
		std::string response = Responses::HeartbeatResponse(Responses::get_utc_time());
		return response;
	});

	m_ocpp.add_user_callback("StatusNotification", [](const std::string &payload) -> std::string {
		std::cout << "StatusNotification: " << payload << std::endl;
		std::string response = Responses::StatusNotificationResponse();
		return response;
	});

	m_ocpp.add_user_callback("Authorize", [](const std::string &payload) -> std::string {
		std::cout << "Authorize: " << payload << std::endl;
		std::string response = Responses::AuthorizeResponse(Responses::IdTagInfoStatus::Accepted, Responses::get_utc_time());
		return response;
	});

	m_ocpp.add_user_callback("MeterValues", [](const std::string &payload) -> std::string {
		std::cout << "MeterValues: " << payload << std::endl;
		std::string response = Responses::MeterValuesResponse();
		return response;
	});

	m_ocpp.add_user_callback("StartTransaction", [this](const std::string &payload) -> std::string {
		std::cout << "StartTransaction: " << payload << std::endl;
		this->m_transaction = true;
		std::string response = Responses::StartTransactionResponse(234, Responses::IdTagInfoStatus::Accepted);
		return response;
	});

	m_ocpp.add_user_callback("StopTransaction", [](const std::string &payload) -> std::string {
		std::cout << "StopTransaction: " << payload << std::endl;
		std::string response = Responses::StopTransactionResponse(Responses::IdTagInfoStatus::Accepted);
		return response;
	});

	std::cout << "Callbacks added" << std::endl;

}

void Service::initAndRun() {
	add_callbacks();
	m_server.start_blocking(); // Notifies observer by itself
	/*m_server.start_read(); // this starts the server in a separate thread and it does not notify the observer (the observer must be notified by the user)
	while (m_server.get_running()) {
		message_request msg = m_server.read_message(10);
		if (msg.message != "" && msg.id != "") {
			m_server.notifyobservers(msg.message, msg.id);
		}
		else {
			std::cout << "No message received (timeout)" << std::endl;
		}
	} 
	m_server.start_async(); // this starts the server in a separate thread and it does notify the observer
	while (m_server.get_running()) {
		std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep to avoid busy waitin
		if (m_transaction) {
			std::cout << "Transaction is active" << std::endl;

		}
		else {
			std::cout << "No active transaction" << std::endl;
		}
	}*/
}
