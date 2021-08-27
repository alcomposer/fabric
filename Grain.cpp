#include "fabricMaths.hpp"
#include "Grain.hpp"
#include <iostream>
#include <math.h>
#include <algorithm>

Grain::Grain() : 
     m_playing(false)
    ,m_startTimeFrameOffset(0)
    ,m_length(0)
    ,m_gain(1.f)
    ,m_startTimeBuffer(0)
    ,m_age(0)
    ,m_positionInAudioBuffer(0)
    ,m_pitch(1.f)
    ,m_direction(Direction::forward)
    ,m_stereoWidth(1.f)
{

}

Grain::~Grain()
{

}

void Grain::process(float** outputs, float** st_audioBuffer, int st_audioBufferSize, int subdivStart, int frames)
{
    //copy the data from member variable, to local variable to improve performance
    float startTimeBuffer = m_startTimeBuffer;

    float *leftOutput = outputs[0] + subdivStart;
    float *rightOutput = outputs[1] + subdivStart;
 
    const float *leftBuffer = st_audioBuffer[0];
    const float *rightBuffer = st_audioBuffer[1];
 
    //player should guarantee this
    assert(m_startTimeFrameOffset < frames);
 
    //offset output by the starting delay
    leftOutput += m_startTimeFrameOffset;
    rightOutput += m_startTimeFrameOffset;
    frames -= m_startTimeFrameOffset;
    m_startTimeFrameOffset = 0;
    
    //do not output past the grain's lifetime - we need to calculate the frames that m_age takes up
    //because we are also playing the grains back at speed depending on m_pitch
    frames = std::min(frames, (int)std::ceil(m_age / m_pitch));

    for (int framePos = 0; framePos < frames; ++framePos)
    {
        startTimeBuffer = fmod(startTimeBuffer, (float)st_audioBufferSize);
        
        float fIndexGrain = (m_length - m_age) / m_length;
        float window = fabricMaths::tukeyWindow(fIndexGrain, m_sides, m_tilt);

        // lerp for accessing the stereo audio buffer
        int startTimeBufferLerp = ((int)startTimeBuffer + 1) % st_audioBufferSize;
        float fraction = startTimeBuffer - (int)startTimeBuffer;

        float left = fabricMaths::lerp(leftBuffer[(int)startTimeBuffer], leftBuffer[startTimeBufferLerp], fraction);
        float right = fabricMaths::lerp(rightBuffer[(int)startTimeBuffer], rightBuffer[startTimeBufferLerp], fraction);

        // stereo width
        float mid = 0.5f * (left + right);
        float side = 0.5f * (right - left);
        float att = std::max(1.0f + m_stereoWidth, 2.0f);
        left = (mid - m_stereoWidth * side) / att;
        right = (mid + m_stereoWidth * side) / att;

        // pan
        left *= std::sqrt(1.0f - m_pan);
        right *= std::sqrt(m_pan);

        leftOutput[framePos]  += left * window * m_gain;
        rightOutput[framePos] += right * window * m_gain;

        m_age -= m_pitch;
        startTimeBuffer += m_pitch * (float)m_direction;
        startTimeBuffer = startTimeBuffer < 0.0f ? startTimeBuffer + st_audioBufferSize : startTimeBuffer;
    }
    m_startTimeBuffer = startTimeBuffer;
}