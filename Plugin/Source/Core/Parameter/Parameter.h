#pragma once

#include <JuceHeader.h>
#include <Definitions.h>
#include <TypeDefs.h>
#include <memory>
#include <string>

namespace VSTZ::Core {
enum ParameterTypes { Integer = 0, Float, Boolean };

class Parameter : public juce::AudioProcessorParameter::Listener {
protected:
  size_t m_id{};
  std::string m_name;
  std::string m_showName;
  double m_max{};
  double m_min{};
  double m_value{};

public:
  Parameter(std::string name, std::string showName, double min, double max,
            double value, size_t id);
  ~Parameter() override = default;
  bool getBool() const;
  int getInt() const;
  double getValue() const;
  virtual void setValue(double value);
  double getMin() const;
  double getMax() const;
  std::string &getName();
  std::string &getShowName();
  std::unique_ptr<juce::RangedAudioParameter> createParameter(ParameterTypes type);
  void parameterValueChanged(int parameterIndex, float newValue) override;
  void parameterGestureChanged(int parameterIndex,
                               bool gestureIsStarting) override;

  explicit operator bool() const { return m_value < 0.5; }
  explicit operator int() const { return (int)m_value; }
  explicit operator double() const { return m_value; }
};
} // namespace VSTZ::Core