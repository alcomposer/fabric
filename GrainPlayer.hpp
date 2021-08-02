#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "Grain.hpp"

#define MAX_GRAINS 256

struct GrainPlayerControl
{
    float speed;
    float density;
    float length;
    float spray;
    float sides;
    float playHeadPos;
};

class GrainPlayer
{
public:
    GrainPlayer();
    ~GrainPlayer();
    void generate(float** outputs, float** st_audioBuffer, int bufferSize, uint32_t frames);
    GrainPlayerControl controls;

    Grain grainArray[MAX_GRAINS];
private:
    uint32_t _frames;
};