#include "dialog.h"
#include "gui.h"

gui_dialog::gui_dialog()
{
    gui_register_dialog(this);
}

void gui_dialog::show()
{
    lv_scr_load(m_scr);
}