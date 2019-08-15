#include "menu.h"
#include "gui.h"
#include "serial.h"

lv_res_t menu_item_click(lv_obj_t * btn)
{
    gui_dialog *dialog = (gui_dialog*)lv_obj_get_free_num(btn);
    gui_show(dialog);
    return 1;
}

int x, y;

void gui_base_menu::create_menu(uint8_t num_buttons, button_t buttons[])
{
    m_scr = lv_page_create(NULL, NULL);

    lv_obj_t * cont;
    cont = lv_cont_create(m_scr, NULL);
    lv_obj_set_width(cont, 310);
    lv_obj_set_height(cont, 220);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY);
    //lv_cont_set_fit(cont, true, true);

    for(int i = 0; i < num_buttons; i++)
    {
        lv_obj_t* button = lv_btn_create(cont, NULL);
        lv_btn_set_action(button, LV_BTN_ACTION_CLICK, menu_item_click);
        lv_obj_set_width(button, 320/2 - 20);
        lv_obj_set_height(button, 240/3 - 10);
        
        lv_obj_t *label = lv_label_create(button, NULL);

        lv_label_set_text(label, buttons[i].label);
        lv_obj_set_free_num(button, (uint32_t)buttons[i].dialog);
    }

    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
}
void gui_base_menu::tick()
{

}
