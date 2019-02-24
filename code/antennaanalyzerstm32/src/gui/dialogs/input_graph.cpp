#include "input_graph.h"
#include "adc.h"
#include "dialogs/menus.h"
#include "measure.h"

lv_res_t back_button_callback(lv_obj_t * btn)
{
    gui_show(GET_DIALOG_REF(gui_main_menu));
    return 1;
}

void gui_input_graph::init()
{
    m_scr = lv_page_create(NULL, NULL);
    m_chart = lv_chart_create(m_scr, NULL);                         /*Create the chart*/
    lv_obj_set_size(m_chart, lv_obj_get_width(m_scr)-20, lv_obj_get_height(m_scr)-70);   /*Set the size*/
    lv_obj_align(m_chart, NULL, LV_ALIGN_CENTER, 0, 0);                   /*Align below the slider*/
    lv_chart_set_series_width(m_chart, 1);                                            /*Set the line width*/
    lv_chart_set_range(m_chart, 600,3400);
    /*Add a RED data series and set some points*/
    m_dl1 = lv_chart_add_series(m_chart, LV_COLOR_RED);
    /*Add a BLUE data series and set some points*/
    m_dl2 = lv_chart_add_series(m_chart, LV_COLOR_MAKE(0x40, 0x70, 0xC0));
    lv_chart_set_point_count(m_chart, 256);

    m_back_button = lv_btn_create(m_scr, NULL);
    lv_btn_set_action(m_back_button, LV_BTN_ACTION_CLICK, back_button_callback);
    lv_obj_align(m_back_button, m_chart, LV_ALIGN_IN_BOTTOM_LEFT, 0, 50);

    /*Create a label on the button (the 'label' variable can be reused)*/
    lv_obj_t *label = lv_label_create(m_back_button, NULL);
    lv_label_set_text(label, "Back");
}

void gui_input_graph::tick()
{
    g_impedance_tester.make_measurement(10000000);

    for(int i = 0; i < 256; i++)
    {
        lv_chart_set_next(m_chart, m_dl1, g_ADCValues[i*8].vv);
        //serial_print("%d\r\n", ADC1ConvertedValues[i].vv);
        lv_chart_set_next(m_chart, m_dl2, g_ADCValues[i*8].vi);
    }
}

IMPLEMENT_DIALOG(gui_input_graph);