# CMake file for DISTRHO Plugins #
# ------------------------------ #
cmake_minimum_required(VERSION 3.5)
project(fabric)
add_subdirectory(DPF)

dpf_add_plugin(fabric MONOLITHIC
  TARGETS jack lv2 vst2
  FILES_DSP
      fabricDSP.cpp
      Grain.cpp
      GrainPlayer.cpp
  FILES_UI
      fabricUI.cpp

      fabricWaveformDisplay.cpp

      fabricEnvelopeDisplay.cpp

      fabricButton.cpp

      fabricController.cpp

      widgets/src/WolfWidget.cpp
      widgets/src/NanoKnob.cpp
      widgets/src/VolumeKnob.cpp
      widgets/src/NanoLabel.cpp
      widgets/src/NanoSwitch.cpp

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