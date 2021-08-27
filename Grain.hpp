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
    enum Direction {forward = 1, reverse = -1};
    
    bool  m_playing;
    int   m_startTimeFrameOffset;
    float m_length;
    float m_gain;
    float m_startTimeBuffer;
    int   m_positionInAudioBuffer;
    float m_age;
    float m_sides;
    float m_tilt;
    float m_pitch;
    float m_stereoWidth;
    float m_pan;
    Direction m_direction;
};