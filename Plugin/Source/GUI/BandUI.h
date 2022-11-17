#pragma once

#include "Core/Instance.h"
#include "GUI/Components/Knob.h"
#include "GUI/Components/Switch.h"
#include "GUI/Components/VComponent.h"
#include "TypeDefs.h"

#include <string>

namespace VSTZ {
class BandUI : public GUI::VComponent {
public:
  BandUI(const std::string& band, InstanceID id);
  void resized() override;

protected:
  InstanceID m_ID{};
  Core::Instance* m_Instance;
  Scope<GUI::Knob> m_Input;
  Scope<GUI::Knob> m_DistMix;
  Scope<GUI::Switch> m_Bypass;
  Scope<GUI::Switch> m_Mute;
};
} // namespace VSTZ
