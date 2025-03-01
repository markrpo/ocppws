#include "server.hpp"

int main() {
	std::string path = "/";
	OCPPServer server(9000, path);
	server.add_user_callback("BootNotification", [](const std::string &payload) -> std::string {		// lambda function  [] is used to capture the variables from the enclosing scope (in this case, the main function)
																										// The function takes a string as input and returns a string as output (-> is not necessary, but it makes the code more readable)
		std::cout << "BootNotification: " << payload << std::endl;
		return "{\"status\":\"Accepted\"}";
	});
	server.start();
}
