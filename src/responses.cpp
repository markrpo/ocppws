#include "responses.hpp"

std::string Responses::get_utc_time() {
	time_t now = time(0);
	tm *gmtm = gmtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.000Z", gmtm);
	return std::string(buffer);
}

json Responses::BootNotificationResponse(Responses::BootStatus status, std::string currentTime, int interval) {
	json response;
	response["status"] = (status == Responses::BootStatus::Accepted) ? "Accepted" : (status == Responses::BootStatus::Rejected) ? "Rejected" : "Unknown";
	response["currentTime"] = currentTime;
	response["interval"] = interval;
	return response;
}

json Responses::HeartbeatResponse(std::string currentTime) {
	json response;
	response["currentTime"] = currentTime;
	return response;
}

json Responses::StatusNotificationResponse() {
	json response;
	return response;
}
