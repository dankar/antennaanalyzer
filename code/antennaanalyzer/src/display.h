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

    void show_graph_screen(float start_freq, float stop_freq);
    void graph_add_datapoint(float vswr, uint32_t freq, int steps = 1);

    void show_calibration_screen(calibration_type type, uint16_t ticks);
    void calibration_tick();
    void show_new_load_prompt(calibration_type type);

    void show_impedance_viewer(complex_t comp);

    void clear_screen();
    void show_load_screen();
    void show_save_screen();
private:
    uint32_t x_from_index(uint32_t index);
    uint32_t y_from_value(float value);

    TS_Point get_touch_point();
    void print_load_type(calibration_type type);

    Adafruit_ILI9341 m_tft;
    XPT2046_Touchscreen m_touch;
    uint8_t m_current_index;
    float m_previous_vswr;
    uint8_t m_number_of_calibration_steps;
    uint8_t m_current_calibration_step;
};