#include "fabricMaths.hpp"
#include "Grain.hpp"
#include <iostream>
#include <math.h>

Grain::Grain() : 
     playing(false)
    ,startTimeFrameOffset(0)
    ,length(0)
    ,startTimeBuffer(0)
    ,age(0)
    ,positionInAudioBuffer(0)
{

}

Grain::~Grain()
{

}

void Grain::process(float** outputs, float** st_audioBuffer, int st_audioBufferSize, int subdivStart, int frames)
{
    for (int framePos = startTimeFrameOffset; framePos < frames && age > 0; ++framePos)
    {
        startTimeFrameOffset = 0;
        double i = (double)(length-age)/length;
        float window = fabricMaths::tukeyWindow(i, sides, tilt);
        startTimeBuffer = startTimeBuffer % st_audioBufferSize;

        outputs[0][subdivStart + framePos] += st_audioBuffer[0][startTimeBuffer] * window;
        outputs[1][subdivStart + framePos] += st_audioBuffer[1][startTimeBuffer] * window;

        startTimeBuffer++;
        age--;
    }
}