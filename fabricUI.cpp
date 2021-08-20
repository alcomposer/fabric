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
      ,waveformDisplaySize(850, 250)
      ,fOutLeft(0.0f)
      ,fOutRight(0.0f)
{
    _plugin = static_cast<fabricDSP *>(getPluginInstancePointer());

    setGeometryConstraints(850, 510);

    Size<uint> knobSizeStandard(50,50);
    int lazyYPosRow1 = 340;
    int lazyYPosRow2 = 430;
    int lazyXPos = 20;
    int lazyXposSpacer = 85;

    m_pluginNameTemp = new NanoLabel(this, Size<uint>(200,200));
    m_pluginNameTemp->setText("FABRIC");
    m_pluginNameTemp->setAbsolutePos(20,5);
    m_pluginNameTemp->setFontSize(52.0);

    m_pluginStatusTemp = new NanoLabel(this, Size<uint>(400,400));
    m_pluginStatusTemp->setText("Pre-Alpha");
    m_pluginStatusTemp->setColor(DGL::Color(150,0,0));  
    m_pluginStatusTemp->setAbsolutePos(190,32);
    m_pluginStatusTemp->setFontSize(18.0);

    fwaveformDisplay = new fabricWaveformDisplay(this, waveformDisplaySize);
    fwaveformDisplay->setAbsolutePos(0,60);
    fwaveformDisplay->show();

    frecButton = new fabricButton(this, knobSizeStandard);
    frecButton->setAbsolutePos(lazyXPos,lazyYPosRow1);
    frecButton->setText("Rec");
    frecButton->setId(id_rec);
    frecButton->setCallback(this);
    frecButton->show();

    lazyXPos += lazyXposSpacer;

    fcontrolScan = new fabricController(this, knobSizeStandard);
    fcontrolScan->setText("Scan");
    fcontrolScan->setId(id_scan);
    fcontrolScan->setCallback(this);
    fcontrolScan->setBipolar(true);
    fcontrolScan->setRange(-5.0f, 5.0f);
    fcontrolScan->setUnit("x");
    fcontrolScan->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolScan->show();

    fcontrolSpray = new fabricController(this, knobSizeStandard);
    fcontrolSpray->setText("Spray");
    fcontrolSpray->setId(id_spray);
    fcontrolSpray->setCallback(this);
    fcontrolSpray->setUnit("ms");
    fcontrolSpray->setRange(0.f, 10000.f);
    fcontrolSpray->setAbsolutePos(lazyXPos,lazyYPosRow2);
    fcontrolSpray->show();

    lazyXPos += lazyXposSpacer;

    fcontrolDensity = new fabricController(this, knobSizeStandard);
    fcontrolDensity->setText("Density");
    fcontrolDensity->setId(id_density);
    fcontrolDensity->setCallback(this);
    fcontrolDensity->setRange(0.1f, 500.f);
    fcontrolDensity->setUnit("Hz");
    fcontrolDensity->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolDensity->show();

    lazyXPos += lazyXposSpacer;

    fcontrolLength = new fabricController(this, knobSizeStandard);
    fcontrolLength->setText("Length");
    fcontrolLength->setId(id_length);
    fcontrolLength->setCallback(this);
    fcontrolLength->setUnit("ms");    
    fcontrolLength->setRange(0.f, 10000.f);
    fcontrolLength->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolLength->show();

    lazyXPos += lazyXposSpacer;

    fcontrolDirection = new fabricController(this, knobSizeStandard);
    fcontrolDirection->setText("Dir.");
    fcontrolDirection->setId(id_direction);
    fcontrolDirection->setCallback(this);    
    fcontrolDirection->setRange(-1.f, 1.f);
    fcontrolDirection->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolDirection->setBipolar(true);
    fcontrolDirection->show();

    lazyXPos += lazyXposSpacer;

    fcontrolPitch = new fabricController(this, knobSizeStandard);
    fcontrolPitch->setText("Pitch");
    fcontrolPitch->setId(id_pitch);
    fcontrolPitch->setCallback(this);    
    fcontrolPitch->setUnit("Oct");
    fcontrolPitch->setRange(-2.f, 2.f);
    fcontrolPitch->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolPitch->setBipolar(true);
    fcontrolPitch->show();

    fcontrolPitchSpray = new fabricController(this, knobSizeStandard);
    fcontrolPitchSpray->setText("Pitch Spray");
    fcontrolPitchSpray->setId(id_pitch_spray);
    fcontrolPitchSpray->setCallback(this);    
    fcontrolPitchSpray->setRange(0.f, 1.f);
    fcontrolPitchSpray->setAbsolutePos(lazyXPos,lazyYPosRow2);
    fcontrolPitchSpray->show();

    lazyXPos += lazyXposSpacer;

    fcontrolStereoWidth = new fabricController(this, knobSizeStandard);
    fcontrolStereoWidth->setText("Stereo Width");
    fcontrolStereoWidth->setId(id_stereo_width);
    fcontrolStereoWidth->setCallback(this);    
    fcontrolStereoWidth->setRange(-0.f, 1.f);
    fcontrolStereoWidth->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolStereoWidth->show();

    fcontrolPanSpray = new fabricController(this, knobSizeStandard);
    fcontrolPanSpray->setText("Pan Spray");
    fcontrolPanSpray->setId(id_pan_spray);
    fcontrolPanSpray->setCallback(this);    
    fcontrolPanSpray->setRange(-0.f, 1.f);
    fcontrolPanSpray->setAbsolutePos(lazyXPos,lazyYPosRow2);
    fcontrolPanSpray->show();

    lazyXPos += lazyXposSpacer;

    fcontrolSides = new fabricController(this, knobSizeStandard);
    fcontrolSides->setText("Sides");
    fcontrolSides->setId(id_sides);
    fcontrolSides->setRange(0.f, 1.f);
    fcontrolSides->setCallback(this);
    fcontrolSides->setAbsolutePos(lazyXPos,lazyYPosRow2);
    fcontrolSides->show();

    fenvelopeDisplay = new fabricEnvelopeDisplay(this, Size<uint>(50+lazyXposSpacer,50));
    fenvelopeDisplay->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fenvelopeDisplay->show();

    lazyXPos += lazyXposSpacer;

    fcontrolTilt = new fabricController(this, knobSizeStandard);
    fcontrolTilt->setText("Tilt");
    fcontrolTilt->setId(id_tilt);
    fcontrolTilt->setCallback(this);
    fcontrolTilt->setRange(-1.f, 1.f);
    fcontrolTilt->setAbsolutePos(lazyXPos,lazyYPosRow2);
    fcontrolTilt->setBipolar(true);
    fcontrolTilt->show();

    lazyXPos += lazyXposSpacer;

    fcontrolMix = new fabricController(this, knobSizeStandard);
    fcontrolMix->setText("Mix");
    fcontrolMix->setId(id_mix);
    fcontrolMix->setCallback(this);
    fcontrolMix->setRange(-100.f, 100.f);
    fcontrolMix->setAbsolutePos(lazyXPos,lazyYPosRow1);
    fcontrolMix->setBipolar(true);
    fcontrolMix->show();
}

//FROM HOST
void fabricUI::parameterChanged(uint32_t index, float value)
{
    switch (index)
    {
    case id_rec:
        frecButton->setDown(value);
        break;
    case id_scan:
        fcontrolScan->setValue(value);
        break;
    case id_spray:
        fcontrolSpray->setValue(value);
        break;
    case id_density:
        fcontrolDensity->setValue(value);
        break;
    case id_length:
        fcontrolLength->setValue(value);
        break;
    case id_direction:
        fcontrolDirection->setValue(value);
        break;
    case id_pitch:
        fcontrolPitch->setValue(value);
        break;
    case id_pitch_spray:
        fcontrolPitchSpray->setValue(value);
        break;
    case id_stereo_width:
        fcontrolStereoWidth->setValue(value);
        break;
    case id_pan_spray:
        fcontrolPanSpray->setValue(value);
        break;
    case id_sides:
        fenvelopeDisplay->setSidesValue(value); // FIXME (alex) should we be doing this here?
        fcontrolSides->setValue(value);
        break;
    case id_tilt:
        fenvelopeDisplay->setTiltValue(value);  // FIXME (alex) should we be doing this here?
        fcontrolTilt->setValue(value);
        break;
    case id_mix:
        fcontrolMix->setValue(value);
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
    static const Color k_grey(90, 90, 90);
    beginPath();
    rect(0.0f, 0.0f, getWidth(), getHeight());
    fillColor(k_grey);
    fill();
}

bool fabricUI::onMouse(const MouseEvent &ev)
{
    return false; // set false to allow the mouse event to propagate to child widgets
}

void fabricUI::nanoKnobDragStarted(NanoKnob *nanoKnob)
{
    const uint id = nanoKnob->getId();
    editParameter(id, true);
}

void fabricUI::nanoKnobDragFinished(NanoKnob *nanoKnob)
{
    const uint id = nanoKnob->getId();
    editParameter(id, false);
}

void fabricUI::nanoKnobValueChanged(NanoKnob *nanoKnob, const float value)
{
    const uint id = nanoKnob->getId();
    float newValue = value;

    if (id == id_sides)
    {
        setParameterValue(id_sides, value);
        fenvelopeDisplay->setSidesValue(value);
    }
    if (id == id_tilt)
    {
        setParameterValue(id_tilt, value);
        fenvelopeDisplay->setTiltValue(value);
    }
    setParameterValue(id, value);
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

UI *createUI()
{
    return new fabricUI();
}

END_NAMESPACE_DISTRHO
