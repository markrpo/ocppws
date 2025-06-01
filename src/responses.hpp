#include <string>
#include <variant>
#include <optional>

#ifndef RESPONSES_HPP
#define RESPONSES_HPP

class Responses {
public:
	static std::string get_utc_time();

	enum class BootStatus { Accepted, Pending, Rejected };
	struct BootNotificationData { 
		std::string chargingStation;
		std::string chargePointVendor;
		std::string chargePointModel;
		std::string chargePointSerialNumber;
		std::string chargeBoxSerialNumber;
		std::string firmwareVersion;
		std::string iccid;
		std::string imsi;
		std::string meterType;
		std::string meterSerialNumber;
		std::string meterFirmwareVersion;
		std::string vendorId;
		std::string vendorErrorCode;
	};
    static std::string BootNotificationResponse(Responses::BootStatus status, std::string currentTime, int interval);

	struct HeartbeatData {
	};
	static std::string HeartbeatResponse(std::string currentTime);

	struct StatusNotificationData {
		std::string connectorId;
		std::string errorCode;
		std::string info;
		std::string status;
		std::string timestamp;
	};
	static std::string StatusNotificationResponse();

	enum class IdTagInfoStatus { Accepted, Blocked, Expired, Invalid, ConcurrentTx };
	static std::string StartTransactionResponse(int transactionId, Responses::IdTagInfoStatus idTagInfoStatus);
	static std::string StopTransactionResponse(Responses::IdTagInfoStatus idTagInfoStatus);
	static std::string MeterValuesResponse();
	static std::string AuthorizeResponse(Responses::IdTagInfoStatus idTagInfoStatus, std::string expiryDate);

	enum class DataTransferStatus { Accepted, Rejected, UnknownMessageId, UnknownVendorId };
	static std::string DataTransferResponse(Responses::DataTransferStatus status, std::string data);


	using Usserdata = std::variant<BootNotificationData, HeartbeatData, StatusNotificationData>; // using creates an alias for a type in this case a variant
	// other option is to define a struct of usserdata with optional fields
	struct Usserdataoptional {
		std::string raw_message;
		std::optional<BootNotificationData> bootNotificationData;
		std::optional<HeartbeatData> heartbeatData;
		std::optional<StatusNotificationData> statusNotificationData;
	};
};

#endif // RESPONSES_HPP
