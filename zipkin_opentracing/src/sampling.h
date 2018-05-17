#pragma once

namespace zipkin {
  class Sampler {
    public:
      virtual ~Sampler() = default;
      virtual bool ShouldSample() = 0;
  };

  class ProbabilisticSampler : public Sampler {
    public:
      ProbabilisticSampler(double sample_rate) : sample_rate_(sample_rate) {};
      bool ShouldSample() override;

    private:
      double sample_rate_;
  };
} // namespace zipkin