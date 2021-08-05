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

#include "DistrhoPlugin.hpp"
#include "GrainPlayer.hpp"
#include "fabricParamSmooth.hpp"
#include <vector>

START_NAMESPACE_DISTRHO

class fabricDSP : public Plugin
{
public:
    fabricDSP();
    ~fabricDSP();
    //stereo audio buffer
    float* st_audioBuffer[2];
    int st_audioBufferSize;
    //position inside audio buffer
    uint bufferPos = {0};
    //The grainplayer object
    GrainPlayer grainPlayer;
protected:
    /**
        Get the plugin label.
        A plugin label follows the same rules as Parameter::symbol, with the exception that it can start with numbers.
    */
    const char *getLabel() const override;
    /**
        Get an extensive comment/description about the plugin.
    */
    const char *getDescription() const override;
    /**
        Get the plugin author/maker.
    */
    const char *getMaker() const override;
    /**
        Get the plugin homepage.
    */
    const char *getHomePage() const override;
    /**
        Get the plugin license name (a single line of text).
        For commercial plugins this should return some short copyright information.
    */
    const char *getLicense() const override;
    /**
        Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override;
    /**
        Get the plugin unique Id.
        This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override;

    /* Init */

    /**
        Initialize the parameter @a index.
        This function will be called once, shortly after the plugin is created.
    */
    void initParameter(uint32_t index, Parameter &parameter) override;
    /**
        Set a state key and default value.
        This function will be called once, shortly after the plugin is created.
    */
    void initState(uint32_t, String &, String &) override;

    /* --------------------------------------------------------------------------------------------------------
    * Internal data */

    /**
        Get the current value of a parameter.
    */
    float getParameterValue(uint32_t index) const override;
    /**
        Change a parameter value.
    */
    void setParameterValue(uint32_t index, float value) override;
    /**
      Change an internal state.
    */
    void setState(const char *key, const char *) override;

    String getState(const char* key) const;
    /* --------------------------------------------------------------------------------------------------------
    * Process */

    /**
      Run/process function for plugins without MIDI input.
    */
    void run(const float **inputs, float **outputs, uint32_t frames) override;

    // -------------------------------------------------------------------------------------------------------

private:
    float _sampleRate = {0.0f};
    void clearOutputs(float** outputs, uint32_t frames);
    void copyInputs(const float** inputs, uint32_t frames);
    void mixToOutputs(float** wet, float** dry, uint32_t frames);
    float* mixDry[2];
    fabricParamSmooth * smooth_mix;


    /**
      Parameters.
    */
    bool _recording;
    float _wet, _dry, _mix;
    /**
      Boolean used to reset meter values.
      The UI will send a "reset" message which sets this as true.
    */
    volatile bool fNeedsReset;
    /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */

    //Get access to UI
    friend class fabricUI;
    
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(fabricDSP);
};

END_NAMESPACE_DISTRHO