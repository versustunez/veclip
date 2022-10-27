#include "Buffer.h"

namespace VSTZ {
Buffer::Buffer(InstanceID id) : m_ID(id) {}
void Buffer::Reset(int size) {
  m_Right.resize((size_t)size);
  m_Left.resize((size_t)size);
}

void Buffer::AddSamples(const juce::AudioBuffer<float> &buffer) {
  auto size = (size_t)buffer.getNumSamples();
  if (size < m_Left.size()) {
    size_t until = m_Left.size() - size;
    for (size_t i = size; i < m_Left.size(); ++i) {
      m_Left[i - size] = m_Left[i];
      m_Right[i - size] = m_Right[i];
    }
    for (size_t i = 0; i < size; ++i) {
      m_Left[i + until] = buffer.getSample(0, (int)i);
      m_Right[i + until] = buffer.getSample(1, (int)i);
    }
  } else {
    for (size_t i = 0; i < size; ++i) {
      m_Left[i] = buffer.getSample(0, (int)i);
      m_Right[i] = buffer.getSample(1, (int)i);
    }
  }
}
void Buffer::SetSamples(const juce::AudioBuffer<float> &buffer) {
  m_FilledSamples = (size_t)buffer.getNumSamples();
  Channel peaks{};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    m_Left[(size_t)i] = buffer.getSample(0, (int)i);
    m_Right[(size_t)i] = buffer.getSample(1, (int)i);
    peaks.Left = std::max(peaks.Left, std::abs(m_Left[(size_t)i]));
    peaks.Right = std::max(peaks.Right, std::abs(m_Right[(size_t)i]));
  }
  m_Peaks.Left = juce::Decibels::gainToDecibels(peaks.Left, -70.0);
  m_Peaks.Right = juce::Decibels::gainToDecibels(peaks.Right, -70.0);

}
}; // namespace VSTZ