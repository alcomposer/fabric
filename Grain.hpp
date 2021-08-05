#pragma once

#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h>

class GrainPlayer;

class Grain
{
public:
    Grain();
    ~Grain();
    void process(float** outputs, float** st_audioBuffer, int st_audioBufferSize, int frames);
    
    bool playing;
    int startTimeFrameOffset;
    int length;
    int startTimeBuffer;
    int positionInAudioBuffer;
    int age;
    float sides;
};