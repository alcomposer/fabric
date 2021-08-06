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
    for (Grain &grain : grainArray)
    {
        grains_free.push_back(grain);
    }
}

GrainPlayer::~GrainPlayer()
{

}

void GrainPlayer::addGrain(int currentFrame)
{
    if (grains_free.empty())
    {
        return;
    }
    Grain &slot = grains_free.front();
    grains_free.pop_front();
    grains_used.push_back(slot);

    //std::cout << "grains used LL: " << grains_used.size() << " grains free LL: " << grains_free.size()<< std::endl;

    Grain grain;

    float calcSpray = controls.spray / 1000.f * controls.sampleRate;
    
    grain.playing              = true;
    grain.startTimeFrameOffset = currentFrame;
    grain.length               = (int)(controls.length/1000 * controls.sampleRate);
    grain.age                  = grain.length; 
    grain.startTimeBuffer      = modulo((int)(controls.playHeadPos - calcSpray/2 + ((float)(std::rand() % 100000)/100000) * calcSpray), _bufferSize); 
    grain.sides                = controls.sides;
    
    slot = grain;
}

void GrainPlayer::generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames)
{
    _bufferSize = bufferSize;

    //Reset grains that have already played
    //for(int grainArrayPos = 0; grainArrayPos < MAX_GRAINS; ++grainArrayPos)
    //{
    //    Grain* currentGrain = &grainArray[grainArrayPos];
    //    if (currentGrain->age <= 0){
    //        currentGrain->playing = false;
    //    }
    //}

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

    GrainList::iterator grain_itterator = grains_used.begin();

    while (grain_itterator != grains_used.end())
    {
        Grain &grain = *grain_itterator;
        grain.process(outputs, st_audioBuffer, bufferSize, frames);

        GrainList::iterator saved_itterator = grain_itterator++;
        if (grain.age == 0)
        {
            grain.playing = false;
            grains_used.erase(saved_itterator);
            grains_free.push_back(grain);
        }
    }
}