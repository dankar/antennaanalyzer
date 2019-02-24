#include "gui.h"
#include "menu.h"
#include "dialogs/input_graph.h"
#include "display.h"

uint32_t g_num_dialogs = 0;
gui_dialog *g_dialogs[20];
gui_dialog *g_current_dialog = NULL;

gui_dialog* gui_get_current_dialog()
{
    return g_current_dialog;
}

void gui_register_dialog(gui_dialog *dialog)
{
    g_dialogs[g_num_dialogs++] = dialog;
}

void gui_init()
{
    display_init();
    for(uint32_t i = 0; i < g_num_dialogs; i++)
    {
        g_dialogs[i]->init();
    }
}

void gui_show(gui_dialog *dialog)
{
    if(g_current_dialog)
    {
        g_current_dialog->exit();
    }
    if(dialog)
    {
        g_current_dialog = dialog;
        g_current_dialog->enter();
        dialog->show();
    }
}

void gui_tick()
{
    if(g_current_dialog)
    {
        g_current_dialog->tick();
    }
}
