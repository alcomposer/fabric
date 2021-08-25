#include "NanoKnob.hpp"
#include "Mathf.hpp"

START_NAMESPACE_DISTRHO

NanoKnob::NanoKnob(Widget  *widget, Size<uint> size) noexcept
    : WolfWidget(widget),
      fMin(0.0f),
      fMax(1.0f),
      fStep(0.0f),
      fValue(0.5f),
      fCurve(0.0f),
      fLeftMouseDown(false),
      fIsHovered(false),
      fColor(Color(255, 0, 0, 255)),
      fCallback(nullptr)
{
    setSize(size);
}

void NanoKnob::setCurve(float curve)
{
    if (fCurve == curve)
        return;

    fCurve = curve;
    repaint();
}

float NanoKnob::normalizeValue(float value)
{
    float nv = (value - fMin) / (fMax - fMin);

    if (fCurve > 0)
        nv = std::pow(nv, 1.0f - fCurve);
    else if (fCurve < 0)
        nv = 1.0f - std::pow(1.0f - nv, fCurve - 1.0f);

    return nv;
}

float NanoKnob::denormalizeValue(float nv)
{
    if (fCurve > 0)
        nv = std::pow(nv, 1.0f / (1.0f - fCurve));
    else if (fCurve < 0)
        nv = 1.0f - std::pow(1.0f - nv, 1.0f / (fCurve - 1.0f));

    return nv * (fMax - fMin) + fMin;
}

float NanoKnob::getValue() const noexcept
{
    return fValue;
}

void NanoKnob::setRange(float min, float max) noexcept
{
    DISTRHO_SAFE_ASSERT(min < max);

    fMin = min;
    fMax = max;

    fValue = wolf::clamp(fValue, min, max);
}

void NanoKnob::setStep(float step) noexcept
{
    fStep = step;
}

float NanoKnob::getMin() noexcept
{
    return fMin;
}

float NanoKnob::getMax() noexcept
{
    return fMax;
}

// NOTE: value is assumed to be scaled if using log
void NanoKnob::setValue(float value, bool sendCallback) noexcept
{
    value = wolf::clamp(value, fMin, fMax);

    if (d_isEqual(fValue, value))
        return;

    fValue = value;

    knobValueChanged(sendCallback);

    repaint();
}

void NanoKnob::setCallback(Callback *callback) noexcept
{
    fCallback = callback;
}

void NanoKnob::setColor(Color color) noexcept
{
    fColor = color;
}

Color NanoKnob::getColor() noexcept
{
    return fColor;
}

void NanoKnob::knobValueChanged(bool sendCallback)
{
    if (sendCallback)
    {
        if (fCallback != nullptr)
            fCallback->nanoKnobValueChanged(this, fValue);
    }
}

void NanoKnob::knobDragStarted()
{
    if (fCallback != nullptr)
        fCallback->nanoKnobDragStarted(this);
}

void NanoKnob::knobDragFinished()
{
    if (fCallback != nullptr)
        fCallback->nanoKnobDragFinished(this);
}

void NanoKnob::onNanoDisplay()
{
    draw();
}

bool NanoKnob::onMouse(const MouseEvent &ev)
{
    if (ev.button != 1)
        return fLeftMouseDown;

    if (!ev.press)
    {
        if (fLeftMouseDown == true)
        {
            fLeftMouseDown = false;
            setFocus(false);

            onMouseUp();

            knobDragFinished();

            return true;
        }

        return false;
    }

    if (contains(ev.pos))
    {
        fLeftMouseDownLocation = ev.pos;
        fLeftMouseDown = true;

        setFocus(true);
        onMouseDown();

        knobDragStarted();

        return true;
    }

    return false;
}

void NanoKnob::onMouseHover()
{
}

void NanoKnob::onMouseLeave()
{
}

void NanoKnob::onMouseUp()
{
}

void NanoKnob::onMouseDown()
{
}

bool NanoKnob::onMotion(const MotionEvent &ev)
{
    if (fLeftMouseDown)
    {
        const float resistance = (ev.mod & kModifierControl) ? 2000.0f : 200.0f;
        const float difference = (fLeftMouseDownLocation.getY() - ev.pos.getY()) / resistance;

        fLeftMouseDownLocation.setY(ev.pos.getY());

        float newValue = denormalizeValue( normalizeValue(fValue) + difference);
        setValue(newValue, true);

        return true;
    }

    if (contains(ev.pos))
    {
        if (!fIsHovered)
        {
            fIsHovered = true;
            onMouseHover();
        }
    }
    else
    {
        if (fIsHovered)
        {
            fIsHovered = false;
            onMouseLeave();
        }
    }

    return false;
}

bool NanoKnob::onScroll(const ScrollEvent &ev)
{
    if (!contains(ev.pos))
        return false;

    const float resistance = (ev.mod & kModifierControl) ? 400.0f : 40.0f;

    setValue(getValue() + ev.delta.getY() / resistance * (fMax - fMin), true);

    return true;
}

END_NAMESPACE_DISTRHO
