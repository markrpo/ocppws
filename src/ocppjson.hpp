#ifndef OCPPJSON_HPP
#define OCPPJSON_HPP

#include "ijson.hpp"
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

class OcppJson : public IJson
{
public:
    OcppJson() = default;
    ~OcppJson() override = default;

    bool parse(const std::string& jsonString) final;
    bool verifySchema(const std::string& schemaString, const std::string& jsonString) final;
	bool isSizeHigher(const std::string& jsonString, size_t size) final;

	bool getOcppMessageType(const std::string& jsonString, int& messageType) final;
	bool getOcppId(const std::string& jsonString, std::string& ocppId) final;
	bool getOcppAction(const std::string& jsonString, std::string& ocppAction) final;
	bool getOcppPayload(const std::string& jsonString, std::string& payload) final;

	std::string generateOcppMessage(int messageType, 
					const std::string& ocppId, 
					const std::string& payload) final;
};


#endif // OCPPJSON_HPP
