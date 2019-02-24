#include "complex.h"

complex_t operator*(float v, complex_t x)
{
    return x * v;
}

complex_t complex_lerp(complex_t a, complex_t b, float t)
{
    return (1 - t) * a + t * b;
}