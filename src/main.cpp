#include "ocpp.hpp"
#include "websocket.hpp"
#include <iostream>  

void add_callbacks(IOCPPServer &ocpp) {

	ocpp.add_user_callback("BootNotification", [](const std::string &payload) -> std::string { // lambda function  [] is used to capture the variables from the enclosing scope (in this case, the main function)
																						// The function takes a string as input and returns a string as output (-> is not necessary, but it makes the code more readable)
		std::cout << "BootNotification: " << payload << std::endl;
		json response = Responses::BootNotificationResponse(Responses::BootStatus::Accepted, Responses::get_utc_time() , 60);
		return response.dump();
	});

	ocpp.add_user_callback("Heartbeat", [](const std::string &payload) -> std::string {
		std::cout << "Heartbeat: " << payload << std::endl;
		json response = Responses::HeartbeatResponse(Responses::get_utc_time());
		return response.dump();
	});

	ocpp.add_user_callback("StatusNotification", [](const std::string &payload) -> std::string {
		std::cout << "StatusNotification: " << payload << std::endl;
		json response = Responses::StatusNotificationResponse();
		return response.dump();
	});

	std::cout << "Callbacks added" << std::endl;

}

int main() {
	std::string path = "/";
	WebSocketServer server(9000, path);
	OCPPServer ocpp(&server);
	server.addobserver(&ocpp);
	add_callbacks(ocpp);
	/* server.start_async(); // this starts the server in a separate thread (and it notifies de observer by its own)
	server.start_blocking(); // this starts the server in the main thread (and it notifies the observer by its own) */
	server.start_read(); // this starts the server in a separate thread and it does not notify the observer (the observer must be notified by the user)
	// example of how to read messages from the server:
	while (server.get_running()) {
		message_request msg = server.read_message(10);
		if (msg.message != "" && msg.id != "") {
			server.notifyobservers(msg.message, msg.id);
		}
		else {
			std::cout << "No message received (timeout)" << std::endl;
		}
	} 
	//server.start_blocking();
}
