#include "BandUI.h"

namespace VSTZ {

BandUI::BandUI(const std::string &band, InstanceID id) : m_ID(id) {
  m_Instance = Core::Instance::get(id);

  m_Input.Create("drive_" + band, "Drive", m_ID);
  m_Input->EnableLinearModeBar();
  m_Input->enableLiveLabel(true);

  m_DistMix.Create("mix_" + band, "Dist", m_ID);
  m_DistMix->setIsValueBox(true);

  m_Bypass.Create("bypass_" + band, "", m_ID);
  m_Bypass->setButtonText("Bypass");

  m_Mute.Create("mute_" + band, "", m_ID);
  m_Mute->setButtonText("Mute");

  addAndMakeVisible(*m_Input);
  addAndMakeVisible(*m_DistMix);
  addAndMakeVisible(*m_Bypass);
  addAndMakeVisible(*m_Mute);
}

void BandUI::resized() {
  juce::FlexBox fb;
  fb.flexWrap = juce::FlexBox::Wrap::wrap;
  fb.justifyContent = juce::FlexBox::JustifyContent::center;
  fb.flexDirection = juce::FlexBox::Direction::column;

  float opH = (getHeight() - 50.0f) / 100.0f;
  fb.items.add(juce::FlexItem(*m_Input).withMinHeight(opH * 75.0f));
  fb.items.add(juce::FlexItem(*m_DistMix).withMinHeight(opH * 25.0f));
  fb.items.add(juce::FlexItem(*m_Bypass).withHeight(20));
  fb.items.add(juce::FlexItem(*m_Mute).withHeight(20));
  fb.performLayout(getLocalBounds().toFloat());
}
} // namespace VSTZ