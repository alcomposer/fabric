# CMake file for DISTRHO Plugins #
# ------------------------------ #
add_subdirectory(DPF)

dpf_add_plugin(fabric
  TARGETS jack lv2 vst2
  FILES_DSP
      fabricDSP.cpp
  FILES_UI
      fabricUI.cpp

      fabricController.cpp

      widgets/src/WolfWidget.cpp
      widgets/src/NanoKnob.cpp
      widgets/src/VolumeKnob.cpp
      widgets/src/NanoLabel.cpp

      util/src/Mathf.cpp

      Structures/src/Margin.cpp
      
      Resources/Fonts/src/chivo_bold.cpp)

target_include_directories(
  fabric PUBLIC 
      "."
      "widgets"
      "Structures"
      "util"
      "Resources/Fonts")