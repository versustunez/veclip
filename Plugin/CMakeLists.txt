project(VeClip VERSION 0.0.1)

set(BaseTargetName ${PROJECT_NAME})

juce_add_plugin(
  "${BaseTargetName}"
  COMPANY_NAME "VstZ"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  VST3_CATEGORIES FX
  AU_MAIN_TYPE kAudioUnitType_Generator
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  PLUGIN_MANUFACTURER_CODE VstZ
  PLUGIN_CODE VeCp
  FORMATS VST3 Standalone
  PRODUCT_NAME "VeClip"
)

set_target_properties(${BaseTargetName} PROPERTIES CXX_STANDARD 17)

add_subdirectory(Assets)
add_subdirectory(Source)
add_subdirectory(Vendor)


get_property(source_files GLOBAL PROPERTY SRCS)
target_sources(${BaseTargetName} PRIVATE ${source_files})

target_compile_definitions(
  ${BaseTargetName}
  PUBLIC
  JUCE_DISPLAY_SPLASH_SCREEN=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

target_include_directories(${BaseTargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source/)
target_include_directories(${BaseTargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/)

target_link_libraries(
  ${BaseTargetName} PRIVATE
  juce::juce_audio_utils
  juce::juce_opengl
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)
