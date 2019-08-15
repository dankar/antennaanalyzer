#pragma once

#include "gui.h"
#include "dialog.h"

class gui_fft_graph : public gui_dialog
{
public:
    void init();
    void tick();
    void enter() {}
    void exit() {}
private:
    lv_obj_t *m_chart;
    lv_chart_series_t *m_dl1;
    lv_chart_series_t *m_dl2;
    lv_obj_t *m_back_button;
};

DEFINE_DIALOG(gui_fft_graph);