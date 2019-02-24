#pragma once

#include "stm32f4xx_hal.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t touched;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
} touch_t;

void xtp2046_init();
touch_t xtp2046_update();

#ifdef __cplusplus
}
#endif
