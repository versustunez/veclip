#pragma once

#include "Buffer.h"

#include <Core/Parameter/Handler.h>
#include <Core/State/InstanceState.h>
#include <JuceHeader.h>
#include <TypeDefs.h>

namespace VSTZ::Core {
// Instance has a GetInstance method
class Instance {
public:
  InstanceID id;
  double bpm{120};

public:
  static Instance *get(InstanceID m_id);
  static Instance *create();
  static void remove(InstanceID m_id);
  static State::InstanceState *getState(InstanceID id);
  void init();
  Scope<ParameterHandler> handler{nullptr};
  juce::AudioProcessorValueTreeState *treeState = nullptr;
  State::InstanceState state{id};
  Ref<Buffer> buffer{};
  juce::AudioProcessorEditor *editor{};
  bool isClipping{false};

protected:
  explicit Instance(InstanceID _id) : id(_id) {}
  ~Instance();
  static std::vector<Instance *> m_instances;
protected:
  static Mutex s_CreateInstanceGuard;
};
} // namespace VSTZ::Core