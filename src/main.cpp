#include "server.hpp"
#include "responses.hpp"
// to get utc time
#include <iostream>

int main() {
	std::string path = "/";
	OCPPServer server(9000, path);
	server.add_user_callback("BootNotification", [](const std::string &payload) -> json {		// lambda function  [] is used to capture the variables from the enclosing scope (in this case, the main function)
																								// The function takes a string as input and returns a string as output (-> is not necessary, but it makes the code more readable)
		std::cout << "BootNotification: " << payload << std::endl;
		json response = Responses::BootNotificationResponse(Responses::BootStatus::Accepted, Responses::get_utc_time() , 60);
		return response;
	});

	server.add_user_callback("Heartbeat", [](const std::string &payload) -> json {
		std::cout << "Heartbeat: " << payload << std::endl;
		json response = Responses::HeartbeatResponse(Responses::get_utc_time());
		return response;
	});

	server.add_user_callback("StatusNotification", [](const std::string &payload) -> json {
		std::cout << "StatusNotification: " << payload << std::endl;
		json response = Responses::StatusNotificationResponse();
		return response;
	});

	server.start();
}
