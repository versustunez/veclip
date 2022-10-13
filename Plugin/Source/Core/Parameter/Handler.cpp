#include <Core/Parameter/Handler.h>
#include <Core/Parameter/Parameter.h>
#include <JuceHeader.h>

namespace VSTZ::Core {
ParameterHandler::ParameterHandler(InstanceID id) : m_ID(id) {}
ParameterHandler::~ParameterHandler() {
  for (auto &param : m_Parameters)
    delete param.second;
  m_Params.clear();
  m_ParamMaps.clear();
  m_Parameters.clear();
}

void ParameterHandler::AddParameter(const std::string &name,
                                    const std::string &showName, double min,
                                    double max, double value,
                                    ParameterTypes type) {
  m_ParamMaps.push_back(name);
  auto *parameter = new Parameter(name, showName, min, max, value, m_ID);
  m_Parameters[name] = parameter;
  m_Params.push_back(parameter->createParameter(type));
  m_Params.back()->addListener(parameter);
}
juce::AudioProcessorValueTreeState::ParameterLayout
ParameterHandler::SetupProcessor() {
  if (m_Params.empty())
    SetupParameter();
  return {m_Params.begin(), m_Params.end()};
}
void ParameterHandler::parameterChanged(const juce::String &, float) {}
std::string ParameterHandler::GetShowName(const std::string &parameter) {
  if (m_Parameters.find(parameter) != m_Parameters.end())
    return m_Parameters[parameter]->getShowName();
  return "Unknown";
}
Core::Parameter *ParameterHandler::GetParameter(const std::string &name) {
  if (m_Parameters.find(name) != m_Parameters.end())
    return m_Parameters[name];
  return nullptr;
}
Parameter *ParameterHandler::operator[](const char *name) {
  if (m_Parameters.find(name) != m_Parameters.end())
    return m_Parameters[name];
  return nullptr;
}

} // namespace VSTZ::Core
