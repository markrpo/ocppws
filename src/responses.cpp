#include "responses.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

std::string Responses::get_utc_time() {
	time_t now = time(0);
	tm *gmtm = gmtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.000Z", gmtm);
	return std::string(buffer);
}

std::string Responses::BootNotificationResponse(Responses::BootStatus status, std::string currentTime, int interval) {
	json response;
	response["status"] = (status == Responses::BootStatus::Accepted) ? "Accepted" : (status == Responses::BootStatus::Rejected) ? "Rejected" : "Unknown";
	response["currentTime"] = currentTime;
	response["interval"] = interval;
	return response.dump();
}

std::string Responses::HeartbeatResponse(std::string currentTime) {
	json response;
	response["currentTime"] = currentTime;
	return response.dump();
}

std::string Responses::StatusNotificationResponse() {
	json response;
	return response.dump();
}

