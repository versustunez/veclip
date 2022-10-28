#include "Distroyer.h"

#include <algorithm>
#include <complex>

namespace VSTZ {
static double lerp(double a, double b, double f) { return a + f * (b - a); }

static double mixed(double input, double mix) {
  double hardClip =
      std::clamp(input, -1.0, 1.0);
  double softClip = std::atan(input);
  return lerp(softClip, hardClip, mix);
}

static Channel calculateReduction(const Channel &original,
                                  const Channel &processed,
                                  double autoGainVal) {
  Channel autoGain{autoGainVal, autoGainVal};
  Channel processedAutoGain = processed * autoGain;
  Channel newChannel = processedAutoGain - original;
  Channel output{1.0 - newChannel.Left, 1.0 - newChannel.Right};
  output.Left = std::min(output.Left, 1.0);
  output.Right = std::min(output.Right, 1.0);
  return autoGain * output;
}

void Distroyer::Setup(double sampleRate) {
  sampleRate *= 2.0;
  m_PostFilter[0].SetSampleRate(sampleRate);
  m_PostFilter[1].SetSampleRate(sampleRate);
  m_PreFilter[0].SetSampleRate(sampleRate);
  m_PreFilter[1].SetSampleRate(sampleRate);
}

Channel Distroyer::Process(juce::AudioBuffer<float> &buffer) {
  float *leftChannelData = buffer.getWritePointer(0);
  float *rightChannelData = buffer.getWritePointer(1);
  Channel oversamplingInc{2.1, 2.1};
  Channel m_HighestPeak = {0, 0};
  for (auto i = 0; i < buffer.getNumSamples(); i++) {
    Channel originalChannelData = {leftChannelData[i], rightChannelData[i]};
    Channel data[2] = {originalChannelData, {0, 0}};
    Channel output[2] = {{0.0, 0.0}, {0.0, 0.0}};
    for (int channelIdx = 0; channelIdx < 2; ++channelIdx) {
      auto &channel = data[channelIdx];
      channel.Left = m_PreFilter[0].DoFilter(channel.Left);
      channel.Right = m_PreFilter[1].DoFilter(channel.Right);
      output[channelIdx] = ProcessSample(channel);
      channel.Left = m_PostFilter[0].DoFilter(output[channelIdx].Left);
      channel.Right = m_PostFilter[1].DoFilter(output[channelIdx].Right);
    }
    Channel reduction =
        calculateReduction(originalChannelData, output[0], AutoGain);
    data[0] *= reduction;
    m_HighestPeak.Left =
        std::max(std::abs(data[0].Left - originalChannelData.Left), m_HighestPeak.Left);
    m_HighestPeak.Right = std::max(std::abs(data[0].Right - originalChannelData.Right),
                                   m_HighestPeak.Right);
    data[0] *= oversamplingInc;

    if (OutputDelta) {
      leftChannelData[i] = (float)(data[0].Left - originalChannelData.Left);
      rightChannelData[i] = (float)(data[0].Right - originalChannelData.Right);
    } else {
      leftChannelData[i] = (float)data[0].Left * OutputGain;
      rightChannelData[i] = (float)data[0].Right * OutputGain;
    }


  }
  return m_HighestPeak;
}

Channel Distroyer::ProcessSample(const Channel &channel) const {
  return Channel{mixed(channel.Left * InputGain, MixValue),
                 mixed(channel.Right * InputGain, MixValue)};
}

} // namespace VSTZ