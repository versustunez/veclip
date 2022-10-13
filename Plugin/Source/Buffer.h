#pragma once

#include <JuceHeader.h>
#include <TypeDefs.h>

namespace VSTZ {
struct Channel {
  double left, right;
};
class Buffer {
public:
  explicit Buffer(InstanceID id);
  void Reset(int size);
  void AddSamples(const juce::AudioBuffer<float> &buffer);
  void SetSamples(const juce::AudioBuffer<float> &buffer);
  // COPY!
  Vector<double> GetChannel(int channel) {
    return channel == 0 ? m_Left : m_Right;
  }

  auto &GetBuffer(int channel) { return channel == 0 ? m_Left : m_Right; }

  Channel &Peak() { return m_Peaks; }
  size_t GetFilledSampleCount() { return m_FilledSamples; }

protected:
  InstanceID m_ID{};
  Vector<double> m_Left{};
  Vector<double> m_Right{};
  size_t m_FilledSamples{0};
  Channel m_Peaks{-70, -70};
};
} // namespace VSTZ