#include "zipkin_http_transporter.h"

#include <curl/curl.h>
#include <iostream>

namespace zipkin {
ZipkinHttpTransporter::ZipkinHttpTransporter(const char* collector_host,
                                             uint32_t collector_port) {
}

ZipkinHttpTransporter::~ZipkinHttpTransporter() {
}

void ZipkinHttpTransporter::transportSpans(SpanBuffer& spans) {
  std::cout << spans.toStringifiedJsonArray() << "\n";
}
} // namespace zipkin
