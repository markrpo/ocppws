#include "service.hpp"
#include <iostream>

void Service::add_callbacks() {

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

	std::cout << "Callbacks added" << std::endl;

}

void Service::initAndRun() {
	add_callbacks();
	m_server.start_blocking(); // Notifies observer by itself
	// server.start_async();
	/*m_server.start_read(); // this starts the server in a separate thread and it does not notify the observer (the observer must be notified by the user)
	while (m_server.get_running()) {
		message_request msg = m_server.read_message(10);
		if (msg.message != "" && msg.id != "") {
			m_server.notifyobservers(msg.message, msg.id);
		}
		else {
			std::cout << "No message received (timeout)" << std::endl;
		}
	} */
}
