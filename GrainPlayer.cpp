#include "GrainPlayer.hpp"
#include <random>
#include <iostream>

inline int modulo(int a, int b) {
    if(b < 0) return modulo(-a, -b);
    const int result = a % b;
    return result >= 0 ? result : result + b;
}

GrainPlayer::GrainPlayer()
{

}

GrainPlayer::~GrainPlayer()
{

}

void GrainPlayer::addGrain(int currentFrame)
{
    for (int grainArrayPos = 0; grainArrayPos < MAX_GRAINS; ++grainArrayPos)
    {
        Grain* currentGrain = &grainArray[grainArrayPos];

        float calcSpray = controls.spray / 1000.f * controls.sampleRate;
        
        if (currentGrain->playing == false)
        {
            currentGrain->playing              = true;
            currentGrain->startTimeFrameOffset = currentFrame;
            currentGrain->length               = (int)(controls.length/1000 * controls.sampleRate);
            currentGrain->age                  = currentGrain->length; 
            currentGrain->startTimeBuffer      = modulo((int)(controls.playHeadPos - calcSpray/2 + ((float)(std::rand() % 100000)/100000) * calcSpray), _bufferSize); 
            currentGrain->sides                = controls.sides;
            return;
        }
    }
}

void GrainPlayer::generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames)
{
    _bufferSize = bufferSize;

    //Reset grains that have already played
    for(int grainArrayPos = 0; grainArrayPos < MAX_GRAINS; ++grainArrayPos)
    {
        Grain* currentGrain = &grainArray[grainArrayPos];
        if (currentGrain->age <= 0){
            currentGrain->playing = false;
        }
    }

    //Pre-process all new grains for this frame buffer cycle
    int densityFramesInterval = (int)(controls.sampleRate / controls.density);
    for(int currentFrame = 0; currentFrame < frames; ++currentFrame)
    {
        if (_nextGrainTime == 0)
        {   
            addGrain(currentFrame);
        }
        if (_nextGrainTime > densityFramesInterval)
        {
            _nextGrainTime = 0;
        }else
        {
            _nextGrainTime++;
        }

        controls.playHeadPos += controls.speed;
        if (controls.playHeadPos > bufferSize)
            controls.playHeadPos = 0;
        if (controls.playHeadPos < 0)
            controls.playHeadPos = bufferSize;
    }
    //Process the grains
    for(int grainArrayPos = 0; grainArrayPos < MAX_GRAINS; ++grainArrayPos)
    {
        Grain* currentGrain = &grainArray[grainArrayPos];
        if (currentGrain->playing){
            currentGrain->process(outputs, st_audioBuffer, bufferSize, frames);
        }
    }
}