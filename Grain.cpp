#include "Grain.hpp"

Grain::Grain()
{
    grainBuffer[0] = (float*)calloc(2 * 8192, sizeof(float)); //allocate largest audio buffer possible
    grainBuffer[1] = grainBuffer[0] + 8192; 
}

Grain::~Grain()
{

}