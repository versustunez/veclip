#include "PluginProcessor.h"

#include "Core/Config.h"
#include "Core/Instance.h"
#include "PluginEditor.h"

VSTProcessor::VSTProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      instance(VSTZ::Core::Instance::create()),
      m_id(juce::Uuid().toString().toStdString()),
      treeState(*this, nullptr, JucePlugin_Name,
                instance->handler->SetupProcessor()) {
  instance->treeState = &treeState;
  VSTZ::Core::Config::get().registerProcessor(m_id, this);
  m_ProcessorParameters.Input = instance->handler->GetParameter("input");
  m_ProcessorParameters.Output = instance->handler->GetParameter("output");
  m_ProcessorParameters.DistMix = instance->handler->GetParameter("dist_mix");
  m_ProcessorParameters.DryWet = instance->handler->GetParameter("dry_wet");
  m_ProcessorParameters.Bypass = instance->handler->GetParameter("bypass");
}

float lerp(float a, float b, float f) { return a + f * (b - a); }

float mixed(float input, float mix) {
  float hardClip = std::clamp(input, -1.0f, 1.0f);
  float softClip = std::clamp(std::atan(input) * 0.637, -1.0,
                              1.0); // -> Range to max 1.00059
  return lerp(softClip, hardClip, mix);
}

void VSTProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                juce::MidiBuffer &) {
  for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    buffer.clear(i, 0, buffer.getNumSamples());
  instance->isClipping = false;
  if (m_ProcessorParameters.Bypass->getBool()) {
    instance->buffer->SetSamples(buffer);
    return;
  }

  float inputGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Input->getValue(), -50.0f);
  float outputGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Output->getValue(), -50.0f);

  float autoGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Input->getValue() * 0.5f * -1.0f, -50.0f);

  juce::AudioBuffer<float> processBuffer;
  processBuffer.makeCopyOf(buffer);

  int numSamples = buffer.getNumSamples();
  float mixValue = m_ProcessorParameters.DistMix->getValue();
  for (int channel = 0; channel < 2; ++channel) {
    float *channelData = processBuffer.getWritePointer(channel);
    for (int i = 0; i < numSamples; i++) {
      auto val = mixed(channelData[i] * inputGain, mixValue);
      if (val >= 0.95)
        instance->isClipping = true;
      channelData[i] = val * autoGain;
    }
  }
  float mix = m_ProcessorParameters.DryWet->getValue();
  for (int channel = 0; channel < 2; ++channel) {
    float *channelData = buffer.getWritePointer(channel);
    const float *processedData = processBuffer.getReadPointer(channel);

    for (int i = 0; i < numSamples; i++) {
      channelData[i] = lerp(channelData[i], processedData[i], mix) * outputGain;
    }
  }
  instance->buffer->SetSamples(buffer);
}

juce::AudioProcessorEditor *VSTProcessor::createEditor() {
  return new VSTEditor(*this, m_id);
}

void VSTProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = treeState.copyState();
  auto xmlState = VSTZ::Scope<juce::XmlElement>();
  xmlState.Create("State");
  xmlState->addChildElement(state.createXml().release());
  copyXmlToBinary(*xmlState, destData);
}

void VSTProcessor::setStateInformation(const void *data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState == nullptr)
    return;
  auto *parameters = xmlState->getChildByName(treeState.state.getType());
  auto name = xmlState->getStringAttribute("name");
  if (parameters)
    treeState.replaceState(juce::ValueTree::fromXml(*parameters));
}
VSTProcessor::~VSTProcessor() {
  VSTZ::Core::Config::get().removeProcessor(m_id);
  VSTZ::Core::Instance::remove(instance->id);
}
void VSTProcessor::prepareToPlay(double sampleRate, int blockSize) {
  auto &config = VSTZ::Core::Config::get();
  if (config.sampleRate != sampleRate)
    config.sampleRate = sampleRate;
  instance->buffer->Reset(blockSize);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new VSTProcessor();
}
