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

#pragma once

#include "DistrhoUI.hpp"
#include "VolumeKnob.hpp"
#include "NanoLabel.hpp"
#include "Widget.hpp"

START_NAMESPACE_DISTRHO

class fabricController : public NanoSubWidget
                 ,public NanoKnob::Callback
{
public:
    explicit fabricController(Widget *widget, Size<uint> size) noexcept;

protected:

    void nanoKnobValueChanged(NanoKnob *nanoKnob, const float value) override;

    void onNanoDisplay() override;

    

    //void onDisplay() override;
    /**
        Mouse press event.
        This UI will change color when clicked.
    */
    //bool onMouse(const MouseEvent &ev) override;

private:
    ScopedPointer<VolumeKnob> fdensity;
    ScopedPointer<NanoLabel> flabel;

    DISTRHO_LEAK_DETECTOR(fabricController);
};

END_NAMESPACE_DISTRHO
