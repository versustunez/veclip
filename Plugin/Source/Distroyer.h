#pragma once

#include "Core/Parameter/Parameter.h"
#include "Oversampler.h"
#include "Structs.h"
#include "ThreeBandProcessor.h"

#include <JuceHeader.h>

namespace VSTZ {

struct DistoryerParameters {
  Core::Parameter* Input{nullptr};
  Core::Parameter* Mix{nullptr};
  Core::Parameter* Mute{nullptr};
  Core::Parameter* Bypass{nullptr};
};
class Distroyer {
public:
  Distroyer();
  void Setup(double sampleRate, int sampleSize);
  Channel Process(juce::AudioBuffer<float>& buffer);
  float OutputGain{0.0};
  bool OutputDelta{false};

  double m_LowCrossFrequency{0};
  double m_HighCrossFrequency{0};
  DistoryerParameters Parameters_[3]{};

protected:
  // Stereo LowPass ;)
  ThreeBandProcessor m_ThreeBandProcessor;
  StepLowPass m_PreFilter[2]{};
  StepLowPass m_PostFilter[2]{};
};
}
