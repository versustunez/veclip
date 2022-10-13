#include <Core/Config.h>
#include <GUI/Theme/LookAndFeel/FlatLook.h>
#include <GUI/Theme/Theme.h>

namespace VSTZ::GUI {
typedef VSTZ::Theme::Colors Colors;
FlatLook::FlatLook() {
  theme = Core::Config::get().theme().Get();
}

FlatLook::~FlatLook() {
  theme = nullptr;
}

void FlatLook::drawButtonBackground(juce::Graphics &graphics,
                                    juce::Button &button, const juce::Colour &,
                                    bool shouldDrawButtonAsHighlighted, bool) {
  auto buttonArea = button.getLocalBounds();
  if (shouldDrawButtonAsHighlighted)
    graphics.setColour(theme->getColor(Theme::Colors::accent));
  else
    graphics.setColour(theme->getColor(Theme::Colors::accentTwo));
  graphics.drawRect(buttonArea);
}

void FlatLook::drawRotarySlider(juce::Graphics &g, int x, int y, int width,
                                int height, float sliderPosProportional,
                                float rotaryStartAngle, float rotaryEndAngle,
                                juce::Slider &) {
  float MAX_RADIAN = 2.53073;
  float radius = juce::jmin(width / 2.0f, height / 2.0f) - 3.5f;
  auto centreX = x + width * 0.5f;
  auto centreY = y + height * 0.5f;
  float arcPos = sliderPosProportional;
  auto angle = rotaryStartAngle +
               sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
  float size = 3.5f;

  //---[the real draw]---//
  juce::Path outerArc;
  outerArc.addCentredArc(0, 0, radius, radius, 0, -MAX_RADIAN, MAX_RADIAN,
                         true);
  outerArc.applyTransform(juce::AffineTransform().translated(centreX, centreY));
  g.setColour(theme->getColor(Colors::accent).withAlpha(0.2f));
  g.strokePath(outerArc, juce::PathStrokeType(size));

  // prepare pointer for drawing
  juce::Path arc;
  arc.addCentredArc(
      0, 0, radius, radius, 0, -MAX_RADIAN,
      (float)std::clamp(arcPos * 2.0 - 1.0, -1.0, 1.0) * MAX_RADIAN, true);
  arc.applyTransform(juce::AffineTransform().translated(centreX, centreY));
  auto colourTwo = theme->getColor(Colors::accentTwo);
  auto colourMain = theme->getColor(Colors::accent);
  g.setGradientFill(juce::ColourGradient::horizontal(
      colourTwo, centreX - radius, colourMain, centreX + radius));
  g.strokePath(arc, juce::PathStrokeType(size));

  juce::Path pointer;
  auto pointerThickness = 3;
  auto l = 5;
  pointer.addEllipse(-pointerThickness * 0.25f, -(radius - l), pointerThickness,
                     pointerThickness);
  pointer.applyTransform(
      juce::AffineTransform::rotation(angle).translated(centreX, centreY));
  g.setColour(theme->getColor(Colors::accent));
  g.fillPath(pointer);
}

void FlatLook::drawTextEditorOutline(juce::Graphics &graphics, int width,
                                     int height, juce::TextEditor &) {
  graphics.setGradientFill(juce::ColourGradient::horizontal(
      theme->getColor(Colors::accentTwo), 0, theme->getColor(Colors::accent),
      (float)width));
  float p = (float)width / 100.0f * 2.5f;
  graphics.drawLine(p, (float)height, (float)width - p, (float)height, 1.3f);
}

void FlatLook::drawToggleButton(juce::Graphics &graphics,
                                juce::ToggleButton &button, bool,
                                bool shouldDrawButtonAsDown) {
  int height = button.getHeight();
  int width = button.getWidth();
  auto text = button.getButtonText();
  bool isPressed = shouldDrawButtonAsDown ||
                   button.getToggleStateValue() == button.buttonDown;
  if (text == "")
    text = isPressed ? "On" : "Off";

  auto color = theme->getColor(Colors::accent);
  graphics.setColour(isPressed ? color : color.withAlpha(0.2f));

  auto l = 2;
  auto p = 4;
  graphics.fillRect(l, l, width - p, height - p);
  graphics.setColour(theme->getColor(Colors::font));
  graphics.drawText(text, l, l, width - p, height - p,
                    juce::Justification::centred, true);
}

void FlatLook::drawTabButton(juce::TabBarButton &button,
                             juce::Graphics &graphics, bool, bool) {
  const juce::Rectangle<int> activeArea(button.getActiveArea());
  graphics.setColour(theme->getColor(Colors::bg));
  graphics.fillRect(activeArea);

  int width = button.getWidth();
  int height = button.getHeight() - 1;
  auto color = theme->getColor(Colors::accent);
  graphics.setColour(button.getToggleState() ? color : color.withAlpha(0.2f));
  auto p = 2;
  graphics.fillRect(0, height - p, width, p);
  graphics.setColour(theme->getColor(Colors::font));
  graphics.drawText(button.getButtonText(), 0, 0, width, height,
                    juce::Justification::centred, true);
}

void FlatLook::drawComboBox(juce::Graphics &graphics, int width, int height,
                            bool, int, int, int, int, juce::ComboBox &) {
  auto borderHeight = 2;
  graphics.setColour(theme->getColor(Colors::accent));
  graphics.fillRect(0, height - borderHeight, width, borderHeight);
  graphics.setColour(theme->getColor(Colors::bgTwo).withAlpha(0.2f));
  graphics.fillRect(0, 0, width, height - borderHeight);
}

void FlatLook::drawLabel(juce::Graphics &graphics, juce::Label &label) {
  if (!label.isBeingEdited()) {
    auto alpha = label.isEnabled() ? 1.0f : 0.5f;
    const juce::Font font(getLabelFont(label));
    graphics.setColour(
        label.findColour(juce::Label::textColourId).withAlpha(alpha));
    auto textArea =
        getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
    graphics.drawFittedText(
        label.getText(), textArea, label.getJustificationType(),
        juce::jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
        label.getMinimumHorizontalScale());
  }
}
void FlatLook::drawBubble(juce::Graphics &graphics, juce::BubbleComponent &,
                          const juce::Point<float> &,
                          const juce::Rectangle<float> &body) {
  graphics.setColour(theme->getColor(Colors::bg));
  graphics.fillRect(body);
  graphics.setColour(theme->getColor(Colors::accent));
  graphics.drawRect(body, 1.0f);
}
} // namespace VSTZ::GUI
