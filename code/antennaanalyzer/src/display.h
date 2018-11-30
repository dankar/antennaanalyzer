#pragma once
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "pins.h"
#include "measure.h"

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

class display
{
public:
    display() : m_tft(TFT_CS, TFT_DC, TFT_RESET), m_touch(TOUCH_CS, /*digitalPinToInterrupt(TOUCH_IRQ)*/ 255) {};
    void init();
    void graph_init(float start_freq, float stop_freq);
    void graph_add_datapoint(float vswr, uint32_t freq, int steps = 1);

    void calibration_init(calibration_type type, uint16_t ticks);
    void calibration_tick();
    void new_load(calibration_type type);

    void draw_impedance(complex_t comp);

    void clear();
    void load();
    void save();
private:
    uint32_t x_from_index(uint32_t index);
    uint32_t y_from_value(float value);

    Adafruit_ILI9341 m_tft;
    XPT2046_Touchscreen m_touch;
    uint8_t m_current_index;
    float m_previous_vswr;
    uint8_t m_calibration_steps;
    uint8_t m_current_calibration_step;
};