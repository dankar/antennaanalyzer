#include "menus.h"
#include "dialog.h"
#include "input_graph.h"
#include "fft_graph.h"
#include "impedance_viewer.h"
#include "calibration.h"
#include "vswr_graph.h"
#include "smith_chart.h"
#include "ranges.h"

button_t main_menu_buttons[] = {
    {"VSWR Graph", GET_DIALOG_REF(gui_vswr_graph)}, 
    {"Smith Chart", GET_DIALOG_REF(gui_smith_chart)},
    {"Debug", GET_DIALOG_REF(gui_debug_menu)}, 
    {"Settings", GET_DIALOG_REF(gui_settings_menu)}
};

IMPLEMENT_MENU(gui_main_menu, main_menu_buttons);


button_t settings_menu_buttons[] = {
    {"Ranges", GET_DIALOG_REF(gui_ranges)},
    {"Calibrate", GET_DIALOG_REF(gui_calibration)},
    {"Back", GET_DIALOG_REF(gui_main_menu)}
};
IMPLEMENT_MENU(gui_settings_menu, settings_menu_buttons);


button_t debug_menu_buttons[] = {
    {"Input graph", GET_DIALOG_REF(gui_input_graph)},
    {"FFT graph", GET_DIALOG_REF(gui_fft_graph)}, 
    {"View impedance", GET_DIALOG_REF(gui_impedance_viewer)},
    {"Back", GET_DIALOG_REF(gui_main_menu)}
};
IMPLEMENT_MENU(gui_debug_menu, debug_menu_buttons);