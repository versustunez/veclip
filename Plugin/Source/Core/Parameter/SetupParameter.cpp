#include "Handler.h"

namespace VSTZ::Core {

void ParameterHandler::SetupParameter() {
  AddParameter("drive_low", "Low Drive", 0, 30, 0, Float);
  AddParameter("mix_low", "Dist Mix", 0, 1, 0.5, Float);
  AddParameter("mute_low", "Mute", 0, 1, 0, Boolean);
  AddParameter("bypass_low", "Bypass", 0, 1, 0, Boolean);

  AddParameter("drive_mid", "Mid Drive", 0, 30, 0, Float);
  AddParameter("mix_mid", "Dist Mix", 0, 1, 0.5, Float);
  AddParameter("mute_mid", "Mute", 0, 1, 0, Boolean);
  AddParameter("bypass_mid", "Bypass", 0, 1, 0, Boolean);

  AddParameter("drive_high", "Mid Drive", 0, 30, 0, Float);
  AddParameter("mix_high", "Dist Mix", 0, 1, 0.5, Float);
  AddParameter("mute_high", "Mute", 0, 1, 0, Boolean);
  AddParameter("bypass_high", "Bypass", 0, 1, 0, Boolean);

  AddParameter("output", "Output", -10, 10, 0, Float);
  AddParameter("bypass", "Bypass", 0, 1, 0, Boolean);
  AddParameter("delta", "Delta", 0, 1, 0, Boolean);

  AddParameter("low_cross", "Delta", 100, 1200, 500, Float);
  AddParameter("high_cross", "Delta", 1400, 8000, 2000, Float);
}
} // namespace VSTZ::Core