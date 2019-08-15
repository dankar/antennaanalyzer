#pragma once

#include "main.h"
#include <arm_math.h>
#include "serial.h"

class complex_t
{
public:
    complex_t() : real(0), imag(0) {}
    complex_t(float _real, float _imag) : real(_real), imag(_imag) {}
    complex_t operator+(const complex_t &right)
    {
        return complex_t(real + right.real, imag + right.imag);
    }
    complex_t operator*(const complex_t &right)
    {
        return complex_t(real * right.real - imag * right.imag, real * right.imag + imag * right.real);
    }
    complex_t operator*(float v)
    {
        return complex_t(real * v, imag * v);
    }
    complex_t operator-(const complex_t &right)
    {
        return complex_t(real - right.real, imag - right.imag);
    }
    complex_t operator-()
    {
        return complex_t(-real, -imag);
    }
    complex_t operator/(const complex_t &right)
    {
        return complex_t((real * right.real + imag * right.imag)/(right.real*right.real+right.imag*right.imag), (imag * right.real - real * right.imag) / (right.real*right.real+right.imag*right.imag));
    }
    complex_t operator/(float v)
    {
        return complex_t(real / v, imag/v);
    }
    float magnitude()
    {
        return sqrtf(real*real + imag*imag);
    }

    complex_t conjugate()
    {
        return complex_t(real, imag * -1.0f);
    }

    void print()
    {
        serial_print("%d.%d + j%d.%d\r\n", int(real), int((real-int(real))*10), int(imag), int((imag-int(imag))*10));
    }

    float real;
    float imag;
};

complex_t operator*(float v, complex_t x);
complex_t complex_lerp(complex_t a, complex_t b, float t);
float lerp(float a, float b, float t);