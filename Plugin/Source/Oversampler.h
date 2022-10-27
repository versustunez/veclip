#pragma once

#include "JuceHeader.h"
#include "TypeDefs.h"

namespace VSTZ {
class StepLowPass {
public:
  StepLowPass();
  double DoFilter(double input);
  void SetSampleRate(double sr);

public:
  Vector<juce::IIRFilter> m_Filter{};
};
}

