#ifndef WOLF_NANO_KNOB_HPP_INCLUDED
#define WOLF_NANO_KNOB_HPP_INCLUDED

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "Window.hpp"
#include "WolfWidget.hpp"

START_NAMESPACE_DISTRHO

class NanoKnob : public WolfWidget
{
public:
  class Callback
  {
  public:
    virtual ~Callback() {}
    virtual void nanoKnobDragStarted(NanoKnob *nanoKnob) = 0;
    virtual void nanoKnobDragFinished(NanoKnob *nanoKnob) = 0;
    virtual void nanoKnobValueChanged(NanoKnob *nanoKnob, const float value) = 0;
  };

  explicit NanoKnob(Widget  *widget, Size<uint> size) noexcept;

  float getValue() const noexcept;
  void setValue(float value, bool sendCallback = false) noexcept;

  void setDefault(float def) noexcept;
  void setRange(float min, float max) noexcept;
  void setStep(float step) noexcept;
  void setCallback(Callback *callback) noexcept;
  void setColor(Color color) noexcept;
  void setCurve(float curve);

protected:
  virtual void knobValueChanged(bool sendCallback);
  virtual void knobDragStarted();
  virtual void knobDragFinished();

  void onNanoDisplay() override;
  bool onMouse(const MouseEvent &) override;
  bool onMotion(const MotionEvent &) override;
  bool onScroll(const ScrollEvent &) override;
  
  Color getColor() noexcept;
  float getMin() noexcept;
  float getMax() noexcept;

  virtual void onMouseHover();
  virtual void onMouseLeave();
  virtual void onMouseUp();
  virtual void onMouseDown();

  virtual void draw() = 0;

  float normalizeValue(float value);
  float denormalizeValue(float value);

private:
  float fMin;
  float fMax;
  float fStep;
  float fValue;
  float fCurve;

  bool fLeftMouseDown;
  Point<double> fLeftMouseDownLocation;
  bool fIsHovered;
  
  int fRotationAngle;
  bool fDragging;
  int fLastX;
  int fLastY;

  Color fColor;

  Callback *fCallback;

  DISTRHO_LEAK_DETECTOR(NanoKnob)
};

END_NAMESPACE_DISTRHO

#endif
