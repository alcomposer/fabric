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
#include "fabricController.hpp"

START_NAMESPACE_DISTRHO

fabricController::fabricController(Widget *widget, Size<uint> size) noexcept
    : NanoSubWidget(widget)
{
    setSize(size);
    
    fdensity = new VolumeKnob(this,  Size<uint>(60, 60));
    fdensity->setColor(Color(173, 216, 230, 255));
    fdensity->setAbsolutePos(0, 0);
    fdensity->show();

    
    flabel = new NanoLabel(this, Size<uint>(900,200));
    flabel->setText("This should be at top left edge of red box");
    flabel->setFontSize(20.0);
    flabel->setAbsolutePos(0, 0);
    flabel->show();
}


void fabricController::nanoKnobValueChanged(NanoKnob *nanoKnob, const float value)
{
}

void fabricController::onNanoDisplay()
{
    static const Color k_grey(255, 0, 0);
    beginPath();
    rect(0.0f, 0.0f, getWidth()/2, getHeight()/2);
    fillColor(k_grey);
    fill();
    closePath();
}

//
//}
//void fabricControl::onDisplay()
//{
//


END_NAMESPACE_DISTRHO
