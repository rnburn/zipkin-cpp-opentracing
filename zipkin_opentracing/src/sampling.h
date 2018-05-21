#pragma once
#include <algorithm>
#include <memory>

namespace zipkin {
class Sampler {
public:
  virtual ~Sampler() = default;
  virtual bool ShouldSample() = 0;
};

class ProbabilisticSampler : public Sampler {
public:
  ProbabilisticSampler(double sample_rate)
      : sample_rate_(std::max(0.0, std::min(sample_rate, 1.0))){};
  bool ShouldSample() override;

private:
  double sample_rate_;
};

typedef std::unique_ptr<Sampler> SamplerPtr;
} // namespace zipkin