#include <Arduino.h>
#include "display.h"

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

    clear();
    m_tft.setRotation(3);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
}

void display::load()
{
    clear();
    m_tft.setTextSize(3);
    m_tft.setTextColor(ILI9341_WHITE);
    m_tft.setCursor(70, 50);
    m_tft.print("  Loading"); 
    m_tft.setCursor(70, 80);
    m_tft.print("calibration");
    m_tft.setCursor(70, 110);
    m_tft.print("  data...");
}

void display::save()
{
    clear();
    m_tft.setTextSize(3);
    m_tft.setTextColor(ILI9341_WHITE);
    m_tft.setCursor(70, 50);
    m_tft.print("  Saving"); 
    m_tft.setCursor(70, 80);
    m_tft.print("calibration");
    m_tft.setCursor(70, 110);
    m_tft.print("  data...");
}

void display::graph_init(float start_freq, float stop_freq)
{
    m_current_index = 0;
    clear();

    m_tft.setTextColor(ILI9341_YELLOW);
    m_tft.setTextSize(1);
    m_tft.setCursor(GRAPH_LEFT_MARGIN, 10);
    m_tft.print(start_freq);
    m_tft.print("MHz");

    m_tft.setCursor(DISPLAY_X - GRAPH_RIGHT_MARGIN - 46, 10);
    m_tft.print(stop_freq);
    m_tft.print("MHz");

    // Bounding box
    m_tft.drawLine(GRAPH_LEFT_MARGIN, GRAPH_TOP_MARGIN, DISPLAY_X - GRAPH_RIGHT_MARGIN, GRAPH_TOP_MARGIN, ILI9341_LIGHTGREY);
    m_tft.drawLine(DISPLAY_X - GRAPH_RIGHT_MARGIN, GRAPH_TOP_MARGIN, DISPLAY_X - GRAPH_RIGHT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, ILI9341_LIGHTGREY);
    m_tft.drawLine(DISPLAY_X - GRAPH_RIGHT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, GRAPH_LEFT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, ILI9341_LIGHTGREY);
    m_tft.drawLine(GRAPH_LEFT_MARGIN, DISPLAY_Y - GRAPH_BOTTOM_MARGIN, GRAPH_LEFT_MARGIN, GRAPH_TOP_MARGIN, ILI9341_LIGHTGREY);


    // VSWR text
    for(int i = 1; i <= 12; i+=2)
    {
        float val = float(i);
        uint8_t precision = 1;
        m_tft.setCursor(GRAPH_LEFT_MARGIN-20, y_from_value(val) - 3);

        if(val >= 10.0)
            precision = 0;
            
        m_tft.print(val, 1);
        m_tft.drawLine(GRAPH_LEFT_MARGIN+1, y_from_value(val), DISPLAY_X - GRAPH_RIGHT_MARGIN-1, y_from_value(val), ILI9341_DARKGREY);
        if(i == 1)
            i = 0;
    }
    
    // VSWR lines
    for(int i = 1; i < 10; i++)
    {
        m_tft.drawLine(x_from_index(256*i/10), GRAPH_TOP_MARGIN+1, x_from_index(256*i/10), DISPLAY_Y - GRAPH_BOTTOM_MARGIN - 1, ILI9341_DARKGREY);
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

void display::draw_impedance(complex_t comp)
{
    int16_t x1, x2;
    uint16_t w, h;

    m_tft.setTextSize(2);
    m_tft.setTextColor(ILI9341_BLUE);
    m_tft.getTextBounds("100.00 + 100.00j", 10, 50, &x1, &x2, &w, &h);
    m_tft.fillRect(x1, x2, w, h, ILI9341_BLACK);
    m_tft.setCursor(10, 50);
    m_tft.print(comp.real);
    m_tft.print(" + j");
    m_tft.print(comp.imag);
}

void display::new_load(calibration_type type)
{
    clear();
    m_tft.setTextSize(2);
    m_tft.setCursor(10, 10);
    m_tft.setTextColor(ILI9341_WHITE);
    m_tft.print("Please attach ");
    switch(type)
    {
        case OHM5:
        m_tft.print("5 Ohm");
        break;
        case OHM50:
        m_tft.print("50 Ohm");
        break;
        case OHM500:
        m_tft.print("500 Ohm");
        break;
    }
    m_tft.setCursor(10, 25);
    m_tft.print("and touch screen");

    int count = 0;
    for(;;)
    {
        if(m_touch.touched())
        {
            m_touch.getPoint();
            count++;
        }
        else
        {
            count = 0;
        }
        delay(10);

        if(count > 10)
            break;
    }
}

void display::graph_add_datapoint(float vswr, uint32_t freq, int steps)
{
    uint16_t value = y_from_value(vswr);

    if(m_current_index == 0)
    {
        m_tft.drawPixel(x_from_index(m_current_index), value, ILI9341_WHITE);
    }
    else
    {
        m_tft.drawLine(x_from_index(m_current_index-steps), y_from_value(m_previous_vswr), x_from_index(m_current_index), value, ILI9341_WHITE);
    }

    if(m_previous_vswr > 2.0 && vswr < 2.0)
    {
        m_tft.setCursor(x_from_index(m_current_index) - 20, 100 - 10);
        m_tft.print(float(freq) / 1000000, 3);
        m_tft.print("MHz");
        m_tft.drawLine(x_from_index(m_current_index), 100, x_from_index(m_current_index), DISPLAY_Y - GRAPH_BOTTOM_MARGIN - 1, ILI9341_GREEN);
    }

    if(m_current_index != 0 && m_previous_vswr < 2.0 && vswr > 2.0)
    {
        m_tft.setCursor(x_from_index(m_current_index) - 20, 110 - 10);
        m_tft.print(float(freq) / 1000000, 3);
        m_tft.print("MHz");
        m_tft.drawLine(x_from_index(m_current_index-steps), 110, x_from_index(m_current_index-steps), DISPLAY_Y - GRAPH_BOTTOM_MARGIN - 1, ILI9341_GREEN);
    }
    m_current_index += steps;
    m_previous_vswr = vswr;
}

void display::calibration_init(calibration_type type, uint16_t ticks)
{
    clear();
    m_tft.setCursor(100, 20);
    m_tft.setTextSize(1);
    m_tft.setTextColor(ILI9341_WHITE);
    m_tft.print("Calibrating for ");

    switch(type)
    {
        case OHM5:
        m_tft.print("5 Ohm");
        break;
        case OHM50:
        m_tft.print("50 Ohm");
        break;
        case OHM500:
        m_tft.print("500 Ohm");
        break;
    }

    m_calibration_steps = ticks;
    m_current_calibration_step = 0;

    calibration_tick();
}
void display::clear()
{
    m_tft.fillScreen(ILI9341_BLACK);
}
void display::calibration_tick()
{
    m_tft.fillRect(70, 100, 200, 50, ILI9341_BLACK);
    m_tft.setCursor(70, 100);
    m_tft.setTextSize(5);
    m_tft.setTextColor(ILI9341_YELLOW);
    m_tft.print(float(m_current_calibration_step) * 100.0f / m_calibration_steps, 2);
    m_tft.print("%");

   m_current_calibration_step++;
}