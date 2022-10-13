#include "UI.h"

#include "Core/Config.h"
namespace VSTZ {
void UI::Init() {
  m_Instance = Core::Instance::get(m_ID);
  m_Input.Create("input", "Input", m_ID);
  m_Input->EnableLinearModeBar();
  m_Input->enableLiveLabel(true);

  m_Output.Create("output", "Output", m_ID);
  m_Output->EnableLinearModeBar();
  m_Output->enableLiveLabel(true);

  m_DistMix.Create("dist_mix", "Dist Mix", m_ID);
  m_DistMix->enableLiveLabel(true);

  m_DryWet.Create("dry_wet", "Dry/Wet", m_ID);
  m_DryWet->enableLiveLabel(true);

  m_Bypass.Create("bypass", "Bypass", m_ID);
  m_Bypass->setButtonText("Bypass");

  m_ClipLED.Create(m_ID);
  m_VU.Create(m_ID, true);

  addAndMakeVisible(*m_Input);
  addAndMakeVisible(*m_Output);
  addAndMakeVisible(*m_DistMix);
  addAndMakeVisible(*m_DryWet);
  addAndMakeVisible(*m_Bypass);
  addAndMakeVisible(*m_ClipLED);
  addAndMakeVisible(*m_VU);
}
void UI::resized() {
  m_Bypass->setBounds(10, 10, 50, 20);
  m_ClipLED->setBounds(30, 50, 10, 10);
  m_VU->setBounds(70, 10, 170, 20);
  m_DryWet->setBounds(240, 20, 80, 60);
  m_Input->setBounds(10, 90, 50, 130);
  m_DistMix->setBounds(70, 105, 170, 120);
  m_Output->setBounds(255, 90, 50, 130);
}

void UI::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white);
  g.setFont(36);
  g.drawText("VeClip", 70, 40, 170, 40, juce::Justification::centred, true);
}

BufferDrawer::BufferDrawer(InstanceID id, bool isVU) : m_ID(id), m_IsVU(isVU) {
  m_Instance = Core::Instance::get(m_ID);
}

float normalizeBufferRange(float val) {
  return (float)std::clamp((val - -70.0) / (6.0 - -70.0), 0.0, 1.0);
}
void BufferDrawer::paint(juce::Graphics &g) {
  auto theme = Core::Config::get().theme();
  g.setColour(theme->getColor(Theme::Colors::bgTwo));
  g.drawRect(0, 0, getWidth(), getHeight());
  g.setColour(theme->getColor(Theme::Colors::accent));
  auto buffer = m_Instance->buffer->Peak();
  double half = getHeight() / 2.0;
  int leftWidth =
      normalizeBufferRange((m_Last.left + buffer.left) * 0.5) * getWidth();
  int rightWidth =
      normalizeBufferRange((m_Last.right + buffer.right) * 0.5) * getWidth();
  g.fillRect(0, 0, leftWidth, half);
  g.fillRect(0, half, rightWidth, half);
  m_Last = buffer;
}

ClipLED::ClipLED(InstanceID id) : m_ID(id) {
  m_Instance = Core::Instance::get(m_ID);
}
void ClipLED::paint(juce::Graphics &g) {
  auto theme = Core::Config::get().theme();
  if (m_Instance->isClipping)
    g.fillAll(theme->getColor(Theme::Colors::clip));
  else
    g.fillAll(juce::Colours::green);
}
} // namespace VSTZ
