#include "adc.h"

volatile uint32_t adcReading;
volatile bool adcDone;

ISR (ADC_vect)
{
    uint8_t low, high;
    low = ADCL;
    high = ADCH;
    adcReading = (high << 8) | low;
    adcDone = true;  
}

uint32_t CleanADC::get_measurement(uint8_t pin, uint32_t samples)
{
    uint32_t result = 0;

    ADMUX = bit (REFS1) | (pin & 0x07);
    sleepMode(SLEEP_ADC);
    
    for(uint32_t i = 0; i < samples; i++)
    {
        adcDone = false;
        ADCSRA |= /*bit (ADSC) |*/ bit (ADIE);

        while(!adcDone)
            sleep();

        result += adcReading;
    }

    return result / samples;
}