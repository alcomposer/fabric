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
    st_audioBuffer.resize(10 * _sampleRate);
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
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Density";
        parameter.symbol = "DENSITY";
        parameter.ranges.min = 0.0f;    //Hz
        parameter.ranges.max = 1000.0f;
        parameter.ranges.def = 10.f;
        break;
    case id_length:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Length";
        parameter.symbol = "LENGTH";
        parameter.ranges.min = 0.0f;    //Milliseconds
        parameter.ranges.max = 10000.0f;
        parameter.ranges.def = 100.f;
        break;
    case id_spray:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Spray";
        parameter.symbol = "SPRAY";
        parameter.ranges.min = 0.0f;    //Milliseconds
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
        return _controls.speed;
    case id_density:
        return _controls.density;
    case id_length:
        return _controls.length;
    case id_spray:
        return _controls.spray;
    case id_sides:
        return _controls.sides;
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
        _controls.speed = value;
        break;
    case id_density:
        _controls.density = value;
        break;
    case id_length:
        _controls.length = value;
        break;
    case id_spray:
        _controls.spray = value;
        break;
    case id_sides:
        _controls.sides = value;
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


void fabricDSP::run(const float **inputs, float **outputs, uint32_t frames)
{
    if (_recording){
        for (int pos = 0; pos < frames; pos++)
        {
            st_audioBuffer[_bufferPos].first = inputs[0][pos];
            st_audioBuffer[_bufferPos].second = inputs[1][pos];
            _bufferPos++;
            if (_bufferPos > st_audioBuffer.size()) _bufferPos = 0;
        }
    }

    // copy inputs over outputs if needed
    if (outputs[0] != inputs[0])
        std::memcpy(outputs[0], inputs[0], sizeof(float) * frames);

    if (outputs[1] != inputs[1])
        std::memcpy(outputs[1], inputs[1], sizeof(float) * frames);
}

/* Plugin entry point, called by DPF to create a new plugin instance. */
Plugin *createPlugin()
{
    return new fabricDSP();
}

END_NAMESPACE_DISTRHO
