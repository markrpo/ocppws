#include "iocpp.hpp"
#include "jsons_headers.hpp"

std::string OcppJsons::heartbeatResponse() {
	return std::string((char*)v16_schemas_HeartbeatResponse_json);
}
