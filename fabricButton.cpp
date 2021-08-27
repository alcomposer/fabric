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
#include "fabricButton.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

fabricButton::fabricButton(Widget *widget, Size<uint> size) noexcept
    : NanoSwitch(widget, size)
{    
    fButtonICol = Color(86, 92, 95, 255);
    fButtonOCol = Color(39, 42, 43, 255);

    flabelTop = new NanoLabel(widget, Size<uint>(200,200));
    flabelTop->setFontSize(16.0);
    flabelTop->setAlign(ALIGN_CENTER|ALIGN_TOP);
    flabelTop->show();

    updateBuddyWidgetPositions();
}

void fabricButton::draw()
{

    const Color fKnobTargetOCol = Color(59, 62, 63, 255);

    double width = getWidth();
    double height = getHeight();

    beginPath();
    strokeWidth(4.0f);
    strokePaint(linearGradient(0, 0, 0, height - 10, Color(190, 190, 190, 0.f), Color(23, 23, 23, 1.f)));

    Paint switchPaint = linearGradient(width/2.0, 4, width/2.0, width, fButtonICol, fButtonOCol);
    fillPaint(switchPaint);

    rect(0,0,width,height);
    fill();
    stroke();
}

void fabricButton::setText(std::string text)
{
    topText = text;
    flabelTop->setText(topText.c_str());
    updateBuddyWidgetPositions();
};

void fabricButton::onStateChanged()
{
    if(isDown()){
        //flabelTop->setColor(Color(255,40,40,255)); //Maybe too much to have text turn red as well?
        fButtonICol = Color(150, 92, 95, 255);
        fButtonOCol = Color(60, 42, 43, 255);
    }else{
        //flabelTop->setColor(Color(255,255,255,255));
        fButtonICol = Color(86, 92, 95, 255);
        fButtonOCol = Color(39, 42, 43, 255);
    }
}

void fabricButton::onPositionChanged(const PositionChangedEvent &event)
{
    updateBuddyWidgetPositions();
}

void fabricButton::onResize(const ResizeEvent &event)
{
    updateBuddyWidgetPositions();
}

void fabricButton::updateBuddyWidgetPositions()
{
    Rectangle<int> area = getAbsoluteArea();
    Rectangle<float> labelTopBounds;
    flabelTop->textBounds(0.0, 0.0, topText.c_str(), NULL, labelTopBounds);

    flabelTop->setAbsolutePos(
        area.getX() + ((int)area.getWidth() - (int)flabelTop->getWidth()) / 2.0,
        area.getY() + area.getHeight() * 0.5f - 6);
}

END_NAMESPACE_DISTRHO
