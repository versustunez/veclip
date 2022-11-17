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

  m_Distroyer.Parameters_[0].Mix = instance->handler->GetParameter("mix_low");
  m_Distroyer.Parameters_[0].Input = instance->handler->GetParameter("drive_low");
  m_Distroyer.Parameters_[0].Bypass = instance->handler->GetParameter("bypass_low");
  m_Distroyer.Parameters_[0].Mute = instance->handler->GetParameter("mute_low");

  m_Distroyer.Parameters_[1].Mix = instance->handler->GetParameter("mix_mid");
  m_Distroyer.Parameters_[1].Input = instance->handler->GetParameter("drive_mid");
  m_Distroyer.Parameters_[1].Bypass = instance->handler->GetParameter("bypass_mid");
  m_Distroyer.Parameters_[1].Mute = instance->handler->GetParameter("mute_mid");

  m_Distroyer.Parameters_[2].Mix = instance->handler->GetParameter("mix_high");
  m_Distroyer.Parameters_[2].Input = instance->handler->GetParameter("drive_high");
  m_Distroyer.Parameters_[2].Bypass = instance->handler->GetParameter("bypass_high");
  m_Distroyer.Parameters_[2].Mute = instance->handler->GetParameter("mute_high");

  m_ProcessorParameters.Output = instance->handler->GetParameter("output");
  m_ProcessorParameters.Bypass = instance->handler->GetParameter("bypass");
  m_ProcessorParameters.Delta = instance->handler->GetParameter("delta");
  m_ProcessorParameters.LowCross = instance->handler->GetParameter("low_cross");
  m_ProcessorParameters.HighCross = instance->handler->GetParameter("high_cross");
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
  m_Distroyer.OutputGain = juce::Decibels::decibelsToGain(
      (float)m_ProcessorParameters.Output->getValue(), -50.0f);
  m_Distroyer.OutputDelta = m_ProcessorParameters.Delta->getBool();
  m_Distroyer.m_LowCrossFrequency = m_ProcessorParameters.LowCross->getValue();
  m_Distroyer.m_HighCrossFrequency = m_ProcessorParameters.HighCross->getValue();

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
  m_Distroyer.Setup(sampleRate, blockSize);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new VSTProcessor();
}
