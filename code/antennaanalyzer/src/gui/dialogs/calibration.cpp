#include "calibration.h"
#include "adc.h"
#include "dialogs/menus.h"
#include "measure.h"

volatile uint8_t confirmed = 0;

void gui_calibration::report_percentage(uint8_t percentage)
{
    char buffer[5];
    sprintf(buffer, "%d%%", percentage);
    lv_label_set_text(m_progress_label, buffer);
    lv_task_handler();
}

static lv_res_t confirm_callback(lv_obj_t * mbox, const char * txt)
{
    confirmed = 1;
    return 1;
}

void gui_calibration::do_message_box(const char *text)
{
    confirmed = 0;
    static const char *buttons[] = {"Confirm", ""};
    if(m_mbox)
        lv_obj_del(m_mbox);
    m_mbox = lv_mbox_create(m_scr, NULL);
    lv_mbox_set_text(m_mbox, text);
    lv_mbox_add_btns(m_mbox, buttons, confirm_callback);
    //lv_obj_set_width(m_mbox, 250);
    //lv_obj_set_height(m_mbox, 100);
    lv_obj_align(m_mbox, NULL, LV_ALIGN_CENTER, 0, 0);
}

void gui_calibration::delete_message_box()
{
    if(m_mbox)
        lv_obj_del(m_mbox);
    m_mbox = NULL;
    lv_task_handler();
}

void gui_calibration::ask_for_5ohm()
{
    do_message_box("Please attach 5 Ohm dummy load");
}
void gui_calibration::ask_for_50ohm()
{
    do_message_box("Please attach 50 Ohm dummy load");
}
void gui_calibration::ask_for_500ohm()
{
    do_message_box("Please attach 500 Ohm dummy load");
}

void gui_calibration::init()
{
    m_scr = lv_page_create(NULL, NULL);
    //m_cont = lv_cont_create(m_scr, NULL);
    //lv_obj_set_size(m_cont, 310, 230);

    m_progress_label = lv_label_create(m_scr, NULL);
    lv_label_set_text(m_progress_label, "0%");
    lv_obj_align(m_progress_label, NULL, LV_ALIGN_CENTER, 0, 240/2);

    m_mbox = NULL;
    m_state = NONE;
}

void gui_calibration::tick()
{
    switch(m_state)
    {
    case NONE:
        ask_for_5ohm();
        m_state = CALIBRATE_OHM5;
        break;
    case CALIBRATE_OHM5:
        if(confirmed)
        {
            delete_message_box();
            g_impedance_tester.run_calibration(OHM5);
            ask_for_50ohm();
            m_state = CALIBRATE_OHM50;
        }
        break;
    case CALIBRATE_OHM50:
        if(confirmed)
        {
            delete_message_box();
            g_impedance_tester.run_calibration(OHM50);
            ask_for_500ohm();
            m_state = CALIBRATE_OHM500;
        }
        break;
    case CALIBRATE_OHM500:
        if(confirmed)
        {
            delete_message_box();
            g_impedance_tester.run_calibration(OHM500);
            m_state = CALIBRATE_CALCULATE;
        }
        break;
    case CALIBRATE_CALCULATE:
        g_impedance_tester.run_calibration(CALCULATE);
        m_state = CALIBRATE_SAVE;
        break;
    case CALIBRATE_SAVE:
        m_state = NONE;
        gui_show(GET_DIALOG_REF(gui_settings_menu));
        break;
    }
}

IMPLEMENT_DIALOG(gui_calibration);