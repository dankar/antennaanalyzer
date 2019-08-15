#pragma once
#include <stdint.h>
#include "complex.h"

#define MAX_NUM_SAMPLES 2048

typedef struct 
{
    uint16_t vi;
    uint16_t vv;
} measurement_value_t;

typedef struct
{
    complex_t vv[MAX_NUM_SAMPLES];
    complex_t vi[MAX_NUM_SAMPLES];
} fft_t;

extern measurement_value_t g_ADCValues[];
extern fft_t g_fft;

int adc_init();
int adc_run(uint16_t num_samples);
int adc_fft(uint16_t num_samples);