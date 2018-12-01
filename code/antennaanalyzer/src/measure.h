#pragma once
#include "si5351.h"
#include "Printable.h"
//#include "complex.h"

class display;

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

    void print()
    {
        Serial.print(real);
        if(imag >= 0.0f)
            Serial.print(" + j");
        else
            Serial.print(" - j");
        Serial.print(fabs(imag));
    }
    float real;
    float imag;
};


enum calibration_type
{
    OHM5 = 0,
    OHM50 = 1,
    OHM500 = 2
};

struct gamma_data_t
{
    complex_t gamma[3][128];
};

struct calibration_data_t
{
    complex_t coefficients[3][128];
};

struct result_t
{
    bool measured;
    bool impedance_calculated;
    bool vswr_calculated;
    bool gamma_calculated;

    float vv;
    float vi;
    float phase;
    uint16_t raw_phase;
    complex_t gamma;
    complex_t impedance;
    float vswr;
    bool capacitive;

    uint32_t frequency;
};

class impedance_tester
{
public:
    void init();
    void make_measurement(uint32_t freq);
    complex_t get_impedance();
    complex_t get_gamma();
    float get_vswr();

    void run_calibration(display &disp);
    void save_calibration_data();
    void load_calibration_data();
    void print_calibration_data();
private:
    void set_frequency(uint32_t freq);
    void disable_frequency_output();
    void adjust_gamma();
    void get_coefficients(complex_t m1, complex_t m2, complex_t m3, complex_t &a, complex_t &b, complex_t &c);
    void calculate_coefficients(gamma_data_t &gamma);
    void get_coefficients_from_frequency(uint32_t f, complex_t &a, complex_t &b, complex_t &c);

    result_t m_current_result;
    calibration_data_t m_calibration_data;
    bool m_use_calibration_data;
    Si5351 si5351;
};


