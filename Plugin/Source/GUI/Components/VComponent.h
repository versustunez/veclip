#pragma once

#include <JuceHeader.h>

namespace VSTZ::GUI {

class VComponent : public juce::Component {
public:
  template <class ClassType> ClassType *parent() {
    return findParentComponentOfClass<ClassType>();
  }

  template <class T> static Ref<T> create(InstanceID id) {
    Ref<T> ref;
    ref.template Create("", "Unknown", id);
    return ref;
  }

  template <class AsClass> AsClass *as() {
    return dynamic_cast<AsClass *>(this);
  }
};
} // namespace VSTZ::GUI