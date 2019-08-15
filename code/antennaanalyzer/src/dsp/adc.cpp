#include "adc.h"
#include "main.h"
#include "serial.h"
#include <string.h>

measurement_value_t g_ADCValues[2048];

int adc_init()
{
    return 1;
}

int adc_run(uint16_t num_samples)
{
    memset(g_ADCValues, 0, sizeof(g_ADCValues));
    

    if( HAL_ADC_Start(&hadc2) != HAL_OK) 
    {
        serial_print("Failed to start ADC2\r\n");
        return 0;
    }
    if(HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)g_ADCValues, num_samples) != HAL_OK)
    {
        serial_print("Failed to start MultiModeADC\r\n");
        return 0;
    }

    HAL_Delay(26);

    HAL_ADCEx_MultiModeStop_DMA(&hadc1);

    return 1;
}

fft_t g_fft;

int adc_fft(uint16_t num_samples)
{
    arm_cfft_radix2_instance_f32 S;
    float32_t *input = new float32_t[num_samples*2];

    for(uint16_t i = 0; i < num_samples; i++)
    {
        input[i*2] = (float(g_ADCValues[i].vv) - 2048.0f) / 4096.0f;
        input[i*2+1] = 0;
    }

    arm_cfft_radix2_init_f32(&S, num_samples, 0, 1);
    arm_cfft_radix2_f32(&S, input);

    for(uint16_t i = 0; i < num_samples; i++)
    {
        g_fft.vv[i] = complex_t(input[i*2], input[i*2+1]);
    }

    for(uint16_t i = 0; i < num_samples; i++)
    {
        input[i*2] = (float(g_ADCValues[i].vi) - 2048.0f) / 4096.0f;
        input[i*2+1] = 0;
    }

    arm_cfft_radix2_init_f32(&S, num_samples, 0, 1);
    arm_cfft_radix2_f32(&S, input);

    for(uint16_t i = 0; i < num_samples; i++)
    {
        g_fft.vi[i] = complex_t(input[i*2], input[i*2+1]);
        
    }

    delete[] input;

    return 1;
}