#include <nlohmann/json.hpp>
#include <string>

#ifndef RESPONSES_HPP
#define RESPONSES_HPP

using json = nlohmann::json;

class Responses {
public:
	static std::string get_utc_time();

	enum class BootStatus { Accepted, Pending, Rejected };
    static json BootNotificationResponse(Responses::BootStatus status, std::string currentTime, int interval);

	static json HeartbeatResponse(std::string currentTime);

	static json StatusNotificationResponse();
};

#endif // RESPONSES_HPP
