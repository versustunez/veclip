#pragma once

#include "Core/Instance.h"

#include <GUI/Components/Components.h>
#include <TypeDefs.h>

namespace VSTZ {

class BufferDrawer: public GUI::VComponent {
public:
  explicit BufferDrawer(InstanceID id, bool isVU);
  void paint(juce::Graphics &g) override;
protected:
  InstanceID m_ID{};
  bool m_IsVU{};
  Core::Instance* m_Instance{nullptr};
  Channel m_Last{-70,-70};
};

class ClipLED : public GUI::VComponent {
public:
  explicit ClipLED(InstanceID id);
  void paint(juce::Graphics &g) override;
protected:
  InstanceID m_ID{};
  Core::Instance* m_Instance{nullptr};
};

class UI : public GUI::VComponent {
public:
  explicit UI(InstanceID id) : m_ID(id) { Init(); };
  void Init();
  void paint(juce::Graphics &g) override;
  void resized() override;

protected:
  InstanceID m_ID{};
  Scope<GUI::Knob> m_Input;
  Scope<GUI::Knob> m_Output;
  Scope<GUI::Knob> m_DistMix;
  Scope<GUI::Switch> m_Bypass;
  Scope<GUI::Switch> m_Delta;
  Scope<ClipLED> m_ClipLED;
  Scope<BufferDrawer> m_VU;
  Core::Instance* m_Instance;
};
} // namespace VSTZ
