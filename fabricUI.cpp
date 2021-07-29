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
    : UI(900, 400),
      // default color is green
      fColor(93, 231, 61),
      // which is value 0
      fColorValue(0),
      // init meter values to 0
      fOutLeft(0.0f),
      fOutRight(0.0f)
{
    setGeometryConstraints(900, 400);
    Window &pw = getWindow(); //this is needed to refresh the waveform display
    pw.addIdleCallback(this, 10);


/*
    fdensity = new VolumeKnob(this,  Size<uint>(60, 60));
    fdensity->setId(id_density);
    fdensity->setCallback(this);
    fdensity->setColor(Color(173, 216, 230, 255));   
    fdensity->setAbsolutePos(15, 350);
    fdensity->show();

    flength = new VolumeKnob(this,  Size<uint>(60, 60));
    flength->setId(id_length);
    flength->setCallback(this);
    flength->setColor(Color(173, 216, 230, 255));   
    flength->setAbsolutePos(100, 350);
    flength->setRange(-1.0, 1.0);
    flength->setBipolar(true);
    flength->show();

    flabel = new NanoLabel(this, Size<uint>(900,200));
    flabel->setText("label & these two knobs inside main window");
    flabel->setAbsolutePos(15,300);
    flabel->setFontSize(45.0);
    flabel->show();

    flabelLive = new NanoLabel(this, Size<uint>(200,200));
    flabelLive->setAbsolutePos(getWidth()/2, getHeight()/2);
    flabelLive->setId(id_temp_label);
    flabelLive->setFontSize(45.0);
    flabelLive->show();
    */


    custom_widget = new fabricController(this, Size<uint>(200,200));
    custom_widget->setText("DENSITY");
    custom_widget->setAbsolutePos(100,100);
    custom_widget->show();

    
}

void fabricUI::idleCallback()
{
    //repaint();
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

    if (id == id_density)
    {
        flabelLive->setText(newValue);
    }
    if (id == id_length)
    {
        //std::cout << "grain length value is: " << value << std::endl;
    }
    //{
    //    fGraphWidget->setHorizontalWarpAmount(value);
    //}
    //else if (id == paramVerticalWarpAmount)
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
