#pragma once

#include "NanoVG.hpp"
#include "NanoKnob.hpp"
#include "Window.hpp"
#include "Widget.hpp"
#include <string>


START_NAMESPACE_DISTRHO

class VolumeKnob : public NanoKnob,
                   public IdleCallback
{
public:
  explicit VolumeKnob(Widget  *widget, Size<uint> size) noexcept;
  void setLabel(std::string labelText);
  void setBipolar(bool _isBipolar);

protected:
  void idleCallback() override;
  void onMouseHover() override;
  void onMouseLeave() override;
  void onMouseUp() override;
  void onMouseDown() override;

  void draw() override;

private:
  bool _isBipolar = {false};
  float fKnobDiameter;
  FontId fFontID;

  Color fKnobICol;
  Color fKnobOCol;

  DISTRHO_LEAK_DETECTOR(VolumeKnob)
};

END_NAMESPACE_DISTRHO