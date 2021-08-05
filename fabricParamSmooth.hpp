/**
 * One-pole LPF for smooth parameter changes
 *
 * https://www.musicdsp.org/en/latest/Filters/257-1-pole-lpf-for-smooth-parameter-changes.html
 */

#pragma once
#include <cmath>

class fabricParamSmooth {
public:
    fabricParamSmooth() noexcept = default;

    void clear() noexcept
    {
        z = 0.0f;
    }

    void setSampleRate(float samplingRate) noexcept
    {
        if (samplingRate != fs) {
            fs = samplingRate;
            updateCoefficient();
        }
    }

    void setSmoothingT60(float smoothingT60) noexcept
    {
        if (t60 != smoothingT60) {
            t60 = smoothingT60;
            updateCoefficient();
        }
    }

    float process(float in) noexcept
    {
        float b = 1.0f - a;
        return z = (in * b) + (z * a);
    }

private:
    void updateCoefficient() noexcept
    {
        if (t60 <= 0.0f || fs <= 0.0f) {
            a = 0.0f;
            return;
        }
        float twoPi = 6.283185307179586476925286766559f;
        float t = t60 * (1.0f / 6.907755278982137f);
        a = std::exp(-twoPi / (t * fs));
    }

private:
    float a {};
    float t60 {};
    float z {};
    float fs {};
};