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

#define BACKGROUND_COLOR        ILI9341_WHITE
#define DEFAULT_TEXT_COLOR      ILI9341_BLACK
#define GRAPH_LEGEND_COLOR      ILI9341_NAVY
#define GRAPH_BOUNDARIES_COLOR  ILI9341_BLACK
#define GRAPH_COLOR             ILI9341_BLACK
#define GRAPH_LINE_COLOR        ILI9341_DARKGREY
#define GRAPH_VSWR_LINE         ILI9341_DARKGREEN

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

    void wait_for_touch();
    void draw_centered_text(const String &str, uint16_t x, uint16_t y, uint8_t size, uint16_t color);
    void erase_centered_text(const String &str, uint16_t x, uint16_t y, uint8_t size);
    uint32_t x_from_index(uint32_t index);
    uint32_t y_from_value(float value);

    TS_Point get_touch_point();
    String get_load_type_string(calibration_type type);

    Adafruit_ILI9341 m_tft;
    XPT2046_Touchscreen m_touch;
    uint8_t m_current_index;
    float m_previous_vswr;
    uint8_t m_number_of_calibration_steps;
    uint8_t m_current_calibration_step;
};