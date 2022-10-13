#pragma once

#include "JuceHeader.h"

#include <Core/Instance.h>
#include <Core/Parameter/Parameter.h>

struct ProcessorParameters {
  VSTZ::Core::Parameter *Input;
  VSTZ::Core::Parameter *Output;
  VSTZ::Core::Parameter *DistMix;
  VSTZ::Core::Parameter *DryWet;
  VSTZ::Core::Parameter *Bypass;
};
class VSTProcessor : public juce::AudioProcessor {
public:
  VSTProcessor();
  ~VSTProcessor() override;

  void prepareToPlay(double, int) override;
  void releaseResources() override {}

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled() ||
        layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
      return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
      return false;

    return layouts.getMainInputChannelSet() ==
           layouts.getMainOutputChannelSet();
  }

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }

  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override {
    return juce::String("VeClip");
  }
  void changeProgramName(int, const juce::String &) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  VSTZ::Core::Instance *instance = nullptr;

private:
  std::string m_id;
  ProcessorParameters m_ProcessorParameters;
  juce::AudioProcessorValueTreeState treeState;
};
