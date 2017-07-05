#include "zipkin_http_transporter.h"

#include <curl/curl.h>

namespace zipkin {
ZipkinHttpTransporter::ZipkinHttpTransporter(const char* collector_host,
                                             uint32_t collector_port) {
}

ZipkinHttpTransporter::~ZipkinHttpTransporter() {
}

void ZipkinHttpTransporter::transportSpans(const SpanBuffer& spans) {
}
} // namespace zipkin
