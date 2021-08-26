#include "VolumeKnob.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

VolumeKnob::VolumeKnob(Widget  *widget, Size<uint> size) noexcept : NanoKnob(widget, size)

{
    const float radius = size.getHeight() / 2.0f;
    const float gaugeWidth = 3.5f;
    const float diameter = (radius - gaugeWidth) * 2.0f - 4;

    fKnobICol = Color(86, 92, 95, 255);

    fKnobOCol = Color(39, 42, 43, 255);
    const Color fKnobTargetOCol = Color(59, 62, 63, 255);

    fKnobDiameter = diameter;

    widget->getWindow().addIdleCallback(this);
}

void VolumeKnob::idleCallback()
{

}

void VolumeKnob::onMouseHover()
{
    if (!canBeFocused())
        return;
}

void VolumeKnob::onMouseLeave()
{
    if (!canBeFocused())
        return;
}

void VolumeKnob::onMouseDown()
{

}

void VolumeKnob::onMouseUp()
{

}

void VolumeKnob::setBipolar(bool isBipolar)
{
    _isBipolar = isBipolar;
}

void VolumeKnob::draw()
{
    const float height = getHeight();
    const Color color = getColor();

    const float value = getValue();
    const float min = getMin();
    const float max = getMax();

    const float percentFilled = normalizeValue(value);

    const float radius = height / 2.0f;

    const float indicatorLineHeight = fKnobDiameter / 2.0f - 8;
    const float indicatorLineWidth = 3.0f;
    const float indicatorLineMarginTop = 4.0f;

    const float gaugeWidth = 3.5f;
    Color gaugeColor = Color(0, 0, 40, 255);
    gaugeColor.interpolate(color, 0.4f);

    const float margin = 3.0f;

    //Gauge (empty)
    beginPath();

    strokeWidth(gaugeWidth);
    strokeColor(gaugeColor);
    arc(radius, radius, radius - margin, 0.75f * M_PI, 0.25f * M_PI, NanoVG::Winding::CW);
    stroke();

    //Gauge (value)
    beginPath();

    strokeWidth(gaugeWidth);
    strokeColor(color);
    if (_isBipolar){
        if (percentFilled > 0.5){
            arc(radius, radius, radius - margin, 1.5f * M_PI, (0.75f + 1.5f * percentFilled) * M_PI, NanoVG::Winding::CW);
        }else{
            arc(radius, radius, radius - margin, (0.75f + 1.5f * percentFilled) * M_PI, 1.5f * M_PI, NanoVG::Winding::CW); 
        }
    }else{
        arc(radius, radius, radius - margin, 0.75f * M_PI, (0.75f + 1.5f * percentFilled) * M_PI, NanoVG::Winding::CW);
    }
    stroke();

    //Knob
    beginPath();

    strokeWidth(2.0f);
    strokePaint(linearGradient(0, 0, 0, height - 10, Color(190, 190, 190, 0.f), Color(23, 23, 23, 1.f)));

    Paint knobPaint = linearGradient(radius, gaugeWidth, radius, fKnobDiameter, fKnobICol, fKnobOCol);
    fillPaint(knobPaint);

    circle(radius, radius, fKnobDiameter / 2.0f);
    fill();
    stroke();

    //Indicator line
    beginPath();
    save();

    translate(radius, radius);
    rotate((2.0f + ((percentFilled - 0.5f) * 1.5f)) * M_PI);
    translate(-radius, -radius);

    fillColor(color);
    rect(radius - indicatorLineWidth / 2.0f, margin + indicatorLineMarginTop + fKnobDiameter / 2.0f - radius, indicatorLineWidth, indicatorLineHeight);
    fill();

    restore();

    closePath();
}

END_NAMESPACE_DISTRHO
