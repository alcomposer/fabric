#pragma once

#include <math.h>

namespace FABRICMATHS {

#define  PI   3.141592653589793

inline float tukeyWindow(float x, float sides, float tilt)
{   
    float timeWarp = tilt < 0? (pow(x, 1 + tilt) / pow(1,1 + tilt)) : (pow(1- x, 1 - tilt) / pow(1,1 - tilt));
    return (cos(fmax(fabs((double)timeWarp - 0.5) * (2.0 / sides)  - (1.0 / sides - 1.0), 0.0) * PI) + 1.0) / 2.0; 
}

}