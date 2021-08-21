/*
 * Fabric Granular Audio Effect
 * Copyright (C) 2021 Alexander Waite Mitchell <alex.w.mitchell@gmail.com>
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
#include "fabricEnvelopeDisplay.hpp"
#include "DistrhoPluginInfo.h"
#include "fabricUI.hpp"
#include <iostream>
#include "fabricMaths.hpp"

START_NAMESPACE_DISTRHO

fabricEnvelopeDisplay::fabricEnvelopeDisplay(Widget *widget, Size<uint> size) noexcept
    : NanoSubWidget(widget)
{
    setSize(size); 
}

void fabricEnvelopeDisplay::setTiltValue(float value)
{
    _tilt = value;
    repaint();
}

void fabricEnvelopeDisplay::setSidesValue(float value)
{
    _sides = value;
    repaint();
}

void fabricEnvelopeDisplay::onNanoDisplay()
{
    float width = getWidth();
    float height = getHeight();

    //draw black widget background
    static const Color k_black(0, 0, 0);
    beginPath();
    rect(0.0f, 0.0f, getWidth(), getHeight());
    fillColor(k_black);
    fill();

    int points = 500;

    beginPath();
    strokeColor(Color(173, 216, 230, 255)); //FIXME (alex) make this an enum
    strokeWidth(2.f);
    moveTo(1, height);

    for (uint16_t i = 0; i < points; i++)
    {
        //Tukey Window        
        float fIndex = (float)i / points;
        float windowY = fabricMaths::tukeyWindow(fIndex, _sides, _tilt);
        
        //add padding to left right and top as the stroke is 2px
        lineTo(1 + (fIndex * (width-2)), height - 1 - windowY * (height - 2.0));
    }
    lineTo(width-1, height);
    stroke();
}
END_NAMESPACE_DISTRHO
