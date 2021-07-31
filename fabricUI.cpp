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

    int lazyXPos = 20;
    int lazyXposSpacer = 95;


    fwaveformDisplay = new fabricWaveformDisplay(this, waveformDisplaySize);
    fwaveformDisplay->setAbsolutePos(0,60);
    fwaveformDisplay->show();

    frecButton = new fabricButton(this, Size<uint>(50,50));
    frecButton->setAbsolutePos(lazyXPos,400);
    frecButton->setText("REC");
    frecButton->setId(id_rec);
    frecButton->setCallback(this);
    frecButton->show();

    lazyXPos += lazyXposSpacer;

    fcontrolSpeed = new fabricController(this, knobSizeStandard);
    fcontrolSpeed->setText("SPEED");
    fcontrolSpeed->setId(id_speed);
    fcontrolSpeed->setCallback(this);
    fcontrolSpeed->setBipolar(true);
    fcontrolSpeed->setRange(-2.0f, 2.0f);
    fcontrolSpeed->setUnit("x");
    fcontrolSpeed->setAbsolutePos(lazyXPos,400);
    fcontrolSpeed->show();

    lazyXPos += lazyXposSpacer;

    fcontrolDensity = new fabricController(this, knobSizeStandard);
    fcontrolDensity->setText("DENSITY");
    fcontrolDensity->setId(id_density);
    fcontrolDensity->setCallback(this);
    fcontrolDensity->setUnit("Hz");
    fcontrolDensity->setAbsolutePos(lazyXPos,400);
    fcontrolDensity->show();

    lazyXPos += lazyXposSpacer;

    fcontrolLength = new fabricController(this, knobSizeStandard);
    fcontrolLength->setText("LENGTH");
    fcontrolLength->setId(id_length);
    fcontrolLength->setCallback(this);
    fcontrolLength->setUnit("ms");    
    fcontrolLength->setAbsolutePos(lazyXPos,400);
    fcontrolLength->show();

    lazyXPos += lazyXposSpacer;

    fcontrolSpray = new fabricController(this, knobSizeStandard);
    fcontrolSpray->setText("SPRAY");
    fcontrolSpray->setId(id_spray);
    fcontrolSpray->setCallback(this);
    fcontrolSpray->setUnit("ms");
    fcontrolSpray->setAbsolutePos(lazyXPos,400);
    fcontrolSpray->show();

    lazyXPos += lazyXposSpacer;

    fcontrolSides = new fabricController(this, knobSizeStandard);
    fcontrolSides->setText("SIDES");
    fcontrolSides->setId(id_sides);
    fcontrolSides->setCallback(this);
    fcontrolSides->setAbsolutePos(lazyXPos,400);
    fcontrolSides->show();

    lazyXPos += lazyXposSpacer;

    fcontrolWet = new fabricController(this, knobSizeStandard);
    fcontrolWet->setText("WET");
    fcontrolWet->setId(id_wet);
    fcontrolWet->setCallback(this);
    fcontrolWet->setUnit("%");
    fcontrolWet->setAbsolutePos(lazyXPos,400);
    fcontrolWet->show();

    lazyXPos += lazyXposSpacer;

    fcontrolDry = new fabricController(this, knobSizeStandard);
    fcontrolDry->setText("DRY");
    fcontrolDry->setId(id_dry);
    fcontrolDry->setCallback(this);    
    fcontrolDry->setUnit("%");
    fcontrolDry->setAbsolutePos(lazyXPos,400);
    fcontrolDry->show();

    lazyXPos += lazyXposSpacer;

    fcontrolMix = new fabricController(this, knobSizeStandard);
    fcontrolMix->setText("MIX");
    fcontrolMix->setId(id_mix);
    fcontrolMix->setCallback(this);
    fcontrolMix->setAbsolutePos(lazyXPos,400);
    fcontrolMix->setBipolar(true);
    fcontrolMix->show();
}

void fabricUI::parameterChanged(uint32_t index, float value)
{
    switch (index)
    {
    case id_rec: // rec_on / rec_off
        frecButton->setDown(value);
        break;
    case id_speed:
        fcontrolSpeed->setValue(value);
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

void fabricUI::nanoKnobDragStarted(NanoKnob *nanoKnob)
{
    const uint id = nanoKnob->getId();

    switch (id) {
    case id_speed:
        editParameter(id_speed, true);
        break;
    }
}

void fabricUI::nanoKnobDragFinished(NanoKnob *nanoKnob)
{
    const uint id = nanoKnob->getId();

    switch (id) {
    case id_speed:
        editParameter(id_speed, false);
        break;
    }
}

void fabricUI::nanoKnobValueChanged(NanoKnob *nanoKnob, const float value)
{
    const uint id = nanoKnob->getId();
    float newValue = value;

    setParameterValue(id, value);

    if (id == id_speed)
    {
        std::cout << "speed is: " << value << std::endl;
        //static_cast<fabricController*>(nanoKnob)->setValueText(value);
        setParameterValue(id_speed, value);
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

void fabricUI::nanoSwitchClicked(NanoSwitch *nanoSwitch)
{
    const uint id = nanoSwitch->getId();

    if (id == id_rec)
    {
        if (frecButton->isDown())
        {
            setParameterValue(id_rec,true);
            std::cout << "rec is active" << std::endl;
        }else{
            std::cout << "rec is inactive" << std::endl;
            setParameterValue(id_rec,false);
        }
    }
}
/*
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
*/

UI *createUI()
{
    return new fabricUI();
}

END_NAMESPACE_DISTRHO
