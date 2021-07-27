# CMake file for DISTRHO Plugins #
# ------------------------------ #
add_subdirectory(DPF)

dpf_add_plugin(fabric
  TARGETS jack dssi lv2 vst2
  FILES_DSP
      fabricDSP.cpp
  FILES_UI
      fabricUI.cpp)

target_include_directories(
  fabric PUBLIC ".")