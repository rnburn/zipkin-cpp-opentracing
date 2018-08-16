#include "sampling.h"
#include <random>
#include <zipkin/randutils/randutils.h>

namespace zipkin {
std::mt19937_64 &getTlsRandomEngine();

bool ProbabilisticSampler::ShouldSample() {
  std::bernoulli_distribution dist(sample_rate_);
  return dist(getTlsRandomEngine());
}
} // namespace zipkin
