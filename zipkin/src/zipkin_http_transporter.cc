#include "zipkin_http_transporter.h"

#include "zipkin_core_constants.h"
#include "zipkin_reporter_impl.h"

#include <curl/curl.h>
#include <iostream>

namespace zipkin {
static std::string getUrl(const char *collector_host, uint32_t collector_port) {
  return std::string{"http://"} + collector_host + ":" +
         std::to_string(collector_port) +
         ZipkinCoreConstants::get().DEFAULT_COLLECTOR_ENDPOINT;
}

template <class... Types> void CurlHandle::setopt(CURLoption option, Types... args) {
  auto rcode = curl_easy_setopt(handle_, option, args...);
   if (rcode != CURLE_OK) {
     throw CurlError{rcode};
   }
}

ZipkinHttpTransporter::ZipkinHttpTransporter(const char *collector_host,
                                             uint32_t collector_port,
                                             std::chrono::milliseconds collector_timeout) {
  handle_.setopt(CURLOPT_URL, getUrl(collector_host, collector_port).c_str());

  headers_.append("Content-Type: application/json");
  handle_.setopt(CURLOPT_HTTPHEADER, static_cast<curl_slist *>(headers_));

  handle_.setopt(CURLOPT_ERRORBUFFER, error_buffer_);

  handle_.setopt(CURLOPT_TIMEOUT_MS, collector_timeout.count());
}

ZipkinHttpTransporter::~ZipkinHttpTransporter() {}

void ZipkinHttpTransporter::transportSpans(SpanBuffer &spans) try {
  auto data = spans.toStringifiedJsonArray();
  try {
    handle_.setopt(CURLOPT_POSTFIELDS, data.c_str());
  } catch(const CurlError &e) {
    std::cerr << e.what() << '\n';
    return;
  }
  auto rcode = curl_easy_perform(handle_);
  if (rcode != CURLE_OK) {
    std::cerr << error_buffer_ << '\n';
  }
} catch (const std::bad_alloc &) {
  // Drop spans
}

ReporterPtr makeHttpReporter(const char *collector_host,
                             uint32_t collector_port,
                             std::chrono::milliseconds collector_timeout,
                             SteadyClock::duration reporting_period,
                             size_t max_buffered_spans) try {
  std::unique_ptr<Transporter> transporter{
      new ZipkinHttpTransporter{collector_host, collector_port, collector_timeout}};
  std::unique_ptr<Reporter> reporter{new ReporterImpl{
      std::move(transporter), reporting_period, max_buffered_spans}};
  return reporter;
} catch (const CurlError &error) {
  std::cerr << error.what() << '\n';
  return nullptr;
}
} // namespace zipkin
