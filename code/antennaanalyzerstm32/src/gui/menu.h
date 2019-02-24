#pragma once

#include "dialog.h"

#define DEFINE_MENU(MENU) class MENU : public gui_base_menu { public: void init(); }; extern MENU g_ ## MENU;
#define IMPLEMENT_MENU(MENU, BUTTONS) void MENU::init() { create_menu(sizeof(BUTTONS)/sizeof(button_t), BUTTONS); } MENU g_ ## MENU;

struct button_t
{
    const char *label;
    gui_dialog *dialog;
};

class gui_base_menu : public gui_dialog
{
public:
    void create_menu(uint8_t num_buttons, button_t buttons[]);
    //void show() {}
    void tick();
    void enter() {}
    void exit() {}
};
