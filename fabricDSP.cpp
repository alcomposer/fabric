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
#include "fabricDSP.hpp"
#include "fabricParameters.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

fabricDSP::fabricDSP()
    : Plugin(Parameters::TOTAL, 0, 0)
      ,_recording(false)
      ,fNeedsReset(true)
      ,grainPlayer()
{
    _sampleRate = getSampleRate();

    // smoothing for mix control
    smoothWetValue = new fabricParamSmooth();
    smoothWetValue->setSampleRate(_sampleRate);
    smoothWetValue->setSmoothingT60(1);

    smoothDryValue = new fabricParamSmooth();
    smoothDryValue->setSampleRate(_sampleRate);
    smoothDryValue->setSmoothingT60(1);

    // allocate stereo buffer for 10 second circular delay line
    st_audioBufferSize = 10 * _sampleRate;
    st_audioBuffer[0] = (float*)calloc(2 * st_audioBufferSize, sizeof(float));
    st_audioBuffer[1] = st_audioBuffer[0] + st_audioBufferSize;

    // allocate stereo buffer for mixing output
    mixDry[0] = (float*)calloc(2 * 8192, sizeof(float));
    mixDry[1] = mixDry[0] + 8192;

    grainPlayer.controls.sampleRate = _sampleRate;
}

fabricDSP::~fabricDSP()
{
    free(st_audioBuffer[0]);
}

const char *fabricDSP::getLabel() const
{
    return "Fabric";
}

const char *fabricDSP::getDescription() const
{
    return "Fabric Delay Line Granular Effect ";
}

const char *fabricDSP::getMaker() const
{
    return "AWM";
}

const char *fabricDSP::getHomePage() const
{
    return "https://github.com/alcomposer/fabric";
}
const char *fabricDSP::getLicense() const
{
    return "ISC";
}

uint32_t fabricDSP::getVersion() const
{
    return d_version(1, 0, 0);
}

int64_t fabricDSP::getUniqueId() const
{
    return d_cconst('d', 'M', 't', 'r');
}

void fabricDSP::initParameter(uint32_t index, Parameter &parameter)
{
    /**
        Set parameter data.
    */
    switch (index)
    {
    case id_rec:
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        parameter.name = "Recording";
        parameter.symbol = "REC";
        parameter.enumValues.count = 2;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const values = new ParameterEnumerationValue[2];
            parameter.enumValues.values = values;

            values[0].label = "recording off";
            values[0].value = REC_OFF;
            values[1].label = "recording on";
            values[1].value = REC_ON;
        }
        break;
    case id_scan:  //Scanning speed / direction of the playhead
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Scan";
        parameter.symbol = "SCAN";
        parameter.ranges.min = -5.0f;
        parameter.ranges.max = 5.0f;
        parameter.ranges.def = 1.0f;
        break;
    case id_spray:   //Width of randomness added to the playhead (scan-head)
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Spray";
        parameter.symbol = "SPRAY";
        parameter.ranges.min = 0.0001f;
        parameter.ranges.max = 10000.0f;
        parameter.ranges.def = 100.f;
        break;
    case id_density:   //Grain emission rate in Hz
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Density";
        parameter.symbol = "DENSITY";
        parameter.ranges.min = 0.1f;
        parameter.ranges.max = 500.0f;
        parameter.ranges.def = 10.f;
        break;
    case id_length:   //Length of sampled grain from buffer
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Length";
        parameter.symbol = "LENGTH";
        parameter.ranges.min = 1.f;
        parameter.ranges.max = 10000.0f;
        parameter.ranges.def = 100.f;
        break;
    case id_gain_spray:   //Probability of grain gain attenuation
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Gain Spray";
        parameter.symbol = "GAINSPRAY";
        parameter.ranges.min = 0.f;
        parameter.ranges.max = 1.f;
        parameter.ranges.def = 0.f;
        break;
    case id_direction:   //Factor of grain playback directon pobability
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Direction";
        parameter.symbol = "DIRECTION";
        parameter.ranges.min = -1.f;
        parameter.ranges.max = 1.f;
        parameter.ranges.def = 1.f;
        break;
    case id_sides:   //The shape of the Tukey window
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Sides";
        parameter.symbol = "SIDES";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = .5f;
        break;
    case id_tilt:  //Tilt factor
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Tilt";
        parameter.symbol = "TILT";
        parameter.ranges.min = -1.0f; 
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.f;
        break;
    case id_pitch:     //Octaves up/down
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Pitch";
        parameter.symbol = "PITCH";
        parameter.ranges.min = -2.f;
        parameter.ranges.max = 2.f;
        parameter.ranges.def = 0.f;
        break;
    case id_pitch_spray:   //Pitch random factor up/down from Pitch value
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Pitch Spray";
        parameter.symbol = "PITCHSPRAY";
        parameter.ranges.min = 0.f;
        parameter.ranges.max = 1.f;
        parameter.ranges.def = 0.f;
        break;
    case id_stereo_width:   //Stereo full width - or sum to mono
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Stereo Width";
        parameter.symbol = "STEREOWIDTH";
        parameter.ranges.min = 0.f;
        parameter.ranges.max = 2.f;
        parameter.ranges.def = 1.f;
        break;
    case id_pan_spray:   //The pan spray random factor
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Pan Spray";
        parameter.symbol = "PANSPRAY";
        parameter.ranges.min = 0.f;
        parameter.ranges.max = 1.f;
        parameter.ranges.def = 0.f;
        break;
    case id_dry:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Wet";
        parameter.symbol = "WET";
        parameter.ranges.min = 0.f; 
        parameter.ranges.max = 1.f;
        parameter.ranges.def = 1.f;
        break;
    case id_wet:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Dry";
        parameter.symbol = "DRY";
        parameter.ranges.min = 0.f; 
        parameter.ranges.max = 1.f;
        parameter.ranges.def = 1.f;
        break;
    }
}

void fabricDSP::initState(uint32_t, String &, String &)
{
    // we are using states but don't want them saved in the host yet
}

float fabricDSP::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case id_rec:
        return _recording;
    case id_scan:
        return grainPlayer.controls.scanSpeed;
    case id_spray:
        return grainPlayer.controls.spray;
    case id_density:
        return grainPlayer.controls.density;
    case id_length:
        return grainPlayer.controls.length;
    case id_gain_spray:
        return grainPlayer.controls.gainSpray;
    case id_direction:
        return grainPlayer.controls.direction;
    case id_pitch:
        return grainPlayer.controls.pitch;
    case id_pitch_spray:
        return grainPlayer.controls.pitchSpray;
    case id_stereo_width:
        return grainPlayer.controls.stereoWidth;
    case id_pan_spray:
        return grainPlayer.controls.panSpray;
    case id_sides:
        return grainPlayer.controls.sides;
    case id_tilt:
        return grainPlayer.controls.tilt;
    case id_dry:
        return m_dry;
    case id_wet:
        return m_wet;
    }

    return 0.0f;
}

void fabricDSP::setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    case id_rec:
        _recording = value;
        break;
    case id_scan:
        grainPlayer.controls.scanSpeed = value;
        break;
    case id_spray:
        grainPlayer.controls.spray = value;
        break;
    case id_density:
        grainPlayer.controls.density = value;
        break;
    case id_length:
        grainPlayer.controls.length = value;
        break;
    case id_gain_spray:
        grainPlayer.controls.gainSpray = value;
        break;
    case id_direction:
        grainPlayer.controls.direction = value;
        break;
    case id_pitch:
        grainPlayer.controls.pitch = value;
        break;
    case id_pitch_spray:
        grainPlayer.controls.pitchSpray = value;
        break;
    case id_stereo_width:
        grainPlayer.controls.stereoWidth = value;
        break;
    case id_pan_spray:
        grainPlayer.controls.panSpray = value;
        break;
    case id_sides:
        grainPlayer.controls.sides = value;
        break;
    case id_tilt:
        grainPlayer.controls.tilt = value;
        break;
    case id_dry:
        m_dry = value;
        break;
    case id_wet:
        m_wet = value;
        break;
    }
}


void fabricDSP::setState(const char *key, const char *)
{
    if (std::strcmp(key, "reset") != 0)
        return;

    fNeedsReset = true;
}

String fabricDSP::getState(const char* key) const
{
    return String();
}

void fabricDSP::clearOutputs(float** outputs, uint32_t frames)
{
    std::memset(outputs[0], 0, sizeof(float) * frames);
    std::memset(outputs[1], 0, sizeof(float) * frames);
}

void fabricDSP::copyInputs(const float** inputs, uint32_t frames)
{
    memcpy(mixDry[0], inputs[0], sizeof(float) * frames);
    memcpy(mixDry[1], inputs[1], sizeof(float) * frames);
}

void fabricDSP::mixToOutputs(float** outputs, float** dry, uint32_t frames)
{   
    for(int pos = 0; pos < frames; ++pos)
    {
        float smoothedDryValue = smoothDryValue->process(m_dry);
        float smoothedWetValue = smoothWetValue->process(m_wet);

        outputs[0][pos] = outputs[0][pos] * smoothedWetValue + mixDry[0][pos] * smoothedDryValue;
        outputs[1][pos] = outputs[1][pos] * smoothedWetValue + mixDry[1][pos] * smoothedDryValue;
    }
}

void fabricDSP::run(const float **inputs, float **outputs, uint32_t frames)
{
    // fill audio delay buffer 
    if (_recording){
        for (int pos = 0; pos < frames; pos++)
        {
            st_audioBuffer[0][bufferPos] = inputs[0][pos];
            st_audioBuffer[1][bufferPos] = inputs[1][pos];
            bufferPos++;
            if (bufferPos >= st_audioBufferSize) bufferPos = 0;
        }
    }
    // copy inputs to mixDry
    copyInputs(inputs, frames);

    // then clear the output buffer (which may be the same as the input on some hosts, hence why we do it here.)
    clearOutputs(outputs, frames);

    // run the effect
    grainPlayer.generate(outputs, st_audioBuffer, st_audioBufferSize, frames);

    // mix the wet / dry signal;
    mixToOutputs(outputs, mixDry, frames);
}

/* Plugin entry point, called by DPF to create a new plugin instance. */
Plugin *createPlugin()
{
    return new fabricDSP();
}

END_NAMESPACE_DISTRHO
