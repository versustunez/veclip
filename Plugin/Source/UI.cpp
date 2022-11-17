#include "UI.h"

#include "Core/Config.h"
namespace VSTZ {
void UI::Init() {
  m_Instance = Core::Instance::get(m_ID);
  m_Output.Create("output", "Output", m_ID);
  m_Output->EnableLinearModeBar();
  m_Output->enableLiveLabel(true);
  m_Bypass.Create("bypass", "Bypass", m_ID);
  m_Bypass->setButtonText("Bypass");
  m_Delta.Create("delta", "Delta", m_ID);
  m_Delta->setButtonText("Delta");
  m_ClipLED.Create(m_ID);
  m_VU.Create(m_ID, true);

  m_BandUI_[0].Create("low", m_ID);
  m_BandUI_[1].Create("mid", m_ID);
  m_BandUI_[2].Create("high", m_ID);

  m_LowCross.Create("low_cross", "Low Split", m_ID);
  m_LowCross->setIsValueBox(true);

  m_HighCross.Create("high_cross", "High Split", m_ID);
  m_HighCross->setIsValueBox(true);

  addAndMakeVisible(*m_Output);
  addAndMakeVisible(*m_Bypass);
  addAndMakeVisible(*m_Delta);
  addAndMakeVisible(*m_ClipLED);
  addAndMakeVisible(*m_BandUI_[0]);
  addAndMakeVisible(*m_BandUI_[1]);
  addAndMakeVisible(*m_BandUI_[2]);
  addAndMakeVisible(*m_VU);
  addAndMakeVisible(*m_LowCross);
  addAndMakeVisible(*m_HighCross);
}
void UI::resized() {
  m_ClipLED->setBounds(320, 10, 60, 20);
  m_VU->setBounds(20, 10, 290, 20);
  m_LowCross->setBounds(20, 40, 60, 40);
  m_HighCross->setBounds(320, 40, 60, 40);

  juce::FlexBox fb;
  fb.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

  juce::FlexBox outputFB;
  outputFB.flexDirection = juce::FlexBox::Direction::column;
  outputFB.items.add(juce::FlexItem(*m_Output)
                         .withWidth(60)
                         .withHeight(getHeight() - 145)
                         .withMargin(juce::FlexItem::Margin(5, 0, 0, 0)));
  outputFB.items.add(juce::FlexItem(*m_Bypass).withHeight(20));
  outputFB.items.add(juce::FlexItem(*m_Delta).withHeight(20));

  fb.items.add(juce::FlexItem(*m_BandUI_[0]).withWidth(60));
  fb.items.add(juce::FlexItem(*m_BandUI_[1]).withWidth(60));
  fb.items.add(juce::FlexItem(*m_BandUI_[2]).withWidth(60));
  fb.items.add(juce::FlexItem(outputFB).withWidth(60).withAlignSelf(
      juce::FlexItem::AlignSelf::stretch));
  fb.performLayout(juce::Rectangle<float>{0.f, 85.0f, (float)getWidth(),
                                          getHeight() - 95.0f});
}

void UI::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0.f,0.f,0.f,.4f));
  g.fillRect(0,0,getWidth(), 84);
  auto color = Core::Config::get().theme()->getColor(Theme::Colors::accent);
  g.setColour(color);
  g.fillRect(0,84,getWidth(),1);
  g.setColour(juce::Colours::white);
  g.setFont(36);
  g.drawText("VeClip", 0, 35, getWidth(), 40, juce::Justification::centred,
             true);
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
  constexpr float cornerSize = 4;
  auto theme = Core::Config::get().theme();
  g.setColour(theme->getColor(Theme::Colors::bgTwo));
  g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), cornerSize);
  auto color = theme->getColor(Theme::Colors::clip);
  auto color2 = theme->getColor(Theme::Colors::accent);
  auto buffer = m_Instance->buffer->Peak();
  int halfHeight = (int)(double(getHeight() - 4) / 2.0);
  int barWidth = getWidth() - 2;
  int leftBarWidth =
      normalizeBufferRange((m_Last.Left + buffer.Left) * 0.5) * barWidth;
  int rightBarWidth =
      normalizeBufferRange((m_Last.Right + buffer.Right) * 0.5) * barWidth;

  int zeroDecibelLineX = normalizeBufferRange(0) * barWidth;
  constexpr int xOffset = 1;

  g.setColour(theme->getColor(Theme::Colors::bg).withAlpha(0.5f));
  g.drawLine(zeroDecibelLineX, 0, zeroDecibelLineX, getHeight(), 1);
  // LEFT Channel
  g.setColour(MixColor(color2, color, normalizeUIBufferRange(buffer.Left)));
  g.fillRoundedRectangle(xOffset, 1, leftBarWidth, halfHeight, cornerSize);
  // Right Channel
  g.setColour(MixColor(color2, color, normalizeUIBufferRange(buffer.Right)));
  g.fillRoundedRectangle(xOffset, halfHeight + 3, rightBarWidth, halfHeight,
                         cornerSize);

  m_Last = buffer;
}

ClipLED::ClipLED(InstanceID id) : m_ID(id) {
  m_Instance = Core::Instance::get(m_ID);
}

void ClipLED::paint(juce::Graphics &g) {
  auto theme = Core::Config::get().theme();
  auto color = theme->getColor(Theme::Colors::clip);
  auto color2 = theme->getColor(Theme::Colors::accent);
  auto peak = m_Instance->m_ClippingValue;
  double maxValue = std::max(peak.Left, peak.Right);
  g.setColour(MixColor(color2, color, maxValue));
  g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 5.0);
}
} // namespace VSTZ
