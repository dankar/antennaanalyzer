#include <Arduino.h>
#include "display.h"
#include <Fonts/FreeSans24pt7b.h>

#define DISPLAY_X 320
#define DISPLAY_Y 240

#define GRAPH_LEFT_MARGIN 25
#define GRAPH_RIGHT_MARGIN 10
#define GRAPH_TOP_MARGIN 20
#define GRAPH_BOTTOM_MARGIN 10

void display::init()
{
    m_tft.begin(3000000);
    if(!m_touch.begin())
        Serial.println("Failed to init touch");
    
    while(m_touch.touched())
    {
        m_touch.getPoint();
    }
    m_touch.setRotation(1);

    clear_screen();
    m_tft.setRotation(3);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
}

void display::draw_centered_text(const String &str, uint16_t x, uint16_t y, uint8_t size, uint16_t color)
{
    int16_t bx, by;
    uint16_t bw, bh;
    uint16_t size_x, size_y;

    m_tft.setTextSize(size);
    m_tft.setTextColor(color);

    m_tft.getTextBounds(str, 0, 0, &bx, &by, &bw, &bh);

    size_x = bw-bx;
    size_y = bh-by;

    m_tft.setCursor(x - size_x/2, y - size_y/2);
    m_tft.print(str);
}

void display::erase_centered_text(const String &str, uint16_t x, uint16_t y, uint8_t size)
{
    int16_t bx, by;
    uint16_t bw, bh;
    uint16_t size_x, size_y;

    m_tft.setTextSize(size);

    m_tft.getTextBounds(str, 0, 0, &bx, &by, &bw, &bh);

    size_x = bw-bx;
    size_y = bh-by;

    m_tft.fillRect(x - size_x/2, y - size_y/2, bw, bh, BACKGROUND_COLOR);
}

void display::show_load_screen()
{
    clear_screen();
    draw_centered_text("Loading", m_tft.width()/2, m_tft.height()/2 - 10, 1, DEFAULT_TEXT_COLOR);
    draw_centered_text("calibration", m_tft.width()/2, m_tft.height()/2, 1, DEFAULT_TEXT_COLOR);
    draw_centered_text("data...", m_tft.width()/2, m_tft.height()/2 + 10, 1, DEFAULT_TEXT_COLOR);
}

void display::show_save_screen()
{
    clear_screen();
    draw_centered_text("Saving", m_tft.width()/2, m_tft.height()/2 - 10, 1, DEFAULT_TEXT_COLOR);
    draw_centered_text("calibration", m_tft.width()/2, m_tft.height()/2, 1, DEFAULT_TEXT_COLOR);
    draw_centered_text("data...", m_tft.width()/2, m_tft.height()/2 + 10, 1, DEFAULT_TEXT_COLOR);
}

void display::show_graph_screen(float start_freq, float stop_freq)
{
    m_current_index = 0;
    clear_screen();

    m_tft.setTextColor(GRAPH_LEGEND_COLOR);
    m_tft.setTextSize(1);
    m_tft.setCursor(GRAPH_LEFT_MARGIN, 10);
    m_tft.print(start_freq);
    m_tft.print("MHz");

    m_tft.setCursor(DISPLAY_X - GRAPH_RIGHT_MARGIN - 46, 10);
    m_tft.print(stop_freq);
    m_tft.print("MHz");

    // Bounding box
    m_tft.drawLine(GRAPH_LEFT_MARGIN, GRAPH_TOP_MARGIN, DISPLAY_X - GRAPH_RIGHT_MARGIN, GRAPH_TOP_MARGIN, GRAPH_BOUNDARIES_COLOR);
    m_tft.drawLine(DISPLAY_X - GRAPH_RIGHT_MARGIN, GRAPH_TOP_MARGIN, DISPLAY_X - GRAPH_RIGHT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, GRAPH_BOUNDARIES_COLOR);
    m_tft.drawLine(DISPLAY_X - GRAPH_RIGHT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, GRAPH_LEFT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, GRAPH_BOUNDARIES_COLOR);
    m_tft.drawLine(GRAPH_LEFT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, GRAPH_LEFT_MARGIN, GRAPH_TOP_MARGIN, GRAPH_BOUNDARIES_COLOR);

    // VSWR text
    for(int i = 1; i <= 12; i+=2)
    {
        float val = float(i);
        uint8_t precision = 1;
        m_tft.setCursor(GRAPH_LEFT_MARGIN-20, y_from_value(val) - 3);

        if(val >= 10.0)
            precision = 0;
            
        m_tft.print(val, precision);
        m_tft.drawLine(GRAPH_LEFT_MARGIN+1, y_from_value(val), DISPLAY_X - GRAPH_RIGHT_MARGIN-1, y_from_value(val), GRAPH_LINE_COLOR);
        if(i == 1)
            i = 0;
    }
    
    // VSWR lines
    for(int i = 1; i < 10; i++)
    {
        m_tft.drawLine(x_from_index(256*i/10), GRAPH_TOP_MARGIN+1, x_from_index(256*i/10), DISPLAY_Y - GRAPH_BOTTOM_MARGIN - 1, GRAPH_LINE_COLOR);
    }
}

uint32_t display::x_from_index(uint32_t index)
{
    uint32_t start = GRAPH_LEFT_MARGIN;
    uint32_t size = DISPLAY_X - GRAPH_LEFT_MARGIN - GRAPH_RIGHT_MARGIN;

    return start + size * index / 256;
}

uint32_t display::y_from_value(float value)
{
    uint16_t result_value = log10(value) * 13.02 * 14.0;
    if(result_value > (DISPLAY_Y-GRAPH_TOP_MARGIN - GRAPH_BOTTOM_MARGIN))
        result_value = DISPLAY_Y-GRAPH_TOP_MARGIN - GRAPH_BOTTOM_MARGIN;
    
    return DISPLAY_Y - GRAPH_BOTTOM_MARGIN - result_value;
}

void display::show_impedance_viewer(complex_t comp)
{
    int16_t x1, x2;
    uint16_t w, h;

    m_tft.setTextSize(2);
    m_tft.setTextColor(GRAPH_LEGEND_COLOR);
    m_tft.getTextBounds("100.00 + 100.00j", 10, 50, &x1, &x2, &w, &h);
    m_tft.fillRect(x1, x2, w, h, BACKGROUND_COLOR);
    m_tft.setCursor(10, 50);

    m_tft.print(comp.real);
    if(comp.imag >= 0.0f)
        m_tft.print(" + j");
    else
        m_tft.print(" - j");
    m_tft.print(fabs(comp.imag));
}

String display::get_load_type_string(calibration_type type)
{
    switch(type)
    {
        case OHM5:
        return String("5 Ohm");
        case OHM50:
        return String("50 Ohm");
        case OHM500:
        return String("500 Ohm");
    }

    return String("Unknown");
}

void display::wait_for_touch()
{
    int count = 0;
    for(;;)
    {
        if(m_touch.touched())
        {
            get_touch_point();
            count++;
        }
        else
        {
            count = 0;
        }
        delay(10);

        if(count > 3)
            break;
    }
}

void display::show_new_load_prompt(calibration_type type)
{
    clear_screen();

    draw_centered_text(String("Attach ") + get_load_type_string(type) + " load and touch screen", m_tft.width()/2, m_tft.height()/2, 1, DEFAULT_TEXT_COLOR);

    wait_for_touch();
}

TS_Point display::get_touch_point()
{
    TS_Point p = m_touch.getPoint();
    int temp = p.x;
    p.x = p.y;
    p.y = temp;

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, m_tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, m_tft.height());

    p.y = m_tft.height() - p.y;

    return p;
}

void display::graph_add_datapoint(float vswr, uint32_t freq, int steps)
{
    uint16_t value = y_from_value(vswr);

    if(m_current_index == 0)
    {
        m_tft.drawPixel(x_from_index(m_current_index), value, GRAPH_COLOR);
    }
    else
    {
        m_tft.drawLine(x_from_index(m_current_index-steps), y_from_value(m_previous_vswr), x_from_index(m_current_index), value, GRAPH_COLOR);
    }

    if(m_previous_vswr > 2.0 && vswr < 2.0)
    {
        m_tft.setCursor(x_from_index(m_current_index) - 20, 100 - 10);
        m_tft.print(float(freq) / 1000000, 3);
        m_tft.print("MHz");
        m_tft.drawLine(x_from_index(m_current_index), 100, x_from_index(m_current_index), DISPLAY_Y - GRAPH_BOTTOM_MARGIN - 1, GRAPH_VSWR_LINE);
    }

    if(m_current_index != 0 && m_previous_vswr < 2.0 && vswr > 2.0)
    {
        m_tft.setCursor(x_from_index(m_current_index) - 20, 110 - 10);
        m_tft.print(float(freq) / 1000000, 3);
        m_tft.print("MHz");
        m_tft.drawLine(x_from_index(m_current_index-steps), 110, x_from_index(m_current_index-steps), DISPLAY_Y - GRAPH_BOTTOM_MARGIN - 1, GRAPH_VSWR_LINE);
    }
    m_current_index += steps;
    m_previous_vswr = vswr;
}

void display::show_calibration_screen(calibration_type type, uint16_t ticks)
{
    clear_screen();

    draw_centered_text(String("Calibrating for ") + get_load_type_string(type), m_tft.width()/2, m_tft.height()/2 - 35, 1, DEFAULT_TEXT_COLOR);

    m_number_of_calibration_steps = ticks;
    m_current_calibration_step = 0;

    calibration_tick();
}
void display::clear_screen()
{
    m_tft.fillScreen(BACKGROUND_COLOR);
}
void display::calibration_tick()
{
    if(m_current_calibration_step % 5 == 0)
    {
        m_tft.setFont(&FreeSans24pt7b);
        erase_centered_text("100.0%", m_tft.width()/2, m_tft.height()/2, 1);
        draw_centered_text(String(float(m_current_calibration_step) * 100.0f / m_number_of_calibration_steps, 2) + "%", m_tft.width()/2, m_tft.height()/2, 1, GRAPH_LEGEND_COLOR);
        m_tft.setFont();
    }
    m_current_calibration_step++;
}