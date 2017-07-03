#include "address.h"
#include <arpa/inet.h>
#include <cstring>
#include <string>

namespace zipkin {
static bool isValidIpv4Address(const std::string& address) {
  in_addr sin_addr = {};
  int rc = inet_pton(AF_INET, address.c_str(), &sin_addr);
  return rc == 1;
}

static std::string makeFriendlyIpv6Address(const std::string& address) {
  in6_addr sin6_addr = {};
  int rc = inet_pton(AF_INET6, address.c_str(), &sin6_addr);
  if (rc != 1) {
    return {};
  }
  char str[INET6_ADDRSTRLEN];
  const char* ptr = inet_ntop(AF_INET6, &sin6_addr, str, INET6_ADDRSTRLEN);
  return ptr;
}

IpAddress::IpAddress(IpVersion version, const std::string& address)
    : IpAddress{version, address, 0} {}

IpAddress::IpAddress(IpVersion version, const std::string& address,
                     uint32_t port)
    : version_{version}, friendly_address_{}, port_{0} {
  switch(version) {
    case IpVersion::v4: {
      if (!isValidIpv4Address(address)) {
        return;
      }
      friendly_address_ = address + ":" + std::to_string(port);
    }
    case IpVersion::v6: {
      auto normalized_address = makeFriendlyIpv6Address(address);
      if (normalized_address.empty()) {
        return;
      }
      friendly_address_ =
          std::string{"["} + normalized_address + "]:" + std::to_string(port);
    }
  }
  port_ = port;
}
}  // namespace zipkin
