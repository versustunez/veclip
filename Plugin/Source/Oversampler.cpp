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
  juce::IIRCoefficients coefficients[4]{};
  for (int j = 0; j < 2; ++j) {
    double Q =
        1.0 /
        (2.0 * std::cos((2 * j + 1) * juce::MathConstants<double>::pi / 8.0));
    auto coefficient =
        juce::IIRCoefficients::makeLowPass(sr, sr * 0.5 * 0.5, Q);
    coefficients[j * 2] = coefficient;
    coefficients[j * 2 + 1] = coefficient;
  }
  for (int i = 0; i < 4; i++) {
    m_Filter[i].setCoefficients(coefficients[i]);
  }
}
}; // namespace VSTZ