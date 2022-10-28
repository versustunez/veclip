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
  m_ProcessorParameters.Bypass = instance->handler->GetParameter("bypass");
  m_ProcessorParameters.Delta = instance->handler->GetParameter("delta");
}

void VSTProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                juce::MidiBuffer &) {
  for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  if (m_ProcessorParameters.Bypass->getBool()) {
    instance->m_ClippingValue = {};
    instance->buffer->SetSamples(buffer);
    return;
  }

  m_Distroyer.InputGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Input->getValue(), -50.0f);
  m_Distroyer.OutputGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Output->getValue(), -50.0f);
  m_Distroyer.AutoGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Input->getValue() * -1.0f, -50.0f);
  m_Distroyer.MixValue = m_ProcessorParameters.DistMix->getValue();
  m_Distroyer.OutputDelta = m_ProcessorParameters.Delta->getBool();

  instance->m_ClippingValue = m_Distroyer.Process(buffer);
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
  m_Distroyer.Setup(sampleRate);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new VSTProcessor();
}
