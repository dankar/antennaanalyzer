#pragma once

#include "gui.h"
#include "dialog.h"

class gui_impedance_viewer : public gui_dialog
{
public:
    void init();
    void tick();
    void enter() {}
    void exit() {}
private:
    lv_obj_t *m_back_button;
    lv_obj_t *m_impedance_label;
};

DEFINE_DIALOG(gui_impedance_viewer);