#include "zipkin_http_transporter.h"

#include "zipkin_core_constants.h"
#include "zipkin_reporter_impl.h"

#include <curl/curl.h>
#include <iostream>

namespace zipkin {
static std::string getUrl(const char *collector_base_url) {
  return collector_base_url+
         ZipkinCoreConstants::get().DEFAULT_COLLECTOR_ENDPOINT;
}

ZipkinHttpTransporter::ZipkinHttpTransporter(const char *collector_base_url,
                                             std::chrono::milliseconds collector_timeout) {
  auto rcode = curl_easy_setopt(handle_, CURLOPT_URL,
                                getUrl(collector_base_url).c_str());
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }

  headers_.append("Content-Type: application/json");
  rcode = curl_easy_setopt(handle_, CURLOPT_HTTPHEADER,
                           static_cast<curl_slist *>(headers_));
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }

  rcode = curl_easy_setopt(handle_, CURLOPT_ERRORBUFFER, error_buffer_);
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }

  rcode = curl_easy_setopt(handle_, CURLOPT_TIMEOUT_MS, collector_timeout.count());
  if (rcode != CURLE_OK) {
    throw CurlError{rcode};
  }
}

ZipkinHttpTransporter::~ZipkinHttpTransporter() {}

void ZipkinHttpTransporter::transportSpans(SpanBuffer &spans) try {
  auto data = spans.toStringifiedJsonArray();
  auto rcode = curl_easy_setopt(handle_, CURLOPT_POSTFIELDS, data.c_str());
  if (rcode != CURLE_OK) {
    std::cerr << curl_easy_strerror(rcode) << '\n';
    return;
  }
  rcode = curl_easy_perform(handle_);
  if (rcode != CURLE_OK) {
    std::cerr << error_buffer_ << '\n';
  }
} catch (const std::bad_alloc &) {
  // Drop spans
}

ReporterPtr makeHttpReporter(const char *collector_base_url,
                             std::chrono::milliseconds collector_timeout,
                             SteadyClock::duration reporting_period,
                             size_t max_buffered_spans) try {
  std::unique_ptr<Transporter> transporter{
      new ZipkinHttpTransporter{collector_base_url, collector_timeout}};
  std::unique_ptr<Reporter> reporter{new ReporterImpl{
      std::move(transporter), reporting_period, max_buffered_spans}};
  return reporter;
} catch (const CurlError &error) {
  std::cerr << error.what() << '\n';
  return nullptr;
}
} // namespace zipkin
