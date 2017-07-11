#pragma once

#include <cstdint>

namespace zipkin {
using flags_t = uint64_t;

const flags_t debug_flag = 1 << 0;
const flags_t sampling_set_flag = 1 << 1;
const flags_t sampled_flag = 1 << 2;
const flags_t is_root_flag = 1 << 3;
} // namespace zipkin
