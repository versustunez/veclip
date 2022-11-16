#include "Distroyer.h"

#include <algorithm>
#include <complex>

namespace VSTZ {
static double lerp(double a, double b, double f) { return a + f * (b - a); }

static double mixed(double input, double mix) {
  double hardClip = std::clamp(input, -1.0, 1.0);
  double softClip = std::atan(input);
  return lerp(softClip, hardClip, mix);
}

Distroyer::Distroyer() {
  m_ThreeBandProcessor.SetProcessingFunction(
      [&](auto &channels, size_t count, size_t band) {
        for (size_t i = 0; i < count; ++i) {
          auto &channel = channels[i];
          channel = {mixed(channel.Left * InputGain, MixValue),
                     mixed(channel.Right * InputGain, MixValue)};
        }
      });
}

void Distroyer::Setup(double sampleRate, int sampleSize) {
  sampleRate *= 2.0;
  m_PostFilter[0].SetSampleRate(sampleRate);
  m_PostFilter[1].SetSampleRate(sampleRate);
  m_PreFilter[0].SetSampleRate(sampleRate);
  m_PreFilter[1].SetSampleRate(sampleRate);
  m_ThreeBandProcessor.SetSampleRate(sampleRate);
  m_ThreeBandProcessor.SetSampleSize((size_t)sampleSize * 2);
}

Channel Distroyer::Process(juce::AudioBuffer<float> &buffer) {
  float *leftChannelData = buffer.getWritePointer(0);
  float *rightChannelData = buffer.getWritePointer(1);
  Channel oversamplingInc{2.1, 2.1};
  Channel highestPeak{0, 0};
  m_ThreeBandProcessor.Reset();
  for (auto i = 0; i < buffer.getNumSamples(); i++) {
    Channel data[2] = {{leftChannelData[i], rightChannelData[i]}, {0, 0}};
    for (auto &channel : data) {
      channel.Left = m_PreFilter[0].DoFilter(channel.Left);
      channel.Right = m_PreFilter[1].DoFilter(channel.Right);
      m_ThreeBandProcessor.AddSample(channel);
    }
  }
  m_ThreeBandProcessor.Process();
  auto size = m_ThreeBandProcessor.GetSize();
  auto &processedBuffer = m_ThreeBandProcessor.GetBuffer();
  for (size_t i = 0; i < size; i += 2) {
    {
      auto &channel = processedBuffer[i];
      channel.Left = m_PostFilter[0].DoFilter(channel.Left);
      channel.Right = m_PostFilter[1].DoFilter(channel.Right);
      channel *= Channel{AutoGain, AutoGain};
      channel *= oversamplingInc;
      Channel originalChannelData = {leftChannelData[i / 2],
                                     rightChannelData[i / 2]};
      highestPeak.Left = std::max(
          std::abs(channel.Left - originalChannelData.Left), highestPeak.Left);
      highestPeak.Right = std::max(
          std::abs(channel.Right - originalChannelData.Right), highestPeak.Right);
      channel *= {OutputGain, OutputGain};

      if (OutputDelta) {
        channel = originalChannelData - channel;
      }
    }

    {
      auto& channel = processedBuffer[i + 1];
      channel.Left = m_PostFilter[0].DoFilter(channel.Left);
      channel.Right = m_PostFilter[1].DoFilter(channel.Right);
    }
  }

  m_ThreeBandProcessor.WriteToBuffer(leftChannelData, rightChannelData);
  return highestPeak;
}

} // namespace VSTZ
