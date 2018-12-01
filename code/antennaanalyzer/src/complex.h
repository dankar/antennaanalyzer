#pragma once

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
    float magnitude()
    {
        return sqrtf(real*real + imag*imag);
    }

    String to_string()
    {
        return String(real) + (imag >= 0.0f ? String(" + j") : String(" - j")) + String(fabs(imag)); ;
    }
    float real;
    float imag;
};