#pragma once
#include "Parameter.h"

#include <JuceHeader.h>
#include <TypeDefs.h>

namespace VSTZ::Core {
class ParameterHandler : public juce::AudioProcessorValueTreeState::Listener {
public:
  explicit ParameterHandler(InstanceID id);
  ~ParameterHandler() override;
  juce::AudioProcessorValueTreeState::ParameterLayout SetupProcessor();
  void parameterChanged(const juce::String &parameterID,
                        float newValue) override;

  std::string GetShowName(const std::string &parameter);

  Parameter *GetParameter(const std::string &name);

  Parameter *operator[](const char *name);

protected:
  void SetupParameter();
  void AddParameter(const std::string &name, const std::string &showName,
                    double min, double max, double value, ParameterTypes type);

  InstanceID m_ID = 0;
  Vector<std::string> m_ParamMaps;
  Map<std::string, Parameter *> m_Parameters;
  Vector<std::unique_ptr<juce::RangedAudioParameter>> m_Params;
};
} // namespace VSTZ::Core
