#pragma once

#include <cstdint>
#include <string>

namespace zipkin {
enum class IpVersion { v4, v6 };

class IpAddress {
public:
  IpAddress() = default;

  IpAddress(IpVersion version, const std::string &address);

  IpAddress(IpVersion version, const std::string &address, uint32_t port);

  IpVersion version() const { return version_; }

  uint32_t port() const { return port_; }

  const std::string &addressAsString() const { return friendly_address_; }

  bool valid() const { return !friendly_address_.empty(); }

private:
  IpVersion version_;
  std::string friendly_address_;
  uint32_t port_;
};
} // namespace zipkin
