#include "fabricMaths.hpp"
#include "Grain.hpp"
#include <iostream>
#include <math.h>
#include <algorithm>

Grain::Grain() : 
     playing(false)
    ,startTimeFrameOffset(0)
    ,length(0)
    ,m_startTimeBuffer(0)
    ,age(0)
    ,positionInAudioBuffer(0)
{

}

Grain::~Grain()
{

}

void Grain::process(float** outputs, float** st_audioBuffer, int st_audioBufferSize, int subdivStart, int frames)
{
    //copy the data from member variable, to local variable to improve performance
    int startTimeBuffer = this->m_startTimeBuffer;

    float *leftOutput = outputs[0] + subdivStart;
    float *rightOutput = outputs[1] + subdivStart;
 
    const float *leftBuffer = st_audioBuffer[0];
    const float *rightBuffer = st_audioBuffer[1];
 
    //player should guarantee this
    assert(startTimeFrameOffset < frames);
 
    //offset output by the starting delay
    leftOutput += startTimeFrameOffset;
    rightOutput += startTimeFrameOffset;
    frames -= startTimeFrameOffset;
    startTimeFrameOffset = 0;
    
    //do not output past the grain's lifetime
    frames = std::min(frames, age);

    for (int framePos = 0; framePos < frames; ++framePos)
    {
        float i = (float)(length - (age - framePos)) / length;
        float window = fabricMaths::tukeyWindow(i, sides, tilt);
        startTimeBuffer = startTimeBuffer % st_audioBufferSize;
 
        leftOutput[framePos] += leftBuffer[startTimeBuffer] * window;
        rightOutput[framePos] += rightBuffer[startTimeBuffer] * window;

        startTimeBuffer++;
    }
    this->m_startTimeBuffer = startTimeBuffer;
    age -= frames;
}