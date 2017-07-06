#include "zipkin_http_transporter.h"

#include "zipkin_core_constants.h"

#include <curl/curl.h>
#include <iostream>

namespace zipkin {
static std::string getUrl(const char* collector_host, uint32_t collector_port) {
  return std::string{"http://"} + collector_host + ":" +
         std::to_string(collector_port) +
         ZipkinCoreConstants::get().DEFAULT_COLLECTOR_ENDPOINT;
}

ZipkinHttpTransporter::ZipkinHttpTransporter(const char* collector_host,
                                             uint32_t collector_port) {
  auto rcode = curl_easy_setopt(handle_, CURLOPT_URL,
                                getUrl(collector_host, collector_port).c_str());
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }

  headers_.append("Content-Type: application/json");
  rcode = curl_easy_setopt(handle_, CURLOPT_HTTPHEADER,
                           static_cast<curl_slist*>(headers_));
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }

  curl_easy_setopt(handle_, CURLOPT_ERRORBUFFER, error_buffer_);
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }
}

ZipkinHttpTransporter::~ZipkinHttpTransporter() {
}

void ZipkinHttpTransporter::transportSpans(SpanBuffer& spans) {
  std::cout << spans.toStringifiedJsonArray() << "\n";
}
} // namespace zipkin
