#pragma once

#include <boost/intrusive/list.hpp>
#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h>

class Grain : public boost::intrusive::list_base_hook<>
{
public:
    Grain();
    ~Grain();
    void process(float** outputs, float** st_audioBuffer, int st_audioBufferSize, int subdivStart, int frames);
    
    bool playing;
    int startTimeFrameOffset;
    int length;
    int startTimeBuffer;
    int positionInAudioBuffer;
    int age;
    float sides;
};