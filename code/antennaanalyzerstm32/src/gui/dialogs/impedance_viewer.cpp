#include "impedance_viewer.h"
#include "adc.h"
#include "dialogs/menus.h"
#include "measure.h"
#include "ranges.h"

static lv_res_t back_button_callback(lv_obj_t * btn)
{
    gui_show(GET_DIALOG_REF(gui_main_menu));
    return 1;
}

void gui_impedance_viewer::init()
{
    m_scr = lv_page_create(NULL, NULL);
    m_impedance_label = lv_label_create(m_scr, NULL);
    lv_label_set_text(m_impedance_label, "NULL");
    lv_obj_align(m_impedance_label, NULL, LV_ALIGN_CENTER, -20, 50);

    m_back_button = lv_btn_create(m_scr, NULL);
    lv_btn_set_action(m_back_button, LV_BTN_ACTION_CLICK, back_button_callback);
    lv_obj_align(m_back_button, NULL, LV_ALIGN_CENTER, 0, 100);

    /*Create a label on the button (the 'label' variable can be reused)*/
    lv_obj_t *label = lv_label_create(m_back_button, NULL);
    lv_label_set_text(label, "Back");
}

void gui_impedance_viewer::tick()
{
    char test[256];
    range_t range = g_gui_ranges.get_current_range();
    uint32_t f = range.start_frequency + (range.end_frequency - range.start_frequency) / 2;
    g_impedance_tester.make_measurement(f);
    complex_t impedance = g_impedance_tester.get_impedance();

    sprintf(test, "%d: %d.%d + j%d.%d", (int)f, int(impedance.real), int((impedance.real-int(impedance.real))*10.0f), int(impedance.imag), int((impedance.imag-int(impedance.imag))*10.0f));
    lv_label_set_text(m_impedance_label, test);
}

IMPLEMENT_DIALOG(gui_impedance_viewer);