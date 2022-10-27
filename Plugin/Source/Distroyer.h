#pragma once

#include "Oversampler.h"
#include "Structs.h"

#include <JuceHeader.h>

namespace VSTZ {
class Distroyer {
public:
  void Setup(double sampleRate);
  Channel Process(juce::AudioBuffer<float>& buffer);
  float InputGain{0.0};
  float AutoGain{0.0};
  float OutputGain{0.0};
  double MixValue{0.5};
protected:
  Channel ProcessSample(const Channel& channel) const;

protected:
  // Stereo LowPass ;)
  StepLowPass m_PreFilter[2]{};
  StepLowPass m_PostFilter[2]{};
};
}
