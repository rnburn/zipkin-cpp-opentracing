#include "sampling.h"
#include <random>

namespace zipkin {
  bool
  ProbabilisticSampler::ShouldSample() {
    std::random_device device;
    std::mt19937 gen(device());
    std::bernoulli_distribution dist(sample_rate_);
    
    return dist(gen);
  }
} // namespace zipkin