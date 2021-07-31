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
#include <iostream>
#include <iomanip>
#include <sstream>

START_NAMESPACE_DISTRHO

fabricController::fabricController(Widget *widget, Size<uint> size) noexcept
    : VolumeKnob(widget, size)
{
    addCallback(this);

    setColor(Color(173, 216, 230, 255)); //FIXME (alex) make this an enum

    flabelTop = new NanoLabel(widget, Size<uint>(200,200));
    flabelTop->setFontSize(16.0);
    flabelTop->setAlign(ALIGN_CENTER|ALIGN_TOP);
    flabelTop->show();

    flabelBottom = new NanoLabel(widget, Size<uint>(200,200));
    flabelBottom->setFontSize(16.0);
    flabelBottom->setAlign(ALIGN_CENTER|ALIGN_TOP);
    flabelBottom->show();

    updateBottomValue();

    updateBuddyWidgetPositions();
}

void fabricController::nanoKnobDragStarted(NanoKnob *nanoKnob)
{
}

void fabricController::nanoKnobDragFinished(NanoKnob *nanoKnob)
{
}
void fabricController::updateBottomValue()
{
    std::stringstream bTextFormatted;
    bTextFormatted << std::fixed << std::setprecision(2) << getValue();
    bTextFormatted << " " << _unit;
    bottomText = bTextFormatted.str();
    flabelBottom->setText(bottomText.c_str());
}

void fabricController::nanoKnobValueChanged(NanoKnob *nanoKnob, const float value)
{    
    updateBottomValue();
}

void fabricController::onNanoDisplay()
{
    VolumeKnob::onNanoDisplay();
}

void fabricController::setText(std::string text)
{
    topText = text;
    flabelTop->setText(topText.c_str());
    updateBuddyWidgetPositions();
}

void fabricController::setUnit(std::string unit)
{
    _unit = unit;
}

void fabricController::onPositionChanged(const PositionChangedEvent &event)
{
    updateBuddyWidgetPositions();
}

void fabricController::onResize(const ResizeEvent &event)
{
    updateBuddyWidgetPositions();
}

void fabricController::updateBuddyWidgetPositions()
{
    Rectangle<int> area = getAbsoluteArea();


    Rectangle<float> labelTopBounds;
    flabelTop->textBounds(0.0, 0.0, topText.c_str(), NULL, labelTopBounds);
    //std::cout << labelTopBounds.getWidth() << std::endl;

    // reposition the label at horizontal center and above the knob
    //flabelTop->setAbsolutePos(area.getX() - labelTopBounds.getWidth() / 2.0, area.getY() - labelTopBounds.getHeight() - 4);

    flabelTop->setAbsolutePos(
        area.getX() + ((int)area.getWidth() - (int)flabelTop->getWidth()) / 2.0,
        area.getY() - labelTopBounds.getHeight() - 4);

    flabelBottom->setAbsolutePos(
        area.getX() + ((int)area.getWidth() - (int)flabelTop->getWidth()) / 2.0,
        area.getY() + getHeight());

 //fprintf(stderr, "area %d %d\n", area.getX(), area.getWidth());
 //fprintf(stderr, "label %d %d\n", flabelTop->getAbsoluteX(), flabelTop->getWidth());

    // reposition the knob at top-left
    //fknobCenter->setAbsolutePos(area.getX(), area.getY());
}

END_NAMESPACE_DISTRHO
