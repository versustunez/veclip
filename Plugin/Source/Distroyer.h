#pragma once

#include "Oversampler.h"
#include "Structs.h"
#include "ThreeBandProcessor.h"

#include <JuceHeader.h>

namespace VSTZ {
class Distroyer {
public:
  Distroyer();
  void Setup(double sampleRate, int sampleSize);
  Channel Process(juce::AudioBuffer<float>& buffer);
  float InputGain{0.0};
  float AutoGain{0.0};
  float OutputGain{0.0};
  double MixValue{0.5};
  bool OutputDelta{false};
protected:
  Channel ProcessSample(const Channel& channel) const;

protected:
  // Stereo LowPass ;)
  ThreeBandProcessor m_ThreeBandProcessor;
  StepLowPass m_PreFilter[2]{};
  StepLowPass m_PostFilter[2]{};
};
}
