#include "Handler.h"

namespace VSTZ::Core {

void ParameterHandler::SetupParameter() {
  AddParameter("input", "Input", -30, 30, 0, Float);
  AddParameter("output", "Output", -10, 10, 0, Float);
  AddParameter("dist_mix", "Distortion Mix", 0, 1, 0.5, Float);
  AddParameter("dry_wet", "Dry/Wet", 0.0, 1.0, 0.5, Float);
  AddParameter("bypass", "Bypass", 0, 1, 0, Boolean);
}
} // namespace VSTZ::Core