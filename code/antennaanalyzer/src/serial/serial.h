#pragma once

#include "usbd_cdc_if.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t serial_print(const char *str, ...);

#ifdef __cplusplus
}
#endif