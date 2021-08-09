#pragma once

#include <cmath>
#include <xmmintrin.h>
#include <cstring>
#include <cstdlib>
#include <cstdint>


namespace fabricMaths {

inline __m128 absPS(__m128 x)
{
     float mask;
     uint32_t u32 = UINT32_C(0x7FFFFFFF);
     std::memcpy(&mask, &u32, sizeof(u32));
     return _mm_and_ps(_mm_set1_ps(mask), x);
}
inline __m128 ternaryPS(__m128 cond, __m128 a, __m128 b)
{
    return _mm_or_ps(_mm_and_ps(cond, a), _mm_andnot_ps(cond, b));
}
// Calculate a rough approximation of sine, valid in +-pi
inline __m128 approxSinePS(__m128 x)
{
    __m128 xx = _mm_mul_ps(x, _mm_set1_ps(float(1.0f / M_PI)));
    return _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(4), xx), (_mm_sub_ps(_mm_set1_ps(1), absPS(xx))));
}
// Calculate a rough approximation of cosine, valid in +-pi
inline __m128 approxCosinePS(__m128 x)
{
    __m128 xx = ternaryPS(_mm_cmplt_ps(x, _mm_set1_ps(float(M_PI / 2))),
                          _mm_set1_ps(float(M_PI / 2)), _mm_set1_ps(float(-3 * M_PI / 2)));
    return approxSinePS(xx);
}
// Calculate a rough approximation of sine, valid in +-pi
inline float approxSine(float x)
{
    float xx = x * float(1.0f / M_PI);
    return 4 * xx * (1 - std::fabs(xx));
}
// Calculate a rough approximation of cosine, valid in +-pi
inline float approxCosine(float x)
{
    float xx = x + ((x < float(M_PI / 2)) ? float(M_PI / 2) : float(-3 * M_PI / 2));
    return approxSine(xx);
}
// Calculate a Tukey window with tilt implemented as a timewarp of time
inline float tukeyWindow(float x, float sides, float tilt)
{   
    float timeWarp = tilt < 0? std::pow(x, 1 + tilt) : std::pow(1 - x, 1 - tilt);
    return (approxCosine(std::fmax(std::fabs(timeWarp - 0.5f) * (2.0f / sides)  - (1.0f / sides - 1.0f), 0.0f) * (float)M_PI) + 1.0f) * 0.5f; 
}

}
