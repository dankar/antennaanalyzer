#pragma once

#include "lvgl.h"
#include "dialog.h"

void gui_init();
void gui_show(gui_dialog *dialog);
void gui_tick();
void gui_register_dialog(gui_dialog *dialog);
gui_dialog* gui_get_current_dialog();