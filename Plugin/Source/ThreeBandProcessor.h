#pragma once

#include "Structs.h"

#include <cstdlib>
#include <functional>
#include <vector>

namespace VSTZ {

class LinkWitzRileyFilter {
public:
  enum class FilterType { LOW = 0, HIGH };
  void LowPass(double frequency, double sampleRate);
  void HighPass(double frequency, double sampleRate);

  void Process(Channel &channel);
  double ProcessSingle(double input, int channelVal);

private:
  void MakeFilter(double frequency, double sampleRate);

private:
  struct FilterCoefficents {
    double a0, a1, a2, a3, a4;
  };
  double b1co, b2co, b3co, b4co;
  double coFreqRunningAv = 100.0f;
  struct TempValues {
    double xm1 = 0.0f;
    double xm2 = 0.0f;
    double xm3 = 0.0f;
    double xm4 = 0.0f;
    double ym1 = 0.0f, ym2 = 0.0f, ym3 = 0.0f, ym4 = 0.0f;
  };

private:
  FilterType m_Type{FilterType::LOW};
  FilterCoefficents m_Coeff{};
  TempValues m_Temp_[2]{};
};

class ThreeBandProcessor {
public:
  ThreeBandProcessor();
  void Reset() { m_CurrentIndex = 0; }
  void SetSampleRate(double sampleRate);
  void SetSampleSize(size_t blockSize);
  void SetBandSplit(int band, double frequency);
  void AddSample(Channel channel);
  void Process();
  void SetProcessingFunction(
      const std::function<void(std::vector<Channel> &, size_t count, size_t band)> &function);

  std::vector<Channel>& GetBuffer() { return m_ProcessingBuffer; }
  size_t GetSize() { return m_CurrentIndex; }
  void WriteToBuffer(float *leftBuffer, float *rightBuffer);
protected:
  void SplitBuffer();
  void CombineBuffers();

protected:
  std::function<void(std::vector<Channel> &, size_t count, size_t)> m_ProcessingCallback;
  double m_SampleRate{44100};
  size_t m_CurrentIndex{0};
  std::vector<Channel> m_ProcessingBuffer;
  std::vector<Channel> m_CrossSplitBuffers_[3];
  std::vector<std::vector<LinkWitzRileyFilter>> m_BandFilters;

};
} // namespace VSTZ
