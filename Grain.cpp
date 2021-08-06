#include "Grain.hpp"
#include <iostream>
#include <math.h>

#define  PI   3.141592653589793

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

void Grain::process(float** outputs, float** st_audioBuffer, int st_audioBufferSize, int frames)
{
    for (int framePos = startTimeFrameOffset; framePos < frames && age > 0; ++framePos)
    {
        startTimeFrameOffset = 0;
        double i = (double)age/length;
        float window = (cos(fmax(fabs((double)i - 0.5) * (2.0 / sides)  - (1.0 / sides - 1.0), 0.0) * PI) + 1.0) / 2.0;
        startTimeBuffer = startTimeBuffer % st_audioBufferSize;

        outputs[0][framePos] += st_audioBuffer[0][startTimeBuffer] * window;
        outputs[1][framePos] += st_audioBuffer[1][startTimeBuffer] * window;

        startTimeBuffer++;
        age--;
    }
}