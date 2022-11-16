#include "ThreeBandProcessor.h"

#include <complex>

namespace VSTZ {

void LinkWitzRileyFilter::LowPass(double frequency, double sampleRate) {
  m_Type = FilterType::LOW;
  MakeFilter(frequency, sampleRate);
}
void LinkWitzRileyFilter::HighPass(double frequency, double sampleRate) {
  m_Type = FilterType::HIGH;
  MakeFilter(frequency, sampleRate);
}
void LinkWitzRileyFilter::Process(Channel &channel) {
  channel.Left = ProcessSingle(channel.Left, 0);
  channel.Right = ProcessSingle(channel.Right, 1);
}

double LinkWitzRileyFilter::ProcessSingle(double input, int channelVal) {
  double tempx{input};
  double tempy{};

  auto &tempObj = m_Temp_[channelVal];
  tempy = m_Coeff.a0 * tempx + m_Coeff.a1 * tempObj.xm1 +
          m_Coeff.a2 * tempObj.xm2 + m_Coeff.a3 * tempObj.xm3 +
          m_Coeff.a4 * tempObj.xm4 - b1co * tempObj.ym1 - b2co * tempObj.ym2 -
          b3co * tempObj.ym3 - b4co * tempObj.ym4;

  tempObj.xm4 = tempObj.xm3;
  tempObj.xm3 = tempObj.xm2;
  tempObj.xm2 = tempObj.xm1;
  tempObj.xm1 = tempx;
  tempObj.ym4 = tempObj.ym3;
  tempObj.ym3 = tempObj.ym2;
  tempObj.ym2 = tempObj.ym1;
  tempObj.ym1 = tempy;

  return tempy;
}

void LinkWitzRileyFilter::MakeFilter(double frequency, double sampleRate) {
  const double pi = 3.141f;
  constexpr double sqrt2 = 1.41421356237;

  coFreqRunningAv = frequency;

  double cowc = 2 * pi * coFreqRunningAv;
  double cowc2 = cowc * cowc;
  double cowc3 = cowc2 * cowc;
  double cowc4 = cowc2 * cowc2;

  double cok = cowc / tan(pi * coFreqRunningAv / sampleRate);
  double cok2 = cok * cok;
  double cok3 = cok2 * cok;
  double cok4 = cok2 * cok2;
  double sq_tmp1 = sqrt2 * cowc3 * cok;
  double sq_tmp2 = sqrt2 * cowc * cok3;
  double a_tmp = 4 * cowc2 * cok2 + 2 * sq_tmp1 + cok4 + 2 * sq_tmp2 + cowc4;

  b1co = (4 * (cowc4 + sq_tmp1 - cok4 - sq_tmp2)) / a_tmp;
  b2co = (6 * cowc4 - 8 * cowc2 * cok2 + 6 * cok4) / a_tmp;
  b3co = (4 * (cowc4 - sq_tmp1 + sq_tmp2 - cok4)) / a_tmp;
  b4co = (cok4 - 2 * sq_tmp1 + cowc4 - 2 * sq_tmp2 + 4 * cowc2 * cok2) / a_tmp;

  if (m_Type == FilterType::LOW) {
    m_Coeff.a0 = cowc4 / a_tmp;
    m_Coeff.a1 = 4 * cowc4 / a_tmp;
    m_Coeff.a2 = 6 * cowc4 / a_tmp;
    m_Coeff.a3 = m_Coeff.a1;
    m_Coeff.a4 = m_Coeff.a0;
  } else {
    m_Coeff.a0 = cok4 / a_tmp;
    m_Coeff.a1 = -4 * cok4 / a_tmp;
    m_Coeff.a2 = 6 * cok4 / a_tmp;
    m_Coeff.a3 = m_Coeff.a1;
    m_Coeff.a4 = m_Coeff.a0;
  }
}

ThreeBandProcessor::ThreeBandProcessor() {
  m_BandFilters.resize(3);
  m_BandFilters[0].emplace_back(); // Low Pass
  m_BandFilters[1].emplace_back(); // All Pass
  m_BandFilters[1].emplace_back(); // All Pass
  m_BandFilters[2].emplace_back(); // High Pass
}

void ThreeBandProcessor::SetSampleRate(double sampleRate) {
  m_SampleRate = sampleRate;
  m_BandFilters[0][0].LowPass(500, sampleRate);
  m_BandFilters[1][0].LowPass(2000, sampleRate);
  m_BandFilters[1][1].HighPass(500, sampleRate);
  m_BandFilters[2][0].HighPass(2000, sampleRate);
}

void ThreeBandProcessor::SetSampleSize(size_t blockSize) {
  m_ProcessingBuffer.resize(blockSize);
  m_CrossSplitBuffers_[0].resize(blockSize);
  m_CrossSplitBuffers_[1].resize(blockSize);
  m_CrossSplitBuffers_[2].resize(blockSize);
  m_CurrentIndex = 0;
}

void ThreeBandProcessor::SetBandSplit(int band, double frequency) {
  if (band == 0) {
    m_BandFilters[0][0].LowPass(frequency, m_SampleRate);
    m_BandFilters[1][0].LowPass(frequency, m_SampleRate);
  } else {
    m_BandFilters[1][1].HighPass(frequency, m_SampleRate);
    m_BandFilters[0][0].HighPass(frequency, m_SampleRate);
  }
}

void ThreeBandProcessor::AddSample(Channel channel) {
  m_ProcessingBuffer[m_CurrentIndex++] = channel;
}

void ThreeBandProcessor::SetProcessingFunction(
    const std::function<void(std::vector<Channel> &, size_t count, size_t)>
        &function) {
  m_ProcessingCallback = function;
}

void ThreeBandProcessor::Process() {
  SplitBuffer();
  // split channels then call processing callback ;)
  m_ProcessingCallback(m_CrossSplitBuffers_[0], m_CurrentIndex, 0);
  m_ProcessingCallback(m_CrossSplitBuffers_[1], m_CurrentIndex, 1);
  m_ProcessingCallback(m_CrossSplitBuffers_[2], m_CurrentIndex, 2);
  CombineBuffers();
}

void ThreeBandProcessor::SplitBuffer() {
  for (size_t i = 0; i < m_CurrentIndex; ++i) {
    auto &channel = m_ProcessingBuffer[i];
    m_CrossSplitBuffers_[0][i] = channel;
    m_BandFilters[0][0].Process(m_CrossSplitBuffers_[0][i]);
    m_CrossSplitBuffers_[1][i] = channel;
    m_BandFilters[1][0].Process(m_CrossSplitBuffers_[1][i]);
    m_BandFilters[1][1].Process(m_CrossSplitBuffers_[1][i]);
    m_CrossSplitBuffers_[2][i] = channel;
    m_BandFilters[2][0].Process(m_CrossSplitBuffers_[2][i]);
  }
}
void ThreeBandProcessor::CombineBuffers() {
  for (size_t i = 0; i < m_CurrentIndex; ++i) {
    auto &channel = m_ProcessingBuffer[i];
    channel = m_CrossSplitBuffers_[0][i];
    channel += m_CrossSplitBuffers_[1][i];
    channel += m_CrossSplitBuffers_[2][i];
  }
}

void ThreeBandProcessor::WriteToBuffer(float *leftBuffer, float *rightBuffer) {
  size_t innerIdx = 0;
  for (size_t i = 0; i < m_CurrentIndex; i += 2) {
    leftBuffer[innerIdx] = m_ProcessingBuffer[i].Left;
    rightBuffer[innerIdx] = m_ProcessingBuffer[i].Right;
    innerIdx++;
  }
  m_CurrentIndex = 0;
}
} // namespace VSTZ