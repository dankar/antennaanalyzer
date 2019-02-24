#pragma once

#include "gui.h"
#include "dialog.h"
#include "complex.h"

class gui_smith_chart : public gui_dialog
{
public:
    void init();
    void tick();
    void enter() {}
    void exit() {}
    bool m_chart_drawn;
private:
    void get_xy_from_gamma(int &x, int &y, const complex_t &gamma);
    void draw_resistance_circle(float res);
    void draw_impedance_circle(float gamma_y);
    void draw_negative_resistance_circle(float res);
    void draw_negative_impedance_circle(float gamma_y);
    void draw_smith_chart();
    lv_obj_t *m_back_button;
    lv_obj_t *m_impedance_label;
    int m_mid_x, m_mid_y, m_radius;
    
};

DEFINE_DIALOG(gui_smith_chart);