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
    
    bool  m_playing;
    int   m_startTimeFrameOffset;
    float m_length;
    float m_startTimeBuffer;
    int   m_positionInAudioBuffer;
    float m_age;
    float m_sides;
    float m_tilt;
    float m_speed;
};