#include "GrainPlayer.hpp"

GrainPlayer::GrainPlayer()
{

}

GrainPlayer::~GrainPlayer()
{
    
}

void GrainPlayer::generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames)
{
    for(int i = 0; i < frames; ++i)
    {
        controls.playHeadPos += controls.speed;
        if (controls.playHeadPos > bufferSize)
            controls.playHeadPos = 0;
        if (controls.playHeadPos < 0)
            controls.playHeadPos = bufferSize;
    }
}