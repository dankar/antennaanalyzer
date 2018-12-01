#pragma once
#include "display.h"
#include "measure.h"

class menu_system
{
public:
    enum states
    {
        HOME_SCREEN,
        VSWR_GRAPH,
        SMITH_CHART,
        IMPEDANCE,
        SETTINGS,
        CALIBRATE,
        BRIGHTNESS,
        BRIGHTNESS_DOWN,
        BRIGHTNESS_UP
    };

    menu_system(impedance_tester &imp, display &disp) : m_current_state(HOME_SCREEN), m_tester(imp), m_display(disp), m_brightness(255) {};
    void run();
private:
    void calibrate();
    void draw_vswr_graph();
    void home_screen();
    void settings();
    void brightness();
    void show_impedance();

    states m_current_state;
    impedance_tester &m_tester;
    display &m_display;

    uint8_t m_brightness;
};