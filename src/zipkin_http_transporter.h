#pragma once

#include "transporter.h"

#include <curl/curl.h>

namespace zipkin {
/**
 * RAII class to manage the initialization/deinitialization of the CURL
 * environment.
 */
class CurlEnvironment {
 public:
  CurlEnvironment() {
      curl_global_init(CURL_GLOBAL_ALL);
  }

  ~CurlEnvironment() {
      curl_global_cleanup();
  }
};

/**
 * RAII class to manage the allocation/deallocation of a CURL handle.
 */
class CurlHandle {
  public:
    CurlHandle() {
      handle_ = curl_easy_init();
    }

    ~CurlHandle() {
      curl_easy_cleanup(handle_);
    }

    operator CURL*() { return handle_; }

    CURL* operator->() {
      return handle_;
    }

   private:
    CURL* handle_;
};

/**
 * This class derives from the abstract zipkin::Transporter. It sends spans to
 * a zipkin collector via http.
 */
class ZipkinHttpTransporter : public Transporter {
 public:
  /**
   * Constructor.
   */
  ZipkinHttpTransporter(const char* collector_host, uint32_t collector_port);

  /**
   * Destructor.
   */
  ~ZipkinHttpTransporter();
  
  /**
   * Implementation of zipkin::Transporter::transportSpans().
   *
   * @param spans The spans to be transported.
   */
  void transportSpans(SpanBuffer& spans) override;
 private:
  CurlEnvironment curl_environment_;
  CurlHandle handle_;
};
} // namesapce zipkin
