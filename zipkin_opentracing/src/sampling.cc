#include "sampling.h"
#include <random>
#include <zipkin/randutils/randutils.h>

namespace zipkin {
bool ProbabilisticSampler::ShouldSample() {
  static thread_local randutils::mt19937_rng rng;
  std::bernoulli_distribution dist(sample_rate_);
  return dist(rng.engine());
}
} // namespace zipkin
