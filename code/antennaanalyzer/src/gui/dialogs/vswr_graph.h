#pragma once

#include "gui.h"
#include "dialog.h"

class gui_vswr_graph : public gui_dialog
{
public:
    void init();
    void tick();
    void reset();
    void enter();
    void exit();
    using gui_dialog::show;
    void show() {}
private:
    float m_previous_vswr;
    float m_previous_r;
    float m_previous_phase;
    uint32_t m_current_step;
    uint32_t m_max_steps;
};

DEFINE_DIALOG(gui_vswr_graph);