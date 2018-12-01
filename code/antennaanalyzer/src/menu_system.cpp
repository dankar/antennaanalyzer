#include "menu_system.h"

void menu_system::calibrate()
{
    m_tester.run_calibration(m_display);
    m_display.show_save_screen();
    m_tester.save_calibration_data();
    m_current_state = SETTINGS;
}

void menu_system::draw_vswr_graph()
{
    float start = 0.1;
    float stop = 30;
    uint32_t steps = 256;

    m_display.show_graph_screen(start, stop);
    m_tester.set_measurement_parameters(20, 512);

    for(uint32_t f = start*1000000; f < stop*1000000; f+= (stop*1000000-start*1000000)/steps * 4)
    {
        m_tester.make_measurement(f);
        float VSWR = m_tester.get_vswr();
        m_display.graph_add_datapoint(VSWR, f, 4);
    }

    m_display.wait_for_touch();
    m_current_state = HOME_SCREEN;
}

void menu_system::home_screen()
{
    const char *menu_strings[] = {"VSWR graph", "Smith chart", "Settings"};
    uint8_t menu[] = { VSWR_GRAPH, SMITH_CHART, SETTINGS };
    m_current_state = (states)m_display.draw_menu(menu_strings, menu, 3);
}

void menu_system::settings()
{
    const char *menu_strings[] = {"Calibrate", "Brightness", "Back"};
    uint8_t menu[] = { CALIBRATE, BRIGHTNESS, HOME_SCREEN };
    m_current_state = (states)m_display.draw_menu(menu_strings, menu, 3);
}

void menu_system::brightness()
{
    const uint8_t step = 20;
    const char *menu_strings[] = {"^", "v", "Back"};
    uint8_t menu[] = { BRIGHTNESS_UP, BRIGHTNESS_DOWN, SETTINGS };

    states result = (states)m_display.draw_menu(menu_strings, menu, 3);
    for(;;)
    {
        if(result == SETTINGS)
        {
            m_current_state = SETTINGS;
            return;
        }
        else if(result == BRIGHTNESS_UP)
        {
            if(m_brightness < (255 - step))
            {
                m_brightness += step;
            }
        }
        else if(result == BRIGHTNESS_DOWN)
        {
            if(m_brightness >= step)
            {
                m_brightness -= step;
            }
        }
        analogWrite(TFT_LED, m_brightness);
        result = menu[m_display.get_button_press(3)];
    }
}

void menu_system::run()
{
    switch(m_current_state)
    {
    case HOME_SCREEN:
        home_screen();
        break;
    case VSWR_GRAPH:
        draw_vswr_graph();
        break;
    case SMITH_CHART:
        m_current_state = HOME_SCREEN;
        break;
    case SETTINGS:
        settings();
        break;
    case CALIBRATE:
        calibrate();
        break;
    case BRIGHTNESS:
        brightness();
        break;

    }
}