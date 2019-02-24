#include "serial.h"
#include <stdarg.h>

uint8_t serial_print(const char *str, ...)
{
    char buffer[256];
    size_t len;
    va_list myargs;
    va_start(myargs, str);
    len = vsprintf(buffer, str, myargs);
    va_end(myargs);
    uint8_t result;

    /*do
    {
        result = CDC_Transmit_FS((uint8_t*)buffer, len);
    }while(result == USBD_BUSY);*/

    return result;
}