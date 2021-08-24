# CMake file for DISTRHO Plugins #
# ------------------------------ #
cmake_minimum_required(VERSION 3.5)
project(fabric)
add_subdirectory(DPF)
find_package(Boost REQUIRED)

if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
endif()

set(fabric_TARGETS vst2 lv2)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND fabric_TARGETS jack)
endif()

dpf_add_plugin(fabric MONOLITHIC
  TARGETS ${fabric_TARGETS}
  FILES_DSP
      fabricDSP.cpp
      Grain.cpp
      GrainPlayer.cpp
      LinearSmoother.cpp
  FILES_UI
      fabricUI.cpp
      fabricWaveformDisplay.cpp
      fabricEnvelopeDisplay.cpp
      fabricGrainMeter.cpp
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
      "Resources/Fonts"
      "thirdparty/simde"
      ${Boost_INCLUDE_DIRS})

add_library(simde INTERFACE)
target_include_directories(simde
  INTERFACE
  "thirdparty/simde")

target_link_libraries(fabric
  PRIVATE
  simde)

function(target_enable_fast_math NAME ACCESS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options("${NAME}" ${ACCESS} "-ffast-math")
    elseif(MSVC)
        target_compile_options("${NAME}" ${ACCESS} "/fp:fast")
    endif()
endfunction()

target_enable_fast_math(fabric PUBLIC)
