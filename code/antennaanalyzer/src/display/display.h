#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void display_init();

void enable_gui();
void disable_gui();
int is_gui_enabled();

#ifdef __cplusplus
}
#endif