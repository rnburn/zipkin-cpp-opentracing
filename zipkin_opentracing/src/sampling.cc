#include "sampling.h"
#include <random>

namespace zipkin {
bool ProbabilisticSampler::ShouldSample() {
  static thread_local std::mt19937 rng(std::random_device{}());
  std::bernoulli_distribution dist(sample_rate_);
  return dist(rng);
}
} // namespace zipkin