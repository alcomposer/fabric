#include "GrainPlayer.hpp"
#include <random>
#include <iostream>

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
        
        if (currentGrain->queuedToPlay == false)
        {
            currentGrain->queuedToPlay         = true;
            currentGrain->startTimeFrameOffset = currentFrame;
            currentGrain->length               = (int)(controls.length/1000 * controls.sampleRate);
            currentGrain->age                  = currentGrain->length; 
            currentGrain->startTimeBuffer      = controls.playHeadPos;//causeing floating point exception>>> + (std::rand() % (int)(controls.spray / 1000.0 * controls.sampleRate)) - (int)(controls.spray / 1000.0 * controls.sampleRate / 2.0); 
            currentGrain->sides                = controls.sides;
            return;
        }
    }
}

void GrainPlayer::generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames)
{
    int densityHz = (int)(controls.sampleRate / controls.density);
    //queue the grains
    for(int currentFrame = 0; currentFrame < frames; ++currentFrame)
    {
        if (_nextGrainTime == 0)
        {   
            addGrain(currentFrame);
        }
        if (_nextGrainTime > densityHz)
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
    //process the grains
    for(int grainArrayPos = 0; grainArrayPos < MAX_GRAINS; ++grainArrayPos)
    {
        Grain* currentGrain = &grainArray[grainArrayPos];
        currentGrain->process(outputs, st_audioBuffer, bufferSize, frames);
    }
}