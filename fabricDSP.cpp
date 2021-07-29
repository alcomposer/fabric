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

START_NAMESPACE_DISTRHO

fabricDSP::fabricDSP()
    : Plugin(Parameters::TOTAL, 0, 0), // 8 parameters, 0 programs, 0 states
      waveForm(2000),
      fColor(0.0f),
      fOutLeft(0.0f),
      fOutRight(0.0f),
      fNeedsReset(true)
{
    _sampleRate = getSampleRate();
    st_audioBuffer.resize(10*_sampleRate);
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
          All parameters in this plugin have the same ranges.
        */
    parameter.ranges.min = 0.0f;
    parameter.ranges.max = 1.0f;
    parameter.ranges.def = 0.0f;

    /**
          Set parameter data.
        */
    switch (index)
    {
    case 0:
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        parameter.name = "color";
        parameter.symbol = "color";
        parameter.enumValues.count = 2;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const values = new ParameterEnumerationValue[2];
            parameter.enumValues.values = values;

            values[0].label = "Green";
            values[0].value = METER_COLOR_GREEN;
            values[1].label = "Blue";
            values[1].value = METER_COLOR_BLUE;
        }
        break;
    case 1:
        parameter.hints = kParameterIsAutomable | kParameterIsOutput;
        parameter.name = "out-left";
        parameter.symbol = "out_left";
        break;
    case 2:
        parameter.hints = kParameterIsAutomable | kParameterIsOutput;
        parameter.name = "out-right";
        parameter.symbol = "out_right";
        break;
    }
}

void fabricDSP::initState(uint32_t, String &, String &)
{
    // we are using states but don't want them saved in the host
}

float fabricDSP::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case 0:
        return fColor;
    case 1:
        return fOutLeft;
    case 2:
        return fOutRight;
    }

    return 0.0f;
}

void fabricDSP::setParameterValue(uint32_t index, float value)
{
    // this is only called for input paramters, and we only have one of those.
    if (index != 0)
        return;

    fColor = value;
}

void fabricDSP::setState(const char *key, const char *)
{
    if (std::strcmp(key, "reset") != 0)
        return;

    fNeedsReset = true;
}

void fabricDSP::run(const float **inputs, float **outputs, uint32_t frames)
{
    float tmp;
    float tmpLeft = 0.0f;
    float tmpRight = 0.0f;

    for (uint32_t i = 0; i < frames; ++i)
    {
        // left
        tmp = std::abs(inputs[0][i]);

        if (tmp > tmpLeft)
            tmpLeft = tmp;

        // right
        tmp = std::abs(inputs[1][i]);

        if (tmp > tmpRight)
            tmpRight = tmp;
    }

    if (tmpLeft > 1.0f)
        tmpLeft = 1.0f;
    if (tmpRight > 1.0f)
        tmpRight = 1.0f;

    if (fNeedsReset)
    {
        fOutLeft = tmpLeft;
        fOutRight = tmpRight;
        fNeedsReset = false;
    }
    else
    {
        if (tmpLeft > fOutLeft)
            fOutLeft = tmpLeft;
        if (tmpRight > fOutRight)
            fOutRight = tmpRight;
    }

    if (true){//(recTrue){
        for (int pos = 0; pos < frames; pos++)
        {
            st_audioBuffer[_bufferPos].first = inputs[0][pos];
            st_audioBuffer[_bufferPos].second = inputs[1][pos];
            _bufferPos++;
            if (_bufferPos > st_audioBuffer.size()) _bufferPos = 0;
        }
    }
    
    //convert audio buffer into low quality buffer
    int increment = st_audioBuffer.size()/DISPLAY_WIDTH;
    waveForm.clear();
    for (int pos = 0; pos < (_sampleRate * 10) ; pos+=increment)
    {
        waveForm.push_back(((st_audioBuffer[pos].first + st_audioBuffer[pos].second) * 0.5) * float(DISPLAY_WIDTH * 0.5));
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
