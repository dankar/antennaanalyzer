#include "ranges.h"
#include "adc.h"
#include "dialogs/menus.h"
#include "measure.h"



range_t g_ranges[] = {{"80m", 3500000, 3800000}, {"40m", 7000000, 7200000}, {"20m", 14000000, 14350000}, {"10m", 28000000, 29700000}, {"All", 100000, 30000000}};

static lv_res_t back_button_callback(lv_obj_t * btn)
{
    gui_show(GET_DIALOG_REF(gui_main_menu));
    return 1;
}

lv_res_t range_click(lv_obj_t * btn)
{
    uint32_t index = (uint32_t)lv_obj_get_free_num(btn);
    g_gui_ranges.set_current_range(index);
    return 1;
}

void gui_ranges::init()
{
    m_current_range = 0;
    m_scr = lv_page_create(NULL, NULL);

    lv_obj_t * cont;
    cont = lv_cont_create(m_scr, NULL);
    lv_obj_set_width(cont, 310);
    lv_obj_set_height(cont, 220);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY);

    for(uint32_t i = 0; i < sizeof(g_ranges) / sizeof(range_t); i++)
    {
        lv_obj_t* button = lv_btn_create(cont, NULL);
        lv_btn_set_action(button, LV_BTN_ACTION_CLICK, range_click);
        lv_obj_set_width(button, 320/2 - 20);
        lv_obj_set_height(button, 240/3 - 10);
        
        lv_obj_t *label = lv_label_create(button, NULL);

        lv_label_set_text(label, g_ranges[i].label);
        lv_obj_set_free_num(button, i);
    }
    
    m_back_button = lv_btn_create(cont, NULL);
    lv_btn_set_action(m_back_button, LV_BTN_ACTION_CLICK, back_button_callback);
    lv_obj_set_width(m_back_button, 320/2 - 20);
    lv_obj_set_height(m_back_button, 240/3 - 10);
    /*Create a label on the button (the 'label' variable can be reused)*/
    lv_obj_t *label = lv_label_create(m_back_button, NULL);
    lv_label_set_text(label, "Back");

    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
}

void gui_ranges::set_current_range(uint32_t index)
{
    m_current_range = index;
}

range_t &gui_ranges::get_current_range()
{
    return g_ranges[m_current_range];
}

void gui_ranges::tick()
{
    
}

IMPLEMENT_DIALOG(gui_ranges);