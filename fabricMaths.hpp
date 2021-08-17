// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include <cmath>
#include <xmmintrin.h>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>


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
//Lerp
inline float lerp(float a, float b, float fraction)
{
    return a + fraction * (b - a);
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
    float timeWarp = tilt < 0? std::pow(x, 1.f + tilt) : std::pow(1.f - x, 1.f - tilt);
    return (approxCosine(std::fmax(std::fabs(timeWarp - 0.5f) * (2.0f / sides)  - (1.0f / sides - 1.0f), 0.0f) * (float)M_PI) + 1.0f) * 0.5f; 
}

// The following code is part of the sfizz library and is licensed under a BSD 2-clause
// license. You should have receive a LICENSE.md file along with the code.
// If not, contact the sfizz maintainers at https://github.com/sfztools/sfizz

/**
 * @brief A low-quality random number generator guaranteed to be very fast
 */
class fast_rand {
public:
    typedef uint32_t result_type;

    fast_rand() noexcept
    {
    }

    explicit fast_rand(uint32_t value) noexcept
        : mem(value)
    {
    }

    static constexpr uint32_t min() noexcept
    {
        return 0;
    }

    static constexpr uint32_t max() noexcept
    {
        return std::numeric_limits<uint32_t>::max();
    }

    uint32_t operator()() noexcept
    {
        uint32_t next = mem * 1664525u + 1013904223u; // Numerical Recipes
        mem = next;
        return next;
    }

    void seed(uint32_t value = 0) noexcept
    {
        mem = value;
    }

    void discard(unsigned long long z) noexcept
    {
        for (unsigned long long i = 0; i < z; ++i)
            operator()();
    }

private:
    uint32_t mem = 0;
};

/**
 * @brief A uniform real distribution guaranteed to be very fast
 */
template <class T>
class fast_real_distribution {
public:
    static_assert(std::is_floating_point<T>::value, "The type must be floating point.");

    typedef T result_type;

    fast_real_distribution(T a, T b)
        : a_(a), b_(b), k_(b - a)
    {
    }

    template <class G>
    T operator()(G& g) const
    {
        return a_ + (g() - T(G::min())) * (k_ / T(G::max() - G::min()));
    }

    T a() const noexcept
    {
        return a_;
    }

    T b() const noexcept
    {
        return b_;
    }

    T min() const noexcept
    {
        return a_;
    }

    T max() const noexcept
    {
        return b_;
    }

private:
    T a_;
    T b_;
    T k_;
};

/**
 * @brief Global random singletons
 *
 * TODO: could be moved into a singleton class holder
 *
 */
static fast_rand randomGenerator;
/**
 * @brief Generate normally distributed noise.
 *
 * This sums the output of N uniform random generators.
 * The higher the N, the better is the approximation of a normal distribution.
 */
template <class T, unsigned N = 4>
class fast_gaussian_generator {
    static_assert(N > 1, "Invalid quality setting");

public:
    explicit fast_gaussian_generator(float mean, float variance, uint32_t initialSeed = randomGenerator())
    {
        mean_ = mean;
        gain_ = variance / std::sqrt(N / 3.0);
        seed(initialSeed);
    }

    void seed(uint32_t s)
    {
        seeds_[0] = s;
        for (unsigned i = 1; i < N; ++i) {
            s += s * 1664525u + 1013904223u;
            seeds_[i] = s;
        }
    }

    float operator()() noexcept
    {
        float sum = 0;
        for (unsigned i = 0; i < N; ++i) {
            uint32_t next = seeds_[i] * 1664525u + 1013904223u;
            seeds_[i] = next;
            sum += static_cast<int32_t>(next) * (1.0f / (1ll << 31));
        }
        return mean_ + gain_ * sum;
    }

private:
    std::array<uint32_t, N> seeds_ {{}};
    float mean_ { 0 };
    float gain_ { 0 };
};

}