#include "iocpp.hpp"
#include "jsons_headers.hpp"

std::string OcppJsons::heartbeatRequest() {
	return std::string((char*)v16_schemas_Heartbeat_json);
}

std::string OcppJsons::heartbeatResponse() {
	return std::string((char*)v16_schemas_HeartbeatResponse_json);
}

std::string OcppJsons::bootNotificationRequest() {
	return std::string((char*)v16_schemas_BootNotification_json);
}

std::string OcppJsons::bootNotificationResponse() {
	return std::string((char*)v16_schemas_BootNotificationResponse_json);
}

std::string OcppJsons::statusNotificationRequest() {
	return std::string((char*)v16_schemas_StatusNotification_json);
}

std::string OcppJsons::statusNotificationResponse() {
	return std::string((char*)v16_schemas_StatusNotificationResponse_json);
}

std::string OcppJsons::authorizeRequest() {
	return std::string((char*)v16_schemas_Authorize_json);
}

std::string OcppJsons::authorizeResponse() {
	return std::string((char*)v16_schemas_AuthorizeResponse_json);
}

std::string OcppJsons::startTransactionRequest() {
	return std::string((char*)v16_schemas_StartTransaction_json);
}

std::string OcppJsons::startTransactionResponse() {
	return std::string((char*)v16_schemas_StartTransactionResponse_json);
}

std::string OcppJsons::stopTransactionRequest() {
	return std::string((char*)v16_schemas_StopTransaction_json);
}

std::string OcppJsons::stopTransactionResponse() {
	return std::string((char*)v16_schemas_StopTransactionResponse_json);
}

std::string OcppJsons::meterValuesRequest() {
	return std::string((char*)v16_schemas_MeterValues_json);
}

std::string OcppJsons::meterValuesResponse() {
	return std::string((char*)v16_schemas_MeterValuesResponse_json);
}

std::string OcppJsons::dataTransferRequest() {
	return std::string((char*)v16_schemas_DataTransfer_json);
}

std::string OcppJsons::dataTransferResponse() {
	return std::string((char*)v16_schemas_DataTransferResponse_json);
}
