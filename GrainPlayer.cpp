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

bool GrainPlayer::addGrain(int currentFrame)
{
    if (grains_free.empty())
    {
        return false;
    }
    Grain &slot = grains_free.front();
    grains_free.pop_front();
    grains_used.push_back(slot);

    Grain grain;

    float calcSpray = controls.spray / 1000.f * controls.sampleRate;
    
    grain.playing              = true;
    grain.startTimeFrameOffset = currentFrame;
    grain.length               = (int)(controls.length/1000 * controls.sampleRate);
    grain.age                  = grain.length; 
    grain.startTimeBuffer      = modulo((int)(controls.playHeadPos - calcSpray/2 + ((float)(std::rand() % 100000)/100000) * calcSpray), _bufferSize); 
    grain.sides                = controls.sides;
    
    slot = grain;
    return true;
}

void GrainPlayer::generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames)
{
    _bufferSize = bufferSize;

    //Number of frames synthesized so far
    int synthesizedFrames = 0;

    //Pre-process all new grains for this frame buffer cycle
    int densityFramesInterval = (int)(controls.sampleRate / controls.density);
    for(int currentFrame = 0; currentFrame < frames; ++currentFrame)
    {
        if (_nextGrainTime == 0)
        {
            //The index of the frame within the subdivision
            int subdivisionFrame = currentFrame - synthesizedFrames;

            bool haveGrain = addGrain(subdivisionFrame);
            if (!haveGrain)
            {
                //Synth current grains up to currentFrame not included, then try again
                if (synthesizedFrames < currentFrame)
                {
                    generateSubdivision(outputs, st_audioBuffer, bufferSize, synthesizedFrames, currentFrame - synthesizedFrames);
                    synthesizedFrames = currentFrame;
                    addGrain(0);
                }
            }
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

    //Synth the remaining frames
    if (synthesizedFrames < frames)
        generateSubdivision(outputs, st_audioBuffer, bufferSize, synthesizedFrames, frames - synthesizedFrames);
}

void GrainPlayer::generateSubdivision(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t subdivStart, uint32_t subdivFrames)
{
    GrainList::iterator grain_itterator = grains_used.begin();

    while (grain_itterator != grains_used.end())
    {
        Grain &grain = *grain_itterator;

        grain.process(outputs, st_audioBuffer, bufferSize, subdivStart, subdivFrames);

        GrainList::iterator saved_itterator = grain_itterator++;
        if (grain.age == 0)
        {
            grain.playing = false;
            grains_used.erase(saved_itterator);
            grains_free.push_back(grain);
        }
    }
}
