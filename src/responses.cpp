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
	json response = json::object(); 
	return response.dump();
}

std::string Responses::MeterValuesResponse() {
	json response = json::object();
	return response.dump();
}

std::string Responses::AuthorizeResponse(Responses::IdTagInfoStatus status, std::string expiryDate) {
	json response;
	response["idTagInfo"]["status"] = (status == Responses::IdTagInfoStatus::Accepted) ? "Accepted" : (status == Responses::IdTagInfoStatus::Blocked) ? "Blocked" : "Expired";
	if (!expiryDate.empty()) {
		response["idTagInfo"]["expiryDate"] = expiryDate;
	}
	return response.dump();
}

std::string Responses::StartTransactionResponse(int transactionId, Responses::IdTagInfoStatus idTagInfoStatus) {
	json response;
	response["transactionId"] = transactionId;
	response["idTagInfo"]["status"] = (idTagInfoStatus == Responses::IdTagInfoStatus::Accepted) ? "Accepted" : (idTagInfoStatus == Responses::IdTagInfoStatus::Blocked) ? "Blocked" : "Expired";
	return response.dump();
}

std::string Responses::StopTransactionResponse(Responses::IdTagInfoStatus idTagInfoStatus) {
	json response;
	response["idTagInfo"]["status"] = (idTagInfoStatus == Responses::IdTagInfoStatus::Accepted) ? "Accepted" : (idTagInfoStatus == Responses::IdTagInfoStatus::Blocked) ? "Blocked" : "Expired";
	return response.dump();
}

std::string Responses::DataTransferResponse(Responses::DataTransferStatus status, std::string data) {
	json response;
	response["status"] = (status == Responses::DataTransferStatus::Accepted) ? "Accepted" : (status == Responses::DataTransferStatus::Rejected) ? "Rejected" : "Unknown";
	if (!data.empty()) {
		response["data"] = data;
	}
	return response.dump();
}

