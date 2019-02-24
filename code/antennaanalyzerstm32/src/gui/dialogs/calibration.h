#pragma once

#include "gui.h"
#include "dialog.h"

enum calibration_state_t
{
    NONE,
    CALIBRATE_OHM5,
    CALIBRATE_OHM50,
    CALIBRATE_OHM500,
    CALIBRATE_CALCULATE,
    CALIBRATE_SAVE
};

class gui_calibration : public gui_dialog
{
public:
    void init();
    void tick();
    void enter() {}
    void exit() {}
    void ask_for_5ohm();
    void ask_for_50ohm();
    void ask_for_500ohm();
    void report_percentage(uint8_t percentage);
private:
    void do_message_box(const char *text);
    void delete_message_box();
    lv_obj_t *m_mbox;
    lv_obj_t *m_cont;
    lv_obj_t *m_progress_label;
    calibration_state_t m_state;
};

DEFINE_DIALOG(gui_calibration);