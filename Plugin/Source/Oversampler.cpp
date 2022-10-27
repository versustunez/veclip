#include "Oversampler.h"
namespace VSTZ {
StepLowPass::StepLowPass() {
  m_Filter.reserve(4);
  m_Filter.emplace_back();
  m_Filter.emplace_back();
  m_Filter.emplace_back();
  m_Filter.emplace_back();
}

double StepLowPass::DoFilter(double input) {
  for (auto &filter : m_Filter) {
    input = filter.processSingleSampleRaw((float)input);
  }
  return input;
}

void StepLowPass::SetSampleRate(double sr) {
  double Q = 1.35;
  for (auto &filter : m_Filter) {
    auto coefficients = juce::IIRCoefficients::makeLowPass(
        sr, sr * 0.5 * 0.5, Q / juce::MathConstants<double>::sqrt2);
    filter.setCoefficients(coefficients);
    Q -= 0.125;
  }
}
}; // namespace VSTZ