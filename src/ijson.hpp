#ifndef IJSON_HPP
#define IJSON_HPP

#include <string>

class IJson
{
public:
    virtual ~IJson() = default;

    virtual bool parse(const std::string& jsonString) = 0;
    virtual bool verifySchema(const std::string& schemaString, const std::string& jsonString) = 0;
	virtual bool isSizeHigher(const std::string& jsonString, size_t size) = 0;

	virtual bool getOcppMessageType(const std::string& jsonString, int& messageType) = 0;
	virtual bool getOcppId(const std::string& jsonString, std::string& ocppId) = 0;
	virtual bool getOcppAction(const std::string& jsonString, std::string& ocppAction) = 0;
	virtual bool getOcppPayload(const std::string& jsonString, std::string& payload) = 0;

	virtual std::string generateOcppMessage(int messageType, 
					    const std::string& ocppId, 
					    const std::string& payload) = 0;
};

class OcppJsons
{
public:
	static std::string bootNotificationRequest();
	static std::string bootNotificationResponse();
	static std::string heartbeatRequest();
	static std::string heartbeatResponse();
	static std::string statusNotificationRequest();
	static std::string statusNotificationResponse();
	static std::string authorizeRequest();
	static std::string authorizeResponse();
	static std::string startTransactionRequest();
	static std::string startTransactionResponse();
	static std::string stopTransactionRequest();
	static std::string stopTransactionResponse();
	static std::string meterValuesRequest();
	static std::string meterValuesResponse();
	static std::string dataTransferRequest();
	static std::string dataTransferResponse();
};

#endif // IJSON_HPP
