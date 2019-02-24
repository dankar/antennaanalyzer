#pragma once

#include "gui.h"
#include "dialog.h"

struct range_t
{
    const char *label;
    uint32_t start_frequency;
    uint32_t end_frequency;
};

class gui_ranges : public gui_dialog
{
public:
    void init();
    void tick();
    void enter() {}
    void exit() {}
    void set_current_range(uint32_t index);
    range_t &get_current_range();
private:
    lv_obj_t *m_back_button;
    uint32_t m_current_range;
};

DEFINE_DIALOG(gui_ranges);