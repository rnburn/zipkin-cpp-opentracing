#pragma once

#include "transporter.h"
#include "zipkin_reporter_impl.h"

#include <curl/curl.h>
#include <exception>

namespace zipkin {
/**
 * Exception class used for CURL errors.
 */
class CurlError : public std::exception {
public:
  CurlError(CURLcode code) : code_{code} {}

  const char *what() const noexcept override {
    return curl_easy_strerror(code_);
  }

private:
  CURLcode code_;
};

/**
 * RAII class to manage the initialization/deinitialization of the CURL
 * environment.
 */
class CurlEnvironment {
public:
  CurlEnvironment() { curl_global_init(CURL_GLOBAL_ALL); }

  ~CurlEnvironment() { curl_global_cleanup(); }
};

/**
 * RAII class to manage the allocation/deallocation of a CURL handle.
 */
class CurlHandle {
public:
  CurlHandle() { handle_ = curl_easy_init(); }

  ~CurlHandle() { curl_easy_cleanup(handle_); }

  operator CURL *() { return handle_; }

  CURL *operator->() { return handle_; }

  template <class... Types> void setopt(CURLoption option, Types... args);

private:
  CURL *handle_;
};

/**
 * RAII class to manage the allocation/deallocation of a CURL SList
 */
class CurlSList {
public:
  ~CurlSList() {
    if (list_) {
      curl_slist_free_all(list_);
    }
  }

  operator curl_slist *() { return list_; }

  void append(const char *s) {
    auto list_new = curl_slist_append(list_, s);
    if (!list_new)
      throw std::bad_alloc{};
    list_ = list_new;
  }

private:
  curl_slist *list_ = nullptr;
};

/**
 * This class derives from the abstract zipkin::Transporter. It sends spans to
 * a zipkin collector via http.
 */
class ZipkinHttpTransporter : public Transporter {
public:
  /**
   * Constructor.
   *
   * @param collector_host The host to use when sending spans to the Zipkin
   * service.
   * @param collector_port The port to use when sending spans to the Zipkin service.
   * @param collector_timeout Timeout for http requests
   *
   * Throws CurlError if the handle can't be initialized.
   */
  ZipkinHttpTransporter(const char *collector_host, uint32_t collector_port,
          std::chrono::milliseconds collector_timeout = DEFAULT_TRANSPORT_TIMEOUT);

  /**
   * Destructor.
   */
  ~ZipkinHttpTransporter();

  /**
   * Implementation of zipkin::Transporter::transportSpans().
   *
   * @param spans The spans to be transported.
   */
  void transportSpans(SpanBuffer &spans) override;

private:
  CurlEnvironment curl_environment_;
  CurlHandle handle_;
  CurlSList headers_;
  char error_buffer_[CURL_ERROR_SIZE];
};
} // namespace zipkin
