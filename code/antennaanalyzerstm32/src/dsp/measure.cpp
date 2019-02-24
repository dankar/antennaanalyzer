#include "measure.h"
#include "si5351.h"
#include "main.h"
#include "adc.h"
#include "serial.h"
#include "dialogs/calibration.h"

impedance_tester g_impedance_tester;

void impedance_tester::init()
{
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 27000000, 0);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);

    adc_init();
    m_current_result = {0};
    load_calibration_data();
    m_use_calibration_data = true;
}

void impedance_tester::set_frequency(uint32_t freq)
{
    si5351.set_freq(100ULL * freq, SI5351_CLK0);
    si5351.set_freq(100ULL * (freq + 10083), SI5351_CLK1);
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.output_enable(SI5351_CLK1, 1);
}

void impedance_tester::disable_frequency_output()
{
    si5351.output_enable(SI5351_CLK0, 0);
    si5351.output_enable(SI5351_CLK1, 0);
}

void impedance_tester::make_measurement(uint32_t freq)
{
    m_current_result = {0};
    m_current_result.frequency = freq;
    set_frequency(freq);
    adc_run(2048);
    disable_frequency_output();
    adc_fft(2048);

    m_current_result.vv = g_fft.vv[236];
    m_current_result.vi = g_fft.vi[236];

    m_current_result.vi = m_current_result.vi  / (m_current_result.vv/m_current_result.vv.magnitude());
    m_current_result.vv = m_current_result.vv  / (m_current_result.vv/m_current_result.vv.magnitude());

    m_current_result.vi = m_current_result.vi.conjugate();

}

complex_t impedance_tester::get_impedance()
{
    if(m_current_result.impedance_calculated)
        return m_current_result.impedance;
    
    // Calculate unadjusted impedance
    complex_t Vx(m_current_result.vi);
    complex_t Vin(m_current_result.vv*2);
    m_current_result.impedance = (complex_t(50.0f, 0.0f)*Vx)/(Vin-Vx);

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

    uint32_t index_frequency = calibration_index_to_frequency(index);
    uint32_t next_index_frequency = calibration_index_to_frequency(index+1);

    uint32_t frequency_diff = f - index_frequency;

    float diff = float(frequency_diff) / (next_index_frequency - index_frequency);

    // TODO: add interpolation for frequencies between calibration points (linear?)
    a = complex_lerp(m_calibration_data.coefficients[0][index], m_calibration_data.coefficients[0][index+1], diff);
    b = complex_lerp(m_calibration_data.coefficients[1][index], m_calibration_data.coefficients[1][index+1], diff);
    c = complex_lerp(m_calibration_data.coefficients[2][index], m_calibration_data.coefficients[2][index+1], diff);
}

void impedance_tester::print_calibration_data()
{
    serial_print("print_calibration_data: Not implemented\r\n");
    for(int i = 0; i < 128; i++)
    {
        /*Serial.print("Current calibration: frequency = ");
        Serial.print(calibration_index_to_frequency(i));
        Serial.print(", a: ");
        Serial.print(m_calibration_data.coefficients[0][i].to_string());
        Serial.print(", b: ");
        Serial.print(m_calibration_data.coefficients[1][i].to_string());
        Serial.print(", c: ");
        Serial.print(m_calibration_data.coefficients[2][i].to_string());
        Serial.println();*/
        
    }
}

void impedance_tester::load_calibration_data()
{
    memcpy(&m_calibration_data, (void*)0x08040000, sizeof(m_calibration_data));
    if(get_calibration_data_crc() != m_calibration_data.crc)
    {
        serial_print("Incorrect CRC! %d - %d\r\n", get_calibration_data_crc(), m_calibration_data.crc);
    }
}

uint32_t impedance_tester::get_calibration_data_crc()
{
    if(sizeof(m_calibration_data.coefficients) % 4 != 0)
    {
        serial_print("Coeffs are not %%4...\r\n");
    }
    return HAL_CRC_Calculate(&hcrc, (uint32_t*)&(m_calibration_data.coefficients), sizeof(m_calibration_data.coefficients)/4-1);
}

int flash_erase_sec(int sec_no)
{
	FLASH_EraseInitTypeDef er;
	er.TypeErase = FLASH_TYPEERASE_SECTORS;
    er.Sector = sec_no;
	er.NbSectors = 1;
	er.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	uint32_t fault_sec = 0;
	HAL_FLASH_Unlock();
	HAL_StatusTypeDef res = HAL_FLASHEx_Erase(&er, &fault_sec);
	HAL_FLASH_Lock();
	return res == HAL_OK ? 1 : 0;
}

void impedance_tester::save_calibration_data()
{
    if(!flash_erase_sec(6))
    {
        serial_print("Failed erase\r\n");
    }

    HAL_StatusTypeDef res = HAL_OK;
    HAL_FLASH_Unlock();
    uint32_t i;
    for(i = 0; i < sizeof(m_calibration_data); i++)
    {
        const char *data = (const char*)&m_calibration_data;
        data+=i;
        res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, 0x08040000 + i, *data);
        if (res != HAL_OK)
			break;
    }
    HAL_FLASH_Lock();
    
    if(res != HAL_OK)
    {
        serial_print("Failed to write byte %d\r\n", i);
    }
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

void impedance_tester::run_calibration(calibration_type type)
{
    m_use_calibration_data = false;

    if(type == OHM5 || type == OHM50 || type == OHM500)
    {
        for(uint32_t j = 0; j < 128; j++)
        {
            make_measurement(calibration_index_to_frequency(j));
            m_gamma_data.gamma[type][j] = get_gamma();
            //disp.print_calibration_gamma(gamma.gamma[calibration_loads[i]][j]);
            //disp.calibration_tick();
            g_gui_calibration.report_percentage(j*100/128);
        }
    }

    if(type == CALCULATE)
    {
        // Calculate calibration coefficients based on the reflection coefficients we got and the ones we expect from the predefined loads
        calculate_coefficients(m_gamma_data);
        m_calibration_data.crc = get_calibration_data_crc();
        save_calibration_data();
    }

    m_use_calibration_data = true;
}
