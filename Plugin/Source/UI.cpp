#include "UI.h"

#include "Core/Config.h"
namespace VSTZ {
void UI::Init() {
  m_Instance = Core::Instance::get(m_ID);
  m_Input.Create("input", "Drive", m_ID);
  m_Input->EnableLinearModeBar();
  m_Input->enableLiveLabel(true);

  m_Output.Create("output", "Output", m_ID);
  m_Output->EnableLinearModeBar();
  m_Output->enableLiveLabel(true);

  m_DistMix.Create("dist_mix", "Dist Mix", m_ID);
  m_DistMix->enableLiveLabel(true);

  m_Bypass.Create("bypass", "Bypass", m_ID);
  m_Bypass->setButtonText("Bypass");

  m_ClipLED.Create(m_ID);
  m_VU.Create(m_ID, true);

  addAndMakeVisible(*m_Input);
  addAndMakeVisible(*m_Output);
  addAndMakeVisible(*m_DistMix);
  addAndMakeVisible(*m_Bypass);
  addAndMakeVisible(*m_ClipLED);
  addAndMakeVisible(*m_VU);
}
void UI::resized() {
  m_Bypass->setBounds(10, 10, 50, 20);
  m_ClipLED->setBounds(260, 10, 50, 20);
  m_VU->setBounds(70, 10, 180, 20);
  m_Input->setBounds(10, 90, 50, 130);
  m_DistMix->setBounds(70, 105, 170, 120);
  m_Output->setBounds(260, 90, 50, 130);
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
  return (float)std::clamp((val - -70.0) / (3.0 - -70.0), 0.0, 1.0);
}

float normalizeUIBufferRange(float val) {
  return (float)std::clamp((val - -6.0) / (0.0 - -6.0), 0.0, 1.0);
}

static double lerp(double a, double b, double f) { return a + f * (b - a); }

static juce::Colour MixColor(const juce::Colour &l, const juce::Colour &r,
                             float mixVal) {
  auto red = (float)lerp(l.getFloatRed(), r.getFloatRed(), mixVal);
  auto green = (float)lerp(l.getFloatGreen(), r.getFloatGreen(), mixVal);
  auto blue = (float)lerp(l.getFloatBlue(), r.getFloatBlue(), mixVal);
  return juce::Colour::fromFloatRGBA(red, green, blue, 1.0);
}

void BufferDrawer::paint(juce::Graphics &g) {
  auto theme = Core::Config::get().theme();
  g.setColour(theme->getColor(Theme::Colors::bgTwo));
  g.drawRect(0, 0, getWidth(), getHeight());
  auto color = theme->getColor(Theme::Colors::clip);
  auto color2 = theme->getColor(Theme::Colors::accent);
  auto buffer = m_Instance->buffer->Peak();
  int halfHeight = (int)(double(getHeight() - 4) / 2.0);
  int barWidth = getWidth() - 2;
  int leftBarWidth = normalizeBufferRange((m_Last.Left + buffer.Left) * 0.5) * barWidth;
  int rightBarWidth = normalizeBufferRange((m_Last.Right + buffer.Right) * 0.5) * barWidth;

  int zeroDecibelLineX = normalizeBufferRange(0) * barWidth;
  constexpr int xOffset = 1;

  g.setColour(theme->getColor(Theme::Colors::bgTwo).withAlpha(0.3f));
  g.drawLine(zeroDecibelLineX, 0, zeroDecibelLineX, getHeight(), 1);

  // LEFT Channel
  g.setColour(MixColor(color2, color, normalizeUIBufferRange(buffer.Left)));
  g.fillRect(xOffset, 1, leftBarWidth, halfHeight);
  // Right Channel
  g.setColour(MixColor(color2, color, normalizeUIBufferRange(buffer.Right)));
  g.fillRect(xOffset, halfHeight + 3, rightBarWidth, halfHeight);

  m_Last = buffer;
}

ClipLED::ClipLED(InstanceID id) : m_ID(id) {
  m_Instance = Core::Instance::get(m_ID);
}

void ClipLED::paint(juce::Graphics &g) {
  auto theme = Core::Config::get().theme();
  auto color = theme->getColor(Theme::Colors::clip);
  auto color2 = theme->getColor(Theme::Colors::accent);
  auto &peak = m_Instance->m_ClippingValue;
  double maxValue = std::max(peak.Left, peak.Right);
  g.fillAll(MixColor(color2, color, maxValue));
  g.drawRect(0, 0, getWidth(), getHeight(), 1);
}
} // namespace VSTZ
