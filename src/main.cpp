#include "server.hpp"

int main() {
	std::string path = "/";
	OCPPServer server(9000, path);
	server.add_user_callback("BootNotification", [](const std::string &payload) -> std::string { 
		std::cout << "BootNotification: " << payload << std::endl;
		return "{\"status\":\"Accepted\"}";
	});
	server.start();
}
