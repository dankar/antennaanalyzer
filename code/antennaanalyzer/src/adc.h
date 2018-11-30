#pragma once

#include "Wire.h"
#include "si5351.h"
#include "Printable.h"


class CleanADC
{
public:
    static uint32_t get_measurement(uint8_t pin, uint32_t samples);

};