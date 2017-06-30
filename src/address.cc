#include "address.h"

namespace zipkin {
IpAddress::IpAddress(IpVersion version, const std::string& address)
    : version_{version}, friendly_address_{address}, port_{0} {}

IpAddress::IpAddress(IpVersion version, const std::string& address,
                     uint32_t port)
    : version_{version}, friendly_address_{address}, port_{port} {}
}  // namespace zipkin
