#include "fabricMaths.hpp"
#include "Grain.hpp"
#include <iostream>
#include <math.h>
#include <algorithm>

Grain::Grain() : 
     m_playing(false)
    ,m_startTimeFrameOffset(0)
    ,m_length(0)
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

        // process stereo width
        float coef_s = m_stereoWidth * 0.5f;
        float mid = 0.5f * (left + right);
        float side = coef_s * (right - left);
        left = (mid - side);
        right = (mid + side);

        //angle of pan, m_pan is bipolar -1<0<1
        float r = (m_pan * M_PI);
        float ballanceL = m_pan > 0 ? 1.f : 1.f + m_pan; 
        float ballanceR = m_pan < 0 ? 1.f : 1.f - m_pan;

        // Calculate transformation matrix's coefficients
        float leftOut  = ballanceL * ((left *  fabricMaths::approxCosine(r))  - (right * fabricMaths::approxSine(r)));
        float rightOut = ballanceR * ((left * fabricMaths::approxSine(r)) + (right * fabricMaths::approxCosine(r)));

        leftOutput[framePos]  += leftOut * window;
        rightOutput[framePos] += rightOut * window;

        m_age -= m_pitch;
        startTimeBuffer += m_pitch * m_direction;
        startTimeBuffer = startTimeBuffer < 0.0f ? startTimeBuffer + st_audioBufferSize : startTimeBuffer;
    }
    m_startTimeBuffer = startTimeBuffer;
}