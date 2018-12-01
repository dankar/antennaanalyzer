#include <EEPROM.h>

#include "measure.h"
#include "pins.h"
#include "adc.h"
#include "display.h"

uint32_t samples_per_measurement = 512;
uint32_t measurement_delay = 20;

void impedance_tester::init()
{
    pinMode(CAPACITIVE_PIN, INPUT);
    pinMode(INDUCTIVE_PIN, INPUT);

    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 27000000, 0);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);

    m_current_result = {0};
    m_use_calibration_data = true;
}

void impedance_tester::set_measurement_parameters(uint32_t delay, uint32_t number_of_measurements)
{
    samples_per_measurement = number_of_measurements;
    measurement_delay = delay;
}

void impedance_tester::set_frequency(uint32_t freq)
{
    si5351.set_freq(100ULL * freq, SI5351_CLK0);
    si5351.set_freq(100ULL * (freq + 10045), SI5351_CLK1);
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.output_enable(SI5351_CLK1, 1);
}

void impedance_tester::disable_frequency_output()
{
    si5351.output_enable(SI5351_CLK0, 0);
    si5351.output_enable(SI5351_CLK1, 0);
}

void sort(uint32_t *buffer, int num)
{
    bool swapped;
    do
    {
        swapped = false;
        for(int i = 1; i < num-1; i++)
        {
            if(buffer[i-1] > buffer[i])
            {
                uint32_t temp = buffer[i-1];
                buffer[i-1] = buffer[i];
                buffer[i] = temp;
                swapped = true;
            }
        }
    }while(swapped);
}

void impedance_tester::make_measurement(uint32_t freq)
{
    m_current_result = {0};
    m_current_result.frequency = freq;
    set_frequency(freq);
    delay(measurement_delay);
    float vv = CleanADC::get_measurement(VV_PIN, samples_per_measurement);
    float vi = CleanADC::get_measurement(VI_PIN, samples_per_measurement);
    float phase = CleanADC::get_measurement(PHASE_PIN, samples_per_measurement);
    disable_frequency_output();

    m_current_result.raw_phase = phase;

    phase = phase * 3.1415f / 1024.0f;

    if(digitalRead(CAPACITIVE_PIN))
    {
        m_current_result.capacitive = true;
    }
    else
    {
        m_current_result.capacitive = false;
    }

    m_current_result.vv = vv;
    m_current_result.vi = vi;
    m_current_result.phase = phase;
}

complex_t impedance_tester::get_impedance()
{
    if(m_current_result.impedance_calculated)
        return m_current_result.impedance;
    
    // Calculate unadjusted impedance
    complex_t Vx(m_current_result.vi * cos(m_current_result.phase), m_current_result.vi * sin(m_current_result.phase));
    complex_t Vin(m_current_result.vv*2, 0);
    m_current_result.impedance = (complex_t(50.0f, 0.0f)*Vx)/(Vin-Vx);
    if(m_current_result.capacitive)
    {
        m_current_result.impedance.imag = -m_current_result.impedance.imag;
    }
    
    // Calculate gamma and adjust with calibration data
    // TODO: Maybe calculate gamma directly from measured values instead of going to impedance -> gamma -> adjust -> impedance?
    complex_t Z0(50.0f, 0.0f);
    complex_t ZL = m_current_result.impedance;
    m_current_result.gamma = (ZL-Z0)/(ZL+Z0);

    if(m_use_calibration_data)
        adjust_gamma();

    m_current_result.gamma_calculated = true;

    // Calculate new impedance from adjusted gamma
    m_current_result.impedance = complex_t(50, 0) * (complex_t(1,0)+m_current_result.gamma)/(complex_t(1,0)-m_current_result.gamma);
    m_current_result.impedance_calculated = true;

    
    return m_current_result.impedance;
}

complex_t impedance_tester::get_gamma()
{
    if(!m_current_result.gamma_calculated)
        get_impedance();

    return m_current_result.gamma;
}

float impedance_tester::get_vswr()
{
    if(m_current_result.vswr_calculated)
        return m_current_result.vswr;

    complex_t gamma = get_gamma();
    float gamma_mag = gamma.magnitude();
    if(gamma_mag > 0.9999f)
        gamma_mag = 0.9999f;

    m_current_result.vswr = (1 + gamma_mag)/(1 - gamma_mag);
    m_current_result.vswr_calculated = true;

    return m_current_result.vswr;
}

void impedance_tester::adjust_gamma()
{
    complex_t a, b, c;
    get_coefficients_from_frequency(m_current_result.frequency, a, b, c);

    // Calculate error factors from calibration coefficients and adjust gamma
    complex_t e00 = b;
    complex_t e11 = -c;
    complex_t e10e01 = a + e00*e11;

    m_current_result.gamma = (m_current_result.gamma - e00) / (e11*(m_current_result.gamma - e00) + (e10e01));
}

uint32_t frequency_to_calibration_index(uint32_t f)
{
    if(f < 100000)
    {
        f = 100000;
    }
    f = f - 100000;

    return f / (29972000/127);
}

uint32_t calibration_index_to_frequency(uint32_t i)
{
    return 100000 + i * (float(29972000)/127);
}

void impedance_tester::get_coefficients_from_frequency(uint32_t f, complex_t &a, complex_t &b, complex_t &c)
{
    uint32_t index = frequency_to_calibration_index(f);

    // TODO: add interpolation for frequencies between calibration points (linear?)
    a = m_calibration_data.coefficients[0][index];
    b = m_calibration_data.coefficients[1][index];
    c = m_calibration_data.coefficients[2][index];
}

void impedance_tester::print_calibration_data()
{
    for(int i = 0; i < 128; i++)
    {
        Serial.print("Current calibration: frequency = ");
        Serial.print(calibration_index_to_frequency(i));
        Serial.print(", a: ");
        m_calibration_data.coefficients[0][i].print();
        Serial.print(", b: ");
        m_calibration_data.coefficients[1][i].print();
        Serial.print(", c: ");
        m_calibration_data.coefficients[2][i].print();
        Serial.println();
    }
}

void impedance_tester::load_calibration_data()
{
    uint32_t eeprom_address = 0;

    EEPROM.get(eeprom_address, m_calibration_data);
}

void impedance_tester::save_calibration_data()
{
    uint32_t eeprom_address = 0;

    EEPROM.put(eeprom_address, m_calibration_data);
}

void impedance_tester::get_coefficients(complex_t m1, complex_t m2, complex_t m3, complex_t &a, complex_t &b, complex_t &c)
{
    complex_t a1(-0.81818182, 0);   // Expected reflection coefficient from 5 Ohm load
    complex_t a2(0, 0);             // 50 Ohm load
    complex_t a3(0.81818182, 0);    // 500 Ohm load

    a = -(-m1 * m2 * a1 + m1 * m3 * a1 + m1 * m2 * a2 - m2 * m3 * a2 - m1 * m3 * a3 + m2 * m3 * a3)/(m1 * a1 * a2 - m2 * a1 * a2 + m2 * a3 * a2 - m3 * a3 * a2 - m1 * a1 * a3 + m3 * a1 * a3);
    b = -(-m1 * m3 * a1 * a2 + m2 * m3 * a1 * a2 - m1 * m2 * a3 * a2 + m1 * m3 * a3 * a2 + m1 * m2 * a1 * a3 - m2 * m3 * a1 * a3)/(m1 * a1 * a2 - m2 * a1 * a2 + m2 * a3 * a2 - m3 * a3 * a2 - m1 * a1 * a3 + m3 * a1 * a3);
    c = -(-m2 * a1 + m3 * a1 + m1 * a2 - m3 * a2 - m1 * a3 + m2 * a3)/(m1 * a1 * a2 - m2 * a1 * a2 + m2 * a3 * a2 - m3 * a3 * a2 - m1 * a1 * a3 + m3 * a1 * a3);
}

void impedance_tester::calculate_coefficients(gamma_data_t &gamma)
{
    for(int i = 0; i < 128; i++)
    {
        complex_t a, b, c;
        get_coefficients(gamma.gamma[OHM5][i], gamma.gamma[OHM50][i], gamma.gamma[OHM500][i], a, b, c);
        m_calibration_data.coefficients[0][i] = a;
        m_calibration_data.coefficients[1][i] = b;
        m_calibration_data.coefficients[2][i] = c;
    }
}

void impedance_tester::run_calibration(display &disp)
{
    gamma_data_t gamma;
    m_use_calibration_data = false;

    uint32_t old_samples = samples_per_measurement;
    uint32_t old_delay = measurement_delay;

    samples_per_measurement = 4000;
    measurement_delay = 200;

    calibration_type calibration_loads[] = {OHM5, OHM50, OHM500};

    // Save reflection coefficients for different frequencies and three different loads (loads that are close to open short and load)
    for(uint8_t i = 0; i < sizeof(calibration_loads)/sizeof(calibration_type); i++)
    {
        disp.show_new_load_prompt(calibration_loads[i]);
        disp.show_calibration_screen(calibration_loads[i], 128);

        for(uint32_t j = 0; j < 128; j++)
        {
            make_measurement(calibration_index_to_frequency(j));
            gamma.gamma[calibration_loads[i]][j] = get_gamma();
            gamma.gamma[calibration_loads[i]][j].print();
            Serial.println();
            disp.calibration_tick();
        }
    }

    // Calculate calibration coefficients based on the reflection coefficients we got and the ones we expect from the predefined loads
    calculate_coefficients(gamma);

    m_use_calibration_data = true;

    samples_per_measurement = old_samples;
    measurement_delay = old_delay;
}
