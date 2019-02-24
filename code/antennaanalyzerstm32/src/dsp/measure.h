#pragma once
#include "si5351.h"
#include "complex.h"

class display;
enum calibration_type
{
    OHM5 = 0,
    OHM50 = 1,
    OHM500 = 2,
    CALCULATE = 3
};

struct gamma_data_t
{
    complex_t gamma[3][128];
};

struct calibration_data_t
{
    complex_t coefficients[3][128];
    uint32_t crc;
};

struct result_t
{
    bool measured;
    bool impedance_calculated;
    bool vswr_calculated;
    bool gamma_calculated;

    complex_t vv;
    complex_t vi;
    complex_t gamma;
    complex_t impedance;
    float vswr;

    uint32_t frequency;
};

class impedance_tester
{
public:
    impedance_tester() : si5351(0xC0) {}
    void init();
    void make_measurement(uint32_t freq);
    complex_t get_impedance();
    complex_t get_gamma();
    float get_vswr();

    void run_calibration(calibration_type type);
    void save_calibration_data();
    void load_calibration_data();
    void print_calibration_data();
    uint32_t get_calibration_data_crc();

    void set_frequency(uint32_t freq);
private:
    
    void disable_frequency_output();
    void adjust_gamma();
    void get_coefficients(complex_t m1, complex_t m2, complex_t m3, complex_t &a, complex_t &b, complex_t &c);
    void calculate_coefficients(gamma_data_t &gamma);
    void get_coefficients_from_frequency(uint32_t f, complex_t &a, complex_t &b, complex_t &c);

    result_t m_current_result;
    calibration_data_t m_calibration_data;
    bool m_use_calibration_data;
    gamma_data_t m_gamma_data;
    Si5351 si5351;
};

extern impedance_tester g_impedance_tester;


