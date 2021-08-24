#include "GrainPlayer.hpp"
#include <iostream>
#include <memory.h>

inline int modulo(int a, int b) {
    if(b < 0) return modulo(-a, -b);
    const int result = a % b;
    return result >= 0 ? result : result + b;
}

GrainPlayer::GrainPlayer() :
      m_fRandomNormalized(.0f, 1.0f)
     ,m_fRandomBiPolNormalized(-1.f, 1.f)
{
    for (Grain &grain : grainArray)
    {
        grains_free.push_back(grain);
    }

    // setup the linear smoothers for control parameters
    // pitch
    m_pitchSmoothData = (float*)calloc(8192, sizeof(float));
    m_pitchSmooth.setSampleRate(controls.sampleRate);
    m_pitchSmooth.setSmoothTime(.1f);

    // length
    m_lengthSmoothData = (float*)calloc(8192, sizeof(float));
    m_lengthSmooth.setSampleRate(controls.sampleRate);
    m_lengthSmooth.setSmoothTime(.1f);
}

GrainPlayer::~GrainPlayer()
{   
    free(m_pitchSmoothData);
    free(m_lengthSmoothData);
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
    
    grain.m_playing              = true;
    grain.m_direction            = m_fRandomNormalized(m_seed) <= (controls.direction * .5f) + .5f ? Grain::Direction::forward : Grain::Direction::reverse;
    grain.m_startTimeFrameOffset = currentFrame;
    grain.m_age                  = (m_lengthSmoothData[currentFrame] / 1000.f * controls.sampleRate);
    grain.m_length               = grain.m_age; 

    grain.m_startTimeBuffer      = grain.m_direction == Grain::Direction::forward ? 
                                   modulo((int)(controls.playHeadPos - calcSpray/2 + m_fRandomNormalized(m_seed) * calcSpray), _bufferSize)
                                 : modulo((int)(controls.playHeadPos + grain.m_length - calcSpray/2 + m_fRandomNormalized(m_seed) * calcSpray), _bufferSize);

    grain.m_sides                = controls.sides;
    grain.m_tilt                 = controls.tilt;
    grain.m_pitch                = std::pow(2.f, m_pitchSmoothData[currentFrame] + (m_fRandomBiPolNormalized(m_seed) * controls.pitchSpray));
    grain.m_stereoWidth          = controls.stereoWidth;
    grain.m_pan                  = ((m_fRandomBiPolNormalized(m_seed) * controls.panSpray) + 1.f) * .5f;

    slot = grain;
    return true;
}

void GrainPlayer::generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames)
{
    _bufferSize = bufferSize;
    
    processSmoothers(frames);

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

        controls.playHeadPos += controls.scanSpeed;
        if (controls.playHeadPos > bufferSize)
            controls.playHeadPos = 0;
        if (controls.playHeadPos < 0)
            controls.playHeadPos = bufferSize - 1;
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
        if (grain.m_age <= 0)
        {
            grain.m_playing = false;
            grains_used.erase(saved_itterator);
            grains_free.push_back(grain);
        }
    }
}

void GrainPlayer::processSmoothers(uint32_t frames)
{
    std::fill_n(m_pitchSmoothData, frames, controls.pitch);
    m_pitchSmooth.process(m_pitchSmoothData, m_pitchSmoothData, frames, true);

    std::fill_n(m_lengthSmoothData, frames, controls.length);
    m_lengthSmooth.process(m_lengthSmoothData, m_lengthSmoothData, frames, true);

}
