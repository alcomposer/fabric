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
#include "fabricButton.hpp"
#include "NanoSwitch.hpp"
#include "fabricParameters.hpp"

#include "fabricWaveformDisplay.hpp"
#include "fabricEnvelopeDisplay.hpp"
#include "fabricController.hpp" 
#include "fabricDSP.hpp"
#include "fabricGrainMeter.hpp"

START_NAMESPACE_DISTRHO

class fabricUI :  public UI
                 ,public NanoKnob::Callback
                 ,public NanoSwitch::Callback
                 ,public IdleCallback
{
public:
    fabricUI();
    fabricDSP *_plugin;

    /* DSP/Plugin Callbacks */
protected:
    /**
        A parameter has changed on the plugin side.
        This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override;
    /**
        A state has changed on the plugin side.
        This is called by the host to inform the UI about state changes.
    */
    void stateChanged(const char *, const char *) override;

    /* Widget Callbacks */
    /**
        The NanoVG drawing function.
    */
    void onNanoDisplay() override;
    /**
        Mouse press event.
        This UI will change color when clicked.
    */
    bool onMouse(const MouseEvent &ev) override;

    void idleCallback();

private:
    float fOutLeft, fOutRight;

    Size<uint> waveformDisplaySize;

    ScopedPointer<NanoLabel> m_pluginNameTemp;
    ScopedPointer<NanoLabel> m_pluginStatusTemp;      

    ScopedPointer<fabricGrainMeter> m_fabricGrainMeter;
    ScopedPointer<fabricWaveformDisplay> fwaveformDisplay;
    ScopedPointer<fabricEnvelopeDisplay> fenvelopeDisplay;

    ScopedPointer<fabricButton> frecButton;
    
    ScopedPointer<fabricController> fcontrolScan;
    ScopedPointer<fabricController> fcontrolDensity;
    ScopedPointer<fabricController> fcontrolLength;
    ScopedPointer<fabricController> fcontrolGainSpray;
    ScopedPointer<fabricController> fcontrolDirection;
    ScopedPointer<fabricController> fcontrolSpray;
    ScopedPointer<fabricController> fcontrolPitch;
    ScopedPointer<fabricController> fcontrolPitchSpray;
    ScopedPointer<fabricController> fcontrolStereoWidth;
    ScopedPointer<fabricController> fcontrolPanSpray;        
    ScopedPointer<fabricController> fcontrolSides;
    ScopedPointer<fabricController> fcontrolTilt;

    ScopedPointer<fabricController> fcontrolDry;
    ScopedPointer<fabricController> fcontrolWet;

    void nanoKnobDragStarted(NanoKnob *nanoKnob) override;
    void nanoKnobDragFinished(NanoKnob *nanoKnob) override;
    void nanoKnobValueChanged(NanoKnob *nanoKnob, const float value) override;
    void nanoSwitchClicked(NanoSwitch *nanoSwitch) override;
    
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(fabricUI);
};

END_NAMESPACE_DISTRHO
