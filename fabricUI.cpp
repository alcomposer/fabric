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
        : UI(128, 512),
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
}

void fabricUI::idleCallback(){
        repaint();  
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

            /**/ if (value < 0.001f) value = 0.0f;
            else if (value > 0.999f) value = 1.0f;

            if (fOutLeft != value)
            {
                fOutLeft = value;
                repaint();
            }
            break;

        case 2: // out-right
            value = (fOutRight * kSmoothMultiplier + value) / (kSmoothMultiplier + 1.0f);

            /**/ if (value < 0.001f) value = 0.0f;
            else if (value > 0.999f) value = 1.0f;

            if (fOutRight != value)
            {
                fOutRight = value;
                repaint();
            }
            break;
        }
    }

    void fabricUI::stateChanged(const char*, const char*) 
    {
        // nothing here
    }
    void fabricUI::onNanoDisplay() 
    {
        static const Color kColorBlack(0, 0, 0);
        static const Color kColorRed(255, 0, 0);
        static const Color kColorYellow(255, 255, 0);

        // get meter values
        const float outLeft(fOutLeft);
        const float outRight(fOutRight);

        // tell DSP side to reset meter values
        setState("reset", "");

        // useful vars
        const float halfWidth        = static_cast<float>(getWidth())/2;
        const float redYellowHeight  = static_cast<float>(getHeight())*0.2f;
        const float yellowBaseHeight = static_cast<float>(getHeight())*0.4f;
        const float baseBaseHeight   = static_cast<float>(getHeight())*0.6f;

        // create gradients
        Paint fGradient1 = linearGradient(0.0f, 0.0f,            0.0f, redYellowHeight,  kColorRed,    kColorYellow);
        Paint fGradient2 = linearGradient(0.0f, redYellowHeight, 0.0f, yellowBaseHeight, kColorYellow, fColor);

        // paint left meter
        beginPath();
        rect(0.0f, 0.0f, halfWidth-1.0f, redYellowHeight);
        fillPaint(fGradient1);
        fill();
        closePath();

        beginPath();
        rect(0.0f, redYellowHeight-0.5f, halfWidth-1.0f, yellowBaseHeight);
        fillPaint(fGradient2);
        fill();
        closePath();

        beginPath();
        rect(0.0f, redYellowHeight+yellowBaseHeight-1.5f, halfWidth-1.0f, baseBaseHeight);
        fillColor(fColor);
        fill();
        closePath();

        // paint left black matching output level
        beginPath();
        rect(0.0f, 0.0f, halfWidth-1.0f, (1.0f-outLeft)*getHeight());
        fillColor(kColorBlack);
        fill();
        closePath();

        // paint right meter
        beginPath();
        rect(halfWidth+1.0f, 0.0f, halfWidth-2.0f, redYellowHeight);
        fillPaint(fGradient1);
        fill();
        closePath();

        beginPath();
        rect(halfWidth+1.0f, redYellowHeight-0.5f, halfWidth-2.0f, yellowBaseHeight);
        fillPaint(fGradient2);
        fill();
        closePath();

        beginPath();
        rect(halfWidth+1.0f, redYellowHeight+yellowBaseHeight-1.5f, halfWidth-2.0f, baseBaseHeight);
        fillColor(fColor);
        fill();
        closePath();

        // paint right black matching output level
        beginPath();
        rect(halfWidth+1.0f, 0.0f, halfWidth-2.0f, (1.0f-outRight)*getHeight());
        fillColor(kColorBlack);
        fill();
        closePath();
    }

    bool fabricUI::onMouse(const MouseEvent& ev)
    {
        // Test for left-clicked + pressed first.
        if (ev.button != 1 || ! ev.press)
            return false;

        const int newColor(fColorValue == 0 ? 1 : 0);
        updateColor(newColor);
        setParameterValue(0, newColor);

        return true;
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

UI* createUI()
{
    return new fabricUI();
}

END_NAMESPACE_DISTRHO
