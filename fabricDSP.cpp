/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2018 Filipe Coelho <falktx@falktx.com>
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
    : Plugin(Parameters::TOTAL, 0, 0) // 8 parameters, 0 programs, 0 states
      ,_recording(false)
      ,_wet(50.f)
      ,_dry(50.f)
      ,_mix(0.f)
      ,fNeedsReset(true)
{
    _sampleRate = getSampleRate();

    // allocate stereo buffer for 10 circular delay line
    st_audioBufferSize = 10 * _sampleRate;
    st_audioBuffer[0] = (float*)calloc(2 * st_audioBufferSize, sizeof(float));
    st_audioBuffer[1] = st_audioBuffer[0] + st_audioBufferSize;

    // allocate stereo buffer for mixDry
    mixDry[0] = (float*)calloc(2 * 8192, sizeof(float));
    mixDry[1] = mixDry[0] + 8192;

    grainPlayer = GrainPlayer();
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
    case id_speed:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Speed";
        parameter.symbol = "SPEED";
        parameter.ranges.min = -2.0f;
        parameter.ranges.max = 2.0f;
        parameter.ranges.def = 1.0f;
        break;
    case id_density:
        parameter.hints = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name = "Density";
        parameter.symbol = "DENSITY";
        parameter.ranges.min = 0.01f;    //Hz
        parameter.ranges.max = 1000.0f;
        parameter.ranges.def = 10.f;
        break;
    case id_length:
        parameter.hints = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name = "Length";
        parameter.symbol = "LENGTH";
        parameter.ranges.min = 0.1f;    //Milliseconds
        parameter.ranges.max = 10000.0f;
        parameter.ranges.def = 100.f;
        break;
    case id_spray:
        parameter.hints = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name = "Spray";
        parameter.symbol = "SPRAY";
        parameter.ranges.min = 0.0001f;    //Milliseconds
        parameter.ranges.max = 10000.0f;
        parameter.ranges.def = 100.f;
        break;
    case id_sides:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Sides";
        parameter.symbol = "SIDES";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = .5f;
        break;
    case id_wet:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Wet";
        parameter.symbol = "WET";
        parameter.ranges.min = 0.0f;    //Percent
        parameter.ranges.max = 100.0f;
        parameter.ranges.def = 50.f;
        break;
    case id_dry:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Dry";
        parameter.symbol = "DRY";
        parameter.ranges.min = 0.0f;    //Percent
        parameter.ranges.max = 100.0f;
        parameter.ranges.def = 50.f;
        break;
    case id_mix:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Mix";
        parameter.symbol = "MIX";
        parameter.ranges.min = -100.0f; //Percent
        parameter.ranges.max = 100.0f;
        parameter.ranges.def = 0.f;
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
    case id_speed:
        return grainPlayer.controls.speed;
    case id_density:
        return grainPlayer.controls.density;
    case id_length:
        return grainPlayer.controls.length;
    case id_spray:
        return grainPlayer.controls.spray;
    case id_sides:
        return grainPlayer.controls.sides;
    case id_wet:
        return _wet;
    case id_dry:
        return _dry;
    case id_mix:
        return _mix;
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
    case id_speed:
        grainPlayer.controls.speed = value;
        break;
    case id_density:
        grainPlayer.controls.density = value;
        break;
    case id_length:
        grainPlayer.controls.length = value;
        break;
    case id_spray:
        grainPlayer.controls.spray = value;
        break;
    case id_sides:
        grainPlayer.controls.sides = value;
        break;
    case id_wet:
        _wet = value;
        break;
    case id_dry:
        _dry = value;
        break;
    case id_mix:
        _mix = value;
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
    float mixWetValue = _mix > 0 ? 1.f : (100.f - abs(_mix)) * .01f ;
    float mixDryValue = _mix < 0 ? 1.f : (100.f - _mix) * .01f;
    
    for(int pos = 0; pos < frames; ++pos)
    {
        outputs[0][pos] = outputs[0][pos] * mixWetValue + mixDry[0][pos] * mixDryValue;
        outputs[1][pos] = outputs[1][pos] * mixWetValue + mixDry[1][pos] * mixDryValue;
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
