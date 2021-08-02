#pragma once

#include <stdint.h>
#include <stdlib.h>

class Grain
{
public:
    Grain();
    ~Grain();
private:
    float* grainBuffer[2];
};