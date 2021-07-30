/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2019 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <iostream>
#include "fabricUI.hpp"
#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

/**
  We need the Color class from DGL.
 */
using DGL_NAMESPACE::Color;

/**
  Smooth meters a bit.
 */
static const float kSmoothMultiplier = 3.0f;

// -----------------------------------------------------------------------------------------------------------

fabricUI::fabricUI()
    : UI(850, 500)
      ,knobSizeStandard(50,50)
      ,waveformDisplaySize(850, 300)
      // default color is green
      ,fColor(93, 231, 61)
      // which is value 0
      ,fColorValue(0)
      // init meter values to 0
      ,fOutLeft(0.0f)
      ,fOutRight(0.0f)
{
    _plugin = static_cast<fabricDSP *>(getPluginInstancePointer());

    setGeometryConstraints(850, 500);

    fwaveformDisplay = new fabricWaveformDisplay(this, waveformDisplaySize);
    fwaveformDisplay->setAbsolutePos(0,60);
    fwaveformDisplay->show();

    fcontrolSpeed = new fabricController(this, knobSizeStandard);
    fcontrolSpeed->setText("SPEED");
    fcontrolSpeed->setId(id_speed);
    fcontrolSpeed->setCallback(this);
    fcontrolSpeed->setAbsolutePos(50,400);
    fcontrolSpeed->setBipolar(true);
    fcontrolSpeed->show();

    fcontrolDensity = new fabricController(this, knobSizeStandard);
    fcontrolDensity->setText("DENSITY");
    fcontrolDensity->setId(id_density);
    fcontrolDensity->setCallback(this);
    fcontrolDensity->setAbsolutePos(150,400);
    fcontrolDensity->show();

    fcontrolLength = new fabricController(this, knobSizeStandard);
    fcontrolLength->setText("LENGTH");
    fcontrolLength->setId(id_length);
    fcontrolLength->setCallback(this);
    fcontrolLength->setAbsolutePos(250,400);
    fcontrolLength->show();

    fcontrolSpray = new fabricController(this, knobSizeStandard);
    fcontrolSpray->setText("SPRAY");
    fcontrolSpray->setId(id_spray);
    fcontrolSpray->setCallback(this);
    fcontrolSpray->setAbsolutePos(350,400);
    fcontrolSpray->show();

    fcontrolSides = new fabricController(this, knobSizeStandard);
    fcontrolSides->setText("SIDES");
    fcontrolSides->setId(id_sides);
    fcontrolSides->setCallback(this);
    fcontrolSides->setAbsolutePos(450,400);
    fcontrolSides->show();

    fcontrolWet = new fabricController(this, knobSizeStandard);
    fcontrolWet->setText("WET");
    fcontrolWet->setId(id_wet);
    fcontrolWet->setCallback(this);
    fcontrolWet->setAbsolutePos(550,400);
    fcontrolWet->show();

    fcontrolDry = new fabricController(this, knobSizeStandard);
    fcontrolDry->setText("DRY");
    fcontrolDry->setId(id_dry);
    fcontrolDry->setCallback(this);
    fcontrolDry->setAbsolutePos(650,400);
    fcontrolDry->show();

    fcontrolMix = new fabricController(this, knobSizeStandard);
    fcontrolMix->setText("MIX");
    fcontrolMix->setId(id_mix);
    fcontrolMix->setCallback(this);
    fcontrolMix->setAbsolutePos(750,400);
    fcontrolMix->setBipolar(true);
    fcontrolMix->show();
}

void fabricUI::parameterChanged(uint32_t index, float value)
{
    switch (index)
    {
    case 0: // color
        updateColor(std::round(value));
        break;

    case 1: // out-left
        value = (fOutLeft * kSmoothMultiplier + value) / (kSmoothMultiplier + 1.0f);

        /**/ if (value < 0.001f)
            value = 0.0f;
        else if (value > 0.999f)
            value = 1.0f;

        if (fOutLeft != value)
        {
            fOutLeft = value;
            repaint();
        }
        break;

    case 2: // out-right
        value = (fOutRight * kSmoothMultiplier + value) / (kSmoothMultiplier + 1.0f);

        /**/ if (value < 0.001f)
            value = 0.0f;
        else if (value > 0.999f)
            value = 1.0f;

        if (fOutRight != value)
        {
            fOutRight = value;
            repaint();
        }
        break;
    }
}

void fabricUI::stateChanged(const char *, const char *)
{
    // nothing here
}
void fabricUI::onNanoDisplay()
{    
    //draw grey window background
    static const Color k_grey(99, 99, 99);
    beginPath();
    rect(0.0f, 0.0f, getWidth(), getHeight());
    fillColor(k_grey);
    fill();
    closePath();
}

bool fabricUI::onMouse(const MouseEvent &ev)
{
    return false; // set false to allow the mouse event to propogate to child widgets
}

void fabricUI::nanoKnobValueChanged(NanoKnob *nanoKnob, const float value)
{
    const uint id = nanoKnob->getId();
    float newValue = value;

    setParameterValue(id, value);

    if (id == id_speed)
    {
        std::cout << "speed is: " << value << std::endl;
        //flabelLive->setText(newValue);
    }
    if (id == id_length)
    {
        //std::cout << "grain length value is: " << value << std::endl;
    }
    //{
    //    fGraphWidget->setHorizontalWarpAmount(value);
    //}
    //else if (id == paramVerticalWarpA${appName}: ${sessionName}mount)
    //{
    //    fGraphWidget->setVerticalWarpAmount(value);
    //}
}

void fabricUI::updateColor(const int color)
{
    if (fColorValue == color)
        return;

    fColorValue = color;

    switch (color)
    {
    case METER_COLOR_GREEN:
        fColor = Color(93, 231, 61);
        break;
    case METER_COLOR_BLUE:
        fColor = Color(82, 238, 248);
        break;
    }

    repaint();
}

UI *createUI()
{
    return new fabricUI();
}

END_NAMESPACE_DISTRHO
